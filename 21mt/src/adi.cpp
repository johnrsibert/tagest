#include "precomp.h"
#include "intersav.h"
#include <pthread.h>

#include "tpool.h"

#undef __MT__
#define __MT__	1

#ifdef SIMULATE
  #undef SIMULATE
#endif
#include "trace.h"

extern tpool* adi_thread_pool;
//extern pthread_mutex_t release_lock;

extern setsaveinterflag interflag;
extern int saveinterflag;
extern ofstream clogf;
void grid_report(dmatrix& t, ofstream& c, const char* name);

void print_sem_value(sem_t& s, const int line)
{
  int value, ret;
//int sem_getvalue(sem_t *sem, int *sval);

  ret = sem_getvalue(&s,&value);
  clogf << "semaphore value = " << value << " at line " << line 
        << "; return = " << ret << endl;
}

void coff_t::adi(dmatrix& u, intersavetype * savefile)
{
  const_pointer = this;
  state_variable = &u;

  int nits = k;
  r4  = 4 * k;

  int num_rc = 0;
  int end_rc = 0;
  int start_rc = 0;
  for (int kits = 1; kits <= nits; kits++)
  {
    num_rc = jmax/max_threads;
    end_rc = 0;
    start_rc = 0;

    // initialize the barrier for first use
    //print_sem_value(col_barrier,__LINE__);
    SEM_INIT(&col_barrier, 0,  0); // thread_extensions.c
    /*
    int err= sem_init(&adi_barrier, 0, 0);
    if (err)
    {
      cerr << "Error initializing adi_barrier; " << strerror(err) << endl;
      exit(1);
    }
    */
    for (int tt = 1; tt <= max_threads; tt++)
    {
      start_rc = end_rc + 1;
      if (tt < max_threads)
        end_rc = start_rc + num_rc -1;
      else
        end_rc = jmax;
 
      ((rc_solver_args_t*)solver_args(tt))->start_rc = start_rc;
      ((rc_solver_args_t*)solver_args(tt))->end_rc = end_rc;
      ((rc_solver_args_t*)solver_args(tt))->client = this;
      ((rc_solver_args_t*)solver_args(tt))->thread_no = tt;

    #ifdef __MT__
      /*
      int crtn = pthread_create(&threads[tt-1],&adi_attr,s_column_solver,(void*)((rc_solver_args_t*)solver_args(tt)));
      if (crtn)
      {
        cerr<< "error " << strerror(crtn) << " creating column thread " << tt <<endl;
        HERE
        exit(1);
      }
      */
      int crtn = adi_thread_pool->tpool_add_work(s_column_solver,
                             (void*)((rc_solver_args_t*)solver_args(tt)));
    #else
      s_column_solver((void*)((rc_solver_args_t*)solver_args(tt)));
    #endif
    }

    #ifdef __MT__
    thread_single_barrier(&col_barrier, max_threads);
    /*
    for (int tt = 1; tt <= max_threads; tt++)
    {
       int rtn = pthread_join((pthread_t)threads(tt), NULL);
       if (rtn)
       {
          cerr << "error " << strerror(rtn) << " for join on column thread " << tt << endl;
          exit(1);
       }
    }
    */
    // initialize the barrier for next use
    SEM_INIT(&row_barrier, 0,  0); // thread_extensions.c
    /*
    err= sem_init(&adi_barrier, 0, 0);
    if (err)
    {
      cerr << "Error re-initializing adi_barrier; " << strerror(err) << endl;
      exit(1);
    }
    */
    #endif
 
    if (saveinterflag > 0)
    {
      interflag.msavep(u, "u", savefile );
    }

  #ifdef SIMULATE
    grid_report(psi, clogf, "psi (first half)");
  #endif

    num_rc = imax/max_threads;
    end_rc = 0;
    start_rc = 0;

    for (int tt = 1; tt <= max_threads; tt++)
    {
      start_rc = end_rc + 1;
      if (tt < max_threads)
        end_rc = start_rc + num_rc -1;
      else
        end_rc = imax;

      ((rc_solver_args_t*)solver_args(tt))->start_rc = start_rc;
      ((rc_solver_args_t*)solver_args(tt))->end_rc = end_rc;
      ((rc_solver_args_t*)solver_args(tt))->client = this;
      ((rc_solver_args_t*)solver_args(tt))->thread_no = tt;
    #ifdef __MT__
      /*
      int crtn = pthread_create(&threads[tt-1],&adi_attr,s_row_solver,(void*)((rc_solver_args_t*)solver_args(tt)));
      if (crtn)
      {
         cerr<< "error " << strerror(crtn) << " creating row thread " << tt <<endl;
         HERE
         exit(1);
      }
      */
      int crtn = adi_thread_pool->tpool_add_work(s_row_solver,(void*)((rc_solver_args_t*)solver_args(tt)));
    #else
      s_row_solver((void*)((rc_solver_args_t*)solver_args(tt)));
    #endif

    }

  #ifdef __MT__
    thread_single_barrier(&row_barrier, max_threads);
    /*
    for (int tt = 1; tt <= max_threads; tt++)
    {
       int rtn = pthread_join((pthread_t)threads(tt), NULL);
       if (rtn)
       {
          cerr << "error " << strerror(rtn) << " for join on row thread " << tt << endl;
          exit(1);
       }
    }
    */
  #endif
 
  #ifdef SIMULATE
    grid_report(u, clogf, "u (second half)");
  #endif
    if (saveinterflag > 0)
    {
       interflag.msavep(psi, "psi", savefile );
    }
  } // end of kits loop
} // end of adi

#ifdef SIMULATE
void grid_report(dmatrix& t, ofstream& c, const char* name)
{
  double sum = 0;
  int negatives = 0;
  double tmin = 0;
  double tmax = 0;

  int i1=t.rowmin();
  int i2=t.rowmax();
  for (int i=i1; i<=i2; i++)
  {
    int j1 = t(i).indexmin();
    int j2 = t(i).indexmax();
    for (int j=j1; j<=j2; j++)
    {
      sum += t[i][j];
      if (t[i][j] > tmax)
	tmax = t[i][j];
      if (t[i][j] < 0)
      {
	 negatives ++;
	 if (t[i][j] < tmin)
	   tmin = t[i][j];
      }
    }
  }

  c << "sum of " << name <<" cells = " << sum
    << "; the maximum was " << tmax << endl;
  if (negatives > 0)
  {
    c << "  there were " << negatives << " negatives" << endl;
    c << "  the minimum was " << tmin << endl;
  }
}
#endif
