#include "recap_t.h"


//int recaptype::cohort_pooling_flag = 0;

recaptype_vector::recaptype_vector(int n1, int n2)
{
  v = (recaptype_vrep*) malloc( (size_t)sizeof(recaptype_vrep) );
  v->en1 = n1;
  v->en2 = n2;
  v->nsize = n2-n1+1;
  v->refcnt = 1;
  cohort_pooling_flag = 0;
  total_recaps = 0.0;

  allocate ();
}
void recaptype_vector::allocate(void)
{
  v->p = (recaptype_*) malloc( (size_t)v->nsize*sizeof(recaptype_) );
  if (!v->p)
  {
    cerr << "New failed in vector allocation.\n";
    exit(1);
  }
}
recaptype_vector::~recaptype_vector()
{
  if (--v->refcnt==0)
  {
    free(v->p);
    free(v);
  }
}
recaptype_& recaptype_vector::operator[](int i)
  {
    if ( (i < v->en1) || (i > v->en2) )
    {
      cerr << "subscript out of range in vector\n";
      cerr << "i="<< i << ", n1=" << v->en1 <<", n2="<<v->en2<<"\n";
      exit(1);
    }
    return v->p[i-v->en1];
  }
recaptype_vector& recaptype_vector::operator=(recaptype_vector& x)
{
  if ( (x.v->en1 != v->en1) || (x.v->en2 != v->en2) )
  {
    cerr << "Attempted assignment to vectors of unequal size.\n";
    exit(1);
  }
  x.v->refcnt++;
  v = x.v;
  return *this;
}

ostream& operator << (ostream& s, recaptype_& t)
{
  s << setw(6) << t.fleet
    << setw(3) << t.flindex
    << " " << t.date
    << setw(4) << t.cohort
    << setw(3) << t.i << setw(3) << t.j
    << setw(5) << t.returns;
  return(s);
}

