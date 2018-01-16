//$Id: coff_t.h 2754 2011-01-02 20:57:07Z jsibert $
#ifndef __COFF_T_H__
#define __COFF_T_H__
#include <fvar.hpp>
#include "par_t.h"
#include "yr_mon2.h"
#include "intersav.h"
#include "bound.h"

#include <pthread.h>
#include <semaphore.h>
//#include "thread_extensions.h"

/*
template<class T,class TV> struct CONJUGATE_GRADIENTS_HELPER{
      DEFORMABLE_OBJECT<T,TV>* deformable_object;
      int partition_id;
      T time,dt;
      T alpha,beta;
      ARRAY<TV>* dX_full;
      ARRAY<double> *S_dot_Q_partial,*rho_new_partial,*supnorm_partial;
  };

*/

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE> 
struct rc_solver_args
{
public:
  int start_rc;
  int end_rc;
  coff_t< d3_array, dmatrix, dvector, double> * client;
  coff_t< D3_ARRAY, DMATRIX, DVECTOR, DOUBLE> * dfclient;
  int thread_no;
};

void thread_single_barrier(sem_t *barrier, int count);


/** Manipulation of tridiagonal coefficients for use by adi solver.
A virtual base class with specializations for upwind and centered finite
difference approximations. Uses templates to produce overloads
for both constant and variable ADMB types. 
*/
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE> 
class coff_t 
{
 public:
   static coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE> *instance;
   static coff_t < d3_array, dmatrix, dvector, double> *dfinstance;
   static coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE > * 
     createInstance(const par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE > &param);
   static coff_t < d3_array, dmatrix, dvector, double > *
     createDfInstance(const par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE > &param);
   static void deleteInstance()
   {
      if (instance != 0)
      {
	 delete instance;
	 instance = 0;
      }
      if (dfinstance != 0)
      {
	 delete dfinstance;
	 dfinstance = 0;
      }
   }


 public:
   coff_t(Model_area & param, int ipar6);
   coff_t(coff_t & t);
   virtual ~ coff_t() {;}

 public:
   virtual void comp(par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE > &param,
		     const MATRIX & u, const MATRIX & v,
		     const MATRIX & sigma) = 0;
   virtual void dfcomp(coff_t & coff, par_t < D3_ARRAY, MATRIX, VECTOR,
		       DOUBLE > &param, par_t < d3_array, dmatrix, dvector,
		       double >&dfparam, dmatrix & u, dmatrix & v,
		       dmatrix & sigma, year_month & date, dmatrix & dfu,
		       dmatrix & dfv, dmatrix & dfsigma,
		       intersavetype * savefile) = 0;
   virtual void dfcomp(coff_t & coff, par_t < D3_ARRAY, MATRIX, VECTOR,
		       DOUBLE > &param, dmatrix & u, dmatrix & v,
		       dmatrix & sigma, year_month & date, dmatrix & dfu,
		       dmatrix & dfv, dmatrix & dfsigma,
		       intersavetype * savefile, dmatrix & dcoff,
		       dmatrix & ecoff, dmatrix & fcoff, dmatrix & ybet_c,
		       dmatrix & ygam_c, dmatrix & dfacoff,
		       dmatrix & dfbcoff, dmatrix & dfccoff,
		       dmatrix & dfdcoff, dmatrix & dfecoff,
		       dmatrix & dffcoff, dmatrix & dfybet,
		       dmatrix & dfygam) = 0;

   virtual DOUBLE atype(const DOUBLE um, const DOUBLE bsig,
			const int boundary) = 0;
   virtual DOUBLE btype(const DOUBLE _uu, const DOUBLE bsig,
			const DOUBLE fsig, const double dt,
			const int boundary) = 0;
   virtual DOUBLE ctype(const DOUBLE up, const DOUBLE fsig,
			const int boundary) = 0;

