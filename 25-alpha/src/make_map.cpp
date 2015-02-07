//$Id: make_map.cpp 3127 2013-12-05 22:40:36Z jsibert $
/** \file make_map.cpp
Makes land mask from etopo5 topography.
Scores land as some proportion of elevations in grid cell
above a reference elevaition.
*/
#include <fvar.hpp>
#include <adstring.hpp>
#include "etopo_area.h"
#include "trace.h"
#include "prnstrem.h"

#include <sstream>

adstring make_banner(const adstring& program_name);
ofstream clogf;

void writeLandMask(etopo_area& gm, const double elevation)
{
  //char s[500];
  ostringstream ss;
  ss << "landmask" << elevation << ".csv" << ends; 
  adstring csvname(ss.str().c_str());
  
  csvstream csv(csvname);
  if (!csv)
  {
     cerr << "Error opening csvname" << endl;
     exit(1);
  }
  const int m = gm.get_m();
  const int n = gm.get_n();
  imatrix& gridmap = gm.get_gridmap();
  csv << "  " << "  ";
  for (int i = 1; i <= m; i++)
    csv << i;
  csv << endl;
  csv << "j" << "lat";
  for (int i = 1; i <= m; i++)
    csv << gm.index_to_long(i);
  csv << endl;
  for (int j = n; j >= 1; j--)
  {
    csv << j << gm.index_to_lat(j);
    for (int i = 1; i <=m; i++)
       csv << gridmap(i,j);
    csv << endl;
  }
  cout << "gridmap with threshold elevation = " << elevation 
        << " written to " << csvname << endl;
}

int main(int argc, char ** argv)
{
  adstring banner = make_banner(adstring(argv[0])+": grid map generation procedure");
  cout << banner << endl;

  adstring log_file_name("makemap.log");

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

  adstring fn(filename);
  fn += ".prn";
  //adstring fn("test.prn");
  cout << fn << endl;
  cifstream cif(fn);

  //etopo_area gm(fn);
  etopo_area gm(cif);
  //clogf << "#\n# original gridmap:" << endl;
  //clogf << gm << endl;

  double elevation =  0.0;
  gm.set_threshold_elevation(elevation);
  gm.insert_land_gridmap(0.25); //const double _percent_blocks_above_elvation)
  clogf << "#\n# modified gridmap with threshold elevation = " 
        << elevation << endl;
  clogf << gm << endl;
  cout << "gridmap with threshold elevation = " << elevation 
        << " written to " << log_file_name << endl;
  writeLandMask(gm,elevation);
 
  elevation =  -10;
  gm.set_threshold_elevation(elevation);
  gm.insert_land_gridmap(0.25); //const double _percent_blocks_above_elvation)
  clogf << "#\n# modified gridmap with threshold elevation = " 
        << elevation << endl;
  clogf << gm << endl;
  cout << "gridmap with threshold elevation = " << elevation 
        << " written to " << log_file_name << endl;
  writeLandMask(gm,elevation);
  
  elevation =  -100;
  gm.set_threshold_elevation(elevation);
  gm.insert_land_gridmap(0.25); //const double _percent_blocks_above_elvation)
  clogf << "#\n# modified gridmap with threshold elevation = "
        << elevation << endl;
  clogf << gm << endl;
  cout << "gridmap with threshold elevation = " << elevation 
        << " written to " << log_file_name << endl;
  writeLandMask(gm,elevation);
  //imatrix gridmap = gm.get_gridmap();
  //clogf << gridmap << endl;
  cout << "pau" << endl;
  return 0;
}
