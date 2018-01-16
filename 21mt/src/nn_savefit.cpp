#include "par_t_nn.h"
#include "trace.h"

// version string is used to help backwards compatibility
// in case of file format changes
// change version to reflect neural net
adstring Model_area::par_file_version = "#v21n\0";

int a_move_corner(const int i, const int edge);        //position.cpp
void make_ext(const char c, const int n, char * ext);  	//ext_numb.cpp
int last_ext_number(const char* root, const char letter);   //ext_numb.cpp

const extern  double min_nb_par;
const extern  double max_nb_par;
extern ofstream clogf;

void par_t_nn::savefit(const double _like, const int _nvar, const double gmax, const char* _ext)
{
  char fullname[20];
  strcpy(fullname, pathname.getRoot());
  if (!_ext)
  {
    char ext[5];
    int ext_no = last_ext_number(pathname.getRoot(), 'p') + 1;
    m_ipar[8] = ext_no;
    make_ext('p', ext_no, ext);
    strcat(fullname, ext);
  }
  else
  {
    strcat(fullname, _ext);
  }

  ofstream pf(fullname);
  if (!pf)
  {
    cerr << "Error: Cannot open file " << fullname
         << " in par_t_nn::savefit. " << endl;
    exit(1);
  }

  par_t::savefit(pf, fullname);

  /*
  // output penalties if this is a fit
  if (_nvar > 0)
  {
    pf << "\n# total negative tags penalty = "
       << setw(25) << setprecision(17) << total_negative_tag_penalty
       << "\n# total total tags penalty = "
       << setw(25) << setprecision(17) << total_total_tag_penalty
       << "\n# total bounds penalty = "
       << setw(25) << setprecision(17) << total_bounds_penalty
       << "\n#" << endl;
  }

  */

  pf << "\n#define NEURAL_NET"  << endl;
  pf  <<"#num_layers" << endl;
  pf << nn.get_nlayer() << endl;
  pf <<"#nodes at layer" << endl;
  pf << nn.get_layers() << endl;

  pf << "\n#NEURAL_NET PARAMETERS" << endl;

  pf <<"\n#weights" << endl;
  nn.write_weights(pf);  //write the weights

  pf << "\n#bias" << endl;
  nn.write_bias(pf);  	   //write the bais terms

  pf << "\n#SCALING FUNCTION PARAMETERS" << endl;
  pf << "#u_slope                     u_offset" << endl;
  pf << setw(25) << setprecision(16) << u_slope
     << setw(25) << setprecision(16) << u_offset << endl;
  pf << "#v_slope                     v_offset" << endl;
  pf << setw(25) << setprecision(16) << v_slope
     << setw(25) << setprecision(16) << v_offset << endl;
  pf << "#sig_slope                   sig_offset" << endl;
  pf << setw(25) << setprecision(16) << sig_slope
     << setw(25) << setprecision(16) << sig_offset << endl;

 pf <<"\n#PARAMETER_RANGES" << endl;
 pf <<"#min_weight      max_weight" << endl;
 pf << setw(10)  << setprecision(3) << min_weight
    << setw(10)  << setprecision(3) << max_weight << endl;
 pf <<"#min_bias         max_bias" << endl;
 pf << setw(10)  << setprecision(3) << min_bias
    << setw(10)  << setprecision(3) << max_bias << endl;
 pf <<"#min_slope        max_slope" << endl;
 pf << setw(10)  << setprecision(3) << min_slope 
    << setw(10)  << setprecision(3) << max_slope << endl;
 pf << "#min_offset     max_offset" << endl;
 pf << setw(10)  << setprecision(3) << min_offset
    << setw(10)  << setprecision(3) << max_offset << endl;
 pf << "#\n#end of NEURAL_NET stuff" << endl;

// out of place likelihood stuff that John will move
// (and handle with version string)

  if ( (m_ipar[11] == 4) || (m_ipar[11] == 5) ||
       (m_ipar[11] ==14) || (m_ipar[11] ==15) ||
       (m_ipar[11] ==24) || (m_ipar[11] ==25) )
  {
    pf << "#" << endl;
    pf << "# min_nb_par = " << min_nb_par << endl;
    pf << "# max_nb_par = " << max_nb_par << endl;
    pf << "# negative binomial parameters:" << endl;
    if ( (m_ipar[11] ==24) || (m_ipar[11] ==25) )
    {
       for (int f = 1; f <= nfleet; f++)
       {
          pf << "# " << (char*)fleet_names[f] << endl;
          pf  << setw(23) << setprecision(16)  << nb_par(f) << endl;
       }
    }
    else
      pf  << setw(23) << setprecision(16)  << nb_par << endl;
  }


  pf << "#\n# number of variables:\n" << _nvar << endl;
  pf << "# objective function value:\n" << setprecision(10) << _like <<endl;
  pf << "# maximum gradient magnitude:\n" << setprecision(10) << gmax <<endl;

  cout  << "Parameters saved to " << fullname << endl;
  clogf << "Parameters saved to " << fullname << endl;
}//End savefit
