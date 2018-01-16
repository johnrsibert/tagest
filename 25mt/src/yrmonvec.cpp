//$Id: yrmonvec.cpp 2768 2011-02-18 02:49:17Z jsibert $
#include "yr_mon2.h"

year_month_vector::year_month_vector(const int n1, const int n2)
{
   allocate(n1,n2);
   allocate();
}

void year_month_vector::allocate(const int n1, const int n2)
{
   v = (year_month_vrep *) malloc((size_t) sizeof(year_month_vrep));
   v->en1 = n1;
   v->en2 = n2;
   v->nsize = n2 - n1 + 1;
   v->refcnt = 1;
   allocate();
}

void year_month_vector::allocate(void)
{
   v->p = (year_month_ *) malloc((size_t) v->nsize * sizeof(year_month_));
   if (!v->p)
   {
      cerr << "New failed in vector allocation.\n";
      exit(1);
   }
}

year_month_vector::~year_month_vector()
{
   if (v)
   {
      if (--v->refcnt == 0)
      {
         free(v->p);
         free(v);
      }
   }
}

#ifndef OPT_LIB
year_month_& year_month_vector::operator()(const int i)
{
  if ( (i < v->en1) || (i > v->en2) )
  {
    cerr << "subscript out of range in year_month_vector::operator()(const int i)" << endl;
    cerr << "i = "<< i << ", n1 = " << v->en1 <<", n2 = "<<v->en2<< endl;
    exit(1);
  } 
  return v->p[i-v->en1];
}

year_month_& year_month_vector::operator[](const int i)
{
  if ( (i < v->en1) || (i > v->en2) )
  {
    cerr << "subscript out of range in year_month_vector::operator[](const int i)" << endl;
    cerr << "i = "<< i << ", n1 = " << v->en1 <<", n2 = "<<v->en2<<"\n";
    exit(1);
  }
  return v->p[i-v->en1];
}

const year_month_& year_month_vector::operator()(const int i) const
{
  if ( (i < v->en1) || (i > v->en2) )
  {
    cerr << "subscript out of range in year_month_vector::operator()(const int i)" << endl;
    cerr << "i = "<< i << ", n1 = " << v->en1 <<", n2 = "<<v->en2<< endl;
    exit(1);
  } 
  return v->p[i-v->en1];
}

const year_month_& year_month_vector::operator[](const int i) const
{
  if ( (i < v->en1) || (i > v->en2) )
  {
    cerr << "subscript out of range in year_month_vector::operator[](const int i)" << endl;
    cerr << "i = "<< i << ", n1 = " << v->en1 <<", n2 = "<<v->en2<<"\n";
    exit(1);
  }
  return v->p[i-v->en1];
}
#endif

year_month_vector & year_month_vector::operator=(year_month_vector & x)
{
   if ((x.v->en1 != v->en1) || (x.v->en2 != v->en2))
   {
      cerr << "Attempted assignment to vectors of unequal size.\n";
      exit(1);
   }
   x.v->refcnt++;
   v = x.v;
   return *this;
}

ostream& operator<<(ostream& ostr, const year_month_vector& t)
{
  const int i1 = t.indexmin();
  const int i2 = t.indexmax();
  for (int i = i1; i <= i2; i++)
  {
     ostr << " " << t(i);
  }
  return(ostr);
}
