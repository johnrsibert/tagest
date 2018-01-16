#include <cifstrem.h>
#include <adstring.hpp>
#include "trace.h"
#include "ers12.h"

ers12::ers12()
  {
     /* do nothing here */
   }


ers12::ers12(const char* fname)
{
  cifstream dfile(fname);
  if (!dfile)
   {
    cerr <<"Cannot open " << fname << endl; exit(1);
   }
  dfile >> m >> n >> deltax >> deltay >> sw_coord;
  clogf << m <<"  "<<  n <<"  " << deltax <<"  " << deltay << "  " << sw_coord << endl;
  clogf <<"Reading ers12 data information from " << fname << endl;
  cout <<"\n\nReading ers12 data information from " << fname << endl;
  allocate();
  set_vector_bounds();
  
 }
  
ers12::~ers12()
  {
   /* nothing to clean */
  }



bool ers12::readata(int yr, int mo)
{
    TTRACE(yr,mo)
   #ifdef unix
      adstring fname("/home/aku/msadam/NN_Malskip/ES_New/" + ersfname(yr,mo) );
    #else
       adstring fname("D:\\NN_Malskip\\ES_New\\"+ ersfname(yr,mo) );
    #endif
      
    s.open(fname, ios::in);
    if(!s)
    {
        cerr << fname << " not found " <<endl;
        return ( false );
      }
    TRACE(fname)

    double lat,lon;
    double wn;
    char string[25];
    char* sPtr;

    for(int j = n; j  >= 1; j--)
    {
      double dlat = index_to_lat(j);
      int i1 = ilb(j);
      int i2 = iub(j);
      for(int i = i1;  i <= i2;  i++)
      {
          double dlon = index_to_long(i);
          s   >> lat  >> lon  >>  string;
           if (!strcmp(string,"NaN"))
            wn = 0.0;
          else
            wn = strtod(string, &sPtr);   
                  
          ers12area(i,j)  = wn;
          //clogf << i << "   " <<dlon << "   "<< j <<"   " << dlat << "   " << ers12area(i,j) << endl;
      }
    }
    s.close();   //close the file channel

  return ( true );
} //end of readata



double ers12::wnd(Longitude _long, Latitude _lat)
{
   double ww;
   int i0 = long_to_index(_long);
   int j0 = lat_to_index(_lat);

   ww = ers12area(i0,j0);
   return (ww);
}



 // inverse translation
double ers12::a_move_corner(const int i, const double dx, const int edge)
{
  double tmp = edge + (double)(i-1)*dx/60.0;  //dx*(double)(i-1+edge)/60.0;
  return(tmp);
}


// translates longitude or latitude end equator-dateline system to subscript
int ers12::move_corner(const double x, const double dx, const int edge)
{
  double tmp = 60.0*(x-edge)/dx + 1e-5;
  return ((int)tmp+1);
}


//for converting indices to long   
double ers12::index_to_long(const int i)
{
  double lng = a_move_corner(i, deltax, sw_coord.long_value());
  return(lng);
}

//for converting indices to lat
double ers12::index_to_lat(const int j)
{
  double lat = a_move_corner(j, deltay, sw_coord.lat_value());
  return(lat);
}


int ers12::long_to_index(const Longitude& lng)
{
   return(long_to_index(lng.value()));
}

int ers12::long_to_index(const double lng)
{
  int i = move_corner(lng, deltax, sw_coord.long_value());
  return(i);
}




int ers12::lat_to_index(const Latitude& lat)
{
   return(lat_to_index(lat.value()));
}


int ers12::lat_to_index(const double lat)
{
  int j = move_corner(lat, deltay, sw_coord.lat_value());
  return(j);
}




adstring ers12::ersfname (int yr, int mth)
{
    /* Purpose: to get the names of the files for the ES1 and ES2 data base. Need to check from Dave Foley
     whether the start and and end julian days for the  months will be consistent in all the data. ers12 2/2/2002  */

    char *filename;  //return file name

     ivector StartJulianDay(1,12);  //start of the julian day
     ivector EndJulianDay(1,12);    //end of the julian day

    StartJulianDay[ 1] =1;        EndJulianDay[ 1] = 31;
    StartJulianDay[ 2] =32;       EndJulianDay[ 2] = 59;
    StartJulianDay[ 3] =60;       EndJulianDay[ 3] = 90;
    StartJulianDay[ 4] =91;       EndJulianDay[ 4] = 120;
    StartJulianDay[ 5] =121;      EndJulianDay[ 5] = 151;
    StartJulianDay[ 6] =152;      EndJulianDay[ 6] = 181;
    StartJulianDay[ 7] =182;      EndJulianDay[ 7] = 212;
    StartJulianDay[ 8] =213;      EndJulianDay[ 8] = 242;
    StartJulianDay[ 9] =244;      EndJulianDay[ 9] = 274;
    StartJulianDay[10] =274;      EndJulianDay[10] = 304;
    StartJulianDay[11] =305;      EndJulianDay[11] = 334;
    StartJulianDay[12] =335;      EndJulianDay[12] = 365;

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

} // end of  adstring ers12::ersfname (int yr, int mth)


void ers12::set_vector_bounds()
{
   int i, j;

   for(j = 1; j <= n; j++)
    {
      ilb( j ) = 1;
      iub( j) = m;
    }


   for(i = 1; i <= m;  i++)
   {
     jlb( i ) = 1;
     jub( i) = n;
   }

    cout << "Vector bounds sets successfuly  \n\n"  << endl;
}
   
  
 void ers12::allocate()
  {
      cout <<"allocate " << endl;
      ers12area.allocate(1,m,1,n); ers12area.initialize();
      ilb.allocate(1,n); ilb.initialize();
      iub.allocate(1,n); iub.initialize();
      jlb.allocate(1,m); jlb.initialize();
      jub.allocate(1,m); jub.initialize();
      cout << "out allocate " << endl;
  }
  











  


