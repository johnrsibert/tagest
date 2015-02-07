//$Id: fish_rc2.cpp 3066 2013-04-03 19:54:07Z jsibert $
//#include <adstring.hpp>
#include "par_t.h"
#include "fish_rec.h"
#include "list2.cpp"
#include "shift.h"
#include "trace.h"
#include "recap_t.h"

adstring& adstring_utils_tolower(adstring& ads);

const int frvl = 750000; //260000; //133600;


//void readrecaps(par_t& param, const char* root, recaptype_vector& recaps,
//                int& pnrecs, const char* path = DEFAULT_PATH);
int file_exists(const char* name);
int move_corner(const int x, const int edge);
int a_move_corner(const int i, const int edge);
int regional_fishery_record::statics_set = 0;
par_t<d3_array,dmatrix,dvector,double>* regional_fishery_record::pparam = NULL;

void regional_fishery_record::set_sw_corner(par_t<d3_array,dmatrix,dvector,double>* _param) //(Longitude& _long, Latitude& _lat)
{
  if (statics_set)
  {
    cerr << "Error: attempting to reset static members of class regional_fishery_record"
	 << endl;
    exit(1);
  }
  //sw_long = int(_long.value());
  //sw_lat = int(_lat.value());
  pparam = _param;
  statics_set = 1;
}

fishery_record& fishery_record::operator += (fishery_record& t)
{
   eff += t.eff;
   ctch += t.ctch;
   //recaps += t.recaps;
   return (*this);
}

/*
fishery_record& fishery_record::operator = (recaptype& t)
{
  strcpy(Fleet,t.fleet);
  Date = t.date;
  //int iLong = a_move_corner(t.i, regional_fishery_record::sw_long);
  Long = Longitude(int_to_long(t.i)); //Longitude(double(iLong));
  //int iLat = a_move_corner(t.j, regional_fishery_record::sw_lat);
  Lat = Latitude(int_to_lat(iLat));  //Latitude(double(iLat));
  eff = 0.0;
  recaps = t.returns;

  return(*this);
}
*/

regional_fishery_record& regional_fishery_record::operator = (regional_fishery_record& t)
{
  i = t.i;
  j = t.j;
  effort = t.effort;
  Catch = t.Catch;
  return(*this);
}

regional_fishery_record& regional_fishery_record::operator = (fishery_record& t)
{
  set(t);
  return(*this);
}

int regional_fishery_record::set(fishery_record& t)
{
  if (!statics_set)
  {
    cerr << "Error: attempting to access class regional_fishery_record" << endl
	 << "       before setting static members" << endl;
    exit(1);
  }
  i = pparam->long_to_index(t.Long); //move_corner(int(t.Long.value()),sw_long);
  j = pparam->lat_to_index(t.Lat); //move_corner(int(t.Lat.value()),sw_lat);
  //clogf << "Long, Lat : " << t.Long.value() << ", "<< t.Lat.value()
  //      << " = " << i << ", " << j << endl;
  effort = t.eff;
  Catch = t.ctch;
  return(0);
}

/*
int regional_fishery_record::set2(fishery_record& t, double x, double y)
{
  if (!statics_set)
  {
    cerr << "Error: attempting to access class regional_fishery_record" << endl
	 << "       before setting static members" << endl;
    exit(1);
  }
  i = pparam->long_to_int(t.Long.value()); //move_corner(int(t.Long.value()),sw_long) * 60.0 / x;
  j = pparam->lat_to_int(t.Lat.value()); //move_corner(int(t.Lat.value()),sw_lat) * 60.0 / y;
  clogf << "Long, Lat : " << t.Long << ", "<< t.Lat
        << " = " << i << ", " << j << endl;
  effort = t.eff;
  Catch = t.recaps; //t.ctch;
  return 0;
}
*/

indexed_regional_fishery_record* indexed_regional_fishery_record::instance = 0;

