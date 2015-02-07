#include "precomp2.h"
#include "par_t_nn.h"
#include "trace.h"

extern ofstream clogf;

int par_t_nn::nvarcalc()
{
  int     ii = 0, s, k;

  if (m_ipar[24] > 0)
  {
    for (int i = 1; i <= num_afm; i++)
    {
      ii++;
    }
  }

  if(m_ipar[22] == 1)
  {
      ii++;
      if(m_ipar[19] > 0)
      {
        ii++;
      }
  }

  //uniform q
  if(m_ipar[25] == 1)
  {
      ii++;
  }

  //separate q for each fleet
  if(m_ipar[25] == 2)
  {
      for (int f = 1; f <= get_nfleet(); f++)
      {
          ii++;
      }
  }

  if(m_ipar[28] == 1)
  {
      //separate report_rates for each fleet
      for (int f = 1; f <= nfleet; f++)
      {
          ii++;
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

  if  (m_ipar[11]==24)
  {
    for (int f = 1; f <= nfleet; f++)
    {
      ii++;
    }
  }


  TRACE(ii)
  int nlayer = nn.get_nlayer();
  ivector nrow = nn.get_nrow();
  ivector ncol = nn.get_ncol();

  if( m_ipar[149] == 1)
  {
    // count weights
    for (int i = 1; i < nlayer; i++)
    {
      for (int j = 1; j <= nrow(i); j ++)
      {
        for (int k = 1; k <= ncol(i); k++)
        {
          ii++;
        }
      }
    }
  } //if( m_ipar[150] == 1)
  TRACE(ii)


  if( m_ipar[150] ==1)
  {
    // count biases
    for (int i = 1; i < nlayer; i++)
    {
      ii ++;
    }
  }
  TRACE(ii);

    if( m_ipar[143] == 1 )
  {
   // count slope & offset scaling parameters
   ii ++; //u_slope
  }

 if( m_ipar[144] == 1)
 {
    ii ++;  //u_offest
 }

  if( m_ipar[145] == 1 )
  {
    ii ++;  //v_slope
  }

 if( m_ipar[146] == 1 )
 {
    ii ++; //v_offset
 }


 if( m_ipar[147] == 1)
 {
    ii ++;  // sig_slope
 }


 if( m_ipar[148] == 1 )
 {
    ii ++; //sig_offset
 }

  TRACE(ii);
  return(ii);
}//nvarcalc
