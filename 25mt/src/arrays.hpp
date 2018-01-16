#ifndef ARRAYS_HPP
#define ARRAYS_HPP

//////////////////////////////////////////////////////
// Generic array class definition for use with generic.hpp

#ifdef __TURBOC__
  #include <generic.h>
  #undef name2
  #define name2 _Paste2_x
#endif
#ifdef __ZTC__
  #include <generic.hpp>
#endif
#ifdef __NDPX__
  #include <generic.h>
#endif
#ifdef __SUN__
  #include <generic.h>
#endif
#ifdef __GNUC__
  #include <generic.h>
#endif
#ifdef _MSC_VER
#include "generic.h"
#endif

#ifdef SAFE_ARRAYS
#define MEM_DEBUG 1
#define PROTOTYPING 1
#endif


/////////////////// class vector /////////////////////

#define vrep(type) name2(type,vrep)
#define vector(type) name2(type,vector)


#ifdef SAFE_ARRAYS
///////// class vector declarations with subscript checking ////////

#define vectordeclare(type) \
struct vrep(type) \
{  \
  type * p;  \
  int en1; \
  int en2; \
  int nsize; \
  int refcnt;   \
} ; \
class vector(type) \
{ \
  vrep(type) * v; \
  void allocate(); \
public: \
  friend class name2(type,matrix); \
  vector(type)(int n1, int n2); \
  vector(type)(); \
  type* base() { return v->p; } \
  type& operator[](int i); \
  vector(type)& operator=(vector(type)& x); \
 ~vector(type)(); \
  inline vector(type)(vector(type)& x) \
  { \
    x.v->refcnt++; \
    v = x.v; \
  } \
};


#else
//////// class vector definitions without subscript checking //////

#define vectordeclare(type) \
struct vrep(type) \
{  \
  type * p;  \
  type * p0; \
  int en1; \
  int en2; \
  int nsize; \
  int refcnt;   \
} ; \
class vector(type) \
{ \
  vrep(type) * v; \
  void allocate(); \
  void allocate(int n1, int n2); \
public: \
  friend class name2(type,matrix); \
  vector(type)(int n1, int n2); \
  vector(type)(); \
  type* base() { return v->p; } \
  inline type& operator[](int i) \
  { return v->p[i-v->en1]; }\
  vector(type)& operator=(vector(type)& x); \
 ~vector(type)(); \
  inline vector(type)(vector(type)& x) \
  { \
    x.v->refcnt++; \
    v = x.v; \
  } \
};
#endif // SAFE_ARRAYS for vectordelcare

/////////////////// class matrix /////////////////////
#define mrep(type) name2(type,mrep)
#define matrix(type) name2(type,matrix)

#ifdef SAFE_ARRAYS
///////// class matrix declarations with subscript checking ////////

#define matrixdeclare(type) \
struct mrep(type) \
{ \
  vector(type) ** pp; \
  int em1, em2, msize; \
  int en1, en2, nsize; \
  int refcnt; \
}; \
class matrix(type) \
{ \
  mrep(type) * mdaptr;  \
  void allocate(); \
  void new_handler(); \
public: \
  matrix(type)(int m1, int m2, int n1, int n2); \
  vector(type) & operator[](int i); \
 ~matrix(type)(); \
  inline matrix(type)(matrix(type)& x) \
  { \
  /* copy initialzer merely copies the pointer */ \
    x.mdaptr->refcnt++; \
    mdaptr = x.mdaptr; \
  }\
};

#else
///////// class matrix declarations without subscript checking ////////

#define matrixdeclare(type) \
struct mrep(type) \
{ \
  vector(type) ** pp; \
  vector(type) ** pp0; \
  int em1, em2, msize; \
  int en1, en2, nsize; \
  int refcnt; \
}; \
class matrix(type) \
{ \
  mrep(type) * mdaptr;  \
  void allocate(); \
  void new_handler(); \
public: \
  matrix(type)(int m1, int m2, int n1, int n2); \
  inline vector(type) & operator[](int i) \
    { return *mdaptr->pp[i-mdaptr->em1]; }\
 ~matrix(type)(); \
  inline matrix(type)(matrix(type)& x) \
  { \
  /* copy initialzer merely copies the pointer */ \
    x.mdaptr->refcnt++; \
    mdaptr = x.mdaptr; \
  }\
};
#endif // SAFE_ARRAYS for matrixdelcare

typedef int int_;
declare(vector, int_);
declare(matrix, int_);

typedef long long_int_;
declare(vector, long_int_);

typedef float float_;
declare(vector, float_);
declare(matrix, float_);

typedef double double_;
declare(vector, double_);
declare(matrix, double_);

