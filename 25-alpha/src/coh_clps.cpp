//$Id: coh_clps.cpp 3201 2014-08-06 01:47:04Z jsibert $
#include "par_t.h"
#include "recap_t.h"
#include "trace.h"

template < typename D3_ARRAY, typename MATRIX, typename VECTOR,
   typename DOUBLE > int par_t < D3_ARRAY, MATRIX, VECTOR,
   DOUBLE >::collapse_cohorts(recaptype_vector & recaps, const int nrec)
{
   int ncohort = tr(nrelease).cohort;
   ivector new_cohort_number(1, ncohort);
   new_cohort_number.initialize();

   int k = 0;			// index into tag release vector
   int cohort_number = 0;

   while (k < nrelease)
   {
      k++;
      int cc = tr(k).cohort;
      cohort_number++;

      tr(k).cohort = cohort_number;

      new_cohort_number(cc) = cohort_number;

      // loop through releases with the same date
      while ((k < nrelease) && (tr[k].date == tr[k + 1].date))
      {
	 k++;
	 cc = tr(k).cohort;
	 new_cohort_number(cc) = cohort_number;
	 tr[k].cohort = cohort_number;
      }
   }

   cout << "finished aggregation of release cohorts by date." << endl;
   clogf << "\nfinished aggregation of release cohorts by date." << endl;

   int last_cohort = 0;
   clogf << "\nnew cohort numbers:" << endl;
   for (k = 1; k <= ncohort; k++)
   {
      clogf << setw(5) << k << setw(5) << new_cohort_number(k) << endl;
      if (new_cohort_number(k) > 0)
	 last_cohort = new_cohort_number(k);
   }

   // loop through recapture vector and update to the crecapture record
   // with the aggregated cohort number
   for (int k = 1; k <= nrec; k++)
   {
      int cc = recaps[k].cohort;
      recaps[k].cohort = new_cohort_number(cc);
   }
   cout << "finished renumbering recaputure cohorts; last_cohort = " << last_cohort << endl;
   clogf << "finished renumbering recaputure cohorts; last_cohort = " << last_cohort << endl;
   /*
   clogf << "\nAfter cohort reumbering, recaptures by cohort and fleet:" << endl;
   imatrix fleet_recaps(0, last_cohort, 0, nfleet);
   fleet_recaps.initialize();
   ivector effortless_recaps(0,last_cohort);
   effortless_recaps.initialize();
   int tcaps = 0;
   for (int k = 1; k <= nrec; k++)
   {
      const int cc = recaps[k].cohort;
      const int ff = recaps[k].par_flindex;
      const int eflag = recaps[k].effort_flag; // eflag is either 0 or 1
      const int returns = recaps[k].returns;

      if (ff == 0)
      {
         if (eflag == 1)
            fleet_recaps(cc, ff) += returns;
         else
            effortless_recaps(cc) += returns;
      }
      else
         fleet_recaps(cc, ff) += returns;
   }

   clogf << "cohort";
   for (int ff = fleet0; ff <= nfleet; ff++)
      clogf << "  " << fleet_names(ff);

   clogf << "  " << "sum" << "  "<< "effortless" << endl;

   for (int cc = 0; cc <= last_cohort; cc++)
   {
      clogf << setw(6) << cc;
      for (int ff = 0; ff <= nfleet; ff++)
	 clogf << setw(6) << fleet_recaps(cc, ff);

      clogf << setw(6) << effortless_recaps(cc);
      clogf << setw(6) << sum(fleet_recaps(cc));
      clogf << endl;
   }
   double ssum = 0.0;
   clogf << "   sum";
   for (int ff = 0; ff <= nfleet; ff++)
   {
      clogf << setw(6) << colsum(fleet_recaps, ff);
      ssum += colsum(fleet_recaps,ff);
   }
   clogf << setw(6) << ssum << setw(6) << sum(effortless_recaps) <<  endl;
   clogf << setw(6) << (sum(fleet_recaps)-colsum(fleet_recaps,0)) << setw(6) << (sum(fleet_recaps)+sum(effortless_recaps)) << endl;
  
   clogf << "\nProportion of recaptures by cohort and fleet:" << endl;
   for (int cc = 1; cc <= last_cohort; cc++)
   {
      dvector fleet_recaps_prop(0, nfleet);
      fleet_recaps_prop = fleet_recaps(cc)/(1e-8+sum(fleet_recaps(cc)));
      clogf << setw(6) << cc << setw(6) << setprecision(2) << fleet_recaps_prop << endl;
   }
   */
   /*
   clogf << "\nRecaptures by cohort and date:" << endl;
   year_month first_date = recaps[1].date;
   year_month last_date = recaps[nrec].date;
   int ndate = last_date-first_date + 1;
   imatrix date_recaps(0, ndate, 0, nfleet);
   date_recaps.initialize();
   */

   return (last_cohort);
}

template int par_t < d3_array, dmatrix, dvector,
   double >::collapse_cohorts(recaptype_vector & recaps, const int nrec);
template int par_t < dvar3_array, dvar_matrix, dvar_vector,
   dvariable >::collapse_cohorts(recaptype_vector & recaps,
				 const int nrec);
