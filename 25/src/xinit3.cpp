//$Id: xinit3.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "par_t_reg.h"
#include "trace.h"

extern double min_nb_par;
extern double max_nb_par;

extern ofstream clogf;

namespace movemod {
	double boundpin(const double x, const double fmin, const double fmax);
}
template<> void par_t_reg<d3_array,dmatrix,dvector,double>::xinit(dvector& x, adstring_array& name)
{
	using movemod::boundpin;

  int     ii = 0, s, k;
  imatrix& s_a = stratum_active;

  if (m_ipar[24] > 0)
  {
    for (int i = 1; i <= num_afm; i++)
    {
      ii++;
      x(ii) = boundpin(afm(i),minafm,maxafm);
     name(ii) = "afm("+str(i)+")";
     }
  }

  if(m_ipar[21] == 1)
  {
      ii ++;
      x[ii] = boundpin(usergridSigma[1][1], minsig, maxsig);
      name(ii) = "sigma";
  }

  if(m_ipar[21] == 2)
  {
      for (k=1; k <= ngrid; k++)
      {
          for (s=1; s <= nseason; s++)
          {
              if (s_a(s, k))
             {
                  ii++;
                 x[ii] = boundpin(usergridSigma[s][k], minsig, maxsig);
                 name(ii) = "D("+str(s)+","+str(k)+")";
              }
          }
      }
  }

  if(m_ipar[22] == 1)
  {
      ii++;
      x[ii] = boundpin(mort, minmort, maxmort);
      name(ii) = "M";
      if(m_ipar[19] > 0)
      {
        ii++;
        x[ii] = boundpin(special_mort, minmort, maxmort);
        name(ii) = "M("+str((int)m_ipar[19])+")";
      }
  }

  //uniform q
  if(m_ipar[25] == 1)
  {
      ii++;
      x[ii] = boundpin(q(1), minq, maxq);
      name(ii) = "q";
  }

  //separate q for each fleet
  if(m_ipar[25] == 2)
  {
      for (int f = 1; f <= get_nfleet(); f++)
      {
          ii++;
          x[ii] = boundpin(q(f), minq, maxq);
          name(ii) = "q("+str(f)+")";
      }
  }

  //clogf << "***I'm in xinit" << endl;
  if(m_ipar[28] == 1)
  {
      //separate report_rates for each fleet
      for (int f = 1; f <= nfleet; f++)
      {
          ii++;
          x[ii] = boundpin(report_rates[f], minr_rates, maxr_rates);
          clogf << "*->p.rr::" << report_rates[f] << endl;
          clogf << "x[ii]::" << x[ii] << endl;
          name(ii) = "rr("+str(f)+")";
      }
  }

  if (m_ipar[23]==1)
  {
      for (k = 1; k <= ngrid; k++)
      {
          for (s = 1; s <= nseason; s++)
          {
             if (s_a(s, k))
            {
              ii++;
              x[ii] = boundpin(usergridU[s][k], minvel, maxvel);
              name(ii) = "u("+str(s)+","+str(k)+")";

              ii++;
              x[ii] = boundpin(usergridV[s][k], minvel, maxvel);
              name(ii) = "v("+str(s)+","+str(k)+")";
            }
          }
      }
  }

  if ( (m_ipar[11]==4) || (m_ipar[11]==14) )
  {
    ii++;
    x[ii] = boundpin(nb_par(1),min_nb_par,max_nb_par);
    name(ii) = "NB-par(1)";
    if (m_ipar[11]==14)
    {
      ii++;
      x[ii] = boundpin(nb_par(2),min_nb_par,max_nb_par);
      name(ii) = "NB-par(2)";
    }
  } 

  if  (m_ipar[11]==24)
  {
    for (int f = 1; f <= nfleet; f++)
    {
      ii++;
      x[ii] = boundpin(nb_par(f),min_nb_par,max_nb_par);
      name(ii) = "NB-par("+str(f)+")";
    }
  }
}
template<> void par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>::xinit(dvector& x, adstring_array& name) {}
