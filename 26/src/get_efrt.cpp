//$Id: get_efrt.cpp 2768 2011-02-18 02:49:17Z jsibert $
#include <fvar.hpp>
#include "fish_rec.h"
#include "par_t.h"
#include "recap_t.h"
#include "trace.h"

void get_effort_array(par_t& param, indexed_regional_fishery_record& irfr, 
                      d3_array& effort, year_month& date, ivector& effort_occured)
{
  effort.initialize();
  effort_occured.initialize();

  for (int fl = 1; fl <= param.get_nfleet(); fl++)
  {
     // use fleet names to retrieve effort from irfr
     effort_occured(fl) = irfr.get_effort(param.fleet_names(fl), date, effort(fl));
  }
}



void get_average_effort_array(par_t& param, indexed_regional_fishery_record& irfr, d3_array& effort, year_month& date, ivector& effort_occured)
{
   year_month adate(1,date.get_month_value());
   get_effort_array(param, irfr, effort, adate, effort_occured);
}

void set_effort_to_recaps_and_mean(d3_array& effort, year_month& date, 
     indexed_regional_fishery_record& irfr, 
     const recaptype_vector& recaps, int recap_recs, ivector& effort_occured,
     const int ipar52)
{
  const year_month_vector& last_effort_date = irfr.get_last_effort_date();
  const int f1 = 1; //effort.slicemin();
  const int f2 = effort.slicemax();
  for (int f = f1; f <= f2; f++)
  {
     if ( date > last_effort_date(f) )
     {
        const dvector& mean_effort = irfr.get_mean_effort();
        for (int k=1; k <= recap_recs; k++)
        {
           int i = 0;
           int j = 0;
           if ( (recaps[k].par_flindex == f) && (recaps[k].date == date) )
           {
	      i = recaps[k].i;
	      j = recaps[k].j;
           }	
           if ( (i > 0) && (j > 0) )
           {
             if (ipar52 == 1)
                effort(f,i,j) =  1.0;
             else
                effort(f,i,j) =  mean_effort(f);
             effort_occured(f) ++;
           }
        }
     }
  }
}

