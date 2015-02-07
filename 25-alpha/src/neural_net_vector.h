//$Id: neural_net_vector.h 2754 2011-01-02 20:57:07Z jsibert $
#ifndef __NEURAL_NET_VECTOR_H__
#define __NEURAL_NET_VECTOR_H__
#include "neural_net.h"
#include "trace.h"

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
struct neural_net_vrep
{
  neural_net<D3_ARRAY,MATRIX,VECTOR,DOUBLE>* p;
  neural_net<D3_ARRAY,MATRIX,VECTOR,DOUBLE>* p0;
  int en1;
  int en2;
  int nsize;
  int refcnt;
};

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
class neural_net_vector
{
  public:
  neural_net_vrep<D3_ARRAY,MATRIX,VECTOR,DOUBLE>* v;
  void allocate();
  void initialize();

  void allocate(const int n1, const int n2);
  int indexmin() const { return v->en1; }
  int indexmax() const { return v->en2; }
  int operator !() const  { return v == NULL; }

  neural_net_vector(const neural_net_vector<d3_array,dmatrix,dvector,double>& x);
  neural_net_vector(const neural_net_vector<dvar3_array,dvar_matrix,dvar_vector,dvariable>& x);
  neural_net_vector(const int n1, const int n2);
  neural_net_vector();
  neural_net<D3_ARRAY,MATRIX,VECTOR,DOUBLE>* base()  {  return v->p; }
  inline neural_net<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& operator()(const int i) const { return (*this)[i]; }
  inline neural_net<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& operator[](const int i) const { return v->p[i-v->en1]; }
  neural_net_vector& operator=(const neural_net_vector<d3_array,dmatrix,dvector,double>& x);
  neural_net_vector& operator=(const neural_net_vector<dvar3_array,dvar_matrix,dvar_vector,dvariable>& x);

  ~neural_net_vector();

};

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
ostream& operator << (ostream& s, neural_net_vector<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& nnv);

#endif //define __NEURAL_NET_VECTOR_H__
