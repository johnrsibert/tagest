#include "thread_pool.h"
#include <string.h>
#include <signal.h>

#include <iostream>
#include <fstream>
using std::cerr;
using std::endl;

double elapsed_time(void);
extern ofstream clogf;
#undef HERE
#define HERE clogf << pthread_self() << " reached " << __LINE__ << " in " << __FILE__ << endl;

#undef TRACE
#define TRACE(object) clogf << pthread_self() << " line " << __LINE__ << ", file " << __FILE__ << ", " << #object " = " << object << endl;

#undef TTRACE
#define TTRACE(o1,o2) clogf << pthread_self() << " line " << __LINE__ << ", file " << __FILE__ << ", " << #o1 " = " << o1<< ", " << #o2 " = " << o2 << endl;


thread_pool::thread_pool(void)
{
  clogf << "in thread_pool::thread_pool(void)" << endl;
  cur_queue_size = 0;
  threads = NULL;
  queue_head = NULL;
  queue_tail = NULL;
  start_args = NULL;
  queue_closed = 0;
  shutdown = 0;
  thread_id = NULL;
}

void thread_pool::allocate(const int num_worker_threads, 
                           const int _max_queue_size,
                           const int _do_not_block_when_full)
{
  num_threads = num_worker_threads;
  max_queue_size = _max_queue_size;
  do_not_block_when_full = _do_not_block_when_full;
 
  thread_id = new int [num_threads];
  if (thread_id  == NULL)
  {
    cerr << "error allocating memory for thread_id" << endl;
    exit(1);
  }
 
  start_args = new tpool_work_t [num_threads];
  if (start_args  == NULL)
  {
    cerr << "error allocating memory for start_args" << endl;
    exit(1);
  }

  threads = new pthread_t [num_threads];
  if (threads == NULL)
  {
    cerr << "error allocating memory for thread array" << endl;
    exit(1);
  }  

  join_flag = 0;
  finished_flag = 0;
  
  int rtn = 0;
  if ((rtn = pthread_mutex_init(&queue_lock, NULL)) != 0)
  {
    cerr << "pthread_mutex_init " << strerror(rtn)<< endl;
    exit(1);
  }
  if ((rtn = pthread_mutex_init(&finished_lock, NULL)) != 0)
  {
    cerr << "pthread_mutex_init " << strerror(rtn)<< endl;
    exit(1);
  }
  if ((rtn = pthread_cond_init(&queue_not_empty, NULL)) != 0)
  {
    cerr << "pthread_cond_init " << strerror(rtn) << endl;
    exit(1);
  }
  if ((rtn = pthread_cond_init(&queue_not_full, NULL)) != 0)
  {
     cerr << "pthread_cond_init " << strerror(rtn) << endl;
     exit(1);
  }
  if ((rtn = pthread_cond_init(&queue_empty, NULL)) != 0)
  {
    cerr << "pthread_cond_init " << strerror(rtn) << endl;
    exit(1);
  }
  if ((rtn = pthread_cond_init(&finished_condition, NULL)) != 0)
  {
    cerr << "pthread_cond_init " << strerror(rtn) << endl;
    exit(1);
  }
  
  
  //if (( rtn = pthread_key_create(&thread_id_key, s_thread_id_destroy) ))
  if (( rtn = pthread_key_create(&thread_id_key, 0) ))
  {
    cerr << "pthread_key_create " << strerror(rtn) << endl;
    exit(1);
  }
 
  // create threads
  for (int i = 0; i < num_threads; i++)
  {
    thread_id[i] = i+1;
    start_args[i].routine = NULL;
    start_args[i].arg = this;
    start_args[i].thread_id = &(thread_id[i]);
    start_args[i].next = NULL;
    if ((rtn = pthread_create(&(threads[i]), NULL, s_tpool_thread, (void *)&start_args[i])) != 0)
    {
      cerr << "pthread_create " << rtn << endl;
      exit(1);
    }
  } // for
}

