//$Id: getretrn.cpp 3201 2014-08-06 01:47:04Z jsibert $
#include <fvar.hpp>
#include "yr_mon2.h"
#include "par_t.h"
#include "recap_t.h"
#include "trace.h"
adstring& adstring_utils_tolower(adstring& ads);
adstring_array& adstring_utils_tolower(adstring_array& ads_array);
adstring& adstring_utils_toupper(adstring& ads);
adstring_array& adstring_utils_toupper(adstring_array& ads_array);
int adstring_utils_index(adstring_array& ads_array, const adstring& ads);

/**
\param returns
\param fleet
\param date 
\param cohort
\param recaps,
\param nrec
\param m
\param n
\deprecated Superceded by void recaptype_vector::getreturns(...)
Referenced in fish_rc4.o: In function `indexed_regional_fishery_record::set_effort_to_recaps(adstring&, year_month&, recaptype_vector&, int, par_t<d3_array, dmatrix, dvector, double>&)':
*/
void getreturns(dmatrix& returns, const int fleet, const year_month& date, 
                const int cohort, const recaptype_vector& recaps,
                const int nrec, const int m, const int n)
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
          if ( (recaps[k].par_flindex == fleet) && (recaps[k].date == date) )
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
} // end of getreturns

/*
\param returns
\param date
\param cohort
\param recaps
\param  nrec
\deprecated Probably not used anywhere.

void getreturns(d3_array& returns, const year_month& date, const int cohort, 
                const recaptype_vector& recaps, const int nrec)
{
  returns.initialize();

  for (int k=1; k <= nrec; k++)
  {
     if ( (recaps[k].par_flindex > 0) && (recaps[k].date == date) && 
         ((cohort == 0) || (cohort == recaps[k].cohort)) )
     {
         const int f = recaps[k].par_flindex;
	 const int i = recaps[k].i;
	 const int j = recaps[k].j;
	 returns(f,i,j) += recaps[k].returns;
      }
  }
}
*/

/** Gets tag returns for given cohort and date. 
Returns d3_array objects the layers of which are indexed by fleet and
which contain a matrix on the model domain of tag returns.
\param returns1 On return contains tag returns 
for which fishing effort is available.
Slice '0' contains the sum over all fleets.
\param returns0 On return contains tag returns 
for which fishing effort is NOT available.
Slice '0' contains the sum over all fleets.
\param date year_month object containing the date
for which the returns are requested. 
\param cohort int contining the tag release cohort number
for which the returns are requested. 
*/
void recaptype_vector::getreturns(d3_array& obs_caps, 
                            const year_month& date, const int cohort) 
                //const recaptype_vector& recaps, const int nrec)
                    
{
  obs_caps.initialize();
  int done = 0;

  //TTRACE(current_position,nrec)
  int k = current_position;
  //TTRACE(date,cohort)
  do
  {
    recaptype recaps = (*this)[k];
    //TTRACE(recaps.date,recaps.cohort)
    if (recaps.date < date)
    {
      k ++;
    }
    else if (recaps.date == date)
    {
      if ( (cohort == 0) || (cohort == recaps.cohort) )
      { 
        // f = 0 => fleet included in ifr but not in par
        // interpret as effortless recapture
        int f = recaps.par_flindex; 
        const int i = recaps.i;
        const int j = recaps.j;
        // eflag = 0 => no effort reported in ifr for this recapture
        // interpret as effortless recapture
        const int eflag = recaps.effort_flag;
        const int returns = recaps.returns;
        /*
        if (date == year_month(2006,9))
        {
           TRACE(recaps)
           TTRACE(eflag,f)
           TTRACE(returns,obs_caps(f,i,j))
        }
        */
        // add effortless recaptures to excluded fleet
        if ( (eflag == 0) && (f > 0) )
           f = 0;

        obs_caps(f,i,j) +=  returns;
      } 
      k ++;
    }
    else
    {
      if (k <= nrec)
      {
        done = 1;
        current_position = k;
      }
      else
      {
         done = 1;
         current_position = 1;
      }
    }
  } while (done == 0 && k < nrec);
  //TTRACE(current_position,nrec)
  //TTRACE(sum(returns1),sum(returns0))
} 
