#include "precomp2.h"
#include <fstream>
#include <fvar.hpp>
#include "yr_mon2.h"
#include "par_t.h"
#include "recap_t.h"
#include "trace.h"

extern ofstream clogf;
void getreturns(dmatrix& returns, int fleet, year_month& date, int cohort,
		recaptype_vector& recaps, int nrec, int m, int n)
{
  int      i, j, k;
  recaptype recap;
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


      //recap = recaps[k]; // minimize calls to [] operator
      if ( (cohort == 0) || (cohort == recaps[k].cohort) )
      //if ( (cohort == 0) || (cohort == recap.cohort) )
      {
          if ( (recaps[k].flindex == fleet) && (recaps[k].date == date) )
          //if ( (recap.flindex == fleet) && (recap.date == date) )
          {
	      //matches ++;
	      // index screening moved to readcaps
	      i = recaps[k].i;
	      j = recaps[k].j;
	      returns[i][j] += recaps[k].returns;
	      //tot_caps += recaps[k].returns;
              /*
	      i = recap.i;
	      j = recap.j;
	      returns[i][j] += recap.returns;
              */
          }
      }
  }
  //clogf << "Found " << matches << " matches for cohort " << cohort
  //		    << ", fleet " << fleet
  //		    << " on " << date << endl;
  //clogf << "  Total returns = " << tot_caps << endl;
} // end of getreturns



// this works pretty good for the forward loop

void getreturns(d3_array& returns, year_month& date, int cohort, 
                recaptype_vector& recaps, int nrec)
{
  clogf << "\ngetreturns looking for cohort " << cohort
        <<  " for " << date << endl;
  static int kstart = INT_MAX;
  TRACE(kstart)

  returns.initialize();
	

  if (kstart >= nrec)
     kstart = 1;

  recaptype tcap;

  int k = kstart;
  tcap = recaps[k];
  //`while ( ((cohort == 0) || (cohort == tcap.cohort)) 
  while ( (cohort == tcap.cohort) && (tcap.date==date)
          && (k<=nrec)) 
  {
    TTRACE(k,tcap) 
    returns(tcap.flindex,tcap.i,tcap.j) += tcap.returns;
    tcap = recaps[++k];
  }
  //TRACE(sum(returns))
  kstart = k;
  TRACE(kstart)
}
