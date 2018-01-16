//$Id: nn_nvarcalc.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "par_t_nn.h"
#include "trace.h"

extern ofstream clogf;

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
int par_t_nn<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::nvarcalc()
{
  int     ii = 0;

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


  // count slope & offset scaling parameters
   if( m_ipar[71] == 1 )
    {
     ii ++; //u_slope
    }

   if( m_ipar[72] == 1)
   {
      ii ++;  //u_offest
   }

   if( m_ipar[73] == 1 )
    {
     ii ++; //v_slope
    }

   if( m_ipar[74] == 1)
   {
      ii ++;  //v_offest
   }

   if (m_ipar[21] == 1)
     { ii++; } //uniform_sigma
   else
   {
     if( m_ipar[75] == 1)
     {
        ii ++;  // sig_slope
     }

     if( m_ipar[76] == 1 )
     {
        ii ++; //sig_offset
     }
  }

 TRACE(ii)

 //count and weights and bias parame in each neural_network(s)
 for(int s = 1; s <= num_nn; s++)
 {
     TRACE(ii)
     if(nn(s).nn_ipar(5))
     {
       ii += size_count(nn(s).get_weights());
     }
 }

 //TRACE(ii)
for(int s = 1; s <= num_nn; s++)
 {
   TRACE(ii)
    if(nn(s).nn_ipar(6))
    ii += size_count(nn(s).get_bias());
 }

 TRACE(ii);
 return(ii);
}
template int par_t_nn<d3_array,dmatrix,dvector,double>::nvarcalc();
template int par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>::nvarcalc();
