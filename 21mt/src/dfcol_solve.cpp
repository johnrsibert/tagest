#include "precomp.h"
#include "trace.h"
#include "thread_pool.h"

//extern pthread_mutex_t dfu_lock;
//extern pthread_mutex_t dfpsi_lock;

void coff_t::s_dfcolumn_solver(void* t)
{
   rc_solver_args_t* cs_args = (rc_solver_args_t*)t;

   int start_rc = cs_args->start_rc;
   int end_rc = cs_args->end_rc;
   coff_t* parent = cs_args->client;
   int thread_id = cs_args->thread_no;

   parent->dfcolumn_solver(start_rc, end_rc, thread_id);
   sem_post(&col_barrier);
}

void coff_t::dfcolumn_solver(const int start_rc, const int end_rc, const int thread_no)
{
  coff_t& coff = *const_pointer;
  dmatrix& u = *coff.state_variable;
  dmatrix& dfu = *state_variable;

  dmatrix& a_c = coff.a;
  dmatrix& d_c = coff.d;
  dmatrix& e_c = coff.e;
  dmatrix& f_c = coff.f;
  dmatrix& g_c = coff.g;
  dmatrix& xbet_c = coff.xbet;
  dmatrix& xgam_c = coff.xgam;

  dmatrix& dfa = a;
  dmatrix& dfd = d;
  dmatrix& dfe = e;
  dmatrix& dff = f;
  dmatrix& dfg = g;
  dmatrix& dfxbet = xbet;
  dmatrix& dfxgam = xgam;
  dmatrix& dfpsi  = psi; //dfpsi.initialize();

  int i;

  dvector& rhs = coff._rr(thread_no);
  dvector& uu = coff._uu(thread_no);
  dvector& work = coff._work(thread_no);
  dvector& work1 = coff._work1(thread_no);
  dvector& dfwork = _work(thread_no);		dfwork.initialize();
  dvector& dfwork1 = _work1(thread_no);	dfwork1.initialize();
  dvector& dfrhs = _rr(thread_no);			dfrhs.initialize();
  dvector& dfuu = _uu(thread_no);				dfuu.initialize();

  for (int j = end_rc; j >= start_rc; j--)
  {
    int il = ilb(j);
    int iu = iub(j);
    for (i=il; i<=iu; i++)
    {
      if (j==jlb(i))
        rhs[i]=g_c[i][j] - (e_c[i][j]-r4)*u[i][j] - f_c[i][j]*u[i][j+1];
      else if (j==jub(i))
        rhs[i]=g_c[i][j] - d_c[i][j]*u[i][j-1] - (e_c[i][j]-r4)*u[i][j];
      else
        rhs[i]=g_c[i][j] - d_c[i][j]*u[i][j-1] - (e_c[i][j]-r4)*u[i][j]
                         - f_c[i][j]*u[i][j+1];
    } // i loop
  
    for (i=iu; i>=il; i-- )
    {
  //  psi[i][j] = uu[i];

      dfuu[i]+=dfpsi[i][j];
      dfpsi[i][j]=0;
    }

    dftridag(a_c[j], rhs, xbet_c[j], xgam_c[j],
                   dfa[j], dfrhs, dfxbet[j], dfxgam[j],
                   uu, dfuu, il, iu, work, dfwork, work1, dfwork1);

    for (i=iu; i>=il; i-- )
    {
      if (j==jlb(i))
      {
//      rhs[i]=g[i][j] - (e[i][j]-r)*u[i][j] - f[i][j]*u[i][j+1];

        dfg[i][j]+=dfrhs[i];
        dfe[i][j]-=u[i][j]*dfrhs[i];
        dfu[i][j]-=(e_c[i][j]-r4)*dfrhs[i];
        dff[i][j]-=u[i][j+1]*dfrhs[i];
        dfu[i][j+1]-=f_c[i][j]*dfrhs[i];
      }
      else
      {
        if (j==jub(i))
        {
//        rhs[i]=g[i][j] - d[i][j]*u[i][j-1] - (e[i][j]-r)*u[i][j];

          dfg[i][j]+=dfrhs[i];
          dfd[i][j]-=u[i][j-1]*dfrhs[i];
          dfe[i][j]-=u[i][j]*dfrhs[i];
          dfu[i][j]-=(e_c[i][j]-r4)*dfrhs[i];
          dfu[i][j-1]-=d_c[i][j]*dfrhs[i];
        }
        else
        {
//        rhs[i]=g[i][j] - d[i][j]*u[i][j-1] - (e[i][j]-r)*u[i][j]
//                       - f[i][j]*u[i][j+1];

          dfg[i][j]+=dfrhs[i];
          dfd[i][j]-=u[i][j-1]*dfrhs[i];
          dfe[i][j]-=u[i][j]*dfrhs[i];
          dfu[i][j]-=(e_c[i][j]-r4)*dfrhs[i];
          dff[i][j]-=u[i][j+1]*dfrhs[i];
          dfu[i][j-1]-=d_c[i][j]*dfrhs[i];
          dfu[i][j+1]-=f_c[i][j]*dfrhs[i];
        }
      }
      dfrhs[i] = 0.0;
    }//End i loop
  } // j loop
} // end coff_t::dfcolumn_solver
