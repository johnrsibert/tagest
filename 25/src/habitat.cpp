//$Id: habitat.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include <cifstrem.h>
#include <adstring.hpp>
#include "trace.h"
#include "habitat.h"

habitat::habitat()
  {
     /* do nothing here */
   }


habitat::habitat(const char* fname)
{
  cifstream dfile(fname);
  if (!dfile)
   {
    cerr <<"Cannot open " << fname << endl; exit(1);
   }
  clogf <<"Reading habitat data information from " << fname << endl;
  cout <<"\n\nReading habitat data information from " << fname << endl;

  dfile >> m >> n >> deltax >> deltay >> sw_coord;
  clogf << m <<"  "<<  n <<"  " << deltax <<"  " << deltay << "  " << sw_coord << endl;
  dfile >> stt_yr >> stt_mo >> end_yr >> end_mo;
  clogf << stt_yr  <<"  "<<  stt_mo <<"  " << end_yr <<"  " << end_mo << endl;

  allocate();
  set_vector_bounds();
  TTRACE(stt_ym, end_ym)

 }

habitat::~habitat()
  {
   /* nothing to clean */
  }



bool habitat::readata(const char* fname)
{
  adstring fn(fname);
  TRACE(fn)
      adstring filename(fn);

   cifstream s(filename);
   if(!s)
   {
       cerr << filename << " not found " <<endl;
       cerr <<"Getting out in "<<__FILE__ <<" line: " <<__LINE__<< endl;
       exit(1);
     }
   TRACE(filename)

 int nmonth = (end_ym - stt_ym) + 1;
 for(int mo = 1; mo <= nmonth; mo++)
  {
   for(int j = n; j  >= 1; j--)
    {
      //double dlat = index_to_lat(j);
      int i1 = ilb(j);
      int i2 = iub(j);
      for(int i = i1;  i <= i2;  i++)
       {
        //double dlon = index_to_long(i);
        s >> habitatarea(mo, i,j);
        //clogf << "mo " << mo <<" lat " << dlat <<" lon " << dlon << " sst= " << habitatarea(mo, i,j) << endl;
       }
     }
   }
   s.close();   //close the file channel
  //clogf << habitatarea << endl;
  //clogf << "mo " << 1 <<" lat " << n <<" lon " << m << " sst= " << habitatarea(1,m,n) << endl;
  return ( true );
} //end of readata



double habitat::hdata(year_month _ym, Longitude _long, Latitude _lat)
{
   double ww;
   int i0 = long_to_index(_long);
   int j0 = lat_to_index(_lat);
   int m0 = mon_to_index(_ym);

   ww = habitatarea(m0,i0,j0);
   //clogf << _ym <<"  " <<_long << "  " << _lat << " = " << ww << endl;
   return (ww);
}



 // inverse translation
double habitat::a_move_corner(const int i, const double dx, const int edge)
{
  double tmp = edge + (double)(i-1)*dx/60.0;  //dx*(double)(i-1+edge)/60.0;
  return(tmp);
}


// translates longitude or latitude end equator-dateline system to subscript
int habitat::move_corner(const double x, const double dx, const int edge)
{
  double tmp = 60.0*(x-edge)/dx + 1e-5;
  return ((int)tmp+1);
}


//for converting indices to long
double habitat::index_to_long(const int i)
{
  double lng = a_move_corner(i, deltax, sw_coord.long_value());
  return(lng);
}

//for converting indices to lat
double habitat::index_to_lat(const int j)
{
  double lat = a_move_corner(j, deltay, sw_coord.lat_value());
  return(lat);
}


int habitat::long_to_index(const Longitude& lng)
{
   return(long_to_index(lng.value()));
}

int habitat::long_to_index(const double lng)
{
  int i = move_corner(lng, deltax, sw_coord.long_value());
  return(i);
}




int habitat::lat_to_index(const Latitude& lat)
{
   return(lat_to_index(lat.value()));
}


int habitat::lat_to_index(const double lat)
{
  int j = move_corner(lat, deltay, sw_coord.lat_value());
  return(j);
}


int habitat::mon_to_index(year_month &ym)
{
   int month  = ( ym - stt_ym) + 1;
   return month;
}


void habitat::set_vector_bounds()
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

    cout << "Vector bounds sets successfuly "  << endl;
}


 void habitat::allocate()
  {

      cout <<"allocate " << endl;
      stt_ym.set(stt_yr,stt_mo);
      end_ym.set(end_yr,end_mo);

      int nmonth = (end_ym - stt_ym) + 1; TRACE(nmonth);
      habitatarea.allocate(1,nmonth,1,m,1,n); habitatarea.initialize();
      ilb.allocate(1,n); ilb.initialize();
      iub.allocate(1,n); iub.initialize();
      jlb.allocate(1,m); jlb.initialize();
      jub.allocate(1,m); jub.initialize();
      cout << "out allocate " << endl;
  }















