#include <iostream>
#include <adstring.hpp>
#include "trace.h"

adstring ersfname (int yr, int mth);
bool readata(int yr, int mo, dmatrix& ers12area, const int nrecords);
extern ofstream clogf("ers_mth_wind.log");


/*  
  ERS12 data base information 
  m     n     deltax    deltay    flong    flat
 161  121   30        30       40.00E     40S
 
 test data
  15   13     60        60      67.00E    6S
*/


int main()
{
 /*********************************************************************
   *   purpose : To get the montly averages of the wind data from ERS data base 
   *   Note this is an ad hoc software that has to be kludged to get the montly
   *  wind files . msa  4/17/2002 1:13PM   
   *********************************************************************/
 
 const int m = 161;  //characteristic of the ER12 data set 
 const int n = 121;  //characteristic of the ER12 data set 
 const int nrecords = m * n;
 TRACE(nrecords)
 
 const wnd = 3;  //3rd col. is wind data
 
 dmatrix ers12area(1,nrecords,1,3);
 dvector wind_curl(1,nrecords);

 wind_curl.initialize();
 ers12area.initialize();       

  int num_files;
  for(int yr = 1991; yr <= 2000; yr++)
  {
    for(int mo = 12;  mo <= 12; mo++)
    {

          if (readata(yr,mo, ers12area, nrecords) == true)  //read the data from the file
         {
             cout <<".";
             num_files ++;
             //TTRACE(yr,mo)

                for(int i = 1;  i <= nrecords;  i++)
                 {
                   wind_curl( i ) +=  ers12area(i,wnd) ;  
                 }
  
         } //if (ers.readata(yr,mo) == true)
  
    } //mo
  } //yr


  TRACE(num_files);
   /* average the wind_curl */
   wind_curl /= double(num_files);

  char file[20] = "mean_dec.xyz";
  ofstream ofs(file);

 //write the data to the file
  for(int j = 1; j <= nrecords; j++)
 {
   ofs << ers12area( j, 1 ) << "  " << ers12area( j, 2 ) << "  " << wind_curl( j ) << endl;
 }

 cout << "\nData written to " << "< " << file << " >" << endl;
 cout <<"Pau.! " << endl;

  return ( 0 );
}



adstring ersfname (int yr, int mth)
{
    /* Purpose: to get the names of the files for the ES1 and ES2 data base. Need to check from Dave Foley
     whether the start and and end julian days for the  months will be consistent in all the data. ers12 2/2/2002  */

    char *filename;  //return file name

     ivector StartJulianDay(1,12);  //start of the julian day
     ivector EndJulianDay(1,12);    //end of the julian day

    StartJulianDay[ 1] =1;        EndJulianDay[ 1] = 31;     //jan
    StartJulianDay[ 2] =32;       EndJulianDay[ 2] = 59;    //feb
    StartJulianDay[ 3] =60;       EndJulianDay[ 3] = 90;    //mar
    StartJulianDay[ 4] =91;       EndJulianDay[ 4] = 120;   //apr
    StartJulianDay[ 5] =121;      EndJulianDay[ 5] = 151;   //may
    StartJulianDay[ 6] =152;      EndJulianDay[ 6] = 181;  // jun
    StartJulianDay[ 7] =182;      EndJulianDay[ 7] = 212;   //jul
    StartJulianDay[ 8] =213;      EndJulianDay[ 8] = 242;   //aug
    StartJulianDay[ 9] =244;      EndJulianDay[ 9] = 274;   //sep
    StartJulianDay[10] =274;      EndJulianDay[10] = 304;   //cot
    StartJulianDay[11] =305;      EndJulianDay[11] = 334;   //nov
    StartJulianDay[12] =335;      EndJulianDay[12] = 365;    //dec

    adstring root("E");
    adstring file_ext("_curl.xyz_maldives");  //extension of the file

      char year_str[5];
      sprintf(year_str,"%d",yr);

      char day_str1[4];
      char day_str2[4];
      sprintf(day_str1, "%03d", StartJulianDay(mth) ); //"%03d" will left-pad with zeros up to 3 characters
      sprintf(day_str2, "%03d", EndJulianDay (mth) );

      if( yr < 1996)
      {
        adstring fname( root + "1"+ year_str + day_str1 +"_"+ year_str + day_str2 + file_ext );
        filename = fname;
      }

      else
      {
        adstring  fname( root + "2"+ year_str + day_str1 +"_"+ year_str + day_str2 + file_ext );
        filename = fname;
      }

   return(filename);

} // end of  adstring ersfname (int yr, int mth)




bool readata(int yr, int mo, dmatrix& ers12area, int const nrecords)
{
    ifstream s;
    TTRACE(yr,mo)
    adstring fname("D:\\NN_Malskip\\ES_New\\"+ ersfname(yr,mo) );
    s.open(fname, ios::in);
    if(!s)
    {
        cerr << fname << " not found " <<endl;
        return ( false );
      }
    TRACE(fname)

    double  lat,lon;
    double wn;
    char string[25];
    char* sPtr;
   
     //TRACE(nrecords)
     
      for(int i = 1;  i <= nrecords;  i++)
      {
        
          s   >> lon >> lat >> string;
          wn = strtod (string, &sPtr);  //need only the last column
          ers12area(i,1) = lon;
          ers12area(i,2) = lat;
          ers12area(i,3) = wn;       
         
         // clogf << ers12area(i,1) << "   "<< ers12area(i,2)<< "   " << wn  << endl;
     
      }
    s.close();   //close the file channel

  return ( true );
} //end of readata