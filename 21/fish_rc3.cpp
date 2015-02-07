#include <fvar.hpp>
#include <fvar.hpp>
#include <stdlib.h>
#include "fish_rec.h"
#include "trace.h"
extern ofstream clogf;


#include "arrays.cpp"
int a_move_corner(const int x, const int edge);

adstring& adstring_utils_tolower(adstring& ads);
adstring_array& adstring_utils_tolower(adstring_array& ads_array);
int adstring_utils_index(adstring_array& ads_array, const adstring& ads);

implement(vector, regional_fishery_record_);

void indexed_regional_fishery_record::allocate(void)
{
  TRACE(rfrv_length)
  fishery_records.ptr = new regional_fishery_record_vector(1,rfrv_length);

  TRACE(nfleet)
  fleet_list.allocate(1,nfleet);

  TTRACE(first_year,last_year)
  index.allocate(1,nfleet,first_year,last_year,1,12);
  index.initialize();

  nrec.allocate(1,nfleet,first_year,last_year,1,12);
  nrec.initialize();

  mean_effort.allocate(1,nfleet);
  mean_effort.initialize();
  normalized_flag = 0;
}

indexed_regional_fishery_record::
  indexed_regional_fishery_record(const char* root_name, const char* path)
{
  read(root_name, path);
}

void indexed_regional_fishery_record::read(const char* root_name,
                                           const char* path)
{
/*
#ifdef unix
  adstring file_name = "../";
#else
  adstring file_name = "..\\";
#endif
*/
  adstring file_name = path;
  file_name += adstring(root_name) + ".ifr";
  TRACE(file_name)
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
    //fleet_list.to_lower();
    adstring_utils_tolower(fleet_list);
    ss >> index;
    ss >> nrec;
    for (int i = 1; i <= rfrv_length; i++)
    {
       ss >> fishery_records[i];
    }
    cout << "Finished reading '" << file_name << "'" << endl;
    clogf << "Finished reading '" << file_name << "'" << endl;
    mean_effort_compute();
  }
}

istream& operator>>(istream& istr, regional_fishery_record& t)
{
  istr >> t.i >> t.j >> t.effort >> t.Catch;
  return(istr);
}

ostream& operator<<(ostream& ostr, regional_fishery_record& t)
{
  ostr << t.i << " " << t.j << " " << t.effort << " " << t.Catch;
  return(ostr);
}

indexed_regional_fishery_record* effort_stream::irfr_ptr=NULL;

int indexed_regional_fishery_record::
	get_effort(adstring& fleet, year_month& date, dmatrix& t)
{
  int effort_occured = 0;

  //int fl = fleet_list.index(fleet);
  int fl = adstring_utils_index(fleet_list, fleet);
  
  //TTRACE(fleet,fl)
  //TRACE(fleet_list)
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

int indexed_regional_fishery_record::
   did_effort_occur(adstring& fleet, year_month& date)
{
  int effort_occured = 0;
//  TRACE(fleet)
  //int fl = fleet_list.index(fleet);
  int fl = adstring_utils_index(fleet_list, fleet);

//  TRACE(fl)
  if (fl >= fleet_list.indexmin())
  {
//    TRACE(date)
    int year = date.get_year();
//    TRACE(year)
    if ( (year >= first_year) && (year <= last_year) )
    {
      int month = date.get_month_value();
//      TRACE(month)
      effort_occured = ( (index(fl,year,month) > 0) &&
			 (nrec(fl,year,month) > 0) );
    }
  }
  return(effort_occured);
}

int indexed_regional_fishery_record::
	get_effort(int flindex, int year, int month, dmatrix& t)

{
  t.initialize();
  int effort_occured = 0;
  int n1 = index(flindex,year,month);
  int n2 = n1+nrec(flindex,year,month)-1;
  for (int n = n1; n <= n2; n++)
  {
    regional_fishery_record fr = fishery_records(n);
    int ii = fr.i;
    int jj = fr.j;

    /* we don't need this test
    if ( (ii > t.rowmax()) || (ii < t.rowmin()) )
    {
      cerr << "\nError: longitude index " << jj
           << " incompatible with dmatrix bounds for fishery_record "
           << n << ": " << fr << endl;
      exit(1);
    }
    else if ( (jj < t(ii).indexmin()) || (jj > t(ii).indexmax()) )
    {
      cerr << "\nError: latitude index " << ii
           << " incompatible with dmatrix bounds for fishery_record "
           << n << ": " << fr << endl;
      exit(1);
    }
    else
    */
      t(ii, jj) += fr.effort;

    effort_occured++;
  }
  return effort_occured;
}


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
  //flindex = irfr_ptr->fleet_list.index(fleet);
    flindex = adstring_utils_index(irfr_ptr->fleet_list, fleet);
 
    state = (flindex < 1);

//jca 1.29: Changed "val" did not generate correct values.  Changed to "atoi".
    year = atoi((char*)name(5, 6));
    if (year > 19)
    {
      month = atoi((char*)name(7, 8));
    }
    else
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

effort_stream& effort_stream::operator >> (dmatrix& eff)
{
  if (!irfr_ptr)
    state = 2;
  else
  {
    int effort_occured = irfr_ptr->get_effort(flindex, year, month, eff);
    //TRACE(effort_occured)
    state = (effort_occured <= 0);
  }
  return *this;
}

void indexed_regional_fishery_record::normalize(void)
{
  year_month date1(first_year,1);
  year_month date2(last_year,12);
  normalize(date1,date2);
}

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
    clogf << "Effort standardiszed to " << mean_effort(flindex)
          << " for " << fleet_list(flindex) << endl;

    cout << ".";

  } // flindex loop
  cout << " done." << endl;
  normalized_flag = 1;
}

void indexed_regional_fishery_record::mean_effort_compute()
//(year_month& date1, year_month& date2)
{
  mean_effort.initialize();
  dvector incidents(1,nfleet);
  incidents.initialize();
  year_month date1(first_year,1);
  year_month date2(last_year,12);

  for (int flindex = 1; flindex <= nfleet; flindex++)
  {
    clogf << "Computing average effort for " << fleet_list(flindex) << endl;
    year_month date = date1;
    //for (year_month date = date1; date <= date2; date++)
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
  }
}
