//$Id: elevation.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include <fvar.hpp>
#include <adstring.hpp>
#include "etopo_area.h"
#include "trace.h"
ofstream clogf;
void write_ma_matrix(const dmatrix& t, const char* filename);
void write_ma_matrix(const imatrix& t, const char* filename);
adstring make_banner(const adstring& program_name);
int max(const imatrix& v);
int min(const imatrix& v);
//int max(const dmatrix& v);

int main(int argc, char ** argv)
{
  adstring banner = make_banner(adstring(argv[0])+": elevation matrix generator");
  cout << banner << endl;

  adstring log_file_name("elevation.log");

  clogf.open(log_file_name);
  if ( !clogf )
  {
    cerr << "Cannot open file " << log_file_name << endl;
    exit(1);
  }
  cout << "Opened " << log_file_name << endl;
  clogf << banner << endl;

  cifstream fnt("file_nam.dat");
  if ( !fnt)
  {
    cerr << "Cannot open file_nam.dat in main()" << endl;
    exit(1);
  }
  char filename[9];
  fnt >> filename;
  fnt.close();
  clogf << "Read '" << filename << "' from file_nam.dat" << endl;
  cout << "Read '" << filename << "' from file_nam.dat" << endl;

  adstring prn_name = adstring(filename)+adstring(".prn");
  cout << prn_name << endl;

  cifstream prnfile(prn_name);
  if (!prnfile)
  {
    cerr << "Error: Cannot open " << prn_name << " in " << __FILE__;
    exit(1);
  }

  etopo_area gm(prnfile);
  prnfile.close();

  const int m = gm.get_m();
  const int n = gm.get_n();
  const ivector jlb = gm.get_jlb();
  const ivector jub = gm.get_jub();
  imatrix min_elevation(1,m,1,n);
  imatrix max_elevation(1,m,1,n);
  imatrix diff_elevation(1,m,1,n);
  dmatrix depth(1,m,1,n);

  for (int i = 1; i <= m; i++)
  {
    cout << ".";
    double dlong = gm.index_to_long(i);
    for (int j = 1; j <= n; j++)
    {
      double dlat = gm.index_to_lat(j);

      min_elevation(i,j) = gm.min_elevation(dlong, dlat);
      max_elevation(i,j) = gm.max_elevation(dlong, dlat);
      diff_elevation(i,j) = max_elevation(i,j) - min_elevation(i,j);
      depth(i,j) = -max_elevation(i,j);
      if (depth(i,j) < 0)
        depth(i,j) = 0;
    }
  }
  cout << endl;

  double mmax, mmin;
  clogf << "\nMinimum elevation (upside down):" << endl;
  clogf << min_elevation << endl;

  mmax = max(depth);
  mmin = min(depth);

  TTRACE(mmin,mmax)

  adstring depthname = adstring(filename)+adstring(".zdt");
  write_ma_matrix(depth, depthname);
  clogf << "\nDepth written to file " << depthname << endl;
  cout << "\nDepth written to file " << depthname << endl;

  adstring diffname = adstring(filename)+adstring(".zdf");
  write_ma_matrix(diff_elevation, diffname);
  clogf << "\nElevation differences written to file " << diffname << endl;
  cout << "\nElevation differences written to file " << diffname << endl;


  cout << "elevation pau" << endl;
  return 0;
}