   void b_adjust(const par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE > &param,
		 const MATRIX & z);
   void dfb_adjust(coff_t & coff, par_t < D3_ARRAY, MATRIX, VECTOR,
		   DOUBLE > &param, par_t < d3_array, dmatrix, dvector,
		   double >&dfparam, dmatrix & z);
   void dfb_adjust(par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE > &param,
		   dmatrix & dfz, dmatrix & a, dmatrix & b, dmatrix & c,
		   dmatrix & xbet, dmatrix & xgam, dmatrix & dfa,
		   dmatrix & dfraw_b, dmatrix & dfb, dmatrix & dfc,
		   dmatrix & dfxbet, dmatrix & dfxgam);


   void adi(MATRIX& u, intersavetype * savefile);
   void dfadi(coff_t & coff, dmatrix & u, dmatrix & dfu, intersavetype * savefile);
   void adiADMBIntersave(dvar_matrix& u, const dmatrix& psi);
   void adiADMBIntersave(dmatrix& u, const dmatrix& psi);

   friend void bogus_dfadi(void);
   void dfadi(dmatrix & a, dmatrix & dfa,
	      dmatrix & b, dmatrix & dfb,
	      dmatrix & c, dmatrix & dfc,
	      dmatrix & d, dmatrix & dfd,
	      dmatrix & e, dmatrix & dfe,
	      dmatrix & f, dmatrix & dff,
	      dmatrix & g, dmatrix & dfg,
	      dmatrix & xbet, dmatrix & dfxbet,
	      dmatrix & xgam, dmatrix & dfxgam,
	      dmatrix & ybet, dmatrix & dfybet,
	      dmatrix & ygam, dmatrix & dfygam,
	      dmatrix & psi, dmatrix & dfpsi,
	      dvector & rr, dvector & dfrr,
	      dvector & uu, dvector & dfuu,
	      dvector & work, dvector & dfwork,
	      dvector & work1, dvector & dfwork1,
	      dmatrix & u, dmatrix & dfu, intersavetype * savefile);

   //// probably not used
   void dfadi(dmatrix & a, dmatrix & dfa,
	      dmatrix & b, dmatrix & dfb,
	      dmatrix & c, dmatrix & dfc,
	      dmatrix & d, dmatrix & dfd,
	      dmatrix & e, dmatrix & dfe,
	      dmatrix & f, dmatrix & dff,
	      dmatrix & g, dmatrix & dfg,
	      dmatrix & xbet, dmatrix & dfxbet,
	      dmatrix & xgam, dmatrix & dfxgam,
	      dmatrix & ybet, dmatrix & dfybet,
	      dmatrix & ygam, dmatrix & dfygam,
	      dmatrix & psi, dmatrix & dfpsi,
	      dvector & rr, dvector & dfrr,
	      dvector & uu, dvector & dfuu,
	      dvector & work, dvector & dfwork,
	      dvector & work1, dvector & dfwork1,
	      dmatrix & u, dmatrix & dfu);
   void adi(MATRIX & u);
   void dfadi(coff_t & coff, dmatrix & u, dmatrix & dfu);
   void adi(dmatrix & u, const par_t < D3_ARRAY, MATRIX, VECTOR,
	    DOUBLE > &param);
   void adi(dvar_matrix & u, const par_t < D3_ARRAY, MATRIX, VECTOR,
	    DOUBLE > &param);
   ////////////////////////////

  static void s_column_solver(void* t);
  void column_solver(const int j1, const int j2, const int thread_no);

  static void s_dfcolumn_solver(void* t);
  void dfcolumn_solver(const int j1, const int j2, const int thread_no);

  static void s_row_solver(void* t);
  void row_solver(const int i1, const int i2, const int thread_no);

  static void s_dfrow_solver(void* t);
  void dfrow_solver(const int i1, const int i2, const int thread_no);

  int check_rc_counter(const int rc_limit); //, pthread_cond_t& condition);

