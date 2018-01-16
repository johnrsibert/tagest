//$Id: nn_savefit.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "par_t_nn.h"
#include "trace.h"

int a_move_corner(const int i, const int edge);        //position.cpp
void make_ext(const char c, const int n, char * ext);  	//ext_numb.cpp
int last_ext_number(const char* root, const char letter);   //ext_numb.cpp

const extern  double min_nb_par;
const extern  double max_nb_par;
extern ofstream clogf;

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t_nn<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::savefit(const double _like, const int _nvar, const double gmax, const char* _ext)
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

  par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::savefit(pf, fullname);


  // output penalties if this is a fit
  if (_nvar > 0)
  {
    pf << "#\n# Various penalties:"
       << "\n# Total velocity curvature penalty = "
       << setw(25) << setprecision(17) << total_uv_penalty
       << "\n# Total sigma curvature penalty = "
       << setw(25) << setprecision(17) << total_sigma_penalty
       << "\n# Total negative tags penalty = "
       << setw(25) << setprecision(17) << total_negative_tag_penalty
       << "\n# Total total tags penalty = "
       << setw(25) << setprecision(17) << total_total_tag_penalty
       << "\n# Total weights magnitude penalty = "
       << setw(25) << setprecision(17) << total_wts_penalty
       <<"\n# Total weights switch penalty = "
        << setw(25) << setprecision(17) << total_wts_switch_penalty
       << "\n# Total bounds penalty = "
       << setw(25) << setprecision(17) << total_bounds_penalty
       << "\n#" << endl;
  }

  pf << "\n#Neural Networks Scaling Function Parameters" << endl;
  pf << "#u_slope                     u_offset" << endl;
  pf << setw(25) << setprecision(16) << u_slope
     << setw(25) << setprecision(16) << u_offset << endl;
  pf << "#v_slope                     v_offset" << endl;
  pf << setw(25) << setprecision(16) << v_slope
     << setw(25) << setprecision(16) << v_offset << endl;
  pf << "#sig_slope                   sig_offset" << endl;
  pf << setw(25) << setprecision(16) << sig_slope
     << setw(25) << setprecision(16) << sig_offset << endl;

  pf << "#uniform_sigma" << endl;
  pf << setw(25) << setprecision(16) << uniform_sigma << endl;

 pf <<"\n#Neural Networks Parameter_Ranges" << endl;
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

  pf <<"\n#hidden_layer's diff_hwts ranges" << endl;
  pf <<"#min_diff_hwts      max_diff_hwts" << endl;
  pf << setw(10)  << setprecision(3) << min_diff_hwts
    << setw(10)  << setprecision(3) << max_diff_hwts << endl;

  pf <<"\n#Linear scaling function x-ranges" << endl;
  pf <<"#xmin_uv         xmax_uv" << endl;
  pf <<"#"<< setw(10)  << setprecision(3) << xmin_uv
      << setw(10)  << setprecision(3) << xmax_uv << endl;

  pf <<"#xmin_sig         xmax_sig" << endl;
  pf <<"#"<< setw(10)  << setprecision(3) << xmin_sig
      << setw(10)  << setprecision(3) << xmax_sig << endl;


   pf <<"\n#How many Nueral networks (num_nn)"<< endl;
   pf << "   " <<num_nn << endl;
   pf << "# Levels in each nn (num_levels)" << endl;
   pf  << "  " <<num_levels << endl;

   for(int s = 1; s <= num_nn; s++)
   {
     pf <<"\n#  ==>  Config. for neural_network "<< s <<" <== " <<  endl;
     pf << "# nodes_at_layer NN"<< s <<  endl;
     pf << "   " << nn(s).get_layers() << endl;
     pf <<"#" << endl;
     pf << nn(s).nn_ipar << endl;
     pf <<"# Weights NN"<< s << endl;
     nn(s).write_weights(pf);  //write the weights
     pf <<"\n# Bias NN"<< s << endl;
     nn(s).write_bias(pf) ;
   }

  pf << "#\n# number of variables:\n" << _nvar << endl;
  pf << "# objective function value:\n" << setprecision(10) << _like <<endl;
  pf << "# maximum gradient magnitude:\n" << setprecision(10) << gmax <<endl;

  cout  << "Parameters saved to " << fullname << endl;
  clogf << "Parameters saved to " << fullname << endl;
}
template void par_t_nn<d3_array,dmatrix,dvector,double>::savefit(const double _like, const int _nvar, const double gmax, const char* _ext);
template void par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>::savefit(const double _like, const int _nvar, const double gmax, const char* _ext);
