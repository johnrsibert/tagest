//$Id: fish_rc3.cpp 3066 2013-04-03 19:54:07Z jsibert $
/** \file
Source code for indexed_regional_fishery_record class.
*/
#include <fvar.hpp>
#include <fvar.hpp>
#include <stdlib.h>
#include "fish_rec.h"
#include "trace.h"
#include <iomanip>
extern ofstream clogf;


adstring& adstring_utils_tolower(adstring& ads);
adstring_array& adstring_utils_tolower(adstring_array& ads_array);
adstring& adstring_utils_toupper(adstring& ads);
adstring_array& adstring_utils_toupper(adstring_array& ads_array);
int adstring_utils_index(adstring_array& ads_array, const adstring& ads);

int a_move_corner(const int x, const int edge);

void indexed_regional_fishery_record::allocate(void)
{
  /*
  if (fishery_records.ptr == NULL)
  {
    fishery_records.ptr = new regional_fishery_record_vector(1,rfrv_length);
  }
  */
  fishery_records.allocate(1,rfrv_length);
  fleet_list.allocate(1,nfleet);
  fleet_usage_table.allocate(1,nfleet);

  index.allocate(1,nfleet,first_year,last_year,1,12);
  index.initialize();

  nrec.allocate(1,nfleet,first_year,last_year,1,12);
  nrec.initialize();
  first_effort_date.allocate(1,nfleet);
  last_effort_date.allocate(1,nfleet);
  mean_effort.allocate(1,nfleet);
  mean_effort.initialize();
  normalized_flag = 0;
}

indexed_regional_fishery_record::indexed_regional_fishery_record(void)
{

}


/*
indexed_regional_fishery_record::~indexed_regional_fishery_record(void)
{
   if (fishery_records.ptr)
   {
     delete fishery_records.ptr;
     fishery_records.ptr = NULL;
   }
}
*/

indexed_regional_fishery_record::
  indexed_regional_fishery_record(const char* root_name, const char* path)
{
  read(root_name, path);
}

void indexed_regional_fishery_record::read(const char* root_name,
                                           const char* path)
{

  //fishery_records.ptr = NULL;
  adstring file_name = path;
  file_name += adstring(root_name) + ".ifr";
  ifstream ss(file_name);
  if (!ss)
  {
    cerr << "Error opening '" << file_name << "' in function"
	 <<  "  'void indexed_regional_fishery_record::read(const char* root_name)'"
	 << endl;
    //exit(1);
    nfleet = 1;
    first_year = 1;
    last_year = 2;
    rfrv_length = 1;
    allocate();
    cerr << "Null indexed_regional_fishery_record object created." << endl;
  }
  else
  {
    ss >>  nfleet >> first_year >> last_year >> rfrv_length;
    allocate();
    ss >> fleet_list;
    adstring_utils_tolower(fleet_list);
    ss >> index;
    ss >> nrec;
    for (int i = 1; i <= rfrv_length; i++)
    {
       ss >> fishery_records[i];
    }
    cout << "Finished reading '" << file_name << "'" << endl;
    clogf << "Finished reading '" << file_name << "'" << endl;
    for (int flindex = 1; flindex <= nfleet; flindex++)
    {
       for (int year = first_year; year <= last_year; year ++)
       {
          for (int month = 1; month <= 12; month ++)
          {
             if (nrec(flindex,year,month) > 0)
             {               
                year_month date(year,month);
                last_effort_date(flindex) = date;
                if (first_effort_date(flindex).get_year() <= 0)
                   first_effort_date(flindex) = date;
             }
          }
       }
    }
    mean_effort_compute(1);
  }
}

istream& operator>>(istream& istr, regional_fishery_record& t)
{
  istr >> t.i >> t.j >> t.effort >> t.Catch;
  return(istr);
}

ostream& operator<<(ostream& ostr, regional_fishery_record& t)
{
  ostr << t.i << " " << t.j << " " << setprecision(9) << t.effort << " " << setprecision(9) << t.Catch;
  return(ostr);
}

/** Get fleet index. Returns the valid inded of the fleet argument
as stored in the irfr fleet list.
\param fleet adstring containing the LOWER case 4-byte fleet name.
\return int fleet index.
*/
int indexed_regional_fishery_record::flindex (const adstring& fleet)
{
   int fl = adstring_utils_index(fleet_list, fleet);
   return(fl);
}

