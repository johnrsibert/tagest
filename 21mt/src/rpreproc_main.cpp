#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <adstring.hpp>
#include <cifstrem.h>
#include "par_treg.h"
#include "trace.h"

extern ofstream clogf;

/*
//createCoastFile.cpp
void createCoastFile(double swX,
                     double swY,
                     double neX,
                     double neY,
                     adstring file);

*/
//parproc2.cpp

void createParFile(const adstring& rootPath,
                   const adstring& root_filename,
                   const adstring& tag_data_filename,
                   const adstring& fleet_filename,
                   par_t_reg& par_param);

//effproc.cpp
void eff_process(adstring& path,
                 adstring& root_filename,
                 adstring& date_filename,
                 adstring& fleet_filename,
                 par_t&    eff_param);

void rpreproc_main(char c, const adstring& path)
{
  adstring root_filename(1, 9);
  adstring tag_data_filename(1, 9);
  adstring date_filename(1, 9);
  adstring fleet_filename(1, 9);

  //Open file_nam.dat to read names of files for input
  ifstream fnt(path + "file_nam.dat");
  if ( !fnt)
  {
    cerr << "Error: Cannot open file_nam.dat in rpreproc_main(char)";
    exit(1);
  }
  fnt >> root_filename >> tag_data_filename >> date_filename >> fleet_filename;

  clogf << "Read '" << root_filename << "' from file_nam.dat" << endl;
  clogf << "Read '" << tag_data_filename << "' from file_nam.dat" << endl;
  clogf << "Read '" << date_filename << "' from file_nam.dat" << endl;
  clogf << "Read '" << fleet_filename << "' from file_nam.dat" << endl;

  fnt.close();

//#ifdef unix
  //adstring pfile = path + adstring(root_filename) + adstring(".prn.model");
//#else
  adstring pfile = path + adstring(root_filename) + adstring(".prn");
  TRACE(pfile)
//line 59, file rpreproc_main.cpp, pfile = L:\movemod2\rttp\grid16a\skj16.prn
//#endif

  cifstream prnfile(pfile);
  if (!prnfile)
  {
    cerr << "Error: Cannot open " << pfile << " in " << __FILE__;
    exit(1);
  }

  par_t_reg param(prnfile);
  prnfile.close();

  adstring temp = root_filename;
  root_filename = path + temp;
  //strcpy(param.filename, root_filename);
  param.pathname.setPathFile(pfile);
  TRACE(param.pathname.getPathRoot())
  TRACE(param.pathname.getPath())
  TRACE(param.pathname.getRoot())
  TRACE(c)
  switch(tolower(c))
  {
   /*
    case 'c':
      {
        double lon = param.get_long_lat_long(),
               lat = param.get_long_lat_lat();
        createCoastFile(lon, lat,
                        lon + (param.get_deltax_prn()/60.0) * param.get_m(),
                        lat + (param.get_deltay()/60.0) * param.get_n(),
                        root_filename + ".cst");
        cout << "Created: " << root_filename + ".cst" << endl;
        cout << "Done: rpreproc c" << endl;
      }
      break;
    */
    case 'p':
      createParFile(path, root_filename, tag_data_filename,
                    fleet_filename, param);
      HERE           
      param.savefit(0.0, 0, 0.0, ".p00");
      cout << "Created: " << root_filename + ".p00" << endl;
      cout << "Done: rpreproc p" << endl;
      break;
    case 'e':
      eff_process(path,
                  root_filename,
                  path + date_filename,
                  path + fleet_filename,
                  param);
      cout << "Created: " << root_filename + ".ifr" << endl;
      cout << "Done: rpreproc e" << endl;
      break;
    default :
      cerr << "- Command line error - Invalid option\n"
           << "Try entering p (parameters/tags), or e (effort)\n";
  }
}//rpreproc_main(char)
