//$Id: adi.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "coff_t.h"
#include "intersav.h"
#include <pthread.h>
#include "trace.h"

#include "tpool.h"

#undef __MT__
#define __MT__	1

#ifdef SIMULATE
  #undef SIMULATE
#endif

extern setsaveinterflag interflag;
extern int saveinterflag;

template <typename VECTOR>
void tridag(const VECTOR& a, const VECTOR& b, const VECTOR& c, const VECTOR& r, VECTOR& u, const VECTOR& bet, const VECTOR& gam, const int nl, const int nu, VECTOR& v);

extern ofstream clogf;
void grid_report(dmatrix& t, ofstream& c, const char* name);


template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::
adi(MATRIX& _u, intersavetype * savefile)
{
  const_pointer = this;
  state_variable = &u;

  int nits = k;
  r4  = 4.0 * (double)k;

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

    vSaveValue(psi, "psi", savefile );
  } // end of kits loop

  // condition to prevent gradient structure error with constant objects:
  // Error -- you are trying to create a dvar_vector object when there is 
  // no object of type gradient_structure in scope 
  if (saveinterflag == MM_intersave::ADMB_mode)
       adiADMBIntersave(u, psi);
//line 126, file src/adi.cpp, sum(u) = 10933.32552, sum(_u) = 12090.85051
//  if (saveinterflag == MM_intersave::ADMB_mode)
//     vSet(_u,u); //_u = v_nograd_assign(u);

//line 130, file src/adi.cpp, sum(u) = 10933.32552, sum(_u) = 10933.32552
} // end of adi
template void coff_t<d3_array,dmatrix,dvector,double>::
adi(dmatrix& u, intersavetype * savefile);
template void coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::
adi(dvar_matrix& u, intersavetype * savefile);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void coff_t<D3_ARRAY, MATRIX, VECTOR, DOUBLE>::
adiADMBIntersave(dvar_matrix& u, const dmatrix& psi)
{
   save_identifier_string((char*)"adi1");
   psi.save_dmatrix_position();
   _a.save_dvar_matrix_value();
   _a.save_dvar_matrix_position();
   _b.save_dvar_matrix_value();
   _b.save_dvar_matrix_position();
   _c.save_dvar_matrix_value();
   _c.save_dvar_matrix_position();
   save_identifier_string((char*)"adi2");
   _d.save_dvar_matrix_value();
   _d.save_dvar_matrix_position();
   _e.save_dvar_matrix_value();
   _e.save_dvar_matrix_position();
   _f.save_dvar_matrix_value();
   _f.save_dvar_matrix_position();
   _g.save_dvar_matrix_value();
   _g.save_dvar_matrix_position();
   save_identifier_string((char*)"adi3");
   _xgam.save_dvar_matrix_value();
   _xgam.save_dvar_matrix_position();
   _ygam.save_dvar_matrix_value();
   _ygam.save_dvar_matrix_position();
   save_identifier_string((char*)"xbet");
   _xbet.save_dvar_matrix_value();
   _xbet.save_dvar_matrix_position();
   save_identifier_string((char*)"ybet");
   _ybet.save_dvar_matrix_value();
   _ybet.save_dvar_matrix_position();
   save_identifier_string((char*)"adi4");
   //save_int_value(456);
 
   u.save_dvar_matrix_position();
   //save_int_value(123);
   save_identifier_string((char*)"adi5");

   void * pthis = (void*)this;
   //TTRACE(pthis,(void*)this)
   save_pointer_value(pthis);
   save_identifier_string((char*)"adi6");
 
   gradient_structure::GRAD_STACK1->
      set_gradient_stack(bogus_dfadi); 
} // adiADMBIntersave(...)
template void coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::
adiADMBIntersave(dvar_matrix& u, const dmatrix& psi);

template <> void coff_t<d3_array,dmatrix,dvector,double>::
adiADMBIntersave(dmatrix& u, const dmatrix& psi){}

//---------- vGet overloads

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
dmatrix coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::
vGet(dmatrix& m) {return(m);}
template dmatrix coff_t<d3_array, dmatrix, dvector, double>::vGet(dmatrix& m);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
dmatrix coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::
vGet(dvar_matrix& m)
{
   dmatrix vm = value(m);
   return(vm);
}
template dmatrix coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>:: vGet(dvar_matrix& m);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
dvector coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::
vGet(dvector& v) {return(v);}
template dvector coff_t<d3_array, dmatrix, dvector, double>::vGet(dvector& v);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
dvector coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::
vGet(dvar_vector& m)
{
   dvector vm;
   vm = value(m);
   return(vm);
}
template dvector coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>:: vGet(dvar_vector& m);

template <> double coff_t<dvar3_array, dvar_matrix, dvar_vector, dvariable>::vGet(dvariable x)
{
  return(value(x));
}

template<> double coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::vGet(double x)
{
  return(x);
}

template<> double coff_t<d3_array,dmatrix,dvector,double>::vGet(double x)
{
  return(x);
}

// ---------

template <> void coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::
vSet(prevariable y, const double x)
{
  value(y) = x;
}

template <> void coff_t<d3_array, dmatrix, dvector, double>::
vSet(double & y, const double x)
{
  y = x;
}


// ------------

template <> void coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::
vSaveValue(dmatrix& m, const char * id, intersavetype * savefile)
{
   if (saveinterflag == MM_intersave::ADMB_mode)
   {
      save_identifier_string((char*)id);
      m.save_dmatrix_value();
   }
}

template <> void coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::
vSaveValue(dvar_matrix& m, const char * id, intersavetype * )
{
   if (saveinterflag == MM_intersave::ADMB_mode)
   {
      save_identifier_string((char*)id);
      m.save_dvar_matrix_value();
   }
}

template <> void coff_t<d3_array, dmatrix, dvector, double>::
vSaveValue(dmatrix& m, const char * id, intersavetype * savefile)
{
   if (saveinterflag == MM_intersave::MM_mode)
   {
      save_identifier_string((char*)id);
      interflag.msavep(m, id, savefile);
   }
}

template <> void coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::
vGetValue(dmatrix& m, const char * id, intersavetype * savefile, 
             dvar_matrix_position * pos)
{
  m = restore_dvar_matrix_value(*pos);
  verify_identifier_string((char*)id);
}

template <> void coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::
vGetValue(dvar_matrix& m, const char * id, intersavetype * savefile, 
             dvar_matrix_position * pos)
{
  m = restore_dvar_matrix_value(*pos);
  verify_identifier_string((char*)id);
}

template <> void coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::
vGetValue(dmatrix& m, const char * id, intersavetype * savefile, 
             dmatrix_position * pos)
{
  m = restore_dmatrix_value(*pos);
  verify_identifier_string((char*)id);
}

template <> void coff_t<d3_array,dmatrix,dvector,double>::
vGetValue(dmatrix& m, const char * id, intersavetype * savefile, 
             dmatrix_position * pos)
{
    interflag.mgetp(m, id, savefile);
}

