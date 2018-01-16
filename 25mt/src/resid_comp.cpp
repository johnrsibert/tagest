//$Id: resid_comp.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include <fvar.hpp>
#include "trace.h"

template <typename MATRIX1, typename MATRIX2>
void resid_comp(const MATRIX1& obs, const MATRIX2& pred, const MATRIX1& like, const int report)
{
  //clogf << "entering resid_comp with report = " << report << endl;

  static int b = 0;
  static int lbins = 22;
  static dvector lk_bins;
  static ivector lk_count;
  if (!lk_bins)
  {
    lk_bins.allocate(1,lbins);
    lk_count.allocate(1,lbins);
    lk_count.initialize();
    lk_bins(1) = 0.015625;
    for (b = 2; b <= lbins; b++)
      lk_bins(b) = lk_bins(b-1)*2.0;
    TRACE(lk_bins)
  }

  static int obins = 16;
  static int rbins = 2*obins - 1;
  static dvector resid_bins;
  static ivector resid_count;
  static dvector obs_bins;
  static ivector pred_count;
  static ivector obs_count;

  if (!resid_bins)
  {
    resid_bins.allocate(1,rbins);
    resid_count.allocate(1,rbins);  resid_count.initialize();
    obs_bins.allocate(1,obins);
    pred_count.allocate(1,obins);   pred_count.initialize();
    obs_count.allocate(1,obins);    obs_count.initialize();

    obs_bins(1) = 0.0;
    obs_bins(2) = 0.125;
    for (b = 3; b <= obins; b++)
      obs_bins(b) = obs_bins(b-1)*2.0;
    TRACE(obs_bins)

    for (b = 1; b <= obins; b++)
    {
      resid_bins(b) = -obs_bins(obins-b+1);
      resid_bins(b+obins-1) = obs_bins(b);
    }
    TRACE(resid_bins)
  } 

  if (!report)
  {
    #define OP <=
    int i1 = obs.rowmin();
    int i2 = obs.rowmax();
    //TTRACE(i1,i2)
    for (int i = i1; i <= i2; i++)
    {
      int j1 = obs(i).indexmin();
      int j2 = obs(i).indexmax();
      //TTRACE(j1,j2)
      for (int j = j1; j <= j2; j++)
      {
        //TTRACE(i,j)
      
        if (pred(i,j) > 0.0)
        {
          for (b = 1; b <= lbins; b++)
          {
            if (like(i,j) OP lk_bins(b))
              break;
          }
          lk_count(b)++;
          //HERE  
       
          for (b = 1; b <= obins; b++)
          {
            if (obs(i,j) OP obs_bins(b))
              break;
          }
          obs_count(b)++;
          //HERE
    
    
          for (b = 1; b <= obins; b++)
          {
            if (pred(i,j) OP obs_bins(b))
              break;
          }
          pred_count(b)++;
          //HERE
    
          double rr = obs(i,j) - pred(i,j);
          for (b = 1; b <= rbins; b++)
          {
            if (rr OP resid_bins(b))
              break;
          }
          resid_count(b)++;
          //HERE
        } // if (pred(i,j) > 0.0)
      }
    }
  }
  else
  {
    HERE
    clogf << "\n observed and predicted recapture frequencies" << endl;
    clogf << setw(10) << "num" << setw(10) << "fobs" << setw(10) << "fpred" << endl;
    for (b = 1; b <= obins; b++) 
    {
      clogf << setw(10) << obs_bins(b) 
            << setw(10) << obs_count(b) << setw(10) << pred_count(b) << endl;
    }

    clogf << "\n residual frequencies" << endl;
    clogf << setw(10) << "resid" << setw(10) << "freq" << endl;
    for (b = 1; b <= rbins; b++) 
    {
      clogf << setw(10) << resid_bins(b) << setw(10) << resid_count(b) << endl;
    }

    clogf << "\n likelihood frequencies" << endl;
    clogf << setw(10) << "like" << setw(10) << "freq" << endl;
    for (b = 1; b <= lbins; b++) 
    {
      clogf << setw(10) << lk_bins(b) << setw(10) << lk_count(b) << endl;
    }

  }

  //clogf << "returning from resid_comp with report = " << report << endl;
}
template void resid_comp(const dmatrix& obs, const dmatrix& pred, const dmatrix& like, const int report);
template<> void resid_comp(const dvar_matrix& obs, const dvar_matrix& pred, const dvar_matrix& like, const int report)
	{ resid_comp(value(obs), value(pred), value(like), report); }
template<> void resid_comp(const dmatrix& obs, const dvar_matrix& pred, const dmatrix& like, const int report)
	{ resid_comp(obs, value(pred), like, report); }
