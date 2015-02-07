//$Id: ifr_avrg.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include <fvar.hpp>
//#include <iomanip.h>
#include "fish_rec.h"
#include "trace.h"

extern ofstream clogf;

indexed_regional_fishery_record::
 indexed_regional_fishery_record(indexed_regional_fishery_record& t)
 : fleet_list(t.fleet_list), index(t.index), nrec(t.nrec),
   fishery_records(t.fishery_records),
   mean_effort(t.mean_effort),
   nfleet(t.nfleet), first_year(t.first_year), last_year(t.last_year),
   rfrv_length(t.rfrv_length), normalized_flag(t.normalized_flag)
 {
   /*
   HERE
   fishery_records.ptr = new regional_fishery_record_vector(1,rfrv_length);
   for (int i = 1; i <= rfrv_length; i++)
   {
     fishery_records(i).i = t.fishery_records(i).i;
     fishery_records(i).j = t.fishery_records(i).j;
     fishery_records(i).effort = t.fishery_records(i).effort;
     fishery_records(i).Catch = t.fishery_records(i).Catch;
   }
   */
   //HERE
 }


//void indexed_regional_fishery_record::tabulate(ostream& sout)
indexed_regional_fishery_record
  monthly_average_ifr(const char* root_name, const int m, const int n,
                      const char* path)
{
  indexed_regional_fishery_record ifr(root_name, path);
  int afrvl = ifr.rfrv_length;
  //TTRACE(afrvl,ifr.rfrv_length)

  indexed_regional_fishery_record average_ifr;
  average_ifr.nfleet = ifr.nfleet;
  average_ifr.first_year = 1;
  average_ifr.last_year = 1;
  average_ifr.rfrv_length = afrvl;
  average_ifr.allocate();
  //average_ifr.fleet_list = ifr.fleet_list;

  dmatrix average_effort(1,m,1,n);
  dmatrix monthly_effort(1,m,1,n);

  int idx = 0;
  int nr = 1;
  int nrr = 0;

  for (int fl = 1; fl <= ifr.nfleet; fl++)
  {
    average_ifr.fleet_list(fl) = ifr.fleet_list(fl);
    for (int mo = 1; mo <= 12; mo++)
    {
      average_effort.initialize();
      int count = 0;
      for (int yr = ifr.first_year; yr <= ifr.last_year; yr++)
      {
        int effort_occured = ifr.get_effort(fl,yr,mo,monthly_effort);
        if (effort_occured)
        {
          count ++;
          average_effort += monthly_effort;
        }
      } // yr loop
      if (count)
      {
        average_effort /= (double)count;
        idx += nr;
        nr = 0;
        for (int i = 1; i <= m; i++)
        {
          for (int j = 1; j <= n; j++)
          {
            if (average_effort(i,j) > 0.0)
            {
              nr ++;
              nrr ++;
              average_ifr.fishery_records[nrr].set3(i,j,average_effort(i,j),0.0);
              if (nrr >= afrvl)
              {
                cerr << "need to increase afrvl" << endl;
                cerr << "nr = " << nrr << "; afrvl = " << afrvl << endl;
                exit(1);
              }
            }
          }
        }
      } // if count
      average_ifr.nrec(fl,1,mo) = nr;
      average_ifr.index(fl,1,mo) = idx;
      cout << average_ifr.fleet_list(fl) << setw(3) << mo << setw(4) << count
           << setw(6) << idx << setw(6) << nr << endl;
    } // mo loop
  } // fl loop
  cout << ifr.rfrv_length << " records in ifr" << endl;
  //TTRACE(nrr,average_ifr.rfrv_length )
  average_ifr.rfrv_length = nrr;
  //TTRACE(nrr,average_ifr.rfrv_length )
  cout << average_ifr.rfrv_length << " records in average ifr" << endl;
  cout << nrr << " records in average ifr" << endl;
  //average_ifr.write("test");
  return(average_ifr);
} // end of average


int regional_fishery_record::set3(const int _i, const int _j,
                                  const double eff, const double ctch)
{
  i = _i;
  j = _j;
  effort = eff;
  Catch = ctch;
  return(0);
}

#ifdef __IFR_AVRG_TEST__
void indexed_regional_fishery_record::write(const char* root_name)
{
  adstring file_name = adstring(root_name) + ".ifr";
  ofstream ss(file_name);
  ss <<  nfleet << "  " << first_year << "  " << last_year
		<< "  " << rfrv_length << endl;
  cout <<"saved size info" << endl;
  ss << fleet_list << endl;
  cout << "saved fleet list" << endl;
  ss << index << endl;
  cout << "saved index matrix" << endl;
  ss << nrec << endl;
  cout << "saved nrec matrix" << endl;
  for (int i = 1; i <= rfrv_length; i++)
  {
     ss << fishery_records[i] << endl;
  }
  cout << "saved indexed fishery records" << endl;
  cout << "indexed_regional_fishery_record data structure saved to file "
	<< file_name << endl;
  clogf << "indexed_regional_fishery_record data structure saved to file "
	<< file_name << endl;
}
ofstream clogf;
int main(int argc, char ** argv)
{
  if (argc <= 1)
  {
    cerr << "Enter ifr file root name on command line." << endl;
    exit(1);
  }

  clogf.open("ifr_avrg.log");
  adstring file_name(argv[1]);
  //file_name += ".ifr";
  cout << "Averaging IFR file for " << file_name << endl;

  int m = 60;
  int n = 35;
  indexed_regional_fishery_record irfr = monthly_average_ifr (file_name, m, n);

  cout << "Averaging average IFR file for " << file_name << endl;
  irfr.tabulate();
  irfr.tabulate(clogf);

  exit(0);
} // end of main

#endif //ifdef __IFR_AVRG_TEST__

