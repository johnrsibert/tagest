//$Id: coh_clps.cpp 2823 2011-06-03 23:44:38Z jsibert $
#include "par_t.h"
#include "recap_t.h"
#include "trace.h"

void par_t::collapse_cohorts(recaptype_vector& recaps, const int nrec)
{
  int last_cohort = tr(nrelease).cohort;
  ivector new_cohort_number(1,last_cohort);
  new_cohort_number.initialize();

  int k = 0;
  int cohort_number = 0;

  //clogf << "\nRelease cohort numbers:" << endl;
  while (k < nrelease)
  {
     k++;
     int cc = tr(k).cohort;
     cohort_number ++;

     //clogf << "\nk(outer) = " << k << endl;
     //clogf << "old" << tr(k) << endl;
     tr(k).cohort = cohort_number;

     //clogf << "new" << tr(k) << endl;
     new_cohort_number(cc) = cohort_number;
     while ( (k < nrelease) && (tr[k].date == tr[k+1].date) )
     {
       k++;
       cc = tr(k).cohort;
       new_cohort_number(cc) = cohort_number;
       //clogf << "\nk(inner) = " << k << endl;
       //clogf << "old" << tr(k) << endl;
       tr[k].cohort = cohort_number;
       //clogf << "new" << tr(k) << endl;
     }
  }
  cout << "finished aggregation of release cohorts by date." << endl;
  clogf << "\nfinished aggregation of release cohorts by date." << endl;
  
  clogf << "\nnew cohort numbers:" << endl;
  for (k = 1; k <= last_cohort; k++)
  {
    clogf << setw(5) << k << setw(5) << new_cohort_number(k) << endl;
  }

  //clogf << "\nRecapture cohort numbers:" << endl;
  for (k = 1; k <= nrec;  k++)
  {
    int cc = recaps[k].cohort;
    //clogf << "\nk = " << k << endl;
    //clogf << "old" << recaps[k] << endl;
    recaps[k].cohort = new_cohort_number(cc);
    //clogf << "new" << recaps[k] << endl;
  }
  cout << "finished renumbering recaputure cohorts." << endl;
  clogf << "finished renumbering recaputure cohorts." << endl;
}