indexed_regional_fishery_record::indexed_regional_fishery_record
                                 (const adstring& path,
                                  par_t<d3_array,dmatrix,dvector,double>& param,
                                  const int year1,
                                  const int year2,
                                  List<Shift>& lse)
{
  // Add to LinkList corrected postions.
  //adstring pos_file_name = adstring(param.filename) + ".pos";
  //adstring recap_filename = adstring(param.pathname.getRoot()) + ".tag\0";
  adstring pos_file_name = adstring(param.pathname.getRoot()) + ".pos\0";
  cout << "Opening shift position file for input: " << pos_file_name << endl;
  clogf << "Opening shift position file for input: " << pos_file_name << endl;
  ifstream psf((char*)pos_file_name);
  if (!psf)
  {
    clogf << "Error opening shift position file " << pos_file_name << endl;
    cerr << "Error opening shift position file " << pos_file_name << endl;
  //exit(1);
  }
  psf >> lse;
  if (!psf)
  {
    cerr << "Error reading shift position file " << pos_file_name << endl;
    clogf << "Error reading shift position file " << pos_file_name << endl;
  }
  psf.close();

  nfleet = param.get_nfleet();
  first_year = year1;
  last_year = year2;
  year_month first_date(year1,1);
  year_month last_date(year2,12);

  fishery_record_vector frv(1, frvl);
  fishery_record fr;
  adstring_array fishery_files(1, nfleet);
  adstring_array& fleet_names = param.fleet_names;
  //********************************
  //jca4.17 use prn file path to find fleet data
  //        eq m:\movmod\rttp\grid20\..\\<fleet>.dat
  //*******************************

  for (int f = 1; f <= nfleet; f++)
  {
  #ifdef unix
    fishery_files(f) = path + adstring("../");
  #else
    fishery_files(f) = path + adstring("..\\");
  #endif
    fishery_files(f)+= fleet_names(f) + ".dat";
    //adstring_utils_tolower(fishery_files(f));
  }//forf

  int good_fleet = nfleet;
  int total = 0;
  int sum_n2 = 0;
  for (int f = 1; f <= nfleet; f++)
  {
    cout << "Processing " << fishery_files(f) << endl;
    clogf << "\nProcessing " << fishery_files(f) << endl;
    cifstream fs(fishery_files(f));
    if (!fs)
    {
      cerr << "error opening file " << fishery_files(f) << endl;
      exit(1);
    }
    fr.set_fleet(adstring_utils_tolower(fleet_names(f)));
    int n1 = 0;
    int n2 = 0;

    while (!fs.eof())
    {
      fr.read_info(fs);
      n1++;

      // Is record in model domain and time period
      if ( (param.inside(fr.longitude(), fr.latitude()))
          && (fr.date() >= first_date)
          && (fr.date() <= last_date) )
      {
        Longitude elong = fr.get_longitude();
        Latitude elat = fr.get_latitude();
        // Is record on Land?
        if ( param.is_land(fr.longitude(), fr.latitude()) )
        {
          cout << "\nRecord " << n1 << " " << elong << " " << elat
               << " on land; needs to be shifted." << endl;
          clogf << "\nRecord " << n1 << " " << elong << " " << elat
                << " on land; needs to be shifted: " << endl;
          clogf << "   " << fr.get_input_record() << endl; 
          adstring message = "Land effort: ";
          param.shift_position(message, elong, elat, lse);
          fr.set_longitude(elong);
          fr.set_latitude(elat);
        }//if.is_land

        ASSERT((n2 <= frvl))
        n2++;
        frv[++total] = fr;
        //cout << frv[total] << endl;
      }//if.inside
    }//whilefs
    cout << "Found " << n1 << " fishing records; retained " << n2 << endl;
    clogf << "Found " << n1 << " fishing records; retained " << n2 << endl;
    sum_n2 += n2;

    // No valid record for fleet decrement number of good fleets
    //TTRACE(n2,(!n2))
    //if (!n2) good_fleet--;
  }//fornfleet

  if (total <= 0)
  {
    cerr << "Error: no valid fishing records found: " << total << endl;
    exit(1);
  }
  nfleet = good_fleet;
  rfrv_length = total;
  /*
  cout << "Total effort before appending recaps = " << total_effort(frv, total-1) << endl;
  cout << "Total  catch before appending recaps = " <<  total_catch(frv, total-1) << endl;

  int num_recaps = 0;
  if (file_exists(recap_filename))
  {
    cout << "\nFile " << recap_filename << " found;  appending recaptures." << endl;
    clogf << "\nFile " << recap_filename << " found;  appending recaptures." << endl;
    const int MAXRETURNS = 12000;     // increase for more tag returns
    num_recaps = MAXRETURNS-1; // used as check in readrecaps
    recaptype_vector recaps(1, MAXRETURNS);
    readrecaps(param, param.pathname.getRoot(), recaps, num_recaps, ".\\");
    TRACE(num_recaps)
    for (int k = 1; k <= num_recaps; k++)
    {
       frv[++total] = recaps[k];
    }
    cout << num_recaps << " recapture records appended" << endl;
    clogf << num_recaps << " recapture records appended" << endl;
  }
  else
  {
    cout << "File " << recap_filename << " does not exist; no recaptures appended." << endl;
    clogf << "File " << recap_filename << " does not exist; no recaptures appended." << endl;
  }
  */

  /*
  for (int k = 1; k <=10; k++)
    TTRACE(k,frv[k])
  for (int k = (total-10); k <=total; k++)
    TTRACE(k,frv[k])
  */

  // Sort it
  clogf << "\nSorting " << rfrv_length << " records" << endl;
  cout << "\nSorting " << rfrv_length << " records" << endl;
  sort(frv, rfrv_length);
  cout << "Finished sort" << endl;
  cout << "Total effort after sort = " << total_effort(frv, rfrv_length) << endl;
  cout << "Total  catch after sort = " <<  total_catch(frv, rfrv_length) << endl;


  /*
  if (num_recaps > 0) // merge appended records
  {
    cout << "\nMerging " << rfrv_length << " records" << endl;
    int k1 = 1;
    int k2 = 2;
    while (k2 < rfrv_length)
    {
      if (frv[k1] == frv[k2])
      {
        frv[k1] += frv[k2];
        k2 ++;
      }
      else
      {
        k1 ++;
        frv[k1] = frv[k2];
        k2 ++;
      } // if (frv[k1] == frv[k2])

    } //  while (k2 < rfrv_length)
    rfrv_length = k1;
    cout << "Created " << rfrv_length << " merged records" << endl;
  }

  cout << "Total effort after merge = " << total_effort(frv, rfrv_length) << endl;
  cout << "Total  catch after merge = " <<  total_catch(frv, rfrv_length) << endl;
  */
  // Allocates matrices for fleet_list, nrec and index
  allocate();

  int fl = 1;// Fleet Number
  int nr = 1;// number of catches for a particular cohort
  int idx = 1;// record number after sort
  //Longitude sw_long = param.get_sw_coord().get_long();
  //Latitude sw_lat = param.get_sw_coord().get_lat();
  regional_fishery_record::set_sw_corner(&param); //(sw_long, sw_lat);
  adstring tfleet = frv[idx].fleet();
  adstring_utils_tolower(tfleet);
  year_month tdate = frv[idx].date();
  index(fl,tdate.get_year(),tdate.get_month_value()) = idx;
  nrec(fl,tdate.get_year(),tdate.get_month_value()) = nr;
  fleet_list(fl) = tfleet;
  fishery_records[idx] = frv[idx];
  for (int i = 2; i < total; i++)
  {
    idx ++;
    if ( (tfleet == frv[idx].fleet()) && (tdate == frv[idx].date()) )
    {
      nr ++;
      nrec(fl,tdate.get_year(),tdate.get_month_value()) = nr;
    }
    else
    {
      if (!(tfleet == frv[idx].fleet()))
      {
        fl++;
      }
      tfleet = frv[idx].fleet();
      tdate = frv[idx].date();
      index(fl,tdate.get_year(),tdate.get_month_value()) = idx;
      //last_effort_date(fl) = tdate;
      // in case there is only one record
      nr = 1;
      nrec(fl,tdate.get_year(),tdate.get_month_value()) = nr;
      fleet_list(fl)=tfleet;
    }
    fishery_records[idx] = frv[idx];
  }//for i

  cout << "\nSuccessfully built indexed_regional_fishery_record data structure"
       << endl;
  clogf << "\nSuccessfully built indexed_regional_fishery_record data structure"
        << endl;


  cout << "Opening shift position file for output " << pos_file_name << endl;
  clogf << "Opening shift position file for output " << pos_file_name << endl;
  ofstream opsf((char*)pos_file_name);
  if (!opsf)
  {
    clogf << "Error opening shift position file " << pos_file_name << endl;
    cerr << "Error opening shift position file " << pos_file_name << endl;
    exit(1);
  }
  opsf << lse;
  if (!opsf)
  {
    cerr << "Error writing shift position file " << pos_file_name << endl;
    clogf << "Error writing shift position file " << pos_file_name << endl;
  }
  opsf.close();
  cout << "Shift position file updated " << pos_file_name << endl;
  clogf << "Shift position file updated " << pos_file_name << endl;

}

