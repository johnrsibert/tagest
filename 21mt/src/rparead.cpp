

#include "par_treg.h"
#include "bound.h"

extern ofstream clogf;

par_t_reg::par_t_reg(const char* name): par_t(name)
{
  adstring fullname;
  seasons.allocate(1, 12);
 
  cout << "Reading part_t_reg data for " << name << endl;
  clogf << "Reading part_t_reg data for " << name << endl;
 
  fullname = adstring(name);
  int dot_pos = fullname.pos(".");

  if (dot_pos == 0)
      fullname += ".par";

  cifstream dfile(fullname);
  if ( !dfile )
  {
      cerr << "Cannot reopen file again" << fullname << endl;
      exit(1);
  }
 
  //start where par_t_read finished
  dfile.seekg(sp);

  if (dfile_version != par_file_version)
  {
    cerr << "\nError: File signature difference for " << fullname << endl;
    cerr << "       Found " << dfile_version 
         << " should have " << par_file_version << endl;
    cerr << "       Unable to interpret this file format." << endl;
    cerr << "       Error occured at line " << __LINE__ 
         << " in " << __FILE__ << endl;
    exit(1);
  } 

  dfile  >> ngrid >> nseason; 
  if ( !dfile )
  {
      cerr << "Error reading sizes in file " << fullname << endl;
      exit(1);
  }
  clogf << "Seasons: " << nseason << endl;

  allocate();
  initialize();

  read_seasons(dfile);
  clogf <<"Finished reading seasons." << endl;
  cout << "Finished reading seasons." << endl;

  if ( (m_ipar[11] == 4) || (m_ipar[11] == 5) ||
       (m_ipar[11] ==14) || (m_ipar[11] ==15) )
  {
    dfile >> nb_par(1) >> nb_par(2);
    clogf << "negative binomial parameter = " << nb_par << endl;
    cout << "negative binomial parameter = " << nb_par << endl;
  }

  if ( (m_ipar[11] ==24) || (m_ipar[11] ==25) )
  {
    dfile >> nb_par;
    clogf << "negative binomial parameter = " << nb_par << endl;
    cout << "negative binomial parameter = " << nb_par << endl;
  }

  dfile.set_ignore_eof();
  dfile >> nvar;
  if (!dfile)
      nvar = 0;

  dfile >> objective_function_value;
  clogf << "objective_function_value: " <<  objective_function_value << endl;
  if (!dfile)
      objective_function_value = 0;
  clogf << "objective_function_value: " <<  objective_function_value << endl;
 
  dfile >> maximum_gradient;
  clogf << "maximum gradient magnitude: " << maximum_gradient << endl;
  if (!dfile)
      maximum_gradient = 0.0;
  clogf << "maximum gradient magnitude: " << maximum_gradient << endl;


  dfile.close();
  clogf << "Fishished reading par file." << endl;
  cout << "Fishished reading par file." << endl;
}

void par_t_reg::read_seasons(cifstream& istr)
{
  double uscale, vscale, sigmascale;
  if (m_ipar[66] == 1)
  {
      //read in scale factors from file "scale.dat"
      ifstream scale("scale.dat");
      if (!scale)
      {
          clogf << "Unable to open scale.dat" << endl;
          exit(1);
      }
      scale >> uscale >> vscale >> sigmascale;
  }
	

  for (int k = 1; k <= nseason; k++)
  {
      istr >> startmonth[k];
      if (startmonth[k] <= 0)
      {
          cerr << "Starting month for season " << k << " not specified." << endl;
          exit(1);
      }

      for (int i = 1; i <= ngrid; i++)
      {
          clogf << "season " << k << ", region " << i;
          int j;
          istr >> j;
          clogf << setw(10) << j;

          istr >> stratum_active(k, j);
          clogf << setw(5) << stratum_active(k, j);


          istr >> usergrid[k][j].u;
          if (m_ipar[66] == 1)
              usergrid[k][j].u *= uscale;
          clogf << setw(10) << usergrid[k][j].u;

          istr >> usergrid[k][j].v;
          if (m_ipar[66] == 1)
              usergrid[k][j].v *= vscale;
          clogf << setw(10) << usergrid[k][j].v;

          istr >> usergrid[k][j].sigma;
          if (m_ipar[66] == 1)
              usergrid[k][j].sigma *= sigmascale;
          clogf << setw(10) << usergrid[k][j].sigma << endl;

          if (!istr)
          {
	     cerr << "**  error reading season " << j
	          << " (season record " << i << ")" << endl;
	     exit(1);
          }
      }
  }

  set_seasons();
}

