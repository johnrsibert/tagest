//$Id: adi.cpp 3132 2014-02-04 21:54:57Z johnoel $
#include "coff_t.h"
#include "intersav.h"
#ifdef SIMULATE
  #undef SIMULATE
#endif
//#undef HERE
//#define HERE cout << "reached " << __LINE__ << " in " << __FILE__ << endl;
#include "trace.h"

extern setsaveinterflag interflag;
extern int saveinterflag;

template <> 
void coff_t<d3_array,dmatrix,dvector,double>::
adiADMBIntersave(dmatrix& u, const dmatrix& psi)
{
}

template <> 
double coff_t<dvar3_array, dvar_matrix, dvar_vector, dvariable>::
vGet(dvariable x)
{
  return(value(x));
}

template<> 
double coff_t<d3_array,dmatrix,dvector,double>::
vGet(double x)
{
  return(x);
}

template <> 
void coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::
vSet(prevariable y, const double x)
{
  value(y) = x;
}

template <> 
void coff_t<d3_array, dmatrix, dvector, double>::
vSet(double & y, const double x)
{
  y = x;
}

template <> 
void coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::
vSaveValue(dmatrix& m, const char * id, intersavetype * savefile)
{
   if (saveinterflag == MM_intersave::ADMB_mode)
   {
      save_identifier_string((char*)id);
      m.save_dmatrix_value();
   }
}

template <> 
void coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::
vSaveValue(dvar_matrix& m, const char * id, intersavetype * )
{
   if (saveinterflag == MM_intersave::ADMB_mode)
   {
      save_identifier_string((char*)id);
      m.save_dvar_matrix_value();
   }
}

template <> 
void coff_t<d3_array, dmatrix, dvector, double>::
vSaveValue(dmatrix& m, const char * id, intersavetype * savefile)
{
   if (saveinterflag == MM_intersave::MM_mode)
   {
      save_identifier_string((char*)id);
      interflag.msavep(m, id, savefile);
   }
}

template <typename VECTOR>
void tridag(const VECTOR& a, const VECTOR& b, const VECTOR& c, const VECTOR& r, VECTOR& u, const VECTOR& bet, const VECTOR& gam, const int nl, const int nu, VECTOR& v);

extern ofstream clogf;
void grid_report(dmatrix& t, ofstream& c, const char* name);


template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::
adi(MATRIX& _u, intersavetype * savefile)
{
  /*
    cout << "savefile = " << savefile << endl;
    cout << "saveinterflag = " << saveinterflag << endl;
    HERE
    if (saveinterflag == MM_intersave::ADMB_mode)
       ad_exit(1);
  */
  MATRIX & u = _u;
  //dmatrix u = vGet(_u);
  dmatrix a = vGet(_a);
  dmatrix b = vGet(_b);
  dmatrix c = vGet(_c);
  dmatrix d = vGet(_d);
  dmatrix e = vGet(_e);
  dmatrix f = vGet(_f);
  dmatrix g = vGet(_g);
  dmatrix xgam  = vGet(_xgam);
  dmatrix xbet = vGet(_xbet);
  dmatrix ygam  = vGet(_ygam);
  dmatrix ybet = vGet(_ybet);
  dmatrix psi = vGet(_psi);
  dvector rr = vGet(_rr);
  dvector uu = vGet(_uu);
  dvector work = vGet(_work);

  rr.initialize();
  uu.initialize();
  work.initialize();

  int nits = k;
  double r  = 4.0 * (double)k;



  for (int kits = 1; kits <= nits; kits++)
  {
     for (int j = 1; j <= jmax; j++)
     {
         int il = ilb(j);
         int iu = iub(j);

         for (int i = il; i <= iu; i++)
         {
           if (j == jlb(i))
             rr[i]  = g[i][j] + (r-e[i][j])*vGet(u[i][j]) - f[i][j]*vGet(u[i][j+1]);
           else if (j == jub(i))
             rr[i] = g[i][j] - d[i][j]*vGet(u[i][j-1]) + (r-e[i][j])*vGet(u[i][j]);
           else
             rr[i] = g[i][j] - d[i][j]*vGet(u[i][j-1]) + (r-e[i][j])*vGet(u[i][j]) - f[i][j]*vGet(u[i][j+1]);
         }

         tridag(a[j],b[j],c[j],rr,uu,xbet[j],xgam[j],il, iu,work);

         for (int i=il; i<=iu; i++)
         {
           psi[i][j] = uu[i];
         }
     }//j loop

     vSaveValue(u, "u",savefile);

  #ifdef SIMULATE
    grid_report(psi, clogf, "psi (first half)");
  #endif

     for (int i=1;i<=imax;i++)
     {
         int jl = jlb(i);
         int ju = jub(i);
         for (int j=jl;j<=ju;j++)
         {
           if (i==ilb(j))
             rr[j] = g[i][j] + (r-b[j][i])*psi[i][j] - c[j][i]*psi[i+1][j];
           else if (i==iub(j))
             rr[j] = g[i][j] - a[j][i]*psi[i-1][j] + (r-b[j][i])*psi[i][j];
           else
             rr[j] = g[i][j] - a[j][i]*psi[i-1][j] + (r-b[j][i])*psi[i][j] - c[j][i]*psi[i+1][j];
         }

         tridag(d[i],e[i],f[i],rr,uu,ybet[i],ygam[i],jl,ju,work);

         for (int j=jl; j<=ju; j++)
         {
            //u[i][j]=uu[j];
            vSet(u[i][j],uu[j]);
         }
     }//i loop 

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


template<> double coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::vGet(double x)
{
  return(x);
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