void indexed_regional_fishery_record::write(const char* root_name, const char* path)
{
  adstring file_name = adstring(root_name) + ".ifr";
  ofstream ss(file_name);
  ss <<  nfleet << "  " << first_year << "  " << last_year
		<< "  " << rfrv_length << endl;
  cout <<"saved size info" << endl;
  ss << fleet_list << endl;
  cout << "saved fleet list" << endl;
  //ss << first_effort_date << endl;
  //ss << last_effort_date << endl;
  //cout << "save first and last effort date vectors" << endl;
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

  d3_array total(1,nfleet,1,12,first_year,last_year);
  total.initialize();
  for (int f = 1; f <= nfleet; f++)
  {
    for (int m = 1; m <= 12; m++)
     {
        for (int y = first_year; y <= last_year; y++)
        {
           int n1 = index(f,y,m);
           int n2 = n1+nrec(f,y,m)-1;
           for (int n = n1; n <= n2; n++)
           {
              regional_fishery_record fr = fishery_records(n);
              total(f,m,y) += fr.effort;
           } 
        }
     }
  }

  clogf << "\nTotal effort by fleet:" << endl;
  for (int f = 1; f <= nfleet; f++)
     clogf << setw(3) << f << " " << setw(4) << fleet_list[f] << " " 
           << setw(10) << setfixed() << sum(total(f)) << endl;

  clogf << "\nTotal effort by month:" << endl;
  clogf << setw(5) << "Year";
  for (int m = 1; m <= 12; m++)
     clogf << " " << setw(10) << m;
  clogf << endl;
  for (int y = first_year; y <= last_year; y++)
  {
    clogf << setw(5) << y;
    for (int m = 1; m <= 12; m++)
    {
      double msum = 0.0;
      for (int f = 1; f <= nfleet; f++)
      { 
         msum += total(f,m,y);
      }
      clogf << " " << setw(10) << setfixed() << msum;
    }
    clogf << endl;
  } 

  clogf << "\nTotal effort by year, month and fleet:" << endl;
  clogf << " year mo";
  for (int f = 1; f <= nfleet; f++)
     clogf << setw(8) << fleet_list[f];
  clogf << endl;
  for (int y = first_year; y <= last_year; y++)
  {
    for (int m = 1; m <= 12; m++)
    {
      clogf << setw(5) << y << setw(3) << m;
      for (int f = 1; f <= nfleet; f++)
         clogf <<  " " << setw(10) << setfixed() << total(f,m,y);
      clogf << endl;
    }
  }
    
  
}

double total_effort(fishery_record_vector& v, const int n)
{
   double total = 0.0;
   for (int i = 1; i <= n; i++)
     total += v[i].effort();

   return(total);
}

double total_catch(fishery_record_vector& v, const int n)
{
   double total = 0.0;
   for (int i = 1; i <= n; i++)
     total += v[i].Catch();

   return(total);
}

