#include "precomp.h"
#include "intersav.h"
#include "tpool.h"
#include "trace.h"
#undef  __MT__
#define __MT__	1

extern tpool* adi_thread_pool;

//pthread_mutex_t dfu_lock = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_t dfpsi_lock = PTHREAD_MUTEX_INITIALIZER;



extern ofstream clogf;
extern setsaveinterflag interflag;

void coff_t::dfadi(coff_t& coff, dmatrix& u, dmatrix& dfu, 
                   intersavetype *savefile)
{
  const_pointer = &coff;
  state_variable = &dfu;

  dmatrix& psi_c  = coff.psi;
  dmatrix& dfpsi  = psi; dfpsi.initialize();

  int nits = k;
  r4  = 4 * k;

  int num_rc = 0;
  int end_rc = 0;
  int start_rc = 0;

  for (int kits=nits; kits>=1; kits--)
  {
    interflag.mgetp(psi_c, "psi",savefile);

    num_rc = imax/max_threads;
    end_rc = 0;
    start_rc = 0;

    // initialize the barrier for next use
    SEM_INIT(&row_barrier, 0,  0); // thread_extensions.c
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
      int crtn = pthread_create(&threads[tt-1],&adi_attr,s_dfrow_solver,(void*)((rc_solver_args_t*)solver_args(tt)));
      if (crtn)
      {
         cerr<< "error " << strerror(crtn) << " creating row thread " << tt <<endl;
         HERE
         exit(1);
      }
      */
      int crtn = adi_thread_pool->tpool_add_work(s_dfrow_solver, (void*)((rc_solver_args_t*)solver_args(tt)));
    #else
      s_dfrow_solver((void*)((rc_solver_args_t*)solver_args(tt)));
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
          cerr << "error " << strerror(rtn) << " for join on column thread " << tt << endl;
          exit(1);
       }
    }
    */
    SEM_INIT(&col_barrier, 0,  0); // thread_extensions.c
    #endif

    interflag.mgetp(u, "u",savefile);

    // initialize the barrier for next use
    num_rc = jmax/max_threads;
    end_rc = 0;
    start_rc = 0;

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
      int crtn = pthread_create(&threads[tt-1],&adi_attr,s_dfcolumn_solver,(void*)((rc_solver_args_t*)solver_args(tt)));
      if (crtn)
      {
         cerr<< "error " << strerror(crtn) << " creating column thread " << tt <<endl;
         HERE
         exit(1);
      }
      */
      int crtn = adi_thread_pool->tpool_add_work(s_dfcolumn_solver, (void*)((rc_solver_args_t*)solver_args(tt)));
    #else
      s_dfcolumn_solver((void*)((rc_solver_args_t*)solver_args(tt)));
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
          cerr << "error " << strerror(rtn) << " for join on row thread " << tt << endl;
          exit(1);
       }
    }
    */
  #endif


  }//End kits loop
}//End of dfadi
