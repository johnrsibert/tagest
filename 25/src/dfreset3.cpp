//$Id: dfreset3.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "par_t_reg.h"
#include "trace.h"
extern ofstream clogf;
const extern double min_nb_par;
const extern double max_nb_par;

namespace movemod {
	double dfboundp(const double x, const double fmin, const double fmax, double &dfpen);
}
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::dfreset(dvector& g, const dvector& x)
{
	using movemod::dfboundp;

  int     ii = 0;
  double gpen=0;

  if (m_ipar[24] > 0)
  {
    for (int i = 1; i <= num_afm; i++)
    {
      ii++;
      //afm(i) = boundp(x(ii),minafm,maxafm,fpen);
      g(ii) += afm(i)*dfboundp(x(ii),minafm,maxafm,gpen);
      g(ii) += gpen;
    }
  }

  if(m_ipar[21] == 1)
  {
    ii ++;
    for (int k=1; k <= ngrid; k++)
    {
      for (int s=1; s<=nseason; s++)
      {
	     if (stratum_active(s,k))
	     {
	       //param.usergrid[s][k].sigma = dfatan1(x[ii],minsig,maxsig,fpen);
	       g[ii] += usergridSigma[s][k]*dfboundp(x[ii],minsig,maxsig, gpen);
	       g[ii] += gpen;
    	   }
      }
    }
  }

  if(m_ipar[21] == 2)
  {
    for (int k=1; k <= ngrid; k++)
    {
      for (int s=1; s<=nseason; s++)
      {
	     if (stratum_active(s,k))
	     {
	       ii ++;
	       //param.usergrid[s][k].sigma = dfatan1(x[ii],minsig,maxsig,fpen);
	       g[ii] += usergridSigma[s][k]*dfboundp(x[ii],minsig,maxsig, gpen);
	       g[ii] += gpen;
	     }
      }
    }
  }


  if(m_ipar[22] == 1)
  {
    ii++;
  //param.mort = boundp(x[ii],minmort,maxmort,fpen);
    g[ii] += mort*dfboundp(x[ii],minmort,maxmort, gpen);
    g[ii] += gpen;
    if(m_ipar[19] > 0)
    {
      ii++;
    //param.special_mort = boundp(x[ii],minmort,maxmort,fpen);
      g[ii] += special_mort*dfboundp(x[ii],minmort,maxmort, gpen);
      g[ii] += gpen;
    }
  }

  if(m_ipar[25] == 1)
  {
    ii++;
    for (int f = 1; f <= nfleet; f++)
    {
      //param.q[f] = boundp(x[ii],minq,maxq,fpen);
      g[ii] += q[f]*dfboundp(x[ii],minq,maxq, gpen);
      g[ii] += gpen;
    }
  }

  if(m_ipar[25] == 2)
  {
    for (int f = 1; f <= nfleet; f++)
    {
      ii++;
      //param.q[f] = boundp(x[ii],minq,maxq,fpen);
      g[ii] += q[f]*dfboundp(x[ii],minq,maxq, gpen);
      g[ii] += gpen;
    }
  }

  //report_rates
  if(m_ipar[28] == 1)
  {
    //clogf << "***dfreset" << endl;
    for (int f = 1; f <= nfleet; f++)
    {
      ii++;
      //param.report_rates[f] = boundp(x[ii],minr_rates,maxr_rates,fpen);
      TRACE(x[ii])
      g[ii] += report_rates[f]*dfboundp(x[ii],minr_rates,maxr_rates, gpen);
      g[ii] += gpen;
      //clogf << "*->rr:" << report_rates[f] << endl;
    }
  }

  if(m_ipar[23] == 1)
  {
    for (int k=1; k <= ngrid; k++)
    {
      for (int s=1; s <= nseason; s++)
      {
	if (stratum_active(s,k))
	{
	  ii++;
/*        ug[s][k].u = x[ii]; */
	  g[ii] += usergridU[s][k]*
	  dfboundp(x[ii],minvel,maxvel,gpen);
	  g[ii] += gpen;

	  ii++;
/*        ug[s][k].v = x[ii]; */
	  g[ii] += usergridV[s][k]*
	  dfboundp(x[ii],minvel,maxvel,gpen);
	  g[ii] += gpen;
	}
      }
    }
  }

  if ( (m_ipar[11]==4) || (m_ipar[11]==14) )
  {
    ii++;
    //nb_par(1) = boundp(x[ii],min_nb_par,max_nb_par,fpen);
    g[ii] += nb_par(1)*dfboundp(x[ii],min_nb_par,max_nb_par,gpen);
    g[ii] += gpen;
    if (m_ipar[11]==14)
    {
      ii++;
      //nb_par(2) = boundp(x[ii],min_nb_par,max_nb_par,fpen);
      g[ii] += nb_par(2)*dfboundp(x[ii],min_nb_par,max_nb_par,gpen);
      g[ii] += gpen;
    }
  }

  if  (m_ipar[11]==24)
  {
    for (int f = 1; f <= nfleet; f++)
    {
      ii++;
      //nb_par(f) = boundp(x[ii],min_nb_par,max_nb_par,fpen);
      g[ii] += nb_par(f)*dfboundp(x[ii],min_nb_par,max_nb_par,gpen);
      g[ii] += gpen;
    }
  }
}
template void par_t_reg<d3_array,dmatrix,dvector,double>::dfreset(dvector& g, const dvector& x);
template<> void par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>::dfreset(dvector& g, const dvector& x) {}
