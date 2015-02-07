#include <adstring.hpp>
#include "par_treg.h"
#include "trace.h"

adstring banner;
ofstream clogf;

adstring make_banner(const adstring& program_name);

int main(int argc, char ** argv)
{
  banner = make_banner(adstring(argv[0])+": gis grid map files generation procedure");
  cout << banner << endl;

  adstring log_file_name("gridmap.log");

  clogf.open(log_file_name);
  if ( !clogf )
  {
    cerr << "Cannot open file " << log_file_name << endl;
    exit(1);
  }
  cout << "Opened " << log_file_name << endl;
  clogf << banner << endl;

  cifstream fnt("file_nam.tmp");
  if ( !fnt)
  {
    cerr << "Cannot open file_nam.tmp in main()" << endl;
    exit(1);
  }
  char filename[9];
  fnt >> filename;
  fnt.close();
  clogf << "Read '" << filename << "' from file_nam.tmp" << endl;
  cout << "Read '" << filename << "' from file_nam.tmp" << endl;
  //adstring fn(filename);

  par_t_reg param(filename);
  imatrix gridmap = param.get_gridmap();

  // stuff for GMT
  int size = 8;
  int angle = 0;
  int fontno = 0;
  int justify = 6;

  Geo_coord sw_coord = param.get_sw_coord();
  Longitude sw_long = sw_coord.get_long();
  Latitude sw_lat = sw_coord.get_lat();
  TTRACE(sw_long,sw_lat)
  int n = param.get_n();
  int m = param.get_m();
  double dy2 = 0.5*(param.get_deltay()/60.0);
  double dx2 = 0.5*(param.get_deltax_eq()/60.0);
  TTRACE(dx2,dy2)

  ofstream gmt("gmt_gridmap.txt");

  //double Lat = sw_lat.value() - 0.5;
  for (int j=1; j<=n; j++)
  {
    double Lat = param.index_to_lat(j) + dy2;
    TTRACE(j,Lat)
    //double Long = sw_long.value() - 0.5;
    //Lat ++;
    for (int i=1; i<=m; i++)
    {
      //Long ++;
      double Long = param.index_to_long(i) + dx2;
      TTRACE(i,Long)
      gmt << setw(8) << setprecision(5) << Long
          << setw(7) << setprecision(4) << Lat
          << setw(3) << size
          << setw(2) << angle
          << setw(3) << fontno
          << setw(2) << justify
          << setw(3) << gridmap(i,j) << endl;
    }
  }
  if (!gmt)
  {
     cerr << "Error writing gmt grid map." << endl;
     exit(1);
  }
  cout << "Finishined writing gmt zone map." << endl;
}