int thread_pool::tpool_add_work(worker_fn_t routine, void* arg, void* client)
{
  int rtn = 0;
  tpool_work_t* workp = NULL;

  if ((rtn = pthread_mutex_lock(&(queue_lock))) != 0)
  {
    cerr << "pthread_mutex_lock " << rtn << endl;
    exit(1);
  }

  // no space and this caller doesn't want to wait 
  if ((cur_queue_size == max_queue_size) && do_not_block_when_full)
  {
    if ((rtn = pthread_mutex_unlock(&(queue_lock))) != 0)
    {
       cerr << "pthread_mutex_unlock " << rtn << endl;
       exit(1);
    }
    return -1;
  }

  while( (cur_queue_size == max_queue_size) && (!(shutdown || queue_closed))  )
  {
    if ((rtn = pthread_cond_wait(&(queue_not_full), &(queue_lock))) != 0)
    {
      cerr << "pthread_cond_wait " << rtn << endl;
      exit(1);
    }
  }

  // the pool is in the process of being destroyed 
  if (shutdown || queue_closed)
  {
    if ((rtn = pthread_mutex_unlock(&(queue_lock))) != 0)
    {
      cerr << "pthread_mutex_unlock " << rtn;
      exit(1);
    }
    return -2;
  }

  if ((workp = new tpool_work_t) == NULL)
  {
     cerr << "new error for work structure" << endl;
     exit(1);
  }
  workp->routine = routine;
  workp->arg = arg;
  workp->next = NULL;

  clogf << "adder: adding routine " << (unsigned*)(workp->routine) 
        << " arg " << (unsigned*)(workp->arg) << endl;

  if (cur_queue_size == 0) 
  {
    queue_tail = queue_head = workp;

    clogf << "adder: queue == 0, waking all workers" << endl;

    if ((rtn = pthread_cond_broadcast(&(queue_not_empty))) != 0)
    {
      cerr << "pthread_cond_broadcast " << rtn;
      exit(1);
    }
  }
  else
  {
    queue_tail->next = workp;
    queue_tail = workp;
  }

  cur_queue_size++;
  if ((rtn = pthread_mutex_unlock(&(queue_lock))) != 0)
  {
    cerr << "pthread_mutex_unlock " << rtn << endl;
    exit(1);
  }
  return 1;
}

thread_pool::~thread_pool(void)
{
  HERE
  cerr << "in thread_pool::~thread_pool(void)" << endl;

  int rtn = 0;

  if ((rtn = pthread_mutex_lock(&(queue_lock))) != 0)
  {
    cerr << "pthread_mutex_lock " << rtn;
    exit(1);
  }

  // Is a shutdown already in progress?
  if (queue_closed || shutdown)
  {
    if ((rtn = pthread_mutex_unlock(&(queue_lock))) != 0)
    {
      cerr << "pthread_mutex_unlock " << rtn << endl;
      exit(1);
    }
    return;
  }

  queue_closed = 1;

  int finish = 1;
  // If the finish flag is set, wait for workers to drain queue
  if (finish == 1)
  {
    while (cur_queue_size != 0)
    {
      if ((rtn = pthread_cond_wait(&(queue_empty), &(queue_lock))) != 0)
      {
      	cerr << "pthread_cond_wait " << rtn << endl;
        exit(1);
      }
    }
  }

  shutdown = 1;

  if ((rtn = pthread_mutex_unlock(&(queue_lock))) != 0)
  {
    cerr << "pthread_mutex_unlock " << rtn << endl;
    exit(1);
  }

  // Wake up any workers so they recheck shutdown flag
  if ((rtn = pthread_cond_broadcast(&(queue_not_empty))) != 0)
  {
    cerr << "pthread_cond_broadcast " << rtn << endl;
    exit(1);
  }

  if ((rtn = pthread_cond_broadcast(&(queue_not_full))) != 0)
  {
    cerr << "pthread_cond_broadcast " << rtn << endl;
    exit(1);
  }

  // Wait for workers to exit
  for(int i=0; i < num_threads; i++)
  {
    if ((rtn = pthread_join(threads[i],NULL)) != 0)
    {
      cerr << "pthread_join " << rtn << endl;
      exit(1);
    }
  }

  // free pool structures
  delete[] threads;
  threads = NULL;
 
  delete[] start_args;
  start_args = NULL;

  delete[] thread_id;
  thread_id = NULL;

  tpool_work_t* cur_nodep = NULL;
  while(queue_head != NULL)
  {
    cur_nodep = queue_head->next;
    queue_head = queue_head->next;
    cerr << "deleting " << cur_nodep << endl;
    delete cur_nodep;
  }
}

int thread_pool::join(const int join_code) // = 0
{
  int rtn = 0;
  HERE
  if ((rtn = pthread_mutex_lock(&(queue_lock))) != 0)
  {
    cerr << "pthread_mutex_lock " << rtn;
    exit(1);
  }

  join_flag = 1;
  while (cur_queue_size != 0)
  {
    if ((rtn = pthread_cond_wait(&(queue_empty), &(queue_lock))) != 0)
    {
      cerr << "pthread_cond_wait " << rtn << endl;
      exit(1);
    }
  }

  HERE
  if ((rtn = pthread_cond_broadcast(&(queue_not_full))) != 0)
  {
    cerr << "pthread_cond_broadcast " << rtn << endl;
    exit(1);
  }
  HERE

  if ((rtn = pthread_mutex_lock(&finished_lock)) != 0)
  {
    cerr << "pthread_mutex_lock " << rtn;
    exit(1);
  }

  while(!finished_flag)
  {
    HERE
    if ((rtn = pthread_cond_wait(&finished_condition, &finished_lock)) != 0)
    {
      cerr << "pthread_cond_wait " << rtn << endl;
      exit(1);
    }
  }
  HERE
  finished_flag = 0;
  if ((rtn = pthread_mutex_unlock(&finished_lock)) != 0)
  {
    cerr << "pthread_mutex_unlock " << rtn;
    exit(1);
  }
  HERE

  join_flag = 0;
  if ((rtn = pthread_mutex_unlock(&(queue_lock))) != 0)
  {
    cerr << "pthread_mutex_unlock " << rtn;
    exit(1);
  }
  HERE
 
  return(0);
}

