#include <stdlib.h>
#ifdef __TURBOC__
  #include <iostream.h>
  #include <alloc.h>
#endif

#ifdef __ZTC__
  #include <iostream.hpp>
#endif

#ifdef __NDPX__
  #include <iostream.h>
#endif

//#pragma implementation "arrays.hpp"
#include "arrays.hpp"


#ifdef SAFE_ARRAYS // for vectorimplement
//////////// class vector safe member functions /////////////

#define vectorimplement(type) \
vector(type)::vector(type)(int n1, int n2) \
{ \
  v = (vrep(type)*) malloc( (size_t)sizeof(vrep(type)) ); \
  v->en1 = n1; \
  v->en2 = n2; \
  v->nsize = n2-n1+1; \
  v->refcnt = 1; \
  allocate (); \
} \
void vector(type)::allocate(void) \
{ \
  v->p = (type*) malloc( (size_t)v->nsize*sizeof(type) ); \
  if (!v->p) \
  { \
     cerr << "New failed in vector allocation.\n"; \
     exit(1); \
  } \
} \
vector(type)::~vector(type)() \
{ \
  if (--v->refcnt==0) \
  { \
    free(v->p); \
    free(v); \
  } \
} \
type& vector(type)::operator[](int i) \
{ \
  if  ( (i < v->en1) || (i > v->en2) ) \
  { \
    cerr << "subscript out of range in vector\n"; \
    cerr << "i="<< i << ", n1=" << v->en1 <<", n2="<<v->en2<<"\n";\
    exit(1); \
  } \
  return v->p[i-v->en1]; \
} \
vector(type)& vector(type)::operator=(vector(type)& x) \
{ /* simple pointer assignment only */ \
  if ( (x.v->en1 != v->en1) || (x.v->en2 != v->en2) ) \
  { \
    cerr << "Attempted assignment to vectors of unequal size.\n"; \
    exit(1); \
  } \
  x.v->refcnt++; \
  v = x.v; \
  return *this; \
}

#else

//////////// class vector non-safe member functions /////////////

#define vectorimplement(type) \
vector(type)::vector(type)(int n1, int n2) \
{ \
  v = (vrep(type)*) malloc( (size_t)sizeof(vrep(type)) ); \
  v->en1 = n1; \
  v->en2 = n2; \
  v->nsize = n2-n1+1; \
  v->refcnt = 1; \
  allocate (); \
} \
void vector(type)::allocate(void) \
{ \
  v->p = (type*) malloc( (size_t)v->nsize*sizeof(type) ); \
  v->p0 = v->p - v->en1; \
  if (!v->p) \
  { \
     cerr << "New failed in vector allocation.\n"; \
     exit(1); \
  } \
} \
vector(type)::~vector(type)() \
{ \
  if (--v->refcnt==0) \
  { \
    free(v->p); \
    free(v); \
  } \
} \
vector(type)& vector(type)::operator=(vector(type)& x) \
{ /* simple pointer assignment only */ \
  if ( (x.v->en1 != v->en1) || (x.v->en2 != v->en2) ) \
  { \
    cerr << "Attempted assignment to vectors of unequal size.\n"; \
    exit(1); \
  } \
  x.v->refcnt++; \
  v = x.v; \
  return *this; \
}

#endif // SAFE_ARRAYS for vectorimplement

#ifdef SAFE_ARRAYS
//////////// class matrix safe member functions /////////////

#define matriximplement(type)\
void matrix(type)::new_handler() \
{ \
  cerr << "Operator new failed.\n"; \
  cerr << "Attempting to allocate (" <<  \
                 mdaptr->em1 << ".." << mdaptr->em2 << "," << \
                 mdaptr->en1 << ".." << mdaptr->en2 << ")\n."; \
  exit(1); \
} \
matrix(type)::matrix(type)(int m1, int m2, int n1, int n2) \
{ \
  /* mdaptr = new mrep(type); */\
  mdaptr = (mrep(type)*) malloc( (size_t)sizeof(mrep(type)) ); \
  mdaptr->em1 = m1; \
  mdaptr->em2 = m2; \
  mdaptr->msize = m2 - m1 + 1; \
  mdaptr->en1 = n1; \
  mdaptr->en2 = n2; \
  mdaptr->nsize = n2 - n1 + 1; \
  mdaptr->refcnt = 1; \
  allocate(); \
} \
void matrix(type)::allocate(void) \
{ \
  int en1 = mdaptr->en1; \
  int en2 = mdaptr->en2; \
  /* mdaptr->pp = new vector(type)*[mdaptr->msize]; */ \
  mdaptr->pp = (vector(type)**) malloc( (size_t)mdaptr->msize*sizeof(vector(type))); \
  if (! mdaptr->pp) new_handler(); \
  int len = mdaptr->msize; \
  for (int i = 0; i < len; i++) \
  { \
    mdaptr->pp[i] = new vector(type)(en1, en2); \
  } \
} \
vector(type)& matrix(type)::operator[](int i) \
{ \
  if ( (i < mdaptr->em1) || (i > mdaptr->em2) ) \
  { \
    cerr << "subscript out of range for column (vector *) in matrix\n"; \
    exit(1); \
  } \
  return *mdaptr->pp[i-mdaptr->em1]; \
} \
matrix(type)::~matrix(type)() \
{ \
  if (--mdaptr->refcnt == 0) \
  { \
    int len = mdaptr->msize; \
    for (int i = 0; i < len; i++) \
    { \
      delete mdaptr->pp[i]; \
    }   \
    free(mdaptr->pp);  \
    free(mdaptr); \
  } \
}

#else
//////////// class matrix non-safe member functions /////////////

#define matriximplement(type)\
void matrix(type)::new_handler() \
{ \
  cerr << "Operator new failed.\n"; \
  cerr << "Attempting to allocate (" <<  \
                 mdaptr->em1 << ".." << mdaptr->em2 << "," << \
                 mdaptr->en1 << ".." << mdaptr->en2 << ")\n."; \
  exit(1); \
} \
matrix(type)::matrix(type)(int m1, int m2, int n1, int n2) \
{ \
  /* mdaptr = new mrep(type); */\
  mdaptr = (mrep(type)*) malloc( (size_t)sizeof(mrep(type)) ); \
  mdaptr->em1 = m1; \
  mdaptr->em2 = m2; \
  mdaptr->msize = m2 - m1 + 1; \
  mdaptr->en1 = n1; \
  mdaptr->en2 = n2; \
  mdaptr->nsize = n2 - n1 + 1; \
  mdaptr->refcnt = 1; \
  allocate(); \
} \
void matrix(type)::allocate(void) \
{ \
  int en1 = mdaptr->en1; \
  int en2 = mdaptr->en2; \
  /* mdaptr->pp = new vector(type)*[mdaptr->msize]; */ \
  mdaptr->pp = (vector(type)**) malloc( (size_t)mdaptr->msize*sizeof(vector(type))); \
  mdaptr->pp0 = mdaptr->pp - mdaptr->em1; \
  if (! mdaptr->pp) new_handler(); \
  int len = mdaptr->msize; \
  for (int i = 0; i < len; i++) \
  { \
    mdaptr->pp[i] = new vector(type)(en1, en2); \
  } \
} \
matrix(type)::~matrix(type)() \
{ \
  if (--mdaptr->refcnt == 0) \
  { \
    int len = mdaptr->msize; \
    for (int i = 0; i < len; i++) \
    { \
      delete mdaptr->pp[i]; \
    } \
    free(mdaptr->pp);  \
    free(mdaptr); \
  } \
}

#endif // SAFE_ARRAYS for matrix implement

