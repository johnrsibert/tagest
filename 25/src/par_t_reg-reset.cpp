//$Id: par_t_reg-reset.cpp 3205 2014-08-19 00:50:00Z eunjung $
#include "par_t_reg.h"
#include "trace.h"

//static int par_t_reg_reset_count = 0;

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::
reset(const dvar_vector &q, const dvariable &mort, const dvariable &special_mort, const dvar_vector &report_rates, const dvar_vector& nb_par, const dvar_matrix &seasonGridU, const dvar_matrix &seasonGridV, const dvar_matrix &seasonGridSigma , const dvar_vector& min_D , 
const dvar_vector& logslope , const dvar_vector& inflection, const dvar_vector& loggamma, 
const dvariable& chi) //, const dvariable& loggamma)//const dvariable &sus_spd)
{
  //clogf << "/npar_t_reg_reset_count: " << ++par_t_reg_reset_count << endl;
/*
  int     ii = 0;
  if (m_ipar[24] > 0)
  {
    for (int i = 1; i <= num_afm; i++)
    {
      ii++;
      //x(ii) = boundpin(afm(i),minafm,maxafm);
     //name(ii) = "afm("+str(i)+")";
     }
  }
*/

  if(m_ipar[21] == 1)
  {
      this->usergridSigma(1,1) = seasonGridSigma(1,1);
  } 
  else if (m_ipar[21] == 2)
  {
      for (int k=1; k <= ngrid; k++)
      {
          for (int s=1; s <= nseason; s++)
          {
              if (stratum_active(s, k))
             {
                 this->usergridSigma(s,k) = seasonGridSigma(s,k);
              }
          }
      }
  }

  if(m_ipar[22] == 1)
  {
      this->mort = mort;
      if(m_ipar[19] > 0)
      {
        this->special_mort = special_mort;
      }
  }

  //uniform q
  if(m_ipar[25] == 1)
  {
      this->q(1) = q(1);
      for (int f = 2; f <= get_nfleet(); f++)
      {
          this->q(f) = q(1);
      }
  }

  //separate q for each fleet
  if(m_ipar[25] == 2)
  {
      for (int f = 1; f <= get_nfleet(); f++)
      {
          this->q(f) = q(f);
      }
  }

  if(m_ipar[28] == 1)
  {
      //separate report_rates for each fleet
      for (int f = 1; f <= nfleet; f++)
      {
          this->report_rates[f] = report_rates[f];
      }
  }

  if (m_ipar[23]==1)
  {
      for (int k = 1; k <= ngrid; k++)
      {
          for (int s = 1; s <= nseason; s++)
          {
             if (stratum_active(s, k))
            {
              this->usergridU(s,k) = seasonGridU(s,k);
              this->usergridV(s,k) = seasonGridV(s,k);
            }
          }
      }
  }

  if (m_ipar[11] == 4 || m_ipar[11] == 14)
  {
    this->nb_par(1) = nb_par(1);
    if (m_ipar[11] == 14)
    {
      this->nb_par(2) = nb_par(2);
    }
  } 

  if  (m_ipar[11]==24)
  {
    for (int f = 1; f <= nfleet; f++)
    {
      this->nb_par(f) = nb_par(f);
    }
  }
  //if (m_ipar[85] == 1)
  //{
  //    for (int k=1; k <= ngrid; k++)
  //    {
  //        if (fadmap_stratum_active_d(k))
  //        {
  //               this->usergridSigma(s,k) = seasonGridSigma(k);
}
template<> void par_t_reg<d3_array,dmatrix,dvector,double>::
reset(const dvar_vector &q, const dvariable &mort, const dvariable &special_mort, const dvar_vector &report_rates,
      const dvar_vector& nb_par, const dvar_matrix &seasonGridU, const dvar_matrix &seasonGridV, const dvar_matrix &seasonGridSigma,
      const dvar_vector& min_D, 
      const dvar_vector& logslope, const dvar_vector& inflection, const dvar_vector& loggamma, 
      const dvariable& chi) {HERE}//, const dvariable& loggamma)  {HERE}//const dvariable &sus_spd)  

template void par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>::
reset(const dvar_vector &q, const dvariable &mort, const dvariable &special_mort, const dvar_vector &report_rates,
const dvar_vector& nb_par, const dvar_matrix &seasonGridU, const dvar_matrix &seasonGridV, const dvar_matrix &seasonGridSigma,
const dvar_vector& min_D, 
const dvar_vector& logslope, const dvar_vector& inflection, const dvar_vector& loggamma,  
const dvariable& chi); //, const dvariable& loggamma);//const dvariable &sus_spd);