typedef void * vptr_;
declare(vector, vptr_);
declare(matrix, vptr_);

#endif
#ifndef ARRAYS_HPP
#define ARRAYS_HPP

//////////////////////////////////////////////////////
// Generic array class definition for use with generic.hpp

#ifdef __TURBOC__
  #include <generic.h>
  #undef name2
  #define name2 _Paste2_x
#endif
#ifdef __ZTC__
  #include <generic.hpp>
#endif
#ifdef __NDPX__
  #include <generic.h>
#endif
#ifdef __SUN__
  #include <generic.h>
#endif
#ifdef _MSC_VER
#include "generic.h"
#endif





#ifdef SAFE_ARRAYS
#define MEM_DEBUG 1
#define PROTOTYPING 1
#endif


/////////////////// class vector /////////////////////

#define vrep(type) name2(type,vrep)
#define vector(type) name2(type,vector)


#ifdef SAFE_ARRAYS
///////// class vector declarations with subscript checking ////////

#define vectordeclare(type) \
struct vrep(type) \
{  \
  type * p;  \
  int en1; \
  int en2; \
  int nsize; \
  int refcnt;   \
} ; \
class vector(type) \
{ \
  vrep(type) * v; \
  void allocate(); \
public: \
  friend class name2(type,matrix); \
  vector(type)(int n1, int n2); \
  vector(type)(); \
  type* base() { return v->p; } \
  type& operator[](int i); \
  vector(type)& operator=(vector(type)& x); \
 ~vector(type)(); \
  inline vector(type)(vector(type)& x) \
  { \
    x.v->refcnt++; \
    v = x.v; \
  } \
};


#else
//////// class vector definitions without subscript checking //////

#define vectordeclare(type) \
struct vrep(type) \
{  \
  type * p;  \
  type * p0; \
  int en1; \
  int en2; \
  int nsize; \
  int refcnt;   \
} ; \
class vector(type) \
{ \
  vrep(type) * v; \
  void allocate(); \
  void allocate(int n1, int n2); \
public: \
  friend class name2(type,matrix); \
  vector(type)(int n1, int n2); \
  vector(type)(); \
  type* base() { return v->p; } \
  inline type& operator[](int i) \
  { return v->p[i-v->en1]; }\
  vector(type)& operator=(vector(type)& x); \
 ~vector(type)(); \
  inline vector(type)(vector(type)& x) \
  { \
    x.v->refcnt++; \
    v = x.v; \
  } \
};
#endif // SAFE_ARRAYS for vectordelcare

/////////////////// class matrix /////////////////////
#define mrep(type) name2(type,mrep)
#define matrix(type) name2(type,matrix)

#ifdef SAFE_ARRAYS
///////// class matrix declarations with subscript checking ////////

#define matrixdeclare(type) \
struct mrep(type) \
{ \
  vector(type) ** pp; \
  int em1, em2, msize; \
  int en1, en2, nsize; \
  int refcnt; \
}; \
class matrix(type) \
{ \
  mrep(type) * mdaptr;  \
  void allocate(); \
  void new_handler(); \
public: \
  matrix(type)(int m1, int m2, int n1, int n2); \
  vector(type) & operator[](int i); \
 ~matrix(type)(); \
  inline matrix(type)(matrix(type)& x) \
  { \
  /* copy initialzer merely copies the pointer */ \
    x.mdaptr->refcnt++; \
    mdaptr = x.mdaptr; \
  }\
};

#else
///////// class matrix declarations without subscript checking ////////

#define matrixdeclare(type) \
struct mrep(type) \
{ \
  vector(type) ** pp; \
  vector(type) ** pp0; \
  int em1, em2, msize; \
  int en1, en2, nsize; \
  int refcnt; \
}; \
class matrix(type) \
{ \
  mrep(type) * mdaptr;  \
  void allocate(); \
  void new_handler(); \
public: \
  matrix(type)(int m1, int m2, int n1, int n2); \
  inline vector(type)& operator[](int i) \
    { return *mdaptr->pp[i-mdaptr->em1]; }\
 ~matrix(type)(); \
  inline matrix(type)(matrix(type)& x) \
  { \
  /* copy initialzer merely copies the pointer */ \
    x.mdaptr->refcnt++; \
    mdaptr = x.mdaptr; \
  }\
};

#endif // SAFE_ARRAYS for matrixdelcare

typedef int int_;
declare(vector, int_);
declare(matrix, int_);

typedef long long_int_;
declare(vector, long_int_);

typedef float float_;
declare(vector, float_);
declare(matrix, float_);

typedef double double_;
declare(vector, double_);
declare(matrix, double_);

typedef void * vptr_;
declare(vector, vptr_);
declare(matrix, vptr_);

#endif
