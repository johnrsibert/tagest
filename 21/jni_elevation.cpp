#include <adstring.hpp>
#include <fvar.hpp>
#include <jnigraphics2.h>
#include "etopo_area.h"
#include "trace.h"
ofstream clogf;
void write_ma_matrix(const dmatrix& t, const char* filename);
void write_ma_matrix(const imatrix& t, const char* filename);
adstring make_banner(const adstring& program_name);
int max(const imatrix& v);
int min(const imatrix& v);
//int max(const dmatrix& v);
int min(const dmatrix& v);

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

  adstring prn_name = adstring(filename)+adstring(".prn");
  cout << prn_name << endl;
  HERE

  etopo_area gm(prn_name);

  const int m = gm.get_m();
  const int n = gm.get_n();
  const ivector jlb = gm.get_jlb();
  const ivector jub = gm.get_jub();
  dmatrix min_elevation(1,m,1,n); 	min_elevation.initialize();
  dmatrix max_elevation(1,m,1,n); 	max_elevation.initialize();
  dmatrix diff_elevation(1,m,1,n); 	diff_elevation.initialize();
  dmatrix depth(1,m,1,n); 		depth.initialize();

#ifdef unix
  adstring coastline_file = "./"+filename+".cst";
#else
  adstring coastline_file = ".\\"+adstring(filename)+".cst";
#endif
  TRACE(coastline_file)

  jnigraphics2 jni;
  //ivector tab_list(1,1);
  //jni.addTabs(1, tab_list);
  //int depth_tab = 0;
  //jni.setTabTitle("Depth Statistics",depth_tab);
  ivector dt_layout(1, 4);
  jni.addGridLayout(2, 2, dt_layout); //, tab_list(1));
  TRACE(dt_layout)
  jni.addStatusBar();
  int depth_plot = dt_layout(1);
  int diff_plot = dt_layout(2);
  int min_plot = dt_layout(3);
  int max_plot = dt_layout(4);

  squareregiondef scm(m, n);

  double deltax = gm.get_deltax_eq();
  double deltay = gm.get_deltay();
  scm.region.dx = deltax;
  scm.region.dy = deltay;
  TTRACE(deltax,deltay)
  scm.region.cstfile = coastline_file;

  scm.legend.increment = 200;
  scm.legend.decimals = 0;

  scm.region.title = "Depth (m)";
  scm.legend.min = 0.0;
  scm.legend.max = 6000;
  jni.addSquareRegion(scm, depth_plot);

  scm.region.title = "Elevation Difference (m)";
  scm.legend.min = 0.0;
  scm.legend.max = 6000;
  jni.addSquareRegion(scm, diff_plot);

  scm.legend.min = -6000;
  scm.legend.max =  4000;
  scm.region.title = "Minimum Elevation (m)";
  jni.addSquareRegion(scm, min_plot);

  scm.region.title = "Maximum Elevation (m)";
  jni.addSquareRegion(scm, max_plot);

  int jni_result = 1;
  jni.layoutAll();

  for (int i = 1; (jni_result) && (i <= m); i++)
  {
    cout << ".";


    for (int j = 1; (jni_result) && (j <= n); j++)
    {
      double dlat = gm.index_to_lat(j);
      double dlong = gm.index_to_long(i);

      min_elevation(i,j) = gm.min_elevation(dlong, dlat);
      max_elevation(i,j) = gm.max_elevation(dlong, dlat);
      diff_elevation(i,j) = max_elevation(i,j) - min_elevation(i,j);
      depth(i,j) = -max_elevation(i,j);
      if (depth(i,j) < 0)
        depth(i,j) = 0;

      char sbuf[80];
      ostrstream ss(sbuf,80);
      ss << " i = " << i << ", j = " << j << ends;
      adstring status(sbuf);

      jni.drawSquareRegion(depth, depth_plot);
      jni.drawSquareRegion(min_elevation, min_plot);
      jni.drawSquareRegion(max_elevation, max_plot);
      jni.drawSquareRegion(diff_elevation, diff_plot);
      jni.drawStatusBar(status);
      jni_result = jni.paintAll();
    }
  }
  cout << endl;

  if (jni_result)
  {
    double mmax, mmin;
    clogf << "\nMinimum elevation (upside down):" << endl;
    clogf << min_elevation << endl;

    mmax = max(depth);
    mmin = min(depth);

    TTRACE(mmin,mmax)

    adstring depthname = adstring(filename)+adstring(".zdt");
    write_ma_matrix(depth, depthname);
    clogf << "\nDepth written to file " << depthname << endl;


    adstring diffname = adstring(filename)+adstring(".zdf");
    write_ma_matrix(diff_elevation, diffname);
    clogf << "\nElevation differences written to file " << diffname << endl;

    cout << "elevation pau" << endl;

    while ( (jni.paintAll()) ) {}
  }
  return 0;
}
