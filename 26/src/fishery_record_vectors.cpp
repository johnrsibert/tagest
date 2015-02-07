//$Id: fishery_record_vectors.cpp 3069 2013-04-03 20:37:09Z jsibert $
/** \file
Souce code for various fishery record vectors.
*/


#include "fish_rec.h"

fishery_record_vector::fishery_record_vector(void)
{
   v =  NULL;
}

fishery_record_vector::fishery_record_vector(const int n1, const int n2)
{
   allocate(n1,n2);
}

void fishery_record_vector::allocate(const int n1, const int n2)
{
   v = (fishery_record_vrep *) malloc((size_t) sizeof(fishery_record_vrep));
   v->en1 = n1;
   v->en2 = n2;
   v->nsize = n2 - n1 + 1;
   v->refcnt = 1;
   allocate();
}

void fishery_record_vector::allocate(void)
{
   v->p = (fishery_record_ *) malloc((size_t) v->nsize * sizeof(fishery_record_));
   if (!v->p)
   {
      cerr << "New failed in void fishery_record_vector::allocate(void).\n";
      exit(1);
   }
}

fishery_record_vector::~fishery_record_vector()
{
   if (--v->refcnt == 0)
   {
      free(v->p);
      free(v);
   }
}

fishery_record_ & fishery_record_vector::operator[](const int i)
{
#ifndef OPT_LIB
   if ((i < v->en1) || (i > v->en2))
   {
      cerr << "subscript out of range in fishery_record_vector::operator[](const int i)\n";
      cerr << "i=" << i << ", n1=" << v->en1 << ", n2=" << v->en2 << "\n";
      exit(1);
   }
#endif
   return v->p[i - v->en1];
}

fishery_record_ & fishery_record_vector::operator()(const int i)
{
  return (*this)[i];
}

fishery_record_vector & fishery_record_vector:: operator=(const fishery_record_vector & x)
{
   if ((x.v->en1 != v->en1) || (x.v->en2 != v->en2))
   {
      cerr << "Attempted assignment to fishery_record_vectors of unequal size.\n";
      exit(1);
   }
   x.v->refcnt++;
   v = x.v;
   return *this;
};

regional_fishery_record_vector::regional_fishery_record_vector(void)
{
   v = NULL;
}

regional_fishery_record_vector::regional_fishery_record_vector(int n1, int n2)
{
   allocate(n1,n2);
}

void regional_fishery_record_vector::allocate(int n1, int n2)
{
   v = (regional_fishery_record_vrep *) malloc((size_t) sizeof (regional_fishery_record_vrep));
   v->en1 = n1;
   v->en2 = n2;
   v->nsize = n2 - n1 + 1;
   v->refcnt = 1;
   allocate();
}
void regional_fishery_record_vector::allocate(void)
{
   v->p = (regional_fishery_record_ *) malloc((size_t) v->nsize * sizeof (regional_fishery_record_));
   if (!v->p)
   {
      cerr << "New failed in void regional_fishery_record_vector::allocate(void).\n";
      exit(1);
   }
}

regional_fishery_record_vector::~regional_fishery_record_vector()
{
   if (--v->refcnt == 0)
   {
      free(v->p);
      free(v);
   }
}

regional_fishery_record_ & regional_fishery_record_vector::operator[](const int i)
{
#ifndef OPT_LIB
   if ((i < v->en1) || (i > v->en2))
   {
      cerr << "subscript out of range in regional_fishery_record_vector::operator[](const int i)\n";
      cerr << "i=" << i << ", n1=" << v->en1 << ", n2=" << v->en2 << "\n";
      exit(1);
   }
#endif
   return v->p[i - v->en1];
}

regional_fishery_record_ & regional_fishery_record_vector::operator()(const int i)
{
  return (*this)[i];
}

regional_fishery_record_vector & regional_fishery_record_vector::operator=(const  regional_fishery_record_vector & x)
{
   if ((x.v->en1 != v->en1) || (x.v->en2 != v->en2))
   {
      cerr << "Attempted assignment to regional_fishery_record_vectors of unequal size.\n";
      exit(1);
   }
   x.v->refcnt++;
   v = x.v;
   return *this;
};
