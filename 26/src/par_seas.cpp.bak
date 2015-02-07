//$Id: par_seas.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "par_t_reg.h"
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
int par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::get_season(year_month& date)
{
  const int moy = date.get_month_value();
  return(seasons(moy));
}
template int par_t_reg<d3_array,dmatrix,dvector,double>::get_season(year_month& date);
template int par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>::get_season(year_month& date);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::set_seasons()
{
  int season_length;
  int final_season_length = 12;
  Wrapping_num present_month(1, 12, startmonth[1]);

  if (nseason == 1)
  {
    for (int i = 1; i <= 12; i++)
      seasons[i] = 1;
  }
  else
  {
    for (int s = 1; s < nseason; s++)
    {
      if (startmonth[s+1] > startmonth[s])
        season_length = startmonth[s+1] - startmonth[s];
      else
        season_length = 12 - startmonth[s] + startmonth[s+1];

      final_season_length -= season_length;

      for (int j = 1; j <= season_length; j++)
      {
        int p = int(present_month.get_value());
        seasons[p] = s;
        ++present_month;
      }
    }//for s

    for (int j = 1; j <= final_season_length; j++)
    {
      int p = int(present_month.get_value());
      seasons[p] = nseason;
      ++present_month;
    }
  }//else
}
template void par_t_reg<d3_array,dmatrix,dvector,double>::set_seasons();
template void par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>::set_seasons();
