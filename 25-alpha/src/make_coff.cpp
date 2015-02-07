//$Id: make_coff.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "coff_t.h"
#include "modlarea.h"
#include <fstream>

extern ofstream clogf;

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>* create_coff_t(const Model_area& param, const int ipar6, const bool centered)
{
  coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>* ctp = 0;
                                                                                                                                                                                                     
  if (centered)
  {
    ctp = new Centered_coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>((Model_area&)param,ipar6);
    clogf << "Made Centered_coff_t" << endl;
    cout  << "Made Centered_coff_t" << endl;
  }
  else
  {
    ctp = new Upwind_coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>((Model_area&)param,ipar6);
    clogf << "Made Upwind_coff_t" << endl;
    cout  << "Made Upwind_coff_t" << endl;
  }
  if (!ctp)
  {
     cerr << "Error constructing coff_t in coff_t* make_coff( ... )" << endl;
     cerr << " for argument centered = " << centered << endl;
     exit(1);
  }
  return(ctp);
}
template coff_t<d3_array,dmatrix,dvector,double>* create_coff_t<d3_array,dmatrix,dvector,double>(const Model_area& param, const int ipar6, const bool centered);
template coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>* create_coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>(const Model_area& param, const int ipar6, const bool centered);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>* coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::instance = 0;
template coff_t<d3_array,dmatrix,dvector,double>* coff_t<d3_array,dmatrix,dvector,double>::instance;
template coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>* coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::instance;

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>* coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::createInstance(const par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE> &param) {
        if (instance == 0) {
                  instance = create_coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(param, param.m_ipar[6], param.m_ipar[49]);
        }
        return instance;
}
template coff_t<d3_array,dmatrix,dvector,double>* coff_t<d3_array,dmatrix,dvector,double>::createInstance(const par_t<d3_array,dmatrix,dvector,double> &param);
template coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>* coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::createInstance(const par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable> &param);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
coff_t<d3_array,dmatrix,dvector,double>* coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::dfinstance = 0;
template coff_t<d3_array,dmatrix,dvector,double>* coff_t<d3_array,dmatrix,dvector,double>::dfinstance;
template coff_t<d3_array,dmatrix,dvector,double>* coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::dfinstance;

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
coff_t<d3_array,dmatrix,dvector,double>* coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::createDfInstance(const par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE> &param) {
        if (dfinstance == 0) {
                  dfinstance = create_coff_t<d3_array,dmatrix,dvector,double>(param, param.m_ipar[6], param.m_ipar[49]);
        }
        return dfinstance;
}
template coff_t<d3_array,dmatrix,dvector,double>* coff_t<d3_array,dmatrix,dvector,double>::createDfInstance(const par_t<d3_array,dmatrix,dvector,double> &param);
template coff_t<d3_array,dmatrix,dvector,double>* coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::createDfInstance(const par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable> &param);