void* thread_pool::s_tpool_thread(void *t)
{
  tpool_work_t* tpw = (tpool_work_t*)t;
  thread_pool* p = (thread_pool*)(tpw->arg);
  p->tpool_thread(t);
  return(0);
}

void* thread_pool::tpool_thread(void *t)
{
  tpool_work_t* tpw = (tpool_work_t*)t;
  int* thread_id = tpw->thread_id;
  int rtn = 0;
  tpool_work_t* my_workp=NULL;
  clogf << "created thread " << pthread_self() << " aka " << *thread_id << endl;

  if ((rtn = pthread_setspecific(thread_id_key, (void*)thread_id)) != 0)
  {
     cerr << " pthread_setspecific " << rtn << endl;
     exit(1);
  }
  pthread_sigmask(SIG_BLOCK,&mask,NULL);
 
  for(;;)
  {
    // Check queue for work
    rtn = pthread_mutex_lock(&(queue_lock));
    if (rtn)
    {
      cerr << "pthread_mutex_lock " << rtn << endl;
      exit(1);
    }
  
    while ( (cur_queue_size == 0) && (!shutdown) )
    {
      clogf << "worker " << pthread_self() << ": I'm sleeping again" << endl;
      rtn = pthread_cond_wait(&queue_not_empty, &queue_lock);
      if (rtn)
      {
	cerr << "pthread_cond_wait " << rtn << endl;
        exit(1);
      }
    }

    clogf << "worker " << pthread_self() << ": I'm awake" << endl;

    // Has a shutdown started while i was sleeping?
    if (shutdown == 1)
    {
      rtn = pthread_mutex_unlock(&(queue_lock));
      if (rtn)
      {
	cerr << "pthread_mutex_unlock " << rtn << endl;
        exit(1);
      }
      HERE
      pthread_exit(NULL);
    }

    HERE
    // Get to work, dequeue the next item
    my_workp = queue_head;
    cur_queue_size--;
    if (cur_queue_size == 0)
      queue_head = queue_tail = NULL;
    else
      queue_head = my_workp->next;

    clogf << "worker " <<  pthread_self()
          << ": dequeing item " << my_workp->next << endl;

    // Handle waiting add_work threads
    if ((!do_not_block_when_full) && (cur_queue_size ==  (max_queue_size - 1)))
    {
      rtn = pthread_cond_broadcast(&(queue_not_full));
      if (rtn)
      {
	cerr << "pthread_cond_broadcast " <<rtn << endl;
        exit(1);
      }
    }
    // Handle waiting destroyer threads
    if (cur_queue_size == 0)
    {
      rtn = pthread_cond_signal(&queue_empty);
      if (rtn)
      {
         cerr << "pthread_cond_signal " << rtn << endl;
         exit(1);
      }
    }
    rtn = pthread_mutex_unlock(&(queue_lock));
    if (rtn)
    {
      cerr << "pthread_mutex_unlock " << rtn << endl;
      exit(1);
    }
    
    // Do this work item
    //if (my_workp)
    //{
      (*(my_workp->routine))(my_workp->arg);
      delete my_workp;
      my_workp = NULL;
    //}

  } //for(;;)

  return(NULL);
}

void thread_pool::s_thread_id_destroy(void* t)
{
  thread_pool* p = (thread_pool*)t;
  p->thread_id_destroy(t);
}

void thread_pool::thread_id_destroy(void* _t)
{
  cerr << "in void thread_pool::thread_id_destroy(void* _t)" << endl;
  int rtn = pthread_key_delete(thread_id_key);
  if (rtn)
  {
     cerr << "error from  pthread_key_delete(thread_id_key)" << endl;
     //exit(1);
  }
}

void thread_pool::set_finished(void)
{
  pthread_mutex_lock(&finished_lock);
  finished_flag = 1;
  pthread_mutex_unlock(&finished_lock);
  pthread_cond_signal(&finished_condition);
}

int thread_pool::get_thread_id(void)
{
  int* idp;
  idp = (int*)pthread_getspecific(thread_id_key);
  return (*idp);
}
