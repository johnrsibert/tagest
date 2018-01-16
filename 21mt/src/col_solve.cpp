#include "precomp.h"
//#include "thread_pool.h"
#include "trace.h"

void coff_t::s_column_solver(void* t)
{
   rc_solver_args_t* cs_args = (rc_solver_args_t*)t;

   int start_rc = cs_args->start_rc;
   int end_rc = cs_args->end_rc;
   coff_t* parent = cs_args->client;
   int thread_no = cs_args->thread_no;

   parent->column_solver(start_rc,end_rc,thread_no); 
   sem_post(&col_barrier);
}

void coff_t::column_solver(const int start_rc, const int end_rc, const int thread_no)
{
   dmatrix& u = *state_variable;

   dvector& work = _work(thread_no);
   dvector& rhs = _rr(thread_no);
   dvector& uu = _uu(thread_no);
   for (int j = start_rc; j <= end_rc; j++)
   {
     int il = ilb(j);
     int iu = iub(j);
     for (int i = il; i <= iu; i++)
     {
	  if (j == jlb(i))
	    rhs[i]  = g[i][j] + (r4-e[i][j])*u[i][j] - f[i][j]*u[i][j+1];
	  else if (j == jub(i))
	    rhs[i] = g[i][j] - d[i][j]*u[i][j-1] + (r4-e[i][j])*u[i][j];
	  else
	    rhs[i] = g[i][j] - d[i][j]*u[i][j-1] + (r4-e[i][j])*u[i][j]
                         - f[i][j]*u[i][j+1];
     }

     tridag(a[j],/*b[j],*/ rhs,uu,xbet[j],xgam[j],il, iu,work);

     for (int i=il; i<=iu; i++)
     {
	  psi(i,j) = uu(i);
     } 
   }
} // end of  coff_t::column_solver