/** Returns fishing effort by fleet and date.
\param fleet adstring containing 4 byte fleet code
\param date year_month object containing the year and month of fishing effort
\param [out] t dmatrix containing fishing effort for fleet and date;
all zeros of no effort occured
\return integer containing 1 if effort was recorded for the requested
fleet and date; other wise contains 0.
*/
int indexed_regional_fishery_record::
	get_effort(adstring& fleet, year_month& date, dmatrix& t)
{
  int effort_occured = 0;

  int fl = flindex(fleet);

  if (fl >= fleet_list.indexmin())
  {
    effort_occured = get_effort(fl, date.get_year(), date.get_month_value(),t);
  }
  else
  {
    t.initialize();
  }
  return(effort_occured);
}

/** Returns fishing effort by fleet and date.
\param flindex int containing index of fleet
\param year int containing year
\param month int containing month
\param [out] t dmatrix containing fishing effort for fleet and date;
all zeros of no effort occured
\return integer containing 1 if effort was recorded for the requested
fleet and date; other wise contains 0.
*/
int indexed_regional_fishery_record::
	get_effort(int flindex, int year, int month, dmatrix& t)

{
  t.initialize();
  int effort_occured = 0;

  //jca-2002-08-09:prevent indexing out of array bounds
  if ((index.indexmin() <= flindex && flindex <= index.indexmax())
       && (index.colmin() <= month && month <= index.colmax())
       && (index.rowmin() <= year && year <= index.rowmax())
     )
  {
    int n1 = index(flindex,year,month);
    int n2 = n1+nrec(flindex,year,month)-1;
    for (int n = n1; n <= n2; n++)
    {
      regional_fishery_record fr = fishery_records(n);
      int ii = fr.i;
      int jj = fr.j;
      t(ii, jj) += fr.effort;

      effort_occured++;
    }
  }

  return effort_occured;
}

/** Signals whether effort occured.
\param fleet adstring containing 4 byte fleet code
\param date year_month object containing the year and month of fishing effort
\return 1 if effort occured; 0 if no effort occured.
*/

int indexed_regional_fishery_record::
   did_effort_occur(adstring& fleet, year_month& date)
{
  int effort_occured = 0;
  int fl = adstring_utils_index(fleet_list, fleet);
  if (fl >= fleet_list.indexmin())
  {
    int year = date.get_year();
    if ( (year >= first_year) && (year <= last_year) )
    {
      int month = date.get_month_value();
      effort_occured = ( (index(fl,year,month) > 0) &&
			 (nrec(fl,year,month) > 0) );
    }
  }
  return(effort_occured);
}

#ifdef USE_EFFORT_STREAM
indexed_regional_fishery_record* effort_stream::irfr_ptr=NULL;
/*
int effort_stream::operator ! ()
{
  cout << "effort_stream::state = " << state << endl;
  return(state);
}    // non-zero if state failed
*/

void effort_stream::set_irfr_ptr(indexed_regional_fishery_record* ptr)
{
  if (irfr_ptr)
  {
    cerr << "effort_stream::irfr_ptr is already set" << endl;
    exit(1);
  }
  irfr_ptr = ptr;
}

effort_stream::effort_stream(const char* file_name)
{
  //cout << file_name << "***" << endl;

  state = 0;
  if (!irfr_ptr)
  {
    state = 2;
  }
  else
  {
    adstring name(file_name);
    adstring fleet = name(1, 4);
    flindex = adstring_utils_index(irfr_ptr->fleet_list, fleet);
    state = (flindex < 1);

    if (name.size() == 8)//FFFFYYMM
     {
       year = atoi((char*)name(5, 6));
       month = atoi((char*)name(7, 8));
     }
   else if (name.size() == 10)//FFFFYYYYMM
      {
        year = atoi((char*)name(5, 8));
        month = atoi((char*)name(9, 10));
      }

    if (!state)
    {
      state += (irfr_ptr->index(flindex,year,month) <=0);
      state += (irfr_ptr->nrec(flindex,year,month) <=0);
    }
  }
}

