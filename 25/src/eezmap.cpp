//$Id: eezmap.cpp 2903 2011-12-19 02:14:01Z jsibert $
#include <cifstrem.h>
#include <fvar.hpp>
#include <iomanip>
#include <math.h>
#include "xypair.h"
#include "longtude.h"
#include "latitude.h"
#include "trace.h"

int inside(const dvector& xx, const dvector& yy, const double x0, const double y0);


ofstream clogf("eezmap.log");

int main(void)
{
  int nzone;
  int npoint;
  XY_pair p0;


  adstring eezname("../eez.dat");
  cifstream eez(eezname);
  if (!eez)
  {
     cerr << "Error: unable to open " << eezname << endl;
     exit(1);
  }
  eez.set_ignore_eof();
  nzone = 0;
  eez >> nzone;
  cout << nzone << endl;
  ivector points(1,nzone);

  for (int n = 1; n <= nzone; n++)
  {
    eez >> p0;
    XY_pair pp, t;
    clogf << "\n" << p0 << " : " << pp << endl;
    npoint = 1;
    while (p0 != pp)
    {
      eez >> pp;
      npoint ++;
      clogf << setw(3) <<npoint << ": " << pp << endl;
    }
    cout << "found " << npoint << " points for zone " << n << endl;
    clogf << "found " << npoint << " points for zone " << n << endl;
    points(n) = npoint;
    pp = t;
  }

  eez.close();

  cout << "\npoints: " << points << endl;
  clogf << "\npoints: " << points << endl;

  dmatrix xx(1,nzone,1,points);
  dmatrix yy(1,nzone,1,points);

  cifstream eez2(eezname);
  eez2.set_ignore_eof();
  nzone = 0;
  eez2 >> nzone;
  cout << nzone << endl;

  for (int n = 1; n <= nzone; n++)
  {
    npoint = points(n);
    cout << "\n" << setw(3) << n << setw(4) << npoint << endl;
    for (int j = 1; j <= npoint; j++)
    {
      eez2 >> p0;
      xx(n,j) = p0.get_x();
      yy(n,j) = p0.get_y();
      //cout << xx(n,j) << " " << yy(n,j) << endl;
    }
  }

  eez2.close();

  ifstream file_names("file_nam.dat");
  if (!file_names)
  {
    cerr << "Error opening file_nam.dat." << endl;
    exit(1);
  }
  adstring root_name;
  adstring tag_file;
  adstring date_file;
  adstring fleet_file;
  file_names >> root_name >> tag_file >> date_file >> fleet_file;
  cout << root_name << endl;
  //cout << fleet_file << endl;

  TRACE(root_name)
  adstring ma_name = root_name+adstring(".prn");
  TRACE(ma_name)
  cifstream ma(ma_name);
  if (!ma)
  {
    cerr << "Error opening " << ma_name << endl;
    exit(1);
  }
//  #   M    N  deltax  deltay  sw_long sw_lat
//   60   35     60     60    130E    25S
  int M, N;
  double deltax, deltay;
  Longitude sw_long;
  Latitude sw_lat;
  ma >> M >> N >> deltax >> deltay >> sw_long >> sw_lat;
  cout << sw_lat << endl;

  imatrix zone_map(1,M,1,N);
  zone_map.initialize();

  ivector cells_in_zone(1,nzone);
  cells_in_zone.initialize();

  for (int i = 1; i <= M; i++)
  {
    double tx = sw_long.value()+i-0.5;
    for (int j = 1; j <= N; j++)
    {
      double ty = sw_lat.value()+j-0.5;
      for (int k = 1; k <= nzone; k++)
      {

        int in_zone = inside(xx(k), yy(k), tx, ty);
        if (in_zone)
        {
           int kk = k;
           zone_map(i,j) = kk;
           cells_in_zone(kk) ++;
           break;
        }
      }
    }
  }

  ivector zone_number(1,nzone);
  zone_number.initialize();
  int zone_counter = 0;
  for (int n = 1; n <= nzone; n++)
  {
    if (cells_in_zone(n))
    {
      zone_counter ++;
      zone_number(n) = zone_counter;
    }
  }


  adstring eez_name = root_name+adstring(".eez");
  ofstream eezmap((char*)eez_name);

  cout << "#\n# zone map" << endl;
  eezmap << "#\n# zone map" << endl;
  
  for (int i = 1; i <= M; i++)
  {
    //double tx = sw_long.value()+i-0.5;
    int tx = sw_long.value()+i-1;
    if (i==1)
    {
      cout << "#" << setw(3) << tx;
      eezmap << "#" << setw(3) << tx;
    }
    else
    {
      cout << setw(4) << tx;
      eezmap << setw(4) << tx;
    }
  }
  cout << endl;
  eezmap << endl;

  for (int j = N; j >= 1; j--)
  {
    //double ty = sw_lat.value()+j-0.5;
    //cout << setw(7) << setprecision(4) << ty;

    for (int i = 1; i <= M; i++)
    {
       cout << setw(4) << zone_map(i,j);
       eezmap << setw(4) << zone_map(i,j);
       /*
       int k = zone_map(i,j);
       if (k>0)
       {
         zone_map(i,j) = k; // zone_number(k);
       }
       */
    }
    cout << endl;
    eezmap << endl;
  }
  /*
  cout << "#\n# renumbered zone map" << endl;
  for (int i = 1; i <= M; i++)
  {
    int tx = sw_long.value()+i-1;
    cout << setw(4) << tx;
  }
  cout << endl;
  for (int j = N; j >= 1; j--)
  {
    //double ty = sw_lat.value()+j-0.5;
    //cout << setw(7) << setprecision(4) << ty;

    for (int i = 1; i <= M; i++)
    {
       cout << setw(4) << zone_map(i,j);
    }
    cout << endl;
  }
  */
  cout << "#\n# number of zones  number of phases" << endl;
  cout << setw(11) << nzone << setw(17) << 1 << endl;
  cout << "#\n#        phase duration" << endl;
  cout << "#\n# zone numbers" << endl;
//  cout << "#        grid serial     effort" << endl;
  cout << "#           effort" << endl;
  cout << "# zone  multiplier" << endl;
  for (int n = 1; n <= nzone; n++)
  {
//    cout << setw(7) << n << setw(6) << cells_in_zone(n)
//         << setw(7) << zone_number(n);
     cout << setw(7) << n;
    if (cells_in_zone(n))
      cout << setw(11) << "1.0" << endl;
    else
      cout << setw(11) << "0.0" << endl;
  }

/////////////////////////


  // stuff for GMT
  int size = 6;
  int angle = 0;
  int fontno = 0;
  int justify = 6;

  //Geo_coord sw_coord = param.get_sw_coord();
  //Longitude sw_long = sw_coord.get_long();
  //Latitude sw_lat = sw_coord.get_lat();
  TTRACE(sw_long,sw_lat)
  //int n = param.get_n();
  //int m = param.get_m();
  double dy2 = 0.5*(deltay/60.0);
  double dx2 = 0.5*(deltax/60.0);
  TTRACE(dx2,dy2)

  ofstream gmt("gmt_eezmap.txt");

  //double Lat = sw_lat.value() - 0.5;
  for (int j=1; j<=N; j++)
  {
    //double Lat = param.index_to_lat(j) + dy2;
    //double tmp = edge + (double)(i-1)*dx/60.0;
    double Lat = sw_lat.value() + (double)(j-1)*deltax/60.0 + dx2;
    //TTRACE(j,Lat)
    //double Long = sw_long.value() - 0.5;
    //Lat ++;
    for (int i=1; i<=M; i++)
    {
      //Long ++;
      //double Long = param.index_to_long(i) + dx2;
      double Long = sw_long.value() + (double)(i-1)*deltay/60.0 + dy2;
      //TTRACE(i,Long)
      gmt << setw(8) << setprecision(5) << Long
          << setw(7) << setprecision(4) << Lat
          << setw(3) << size
          << setw(2) << angle
          << setw(3) << fontno
          << setw(2) << justify
          << setw(3) << zone_map(i,j) << endl;
    }
  }
  if (!gmt)
  {
     cerr << "Error writing gmt grid map." << endl;
     exit(1);
  }
  cout << "Finishined writing gmt zone map." << endl;
/////////////////////////
  exit(0);
}

