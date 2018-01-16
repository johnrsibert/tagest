#include "precomp2.h"
#include "par_treg.h"
extern ofstream clogf;

#define TRACE(object) clogf << "line " << __LINE__ << ", file " << __FILE__ << ", " << #object " = " << object << endl; 


/* counts the number of */
int par_t_reg::nvarcalc()
{
  int     ii, s, k;
  ii = 0;
  if (m_ipar[24] > 0)
  {
    for (int i = 1; i <= num_afm; i++)
    {
      ii++;
      //x(ii) = BOUNDPIN(afm(i),minafm,maxafm);
     }
  }


  if(m_ipar[21] == 1) ii ++;
/*  x[ii] = sigma; */

  if(m_ipar(21) == 2)
  {
    for (k=1; k <= ngrid; k++)
    {
      for (s=1; s<=nseason; s++)
      {
        if (stratum_active(s, k))
        {
          ii++;
	  //x[ii] = dftinv(p.usergrid[s][k].sigma, bds.minsig, bds.maxsig);
	  //name(ii) = "sigma("+str(s)+","+str(k)+")";
         }
      }
    }
  }


  if(m_ipar[22] == 1)
  {
    ii++;
/*  x[ii] = mort; */
    if(m_ipar[19] > 0)
    {
      ii ++;
//    x[ii] = special_mort;
    }
  }
  
  if(m_ipar[25] == 1) ii++;
/*  x[ii] = q; */

  if(m_ipar[25] == 2) ii+= nfleet;

/*  x[ii] = report_rates; */
  if(m_ipar[28] == 1) ii+= nfleet;

  if (m_ipar[23]==1)
  {
    for (k=1; k <= ngrid; k++)
    {
      for (s=1; s<=nseason; s++)
      {
//	if ( (m_ipar[26] == 0) || (region_active[k]) )
        if (stratum_active(s, k))
	{
	  ii++;
	  //x[ii] = dftinv(p.usergrid[s][k].u, bds.minvel, bds.maxvel);
	  //name(ii) = "u("+str(s)+","+str(k)+")";
	  ii++;
	  //x[ii] = dftinv(p.usergrid[s][k].v, bds.minvel, bds.maxvel);
	  //name(ii) = "v("+str(s)+","+str(k)+")";
	}
      }
    }
  }

  if ( (m_ipar[11]==4) || (m_ipar[11]==14) )
  {
    ii++;
    if (m_ipar[11]==14)
    {
      ii++; 
    }
  }

  if (m_ipar[11]==24)
  {
    ii += nfleet;
  }
  return ii;
} /* end of nvarcalc */

