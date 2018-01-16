#include "coff_t.h"
#include "par_t.h"
#include "modlarea.h"
#include "modlarea.h"

#include "trace.h"

//extern thread_pool* adi_thread_pool;

//int coff_t::max_threads = 2;
//int coff_t::rc_counter = 0;
//pthread_attr_t coff_t::adi_attr;
sem_t coff_t::col_barrier;
sem_t coff_t::row_barrier;
//sched_param coff_t::adi_priority;



coff_t::coff_t(Model_area& param, int ipar6)
 : a(1,param.n,param.ilb,param.iub), b(1,param.n,param.ilb,param.iub),
 raw_b(1,param.n,param.ilb,param.iub), c(1,param.n,param.ilb,param.iub),
 xbet(1,param.n,param.ilb,param.iub), xgam(1,param.n,param.ilb,param.iub),
 d(1,param.m,param.jlb,param.jub), e(1,param.m,param.jlb,param.jub), 
 f(1,param.m,param.jlb,param.jub), ybet(1,param.m,param.jlb,param.jub),
 ygam(1,param.m,param.jlb,param.jub), g(1,param.m,param.jlb,param.jub),
 imax(param.m), jmax(param.n), psi(1, param.m, param.jlb, param.jub),
 ilb(param.ilb), iub(param.iub), jlb(param.jlb), jub(param.jub)
{
  if (ipar6 > 1)
      k = ipar6;  // time steps per month
  else
      k = 1;

  int JJ;
  if (imax > jmax) 
      JJ = imax;
  else 
      JJ = jmax;
  TTRACE(imax,jmax)
  TTRACE(max_threads,JJ)

  _rr.allocate(1,max_threads,1,JJ);
  _uu.allocate(1,max_threads,1,JJ);
  _work.allocate(1,max_threads,1,JJ);
  _work1.allocate(1,max_threads,1,JJ);
  
  //pthread_mutex_init(&rc_counter_lock,NULL);
  //pthread_mutex_init(&dfdata_lock,NULL);
  //threads.allocate(1,max_threads);
  //threads = new pthread_t [max_threads];

  solver_args.allocate(1,max_threads);
  for (int i = 1; i <= max_threads; i++)
  {
    solver_args(i) = (int) new rc_solver_args_t;
  }

  //int scope;
  //pthread_attr_getscope(&adi_attr,&scope);
  //TRACE(scope);
 
  //TTRACE(sched_get_priority_min(SCHED_FIFO),sched_get_priority_max(SCHED_FIFO))
  //adi_priority.sched_priority = sched_get_priority_max(SCHED_FIFO);
  //TRACE(adi_priority.sched_priority)

  //PTHREAD_ATTR_INIT(&adi_attr); // thread_extensions.c
  /*
  int aerr = pthread_attr_init(&adi_attr);
  if (aerr)
  {
    cerr << "Error initializing adi_attr; " << strerror(aerr) << endl;
    exit(1);
  }
  */
  //pthread_attr_setdetachstate(&adi_attr, PTHREAD_CREATE_DETACHED);
  //pthread_attr_setscope(&adi_attr, PTHREAD_SCOPE_SYSTEM);
  //pthread_attr_setschedpolicy(&adi_attr,SCHED_FIFO);
  //pthread_attr_setschedparam(&adi_attr,&adi_priority);
  //pthread_attr_setscope(&adi_attr,PTHREAD_SCOPE_SYSTEM);
  //pthread_attr_getscope(&adi_attr,&scope);
  //TRACE(scope);

}

coff_t::~coff_t()
{
  cerr << "~coff_t" << endl;
  for (int i = 1; i <= max_threads; i++)
  {
    delete (rc_solver_args_t*)solver_args(i);
    solver_args(i) = 0;
  }
  //delete[] threads;
}

int coff_t::check_rc_counter(const int rc_limit)//, pthread_cond_t& condition)
{
  int rv = 0;
  /*
  pthread_mutex_lock(&rc_counter_lock);
  rc_counter++;
  if (rc_counter >= rc_limit)
  {
    rc_counter = 0;
    adi_thread_pool->set_finished();
    rv = 1;
  }
  pthread_mutex_unlock(&rc_counter_lock);
  */
  return(rv);
}

void coff_t::clean()
{
    a.initialize();
    b.initialize();
    raw_b.initialize();
    c.initialize();
    d.initialize();
    e.initialize();
    f.initialize();
    g.initialize();

    xbet.initialize();
    ybet.initialize();
    xgam.initialize();
    ygam.initialize();

    _rr.initialize();
    _uu.initialize();
    _work.initialize();
    _work1.initialize();
    psi.initialize();
}

void coff_t::write(ostream& s, int i, int j)
{
   s << "coff["<<i<<"]["<<j<<"]";
   s << " .a = " << a[j][i];
   s << " .b = " << b[j][i];
   s << " .raw_b = " << raw_b[j][i];
   s << " .c = " << c[j][i];
   s << endl;
   s << "coff["<<i<<"]["<<j<<"]";
   s << " .d = " << d[i][j];
   s << " .e = " << e[i][j];
   s << " .f = " << f[i][j];
   s << " .g = " << g[i][j];
   s << endl;
   s << "coff["<<i<<"]["<<j<<"]";
   s << " .xgam = " << xgam[j][i];
   s << " .xbet = " << xbet[j][i];
   s << " .ygam = " << ygam[i][j];
   s << " .ybet = " << ybet[i][j];
   s << endl;
}

// from thread_extensions.c

void thread_single_barrier(sem_t *barrier, int count)
{
  while (count > 0)
    {SEM_WAIT(barrier);
     count--;
    }
}

int SEM_INIT(sem_t *sem, int pshared, unsigned int value)
{int err;
 if (err = sem_init(sem, pshared, value))
   {perror("");
    abort();
  }}

int SEM_WAIT(sem_t *arg)        /* Ignore signal interruptions */
{while (sem_wait(arg) != 0) {}}

int PTHREAD_ATTR_INIT(pthread_attr_t *a)
{int err;
 if (err = pthread_attr_init(a))
   {printf("%s\n", strerror(err));
    abort();
  }}

