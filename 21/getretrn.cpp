#include "precomp2.h"
#include <fstream.h>
#include <fvar.hpp>
#include "yr_mon2.h"
#include "par_t.h"
#include "recap_t.h"

extern ofstream clogf;

#define LOGHERE clogf << "reached " << __LINE__ << " in " << __FILE__ << endl;

void getreturns(dmatrix& returns, int fleet, year_month& date, int cohort,
		recaptype_vector& recaps, int nrec, int m, int n)
{
  int      i, j, k;

  returns.initialize();

  for (k=1; k <= nrec; k++)
  {
      //clogf << "k = " << k << endl;
      //if ( (recaps[k].month == month) && (recaps[k].cohort == cohort) )
      //year_month tdate = recaps[k].date;
      //adstring tfleet = recaps[k].fleet;
      //clogf << "comparing " << recaps[k].flindex << " and " << fleet << endl
      //      << "          " << recaps[k].date<< " and " <<  date << endl
      //      << "          " << recaps[k].cohort << " and " << cohort << endl;


      if ( (cohort == 0) || (cohort == recaps[k].cohort) )
      {
          if ( (recaps[k].flindex == fleet) && (recaps[k].date == date) )
          {
	      //matches ++;
	      // index screening moved to readcaps
	      i = recaps[k].i;
	      j = recaps[k].j;
	      returns[i][j] += recaps[k].returns;
	      //tot_caps += recaps[k].returns;
          }
      }
  }
  //clogf << "Found " << matches << " matches for cohort " << cohort
  //		    << ", fleet " << fleet
  //		    << " on " << date << endl;
  //clogf << "  Total returns = " << tot_caps << endl;
} /* end of getreturns */
