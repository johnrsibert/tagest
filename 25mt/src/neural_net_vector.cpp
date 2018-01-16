//$Id: neural_net_vector.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "neural_net_vector.h"

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
neural_net_vector<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::neural_net_vector(): v(0)
{
  /* leave empty */
}
template neural_net_vector<d3_array,dmatrix,dvector,double>::neural_net_vector();
template neural_net_vector<dvar3_array,dvar_matrix,dvar_vector,dvariable>::neural_net_vector();


template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void neural_net_vector<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::allocate(void)
{
  v->p = (neural_net<D3_ARRAY,MATRIX,VECTOR,DOUBLE>*) malloc( (size_t)v->nsize*sizeof(neural_net<D3_ARRAY,MATRIX,VECTOR,DOUBLE>) );
  v->p0 = v->p - v->en1;
  if (!v->p)
  {
    cerr << "New failed in neural_net_vector allocation.\n";
    exit(1);
  }
}
template void neural_net_vector<d3_array,dmatrix,dvector,double>::allocate(void);
template void neural_net_vector<dvar3_array,dvar_matrix,dvar_vector,dvariable>::allocate(void);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void neural_net_vector<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::initialize(void)
{
  for (int i = v-> en1; i <= v->en2; i++)
  {
     (*this)(i).initialize();
  }
}
template void neural_net_vector<d3_array,dmatrix,dvector,double>::initialize(void);
template void neural_net_vector<dvar3_array,dvar_matrix,dvar_vector,dvariable>::initialize(void);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void neural_net_vector<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::allocate(int n1, int n2)
{
  v = (neural_net_vrep<D3_ARRAY,MATRIX,VECTOR,DOUBLE>*) malloc( (size_t)sizeof(neural_net_vrep<D3_ARRAY,MATRIX,VECTOR,DOUBLE>) );
  v->en1 = n1;
  v->en2 = n2;
  v->nsize = n2-n1+1;
  v->refcnt = 1;
  allocate ();
}
template void neural_net_vector<d3_array,dmatrix,dvector,double>::allocate(int n1, int n2);
template void neural_net_vector<dvar3_array,dvar_matrix,dvar_vector,dvariable>::allocate(int n1, int n2);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
neural_net_vector<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::neural_net_vector(const neural_net_vector<d3_array,dmatrix,dvector,double>& x)
{
    x.v->refcnt++;
    v = x.v;
}
template neural_net_vector<d3_array,dmatrix,dvector,double>::neural_net_vector(const neural_net_vector<d3_array,dmatrix,dvector,double>& x);
template<> neural_net_vector<dvar3_array,dvar_matrix,dvar_vector,dvariable>::neural_net_vector(const neural_net_vector<d3_array,dmatrix,dvector,double>& x) {
	cerr << "ERROR: " << __FILE__ << ':' << __LINE__ << endl;
	if (true) exit(1);
}

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
neural_net_vector<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::neural_net_vector(const neural_net_vector<dvar3_array,dvar_matrix,dvar_vector,dvariable>& x)
{
    x.v->refcnt++;
    v = x.v;
}
template neural_net_vector<dvar3_array,dvar_matrix,dvar_vector,dvariable>::neural_net_vector(const neural_net_vector<dvar3_array,dvar_matrix,dvar_vector,dvariable>& x);
template<> neural_net_vector<d3_array,dmatrix,dvector,double>::neural_net_vector(const neural_net_vector<dvar3_array,dvar_matrix,dvar_vector,dvariable>& x)  {
	cerr << "ERROR: " << __FILE__ << ':' << __LINE__ << endl;
	if (true) exit(1);
}

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
neural_net_vector<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::neural_net_vector(int n1, int n2)
{
   allocate(n1,n2);
}


template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
neural_net_vector<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::~neural_net_vector()
{
  if (v != 0 && --v->refcnt == 0)
  {
    free(v->p);
    free(v);
    //cout <<"'neural_net_vector'  object safely destroyed" << endl;
  }
}
template neural_net_vector<d3_array,dmatrix,dvector,double>::~neural_net_vector();
template neural_net_vector<dvar3_array,dvar_matrix,dvar_vector,dvariable>::~neural_net_vector();

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
neural_net_vector<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& neural_net_vector<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::operator=(const neural_net_vector<d3_array,dmatrix,dvector,double>& x)
{
  if ( (x.v->en1 != v->en1) || (x.v->en2 != v->en2) )
  {
    cerr << "Attempted assignment to neural_net_vectors of unequal size.\n";
    exit(1);
  }
  x.v->refcnt++;
  v = x.v;
  return *this;
}
template neural_net_vector<d3_array,dmatrix,dvector,double>& neural_net_vector<d3_array,dmatrix,dvector,double>::operator=(const neural_net_vector<d3_array,dmatrix,dvector,double>& x);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
neural_net_vector<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& neural_net_vector<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::operator=(const neural_net_vector<dvar3_array,dvar_matrix,dvar_vector,dvariable>& x)
{
  if ( (x.v->en1 != v->en1) || (x.v->en2 != v->en2) )
  {
    cerr << "Attempted assignment to neural_net_vectors of unequal size.\n";
    exit(1);
  }
  x.v->refcnt++;
  v = x.v;
  return *this;
}
template<> neural_net_vector<d3_array,dmatrix,dvector,double>& neural_net_vector<d3_array,dmatrix,dvector,double>::operator=(const neural_net_vector<dvar3_array,dvar_matrix,dvar_vector,dvariable>& x) {
	cerr << "ERROR: " << __FILE__ << ':' << __LINE__ << endl;
	if (true) exit(1);
}
