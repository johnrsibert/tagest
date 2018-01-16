#include "precomp.h"
#include "trace.h"
#include "thread_pool.h"

//extern pthread_mutex_t dfu_lock;
//extern pthread_mutex_t dfpsi_lock;


void coff_t::s_dfrow_solver(void* t)
{
   rc_solver_args_t* cs_args = (rc_solver_args_t*)t;

   int start_rc = cs_args->start_rc;
   int end_rc = cs_args->end_rc;
   coff_t* parent = cs_args->client;
   int thread_id = cs_args->thread_no;

   parent->dfrow_solver(start_rc, end_rc, thread_id);
   sem_post(&row_barrier);
}

void coff_t::dfrow_solver(const int start_rc, const int end_rc, const int thread_no)
{
  coff_t& coff = *const_pointer;
  dmatrix& dfu = *state_variable;
  dmatrix& a_c = coff.a;
  dmatrix& b_c = coff.b;
  dmatrix& c_c = coff.c;
  dmatrix& d_c = coff.d;
  dmatrix& g_c = coff.g;
  dmatrix& ybet_c = coff.ybet;
  dmatrix& ygam_c = coff.ygam;
  dmatrix& psi_c  = coff.psi;

  dmatrix& dfa = a;
  dmatrix& dfb = b;
  dmatrix& dfc = c;
  dmatrix& dfd = d;
  dmatrix& dfg = g;
  dmatrix& dfybet = ybet;
  dmatrix& dfygam = ygam;
  dmatrix& dfpsi  = psi; //dfpsi.initialize();

  int j;

  dvector& rhs = coff._rr(thread_no);
  dvector& uu = coff._uu(thread_no);
  dvector& work = coff._work(thread_no);
  dvector& work1 = coff._work1(thread_no);
  dvector& dfwork = _work(thread_no); dfwork.initialize();
  dvector& dfwork1 = _work1(thread_no); dfwork1.initialize();
  dvector& dfrhs = _rr(thread_no);   dfrhs.initialize();
  dvector& dfuu = _uu(thread_no);   dfuu.initialize();

  for (int i = end_rc; i >= start_rc; i--)
  {
    int jl = jlb(i);
    int ju = jub(i);
    for (j=jl;j<=ju;j++)
    {
	 if (i==ilb(j))
	   rhs[j] = g_c[i][j] - (b_c[j][i]-r4)*psi_c[i][j]
                         - c_c[j][i]*psi_c[i+1][j];
	 else if (i==iub(j))
	   rhs[j] = g_c[i][j] - a_c[j][i]*psi_c[i-1][j]
                         - (b_c[j][i]-r4)*psi_c[i][j];
	 else
	   rhs[j]= g_c[i][j] - a_c[j][i]*psi_c[i-1][j]
                        - (b_c[j][i]-r4)*psi_c[i][j] - c_c[j][i]*psi_c[i+1][j];
    } // j loop

    for (j=jl; j<=ju; j++)
    {
//    u[i][j] = uu[j];
      dfuu[j] += dfu[i][j];
      dfu[i][j]=0;
    }

    dfwork.initialize();
    dfwork1.initialize();
    dftridag(d_c[i], rhs, ybet_c[i], ygam_c[i],
                 dfd[i], dfrhs, dfybet[i], dfygam[i],
                 uu, dfuu, jl, ju, work, dfwork, work1, dfwork1);

    for (j=ju; j>=jl; j--)
    {
       if (i==ilb(j))
       {
//        rr_c[j]=g[i][j] - (b[j][i]-r)*psi[i][j] - c[j][i]*psi[i+1][j];

          dfg[i][j]+=dfrhs[j];
	  dfb[j][i]-=psi_c[i][j]*dfrhs[j];
	  dfpsi[i][j]-=(b_c[j][i]-r4)*dfrhs[j];
	  dfc[j][i]-=psi_c[i+1][j]*dfrhs[j];
	  dfpsi[i+1][j]-=c_c[j][i]*dfrhs[j];
	}
 	else
       {
         if (i==iub(j))
	 {
//          rr[j]=g[i][j] - a[j][i]*psi[i-1][j] - (b[i][i]-r)*psi[i][j];

	    dfg[i][j]+=dfrhs[j];
	    dfa[j][i]-=psi_c[i-1][j]*dfrhs[j];
	    dfpsi[i-1][j]-=a_c[j][i]*dfrhs[j];
	    dfb[j][i]-=psi_c[i][j]*dfrhs[j];
	    dfpsi[i][j]-=(b_c[j][i]-r4)*dfrhs[j];
	  }
	  else
	  {
//          rr[j]=g[i][j] - a[j][i]*psi[i-1][j] - (b[j][i]-r)*psi[i][j]
//                          - c[j][i]*psi[i+1][j];

	    dfg[i][j]+=dfrhs[j];
	    dfa[j][i]-=psi_c[i-1][j]*dfrhs[j];
	    dfpsi[i-1][j]-=a_c[j][i]*dfrhs[j];
	    dfb[j][i]-=psi_c[i][j]*dfrhs[j];
	    dfpsi[i][j]-=(b_c[j][i]-r4)*dfrhs[j];
	    dfc[j][i]-=psi_c[i+1][j]*dfrhs[j];
	    dfpsi[i+1][j]-=c_c[j][i]*dfrhs[j];
          }
	}
	dfrhs[j] = 0.0;
      }//j loop

  } //for (int i = end_rc; i >= start_rc; i--)
} // end coff_t::dfrow_solver
