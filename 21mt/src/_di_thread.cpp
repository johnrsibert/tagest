#include "adi_thread.h"
//#include "coff_t.h"
//#include "thread_extensions.h"
#include "trace.h"

adi_threads::adi_threads(coff_t* coff, const int max_threads)
{
  TTRACE(coff,max_threads)
  num_threads = max_threads;
  max_queue_size = coff->jmax; //num_threads;
  do_not_block_when_full = 0;
  cur_queue_size = 0;
  queue_head = NULL;
  queue_tail = NULL;
  queue_closed = 0;
  shutdown = 0;
  int rtn = 0;
  if ((rtn = pthread_mutex_init(&queue_lock, NULL)))  
  {
    cerr << "pthread_mutex_init " << strerror(rtn) << endl;
    exit(1);
  }
  if ((rtn = pthread_cond_init(&queue_not_empty, NULL)))  
  {
    cerr << "pthread_cond_init " << strerror(rtn) << endl;
    exit(1);
  }
  if ((rtn = pthread_cond_init(&queue_not_full, NULL)))  
  {
    cerr << "pthread_cond_init " << strerror(rtn) << endl;
    exit(1);
  }
  if ((rtn = pthread_cond_init(&queue_not_full, NULL)))  
  {
    cerr << "pthread_cond_init " << strerror(rtn) << endl;
    exit(1);
  }

  coff_ptr = coff;
  HERE
  threads.allocate(1,max_threads);
  for (int i = 1; i <= max_threads; i++)
  {
    threads(i) = (int)new pthread_t;
    TTRACE(i,(pthread_t*)threads(i))
    if (( rtn = PTHREAD_CREATE((pthread_t*)threads(i), NULL, 
                               s_executive, (void*)this) ))
    {
      cerr << "pthread create error " << rtn << "for thread " << i << endl;
      exit(1);
    }
    TRACE(thread_name(*(pthread_t*)threads(i)))
  }
}


adi_threads::~adi_threads(void)
{
  for (int i = 1; i <= max_threads; i++)
  {
    delete (pthread_t*)threads(i);
  }

}

int adi_threads::run_solver(solver_fn_t _solverp, const int ij)
  //rc_solver_args_t* _argsp, const int _thrno)
{
  HERE
  pthread_mutex_lock(&queue_lock);
  
  if ( (cur_queue_size == max_queue_size) &&
       (do_not_block_when_full) )
  {
    pthread_mutex_unlock(&queue_lock);
    return(-1);
  }

  while ( (cur_queue_size == max_queue_size) &&
          (!(shutdown || queue_closed)) )
  {
    pthread_cond_wait(&queue_not_full, &queue_lock);
  }
  
  if (shutdown || queue_closed)
  {
    pthread_mutex_unlock(&queue_lock);
    return(-1);
  }

  rc_solver_args_t* argsp = new rc_solver_args_t;
  clogf << "argsp allocated:" << endl;
  TRACE(argsp)
  argsp->ij = ij;
  argsp->parent = coff_ptr;
  argsp->thread_no = cur_queue_size;

  solver_description_t* local_solver = new solver_description_t;
  clogf << "local_solver allocated:" << endl;
  TRACE(local_solver)
  local_solver->solver_functionp = _solverp;
  local_solver->argsp = argsp;
  local_solver->next = NULL;

  if (cur_queue_size == 0)
  {
    queue_tail = queue_head = local_solver;
    pthread_cond_broadcast(&queue_not_empty);
  }
  else
  {
    queue_tail->next = local_solver;
    queue_tail = local_solver;
  }
  cur_queue_size++;

  pthread_mutex_unlock(&queue_lock);
  HERE 
  return(1);
} // run_solver

void* adi_threads::s_executive(void* t)
{
  HERE 
  adi_threads* p = (adi_threads*)t;
  p->executive(t);
}

void* adi_threads::executive(void* t)
{
  while (1)
  {
    HERE 
    pthread_mutex_lock(&queue_lock);
    
    TTRACE(cur_queue_size,shutdown)
    while ( (cur_queue_size == 0) && (!shutdown) )
    {
      pthread_cond_wait(&queue_not_empty, &queue_lock);
    }
    HERE 
    if (shutdown)
    {
      pthread_mutex_unlock(&queue_lock);
      pthread_exit(NULL);
    }
   
    HERE 
    solver_description_t* local_solver = queue_head;

    HERE 
    if (cur_queue_size == 0)
      queue_head = queue_tail = NULL;
    else
      queue_head = local_solver->next; 
  
    HERE 
    if ( (!do_not_block_when_full) &&
         (cur_queue_size = (max_queue_size-1)) )
      pthread_cond_broadcast(&queue_not_full);

    HERE 
    if (cur_queue_size == 0)
      pthread_cond_signal(&queue_empty);
   

    HERE 
    pthread_mutex_unlock(&queue_lock);
    (*(local_solver->solver_functionp))(local_solver->argsp);

    clogf << "executive about to delete:" << endl;
    TRACE(local_solver->argsp)
    delete local_solver->argsp;

    clogf << "executive about to delete:" << endl;
    TRACE(local_solver)
    delete local_solver;

  }

} // executive