  static int max_threads;
  ivector solver_args;
  static sem_t col_barrier;
  static sem_t row_barrier;
  int get_max_threads(void) const { return(max_threads); }
   ////////////////////////////
   int get_k() const { return k; }
   void clean();
   void write(ostream & s, int i, int j);

 public:
   MATRIX & get_a() { return _a; }
   MATRIX & get_b() { return _b; }
   MATRIX & get_raw_b() { return _raw_b; }
   MATRIX & get_c() { return _c; }
   MATRIX & get_d() { return _d; }
   MATRIX & get_e() { return _e; }
   MATRIX & get_f() { return _f; }
   MATRIX & get_g() { return _g; }
   MATRIX & get_xbet() { return _xbet; }
   MATRIX & get_ybet() { return _ybet; }
   MATRIX & get_xgam() { return _xgam; }
   MATRIX & get_ygam() { return _ygam; }
   MATRIX & get_psi() { return _psi; }
   VECTOR & get_rr() { return _rr; }
   VECTOR & get_uu() { return _uu; }
   VECTOR & get_work() { return _work; }
   VECTOR & get_work1() { return _work1; }

   dmatrix vGet(dmatrix& m);
   dmatrix vGet(dvar_matrix& m);
   dvector vGet(dvector& v);
   dvector vGet(dvar_vector& v);
   double vGet(double x);
   double vGet(dvariable x);

   void vSet(prevariable y, const double x);
   void vSet(double & y, const double x);

   void vSaveValue(dmatrix&,  const char *, intersavetype * );
   void vSaveValue(dvar_matrix&,  const char *, intersavetype * );
   void vGetValue(dmatrix& m, const char * id, intersavetype * savefile, 
                  dvar_matrix_position * pos = NULL);
   void vGetValue(dvar_matrix& m, const char * id, intersavetype * savefile, 
                  dvar_matrix_position * pos = NULL);
   void vGetValue(dmatrix& m, const char * id, intersavetype * savefile, 
                  dmatrix_position * pos = NULL);

 protected:
   int k;			//param.m_ipar[6] or 1

 private:
   MATRIX _a;
   MATRIX _b;
   MATRIX _raw_b;
   MATRIX _c;
   MATRIX _d;
   MATRIX _e;
   MATRIX _f;
   MATRIX _g;

   MATRIX _ybet;
   MATRIX _ygam;
   MATRIX _xbet;
   MATRIX _xgam;
   MATRIX _psi;

 protected:
   int imax;			//param.m
   int jmax;			//param.n

   ivector ilb;
   ivector iub;
   ivector jlb;
   ivector jub;

   VECTOR _rr;
   VECTOR _uu;
   VECTOR _work;
   VECTOR _work1;
};

template < typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE > 
class Upwind_coff_t:public coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >
{
 public:
   using coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::ilb;
   using coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::iub;
   using coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::jlb;
   using coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::jub;
   using coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::get_a;
   using coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::get_raw_b;
   using coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::get_b;
   using coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::get_c;
   using coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::get_d;
   using coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::get_e;
   using coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::get_f;
   using coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::get_ybet;
   using coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::get_ygam;

 public:
   Upwind_coff_t(Model_area & param, int ipar6):coff_t < D3_ARRAY, MATRIX,
      VECTOR, DOUBLE > (param, ipar6) {; }
   Upwind_coff_t(Upwind_coff_t & t);
   Upwind_coff_t(coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE > &t);
   virtual ~ Upwind_coff_t() {; }
 public:
   void comp(par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE > &param,
	     const MATRIX & u, const MATRIX & v, const MATRIX & sigma);