/** Retrieves effort matrix from ifr.
\param[out] eff dmatrix. On return contains fishing effort for stream.
\return reference to effort_stream
*/
effort_stream& effort_stream::operator >> (dmatrix& eff)
{
  if (!irfr_ptr)
    state = 2;
  else
  {
    int effort_occured = irfr_ptr->get_effort(flindex, year, month, eff);
    state = (effort_occured <= 0);
  }
  return *this;
}
#endif //USE_EFFORT_STREAM

/** Normaize fishing effort.
Fishing effort for each fleet is divided by the mean effort for the fleet
over all dates in the indexed_regional_fishery_record instance.
*/
void indexed_regional_fishery_record::normalize(void)
{
  year_month date1(first_year,1);
  year_month date2(last_year,12);
  normalize(date1,date2);
}

/** Normaize fishing effort.
Fishing effort for each fleet is divided by the mean effort for the fleet
over the specified time interval.
\param date1 year_month object containing the staring year/month for normalization
\param date2 year_month object containing the ending year/month for normalization
*/
void indexed_regional_fishery_record::normalize(year_month& date1,
                                                year_month& date2)
{
  cout << "Normalizing fishing effort from " << date1
       << " to " << date2 << " ";
  for (int flindex = 1; flindex <= nfleet; flindex++)
  {
    for (year_month date = date1; date <= date2; date++)
    {
        int year = date.get_year();
        int month = date.get_month_value();
        int n1 = index(flindex,year,month);
        int n2 = n1+nrec(flindex,year,month)-1;
        for (int n = n1; n <= n2; n++)
        {
          if (mean_effort(flindex) > 0)
            fishery_records(n).effort /= mean_effort(flindex);
        }
    }
    clogf << "Effort normalized to " << mean_effort(flindex)
          << " for " << fleet_list(flindex) << endl;

    cout << ".";

  } // flindex loop
  cout << " done." << endl;
  normalized_flag = 1;
}

/** Computes average effort.
 Computes average effort by fleet over all months in the ifr.
\f[
 \widehat{E_f} = \frac{\sum_{ij}E_{fij}}{N^\prime_f}
\f]
where \f$ {N^\prime_f} \f$ is the total number of grid cells in the model
domain where fishing was recorded by fleet\f$ f\f$.
\param ncell int containing the number of cells in the model domain. 
Only used to compute and "area correction" to apply to catachability
estimates for computing total fishing mortality.
*/
void indexed_regional_fishery_record::mean_effort_compute(const int ncell)
{
  mean_effort.initialize();
  dvector incidents(1,nfleet);
  incidents.initialize();
  year_month date1(first_year,1);
  year_month date2(last_year,12);
  int ifr_months = 12*(date2.get_year()-date1.get_year()+1);
  double strata = ncell*ifr_months;

  for (int flindex = 1; flindex <= nfleet; flindex++)
  {
    clogf << "Computing average effort for " << fleet_list(flindex) << endl;
    year_month date = date1;
    for (date = date1; date <= date2; date++)
    {
        int year = date.get_year();
        int month = date.get_month_value();
        int n1 = index(flindex,year,month);
        int n2 = n1+nrec(flindex,year,month)-1;
        for (int n = n1; n <= n2; n++)
        {
          mean_effort(flindex) += fishery_records(n).effort;
          incidents(flindex) ++;
        }
    }

    if (incidents(flindex)>0)
      mean_effort(flindex)/=incidents(flindex);

    clogf << "  mean effort  for " << fleet_list(flindex)
          << " is " << mean_effort(flindex)
          << ",  based on " << incidents(flindex) << " cases." << endl;
    clogf << "  area correction = " << (double(incidents(flindex))/strata) << endl;
  }
}

void indexed_regional_fishery_record::set_fleet_usage_table(const  adstring_array& pfleet_list)
{
   fleet_usage_table.initialize();
   for (int f = 1; f <= nfleet; f++)
   {
      for (int pf =1; pf <= pfleet_list.indexmax(); pf++)
      {
         if (fleet_list(f) == pfleet_list(pf))
         {
            fleet_usage_table(f) = pf;
            break;
         }
      }
      clogf << f << " " << fleet_list(f) << " -> "
                 << fleet_usage_table(f) << endl;
   }
}

