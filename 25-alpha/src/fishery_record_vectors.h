//$Id: fishery_record_vectors.h 3067 2013-04-03 20:00:34Z jsibert $
/** \file
Class definitions for various fishery record vectors.
*/

#ifndef  __fishery_record_vectors_h__
#define  __fishery_record_vectors_h__

class fishery_record;
class regional_fishery_record;

typedef fishery_record fishery_record_;
struct fishery_record_vrep
{
   fishery_record_ *p;
   int en1;
   int en2;
   int nsize;
   int refcnt;
};

class fishery_record_vector
{
 private:
   fishery_record_vrep *v;
   void allocate(void);
 public: 
   void allocate(const int n1, const int n2);
   fishery_record_vector(const int n1, const int n2);
   fishery_record_vector();
   fishery_record_ *base()
   {
      return v->p;
   }
   fishery_record_ & operator[] (const int i);
   fishery_record_ & operator() (const int i);
   fishery_record_vector & operator=(const fishery_record_vector & x);
   ~fishery_record_vector();
   inline fishery_record_vector(const fishery_record_vector & x)
   {
      x.v->refcnt++;
      v = x.v;
   }
};

void sort(fishery_record_vector & v, const int n);
double total_effort(fishery_record_vector& v, const int n);
double total_catch(fishery_record_vector& v, const int n);


typedef regional_fishery_record regional_fishery_record_;
struct regional_fishery_record_vrep
{
   regional_fishery_record_ *p;
   int en1;
   int en2;
   int nsize;
   int refcnt;
};

class regional_fishery_record_vector
{
   regional_fishery_record_vrep *v;
   void allocate(void);
 public:
   void allocate(const int n1, const int n2);
   regional_fishery_record_vector(const int n1, const int n2);
   regional_fishery_record_vector();
   regional_fishery_record_ *base()
   {
      return v->p;
   }
   regional_fishery_record_ & operator[] (const int i);
   regional_fishery_record_ & operator() (const int i);
   regional_fishery_record_vector &
      operator=(const regional_fishery_record_vector & x);
   ~regional_fishery_record_vector();
   inline regional_fishery_record_vector(regional_fishery_record_vector & x)
   {
      x.v->refcnt++;
      v = x.v;
   }
};
/*
class rfr_ptr
{
  regional_fishery_record_vector* ptr;
public:
  friend class indexed_regional_fishery_record;
  regional_fishery_record& operator[](const int i) { return (*ptr)[i];}
  regional_fishery_record& operator()(const int i) { return (*ptr)[i];}
};
*/
#endif //ifndef  __fishery_record_vectors_h__
