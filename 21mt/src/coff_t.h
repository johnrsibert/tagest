#ifndef COFF_T_H
#define COFF_T_H
#include <fvar.hpp>
#include "par_t.h"
#include "yr_mon2.h"
#include "intersav.h"

#include <pthread.h>
#include <semaphore.h>
#include "thread_extensions.h"


// bits/pthreadtypes.h
// typedef unsigned long int pthread_t;

//#include "thread_pool.h"

typedef struct
{
  int start_rc;
  int end_rc;
  coff_t* client;
  int thread_no;
  //thread_pool* tpp;
} rc_solver_args_t;

void thread_single_barrier(sem_t *barrier, int count);

class coff_t
{
public:
  //coff_t.cpp
  coff_t(Model_area& param, int ipar6);
  coff_t(coff_t& t); // but not defined
  virtual ~coff_t();

  //coff_t.cpp
  void clean();
  void write(ostream& s, int i, int j);

  virtual void comp(par_t& param, year_month& date, 
                    intersavetype * savefile,
                    dmatrix& u, dmatrix& v, dmatrix& sigma) = 0;

  virtual void dfcomp(coff_t& coff, par_t& param, par_t& dfparam,
                      dmatrix& u, dmatrix& v, dmatrix& sigma,
                      year_month& date, dmatrix& dfu, dmatrix& dfv,
                      dmatrix& dfsigma, intersavetype * savefile) = 0;

  virtual void atype(double& acoff, double um, double bsig, int boundary) = 0;
  virtual void btype(double& bcoff, double _uu, double bsig, double fsig,
                     double dt, int boundary) = 0;
  virtual void ctype(double& ccoff, double up, double fsig, int boundary) = 0;

  void adi(dmatrix& u, intersavetype * savefile);
  void dfadi(coff_t& coff, dmatrix& u, dmatrix& dfu, intersavetype *savefile);

  static void s_column_solver(void* t);
  void column_solver(const int j1, const int j2, const int thread_no);

  static void s_dfcolumn_solver(void* t);
  void dfcolumn_solver(const int j1, const int j2, const int thread_no);

  static void s_row_solver(void* t);
  void row_solver(const int i1, const int i2, const int thread_no);

  static void s_dfrow_solver(void* t);
  void dfrow_solver(const int i1, const int i2, const int thread_no);

  int check_rc_counter(const int rc_limit); //, pthread_cond_t& condition);

  void b_adjust(par_t& param, dmatrix& z);
  void dfb_adjust(coff_t& coff, par_t& param, par_t& dfparam, dmatrix& z);

  int get_k() { return k; }
  //virtual char get_scheme() { return NULL; }


  dmatrix& get_a() { return a; }
  dmatrix& get_b() { return b; }
  dmatrix& get_raw_b() { return raw_b; }
  dmatrix& get_c() { return c; }
  dmatrix& get_d() { return d; }
  dmatrix& get_e() { return e; }
  dmatrix& get_f() { return f; }
  dmatrix& get_g() { return g; }

  dmatrix& get_xbet() { return xbet; }
  dmatrix& get_ybet() { return ybet; }
  dmatrix& get_xgam() { return xgam; }
  dmatrix& get_ygam() { return ygam; }
  dmatrix& get_psi()  { return psi; }
  /*
  dvector& get_rr()   { return rr; }
  dvector& get_uu()   { return uu; }
  dvector& get_work() { return work; }
  dvector& get_work1(){ return work1; }
  */
protected:
  dmatrix a;
  dmatrix b;
  dmatrix raw_b;
  dmatrix c;
  dmatrix d;
  dmatrix e;
  dmatrix f;
  dmatrix g;

  dmatrix xbet;
  dmatrix ybet;
  dmatrix xgam;
  dmatrix ygam;
  dmatrix psi;

  dmatrix _rr;
  dmatrix _uu;
  dmatrix _work;
  dmatrix _work1;

  int imax;   //param.m
  int jmax;   //param.n
  int k;      //param.m_ipar[6] or 1
 
