#include <adstring.hpp>
#include "etopo_area.h"
#include "trace.h"
adstring make_banner(const adstring& program_name);

ofstream clogf;

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

  etopo_area gm(fn);
  clogf << "#\n# original gridmap:" << endl;
  clogf << gm << endl;

  double elevation =  -10;
  gm.set_threshold_elevation(elevation);
  gm.insert_land_gridmap(0.25); //const double _percent_blocks_above_elvation)
  clogf << "#\n# modified gridmap with threshold elevation = " 
        << elevation << endl;
  clogf << gm << endl;

  
  elevation =  -100;
  gm.set_threshold_elevation(elevation);
  gm.insert_land_gridmap(0.25); //const double _percent_blocks_above_elvation)
  clogf << "#\n# modified gridmap with threshold elevation = "
        << elevation << endl;
  clogf << gm << endl;
 
  //imatrix gridmap = gm.get_gridmap();
  //clogf << gridmap << endl;
  cout << "pau" << endl;
  return 0;
}
