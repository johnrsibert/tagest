//$Id: linbcg.h 2754 2011-01-02 20:57:07Z jsibert $
#ifndef __linbcg_h__
#define __linbcg_h__

#include <fvar.hpp>
#include <adstring.hpp>
#include "par_t.h"
#include "bound.h"
#include "dbmatrix.h"
#include "intersav.h"

class linbcg
{
public://Constructors and Destructors
  linbcg(par_t<d3_array,dmatrix,dvector,double>& _param);
  linbcg(const ivector& _DiaLoc, const int _n);
  ~linbcg();

public://Methods
  /////////////////////////////////////////////////////////////////////
  //Input: non-Null u v sigma movement parameter and param
  //Process: determine a b c d e f using input u v sigma to build dbmA_
  //  Note: since mortality occurs monthly the main diagonal of dbmA_
  //        needs to be saved 
  //Output: none of parameters are changed but dbmA_ filled with new values
  //Note: the coefficients are updated seasonally
  void comp(par_t<d3_array,dmatrix,dvector,double>& param, dmatrix& u, dmatrix& v, dmatrix& sigma);

  /////////////////////////////////////////////////////////////////////
  //Input: non-Null mortality matrix
  //Process: lineate _dmMort and add to dvRaw_b then subsitute to main
  //         diagonal of dbmA_
  //Output: dbmA_ main diagonal is changed
  void b_adjust(const dmatrix& _dmMort);

  /////////////////////////////////////////////////////////////////////
  //Input: non-Null release matrix _dmB
  //Process: lineate _dmB into a single vector then solve
  //         dbmA_(m*n x m*n) * dvX(1 x m*n) = dvB(1 x m*n)
  //         then setting dvB = dvX;
  //Note: the matrix is lineated from 1,1 to 1,m then 2,1 to 2,m ....
  //Output: _dmB is filled with new release values for time + 1
  //Note: if the tags are not conserving try to make tol in linbcg2.c
  //      smaller.
  void solver(dmatrix& _dmB, const int _maxnstep);

  // johns
  void g_comp(par_t<d3_array,dmatrix,dvector,double>& param, const dmatrix& recruitment);
  void set_tol(const double t) { tol = t; }
  const double get_tol(void) const { return(tol); }

private://Methods
  void linbcg2(dbMatrix& A, dvector& x, dvector& b);

  // from coff_t.h
  void atype(double& acoff, double um, double bsig, int boundary);
  void btype(double& bcoff, double _uu, double bsig, double fsig, double dt,
             int boundary);
  void ctype(double& ccoff, double up, double fsig, int boundary);

private://Members
  dbMatrix    dbmA_; //coefficients of heat equation
  dvector  dvRaw_b_; //saved main diagonal of dbmA_
  int            n_;
  int            m_;

  // johns additions
  dvector g_;        // right hand side of equation (dt scaled recruitment)
  double tol;
};//End linbcg
#endif