  ivector ilb;
  ivector iub;
  ivector jlb;
  ivector jub;
  static int max_threads;

  //pthread_mutex_t rc_counter_lock;
  //pthread_mutex_t dfdata_lock;
  
  //static int rc_counter;
  ivector solver_args;
  //lvector threads;
  //pthread_t* threads;
  //pthread_t adi_thread;
  //static pthread_attr_t adi_attr;
  static sem_t col_barrier;
  static sem_t row_barrier;
  //static sched_param adi_priority;


  //int lock_dfdata(void){ return(pthread_mutex_lock(&dfdata_lock)); }
  //int unlock_dfdata(void){ return(pthread_mutex_unlock(&dfdata_lock)); }

  double r4; // computed in adi
  dmatrix* state_variable; // initialized in adi AND dfadi

  // pointer to constant version of coff_t instance for use in df..
  coff_t* const_pointer; // initialized in adi AND dfadi

};

void tridag(dvector& a, dvector& r, dvector& u, dvector& bet, dvector& gam,
      int nl, int nu, dvector& v);

void dftridag(dvector& a, dvector& r,
    dvector& bet, dvector& gam,
    dvector& dfa, dvector& dfr,
    dvector& dfbet, dvector& dfgam,
    dvector& u, dvector& dfu, int nl, int nu,
    dvector& v, dvector& dfv, dvector& w, dvector& dfw);


class Upwind_coff_t : public coff_t
{
public:
  Upwind_coff_t(Model_area& param, int ipar6) : coff_t(param, ipar6) { };
  Upwind_coff_t(Upwind_coff_t& t); // but not defined
  Upwind_coff_t(coff_t& t); // but not defined

  void comp(par_t& param, year_month& date, intersavetype * savefile, 
            dmatrix& u, dmatrix& v, dmatrix& sigma);

  void atype(double& acoff, double um, double bsig, int boundary);
  void btype(double& bcoff, double _uu, double bsig, double fsig, double dt,
             int boundary);
  void ctype(double& ccoff, double up, double fsig, int boundary);

  void dfcomp(coff_t& coff, par_t& param, par_t& dfparam,
              dmatrix& u, dmatrix& v, dmatrix& sigma,
              year_month& date, dmatrix& dfu, dmatrix& dfv,
              dmatrix& dfsigma, intersavetype * savefile);

  void dfatype(double& dfacoff, double& dfum, double& dfbsig, double um,
               int boundary);
  void dfbtype(double& dfbcoff, double& dfuu, double& dfbsig, double& dffsig,
               double _uu, int boundary);
  void dfctype(double& dfccoff, double& dfup, double& dffsig, double up,
               int boundary);

  char get_scheme() { return 'U'; }
  int get_max_threads(void) { return(max_threads); }
};

class Centered_coff_t : public coff_t
{
public:
  Centered_coff_t(Model_area& param, int ipar6) : coff_t(param, ipar6) { };
  Centered_coff_t(Centered_coff_t& t); // but not defined
  Centered_coff_t(coff_t& t); // but not defined

  //coffcmp4.cpp
  void comp(par_t& param, year_month& date, intersavetype * savefile,
            dmatrix& u, dmatrix& v, dmatrix& sigma);

  virtual void dfcomp(coff_t& coff, par_t& param, par_t& dfparam,
                      dmatrix& u, dmatrix& v, dmatrix& sigma,
                      year_month& date, dmatrix& dfu, dmatrix& dfv,
                      dmatrix& dfsigma, intersavetype * savefile) 
  {
    cout << "Warning: Centered_coff_t::dfcomp called" << endl;
  }

  //coffcmp4.cpp
  void atype(double& acoff, double um, double bsig, int boundary);
  void btype(double& bcoff, double _uu, double bsig, double fsig, double dt,
             int boundary);
  void ctype(double& ccoff, double up, double fsig, int boundary);

  char get_scheme() { return 'C'; }
};

#endif // COFF_T_H
