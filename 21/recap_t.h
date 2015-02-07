#ifndef __recap_t_h__
#define __recap_t_h__
#include "yr_mon2.h"

class cifstream;
class Model_area;

class recaptype

{
private:
  //static int cohort_pooling_flag;
public:
  char  fleet[5];
  int   flindex;
  year_month  date;
  int   cohort;
  int   i;
  int   j;
  int   returns;
  /*
  static int cohort_pooling(void)
  {
    return(cohort_pooling_flag);
  }
  static void set_cohort_pooling(const int t)
  {
    cohort_pooling_flag = t;
  }
  */
  void write_on(ostream& s, Model_area& m);

  void read_from(istream& s, Model_area& m);

};

typedef recaptype recaptype_;

struct recaptype_vrep
{
  recaptype_ * p;
  int en1;
  int en2;
  int nsize;
  int refcnt;
};

class recaptype_vector
{
  protected:
    recaptype_vrep * v;
    void allocate();
    int cohort_pooling_flag;
    double total_recaps;

  public: 
    friend class recaptype_matrix;
    recaptype_vector(int n1, int n2);
    recaptype_vector();
    recaptype_* base() { return v->p; }
    recaptype_& recaptype_vector::operator[](int i);
    recaptype_vector& recaptype_vector::operator=(recaptype_vector& x);
    ~recaptype_vector();
    inline recaptype_vector(recaptype_vector& x)
    {
      x.v->refcnt++;
      v = x.v;
    }
    void set_cohort_pooling(const int t) { cohort_pooling_flag = t; }
    int cohort_pooling(void) const { return(cohort_pooling_flag); }
    void set_total_recaps(const double t) { total_recaps = t; }
    double get_total_recaps(void) const { return(total_recaps); }
};

ostream& operator << (ostream& s, recaptype_& t);
int recaptype_compare(recaptype_& a, recaptype_& b);
int recaptype_compare(const void* a, const void* b);
void recaptype_sort(recaptype_vector& v, const int n);
int operator == (recaptype_& a, recaptype_& b);
recaptype_& operator += (recaptype_& a, recaptype_& b);

#endif  // __recap_t_h__