/** A version of atan2.
*/
double patan2(const double x, const double y)
{
  const double TWO_PI = 2.0*M_PI;
  double a = atan2(x+1e-8,y);
  if (a < 0.0)
    a = TWO_PI + a;
  if (a >= TWO_PI)
    a -= TWO_PI;
  return (a);
}

/** Determine whether a point lies withing a polygon of exclusive
economic zone vertices.
\param xx Vector of EEZ longitudes
\param yy Vector of EEZ latitudes
\param x0 Longitude of point to be tested
\param y0 Latitude of point to be tested
\return 1 if point is inside EEZ; 0 otherwise 
*/

int inside(const dvector& xx, const dvector& yy, const double x0, const double y0)
{
  const double TWO_PI = 2.0*M_PI;
  int i1 = xx.indexmin();
  int i2 = xx.indexmax();

  double dx = xx(i1) - x0;
  double dy = yy(i1) - y0;
  double a1 = patan2(dx,dy);

  double angle = 0.0;
  for (int i = i1+1; i <= i2; i++)
  {
     double dx = xx(i) - x0;
     double dy = yy(i) - y0;
     double a2 = patan2(dx,dy);
     double da = a2 - a1;

     if (da > M_PI)
     {
       da -= TWO_PI;
     }
     if (da < -M_PI)
     {
       da += TWO_PI;
     }

     angle += da;
     a1 = a2;
  }
  double diff = fabs(angle)- TWO_PI;
  return (fabs(diff)< 1e-5);
}

