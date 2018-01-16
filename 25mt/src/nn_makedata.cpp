//$Id: nn_makedata.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include <fvar.hpp>
#include <cifstrem.h>
#include "habitat.h"
#include "values.h"

#include "trace.h"

void linear_input_scale(dmatrix& t, const double  min_t, const double max_t);
double min_hab_data(const d3_array& dd)
{
   int nslice = dd.slicesize();
   int  n = dd.colmax();
   int  m = dd.rowmax();

   double t =  MAXDOUBLE;

   for(int  s = 1; s <= nslice; s++)
   {
     for(int c = 1; c <= m; c++)
      {
         for( int r = 1; r <= n; r++)
          {
             double tt = dd(s,c,r);

                   //if(    (! (tt < 0.0000000001)  &&  (tt > - 0.0000000001))  && (tt < t)  )
                   if(  ( tt != 0.0)   &&  tt < t )
                     t = tt;
          } //r
      } // c
   } // s


   return t;
}
double max_hab_data(const d3_array& dd)
{
   int nslice = dd.slicesize();
   int  n = dd.colmax();
   int  m = dd.rowmax();

   double t =  - MAXDOUBLE;
   for(int  s = 1; s <= nslice; s++)
   {
     for(int c = 1; c <= m; c++)
      {
         for( int r = 1; r <= n; r++)
          {
            double tt = dd(s,c,r);

              //if(   ! ((tt < 0.0000000001) &&  (tt > - 0.0000000001))  && (tt > t)  )
              if( tt != 0.0 && tt > t )
                    t = tt;
          } //r
      } // c
   } // s

   return t;
}

ofstream clogf("habdata.log");

void nn_makedata(const char* hab_inf_file, // information specific to habitat_data from Patrick Lehody
                 const char* subset_file,  // area of extraction
                 const char* fname,        // name of the data file to extract FROM
                 const char* filename)
{
 /* Purpose: To subset and create input data for neural_net model */
 habitat habitat_data (hab_inf_file);
 habitat subset(subset_file);

 cout <<"\nReading data from <" << fname <<">.. please wait..." << endl;
 habitat_data.readata(fname);


 //get stuff from subset object
 int m = subset.get_m();
 int n  = subset.get_n();
 TTRACE(m,n)

 year_month subset_stt_ym = subset.get_start_ym();
 year_month subset_end_ym = subset.get_end_ym();
 TTRACE(subset_stt_ym, subset_end_ym)

 int  nmonth = ( subset_end_ym - subset_stt_ym ) + 1;
 TRACE(nmonth)

 const ivector ilb = subset.get_ilb();
 const ivector iub = subset.get_iub();
 TTRACE(ilb,iub)

 d3_array hd(1,nmonth, 1,m, 1,n); //allocate memory to subset data array
 hd.initialize();


 //////////////// start extract the subset ///////////////////
 year_month date;
 int kmonth = 0;
 cout <<"\nSubsetting data ... please wait" << endl;
 for( date = subset_stt_ym; date <= subset_end_ym; date++)
  {
    kmonth++;
    for(int j = 1; j <= n; j++)
    {
      double dlat = subset.index_to_lat(j);
      int i1 = ilb(j);
      int i2 = iub(j);
       for(int i = i1; i <= i2; i++)
       {
          double dlon = subset.index_to_long(i);
          const double value = habitat_data.hdata(date,dlon,dlat);
	  const int empty_value = int(value);
	  if (empty_value == -999)
	  {
               hd(kmonth,i, j) = value;
	  }
        } //i
    } //j
  }//mo

 cout <<"Done! " << endl;
 //////////// finish subsetting  //////////////////

//find the _min and _max of the data block
  double _min = min_hab_data(hd);  //TRACE(_min)
  double _max = max_hab_data(hd); //TRACE(_max);
  TTRACE(_min,_max)

  /////////// write data to binary file //////////////
  uostream uo(filename);
  const int yy = subset.get_stt_yr();
  const int mm = subset.get_stt_mo();
  TTRACE(yy,mm)

  uo <<  yy ;
  uo <<  mm;


 dmatrix temp_data(1,m,1,n); temp_data.initialize();

 ivector fp(1, nmonth);

 cout <<"\nWriting data to binary file.. " << endl;
 cout <<"Starting  year/month of the series: " << subset_stt_ym << endl;
 int mo = 0;
 for( date = subset_stt_ym; date <= subset_end_ym; date++)
  {
      mo ++;
      cout << mo <<"." << flush;
      temp_data.initialize();

      temp_data  =  hd(mo);       //read the data into model_area

      linear_input_scale(temp_data,_min, _max);
      fp(mo) = uo.tellp();  //get the position file pointer corresponding for each month block
      uo << temp_data;  //write the binary output
  } //slice

cout <<" \nEnding  year/month of the series: " << subset_end_ym << endl;

/////////////// end write ///////////////


cout <<"\n\nStarting positions for month blocks in <" << filename << "> for random access"<< endl;
cout << "fp: " << fp << endl;
clogf <<"fp: " <<  fp << endl;

 uo.close();
 cout <<"\nData saved in '" <<filename <<"'" << endl;
 cout <<"\nPau!..\n" << endl;
}
int main(const int argc, const char** argv)
{
	cifstream fnt("file_nam.dat");
	if (!fnt) 
	{
		cerr << "Cannot open file_nam.dat in main()" << endl;
		exit(1);
	}
	char filename[9];
	fnt >> filename;
	fnt.close();

	adstring inf = adstring(filename) + adstring(".inf");
	if (false)//argc == 3)
	{
		nn_makedata(inf, inf, argv[1], argv[2]);
		return 0;
	}

	// f_0176-1290.txt (forage)  --> .FRG binary  <> YES
	// t_0176-1290.txt -- > (temperature) --> .SST binary <YES>
	// u_0176-1290.txt (zonal wind stress curl --> usc binary <YES>
	// v_0176-1290.txt  (meridional wind stress curl) -->  .vsc  binary <YES>
	nn_makedata("habitat.inf", inf, "f_0176-1290.txt", adstring(filename) + adstring(".frg"));
	nn_makedata("habitat.inf", inf, "t_0176-1290.txt", adstring(filename) + adstring(".sst"));
	nn_makedata("habitat.inf", inf, "u_0176-1290.txt", adstring(filename) + adstring(".usc"));
	nn_makedata("habitat.inf", inf, "v_0176-1290.txt", adstring(filename) + adstring(".vsc"));
	nn_makedata("habitat.inf", inf, "Hskj_0176-1290.txt", adstring(filename) + adstring(".hab"));

	return 0;
}