   void dfcomp(coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE > &coff,
	       par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE > &param,
	       par_t < d3_array, dmatrix, dvector, double >&dfparam,
	       dmatrix & u, dmatrix & v, dmatrix & sigma,
	       year_month & date, dmatrix & dfu, dmatrix & dfv,
	       dmatrix & dfsigma, intersavetype * savefile);
   void dfcomp(coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE > &coff,
	       par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE > &param,
	       dmatrix & u, dmatrix & v, dmatrix & sigma,
	       year_month & date, dmatrix & dfu, dmatrix & dfv,
	       dmatrix & dfsigma, intersavetype * savefile,
	       dmatrix & dcoff, dmatrix & ecoff, dmatrix & fcoff,
	       dmatrix & ybet_c, dmatrix & ygam_c, dmatrix & dfacoff,
	       dmatrix & dfbcoff, dmatrix & dfccoff, dmatrix & dfdcoff,
	       dmatrix & dfecoff, dmatrix & dffcoff, dmatrix & dfybet,
	       dmatrix & dfygam);

   DOUBLE atype(const DOUBLE um, const DOUBLE bsig, const int boundary);
   DOUBLE btype(const DOUBLE _uu, const DOUBLE bsig, const DOUBLE fsig,
		const double dt, const int boundary);
   DOUBLE ctype(const DOUBLE up, const DOUBLE fsig, const int boundary);

   void dfatype(double &dfacoff, double &dfum, double &dfbsig, double um,
		int boundary);
   void dfbtype(double &dfbcoff, double &dfuu, double &dfbsig,
		double &dffsig, double _uu, int boundary);
   void dfctype(double &dfccoff, double &dfup, double &dffsig, double up,
		int boundary);

   char get_scheme() const { return 'U'; }
};

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE> 
class Centered_coff_t:public coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >
{
 public:
   using coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::ilb;
   using coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::iub;
   using coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::jlb;
   using coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::jub;
   using coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::get_a;
   using coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::get_raw_b;
   using coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::get_b;
   using coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::get_c;
   using coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::get_d;
   using coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::get_e;
   using coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::get_f;
   using coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::get_ybet;
   using coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::get_ygam;

 public:
 Centered_coff_t(Model_area & param, int ipar6):coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE > (param,
						ipar6)
   {;
   }
   Centered_coff_t(Centered_coff_t & t);
   Centered_coff_t(coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE > &t);
   virtual ~ Centered_coff_t()
   {;
   }

 public:
   void comp(par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE > &param,
	     const MATRIX & u, const MATRIX & v, const MATRIX & sigma);

   void dfcomp(coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE > &coff,
	       par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE > &param,
	       par_t < d3_array, dmatrix, dvector, double >&dfparam,
	       dmatrix & u, dmatrix & v, dmatrix & sigma,
	       year_month & date, dmatrix & dfu, dmatrix & dfv,
	       dmatrix & dfsigma, intersavetype * savefile);
   void dfcomp(coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE > &coff,
	       par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE > &param,
	       dmatrix & u, dmatrix & v, dmatrix & sigma,
	       year_month & date, dmatrix & dfu, dmatrix & dfv,
	       dmatrix & dfsigma, intersavetype * savefile,
	       dmatrix & dcoff, dmatrix & ecoff, dmatrix & fcoff,
	       dmatrix & ybet_c, dmatrix & ygam_c, dmatrix & dfacoff,
	       dmatrix & dfbcoff, dmatrix & dfccoff, dmatrix & dfdcoff,
	       dmatrix & dfecoff, dmatrix & dffcoff, dmatrix & dfybet,
	       dmatrix & dfygam);

   DOUBLE atype(const DOUBLE um, const DOUBLE bsig, const int boundary);
   DOUBLE btype(const DOUBLE _uu, const DOUBLE bsig, const DOUBLE fsig,
		const double dt, const int boundary);
   DOUBLE ctype(const DOUBLE up, const DOUBLE fsig, const int boundary);

   void dfctype(double &dfccoff, double &dfup, double &dffsig, double up,
		int boundary);
   void dfbtype(double &dfbcoff, double &dfuu, double &dfbsig,
		double &dffsig, double uu, int boundary);
   void dfatype(double &dfacoff, double &dfum, double &dfbsig, double um,
		int boundary);

   char get_scheme() { return 'C'; }
};


void bogus_dfadi(void);
#endif
