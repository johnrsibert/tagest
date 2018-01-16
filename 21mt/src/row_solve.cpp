#include "precomp.h"
#include "thread_pool.h"
#include "trace.h"

void coff_t::s_row_solver(void* t)
{
   rc_solver_args_t* cs_args = (rc_solver_args_t*)t;

   int start_rc = cs_args->start_rc;
   int end_rc = cs_args->end_rc;
   coff_t* parent = cs_args->client;
   int thread_no = cs_args->thread_no;

   parent->row_solver(start_rc, end_rc, thread_no);
   sem_post(&row_barrier);
}


void coff_t::row_solver(const int start_rc, const int end_rc, const int thread_no)
{
   dmatrix& u = *state_variable;

   int j;

   dvector& rhs = _rr(thread_no);
   dvector& uu = _uu(thread_no);
   dvector& work = _work(thread_no);

   for (int i = start_rc; i <= end_rc; i++)
   {
     int jl = jlb(i);
     int ju = jub(i);
     for (j=jl;j<=ju;j++)
     {
	  if (i==ilb(j))
	    rhs[j] = g[i][j] + (r4-b[j][i])*psi[i][j] - c[j][i]*psi[i+1][j];
	  else if (i==iub(j))
	    rhs[j] = g[i][j] - a[j][i]*psi[i-1][j] + (r4-b[j][i])*psi[i][j];
	  else
	    rhs[j] = g[i][j] - a[j][i]*psi[i-1][j] + (r4-b[j][i])*psi[i][j]
                        - c[j][i]*psi[i+1][j];
     }

     tridag(d[i],rhs,uu,ybet[i],ygam[i],jl,ju,work);
     for (j=jl; j<=ju; j++)
     {
       u[i][j]=uu[j];
     }
  }
}
