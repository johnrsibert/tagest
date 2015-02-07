//$Id: rpreproc_main.cpp 3101 2013-08-19 00:41:23Z jsibert $
#include <fvar.hpp>
//#include <iostream>
#include <stdlib.h>
//#include <fstream.h>
#include <adstring.hpp>
#include <cifstrem.h>
#include "par_t_reg.h"
#include "trace.h"

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
                   par_t<d3_array,dmatrix,dvector,double>& par_param);

void set_ipar_names(ipar_t& t);

//effproc.cpp
void eff_process(const adstring& path,
                 const adstring& root_filename,
                 const adstring& date_filename,
                 const adstring& fleet_filename,
                 par_t<d3_array,dmatrix,dvector,double>&    eff_param);

/** Mangages preprocessor functions.
\param c character indicating which preporcessor to invoke. 
'e' invokes effort preprocessor to make the ifr;
'p' invodes the ".par" file generator.
\param path adstring containing the path to the input data file. Usually called with path = ""; probably obsolete.
*/
void rpreproc_main(char c, const adstring& path)
{
  LongLat::set_input_mode(2);
  adstring root_filename; //(1, 9);
  adstring tag_data_filename; //(1, 9);
  adstring date_filename; //(1, 9);
  adstring fleet_filename; //(1, 9);

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
//line 59, file rpreproc_main.cpp, pfile = L:\movemod2\rttp\grid16a\skj16.prn
//#endif

  cifstream prnfile(pfile);
  if (!prnfile)
  {
    cerr << "Error: Cannot open " << pfile << " in " << __FILE__;
    exit(1);
  }

  par_t_reg<d3_array,dmatrix,dvector,double> param(prnfile);
  prnfile.close();

  adstring temp = root_filename;
  root_filename = path + temp;
  //strcpy(param.filename, root_filename);
  param.pathname.setPathFile(pfile);
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
      set_ipar_names(param.m_ipar);

      // change some initial defaults
      param.m_ipar(3) = 10;
      param.m_ipar(22) = 0;
      param.m_ipar(23) = 0;
      param.maxsig = 500000.0;
      param.minvel = -50.0;
      param.maxvel =  50.0;
      param.maxq = 0.5;
      param.mort = 0.1;

      param.savefit(0.0, 0, 0.0, ".p000");
      cout << "Created: " << root_filename + ".p000" << endl;
      param.savefit(0.0, 0, 0.0, ".par");
      cout << "Created: " << root_filename + ".par" << endl;
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
