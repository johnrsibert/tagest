#include "par_treg.h"

int par_t_reg::get_season(year_month& date)
{
  int moy = date.get_month_value();
  //cout << "\ndate: " << date << endl;
  return(seasons(moy));
}

void par_t_reg::set_seasons()
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
        int p = present_month.get_value();
        seasons[p] = s;
        ++present_month;
      }
    }//for s

    for (int j = 1; j <= final_season_length; j++)
    {
      int p = present_month.get_value();
      seasons[p] = nseason;
      ++present_month;
    }
  }//else
}//End set_seasons
