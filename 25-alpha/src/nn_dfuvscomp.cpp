//$Id: nn_dfuvscomp.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "par_t_nn.h"
#include "trace.h"

template <typename MATRIX, typename VECTOR>
VECTOR nn_cases(int num_nn, const MATRIX& output_layer);
void nn_dfcases(int num_nn, dvector dfnn_output, dmatrix dfoutput_layer);
ivector choose_nn_input_layer_display(imatrix &input_controls);

void dfscale_fn(const double x, const double slope, const double offset, double& dfx, double& dfslope, double& dfoffset, double& dfy, const double min, const double max);
void dfscale_fn(const double x, const dvariable slope, const dvariable offset, double& dfx, const double dfslope, const double dfoffset, double& dfy, const double min, const double max)
	{ dfscale_fn(x, value(slope), value(offset), dfx, dfslope, dfoffset, dfy, min, max); }

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void dfoutput_comp(neural_net<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& nn, neural_net<d3_array,dmatrix,dvector,double>& dfnn, dvector& dfoutput, const int squash_flag);

extern ofstream clogf;

/* dfscale_version */
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t_nn<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::dfuvs_comp(par_t_nn<d3_array,dmatrix,dvector,double>& param, dmatrix& dfu, dmatrix& dfv, dmatrix& dfsigma, year_month& ym)
{

  ivector display_nodes =  choose_nn_input_layer_display(input_controls);

  if(display_nodes(10)  )
       habitat_data(wsc_file_ext, ym, wstress_curl);
  if(display_nodes(12)  )
       habitat_data(forage_file_ext, ym, forage);
  if(display_nodes(13)  )
       habitat_data(sst_file_ext, ym, sst);
  if(display_nodes(14)  )
       habitat_data(zonal_current_file_ext, ym, zonal_current);
  if(display_nodes(15)  )
       habitat_data(meridional_current_file_ext, ym, meridional_current);
  if(display_nodes(16)  )
       habitat_data(habitat_file_ext, ym, habitat);

  //get stuff from neural_net class
   imatrix mncol(1,num_nn,1, param.num_levels-1);
   ivector last_layer(1,num_nn);
   ivector squash_flag(1,num_nn);

   for(int s = 1; s <= num_nn; s++)
   {
     mncol(s) = param.nn(s).get_ncol();
     last_layer(s) =param.nn(s).get_nlayer();
     squash_flag(s) = param.nn(s).nn_ipar(3);
   }

   ivector vinput(1,num_nn);
   ivector voutput(1,num_nn);

   for(int s =1; s <= num_nn; s++)
   {
     vinput(s) = mncol(s,1);
     voutput(s) = param.nodes_at_layer(s,last_layer(s));
   }

   dmatrix input_layer(1,num_nn,1,vinput);     input_layer.initialize();
   dmatrix output_layer(1,num_nn,1,voutput);   output_layer.initialize();

   dmatrix dfoutput_layer(1,num_nn,1,voutput); dfoutput_layer.initialize();
   dvector nn_output(1,3);   //for u, v, and sigma AND in THIS SEQUENCE ONLY!
   dvector dfnn_output(1,3);  //for u, v, and sigma AND in THIS SEQUENCE ONLY!

  double o3 = 0.0;
  double dfo3 = 0.0;


  for (int i = m; i >= 1; i--)
  {
    int j1 = jlb(i);
    int jn = jub(i);
    for (int j = jn; j >= j1; j--)
    {
      //get input and output for each NN
      for(int s = 1; s <= num_nn; s++)
      {
        input_layer(s) =  param.get_input_layer(s, ym, i, j);
        output_layer(s) = param.nn(s).output_comp(input_layer(s), squash_flag(s) );
      }

      //put the <output_layer> ragged matrix  values into a vector
      nn_output = nn_cases<MATRIX,VECTOR>(num_nn,  output_layer);
      dfnn_output.initialize();  //this is neccessary

      //derivative computations
      if (m_ipar[80] == 0)
      {
        //scale the values to use in the main model
        if( param.m_ipar[77] == 0 )
          //sigma(i,j) = scale_fn(output_layer(3), sig_slope, sig_offset, minsig, maxsig);
          dfscale_fn(nn_output(3), param.sig_slope, param.sig_offset, dfnn_output(3),
                                 sig_slope, sig_offset, dfsigma(i,j),  minsig, maxsig);
        else if(param.m_ipar[77] == 1)
        {
          o3 = nn_output(3) * nn_output(3);
          //sigma(i,j) = scale_fn(o3, sig_slope, sig_offset, minsig, maxsig);
          dfscale_fn(o3, param.sig_slope, param.sig_offset, dfo3, sig_slope,
                                sig_offset, dfsigma(i,j), minsig, maxsig);

          // o3 = nn_output(3) * nn_output(3);
          dfnn_output(3) += 2.0*nn_output(3)*dfo3;
          dfo3 = 0.0;
        }
        else if(param.m_ipar[77] == 2)
        {
          //recompute
          double rs = scale_fn(nn_output(3), param.sig_slope, param.sig_offset, minsig, maxsig);

          double dfrs = 0.0;
          //sigma(i,j) = rs*rs;
          dfrs += 2.0*rs*dfsigma(i,j);
          dfsigma(i,j) = 0.0;

          //double rs = scale_fn(nn_ouput(3), sig_slope, sig_offset, min_sig, maxsig);
          dfscale_fn(nn_output(3), param.sig_slope, param.sig_offset, dfnn_output(3),
                  sig_slope, sig_offset, dfrs,  minsig, maxsig);
          dfrs = 0.0;

        }
        else if(param.m_ipar[77] == 3)
        { 
          //double dfuniform_sigma = 0;

          //sigma(i,j) = uniform_sigma;
          uniform_sigma += dfsigma(i, j);
          dfsigma(i, j) = 0;

          //dfuniform_sigma = 0;
        }
        else
        {
          cerr <<"illegal value of m_ipar[77]: " << m_ipar[77] << endl;
          cerr <<"this cannot happen. " << endl;
          exit(0);
        }
      }
      else // (m_ipar[80] == 1)
      {
        //do not scale the values to use in the main model
        if(m_ipar[77] == 0 )
        {
          //sigma(i,j) = nn_output(3);
          dfnn_output(3) += dfsigma(i,j);
          dfsigma(i,j) = 0.0;
        }
        else if(m_ipar[77] == 1)
        {
          //sigma(i,j) = o3;
          dfo3 += dfsigma(i,j);
          dfsigma(i,j) = 0.0;

          //o3 = nn_output(3) * nn_output(3);
          dfnn_output(3) += 2.0*nn_output(3)*dfo3;
          dfo3 = 0.0;
        }
        else if(m_ipar[77] == 2) // in this case same as (m_ipar[77] == 1)
        {
          // recompute
          double rs = nn_output(3);

          double dfrs = 0.0;
          //sigma(i,j) = rs * rs;
          dfrs += 2.0*rs*dfsigma(i,j);
          dfsigma(i,j) = 0.0;

          //double rs = nn_output(3);
          dfnn_output(3) += dfrs;
          dfrs = 0.0;
        }
        else if(param.m_ipar[77] == 3)
        { 
          //double dfuniform_sigma = 0;

          //sigma(i,j) = uniform_sigma;
          uniform_sigma += dfsigma(i, j);
          dfsigma(i, j) = 0;

          //dfuniform_sigma = 0;
        }
	else
        {
          cerr <<"illegal value of m_ipar[77]: " << m_ipar[77] << endl;
          cerr <<"this cannot happen. " << endl;
          exit(0);
        }
        //v(i,j) = nn_output(2);
        //u(i,j) = nn_output(1);
      }

      //scalefn(nn_output(2), v(i,j), v_slope, v_offset, minvel, maxvel);
      dfscalefn(nn_output(2), dfnn_output(2), dfv(i,j), param.v_slope, v_slope, param.v_offset, v_offset, minvel, maxvel);

      //scalefn(nn_output(1), u(i,j), u_slope, u_offset, minvel, maxvel);
      dfscalefn(nn_output(1), dfnn_output(1), dfu(i,j), param.u_slope, u_slope, param.u_offset, u_offset, minvel, maxvel);


       //now putback the values in <dfnnn_output> vector into
       //the  <dfoutput_layer> ragged matrix

       nn_dfcases(num_nn, dfnn_output, dfoutput_layer);

       for(int s = 1; s <= num_nn; s++)
       {
          //output_layer =  nn(s).output_comp(input_layer(s), squash_flag(s));
          dfoutput_comp(param.nn(s), this->nn(s), dfoutput_layer(s), squash_flag(s) );
       } //for s

    } //for i
  }  // for j
}
template void par_t_nn<d3_array,dmatrix,dvector,double>::dfuvs_comp(par_t_nn<d3_array,dmatrix,dvector,double>& param, dmatrix& dfu, dmatrix& dfv, dmatrix& dfsigma, year_month& ym);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t_nn<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::dfuvs_comp(par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>& param, dmatrix& dfu, dmatrix& dfv, dmatrix& dfsigma, year_month& ym)
{

  ivector display_nodes =  choose_nn_input_layer_display(input_controls);

  if(display_nodes(10)  )
       habitat_data(wsc_file_ext, ym, wstress_curl);
  if(display_nodes(12)  )
       habitat_data(forage_file_ext, ym, forage);
  if(display_nodes(13)  )
       habitat_data(sst_file_ext, ym, sst);
  if(display_nodes(14)  )
       habitat_data(zonal_current_file_ext, ym, zonal_current);
  if(display_nodes(15)  )
       habitat_data(meridional_current_file_ext, ym, meridional_current);
  if(display_nodes(16)  )
       habitat_data(habitat_file_ext, ym, habitat);

  //get stuff from neural_net class
   imatrix mncol(1,num_nn,1, param.num_levels-1);
   ivector last_layer(1,num_nn);
   ivector squash_flag(1,num_nn);

   for(int s = 1; s <= num_nn; s++)
   {
     mncol(s) = param.nn(s).get_ncol();
     last_layer(s) =param.nn(s).get_nlayer();
     squash_flag(s) = param.nn(s).nn_ipar(3);
   }

   ivector vinput(1,num_nn);
   ivector voutput(1,num_nn);

   for(int s =1; s <= num_nn; s++)
   {
     vinput(s) = mncol(s,1);
     voutput(s) = param.nodes_at_layer(s,last_layer(s));
   }

   dmatrix input_layer(1,num_nn,1,vinput);     input_layer.initialize();
   dmatrix output_layer(1,num_nn,1,voutput);   output_layer.initialize();

   dmatrix dfoutput_layer(1,num_nn,1,voutput); dfoutput_layer.initialize();
   dvector nn_output(1,3);   //for u, v, and sigma AND in THIS SEQUENCE ONLY!
   dvector dfnn_output(1,3);  //for u, v, and sigma AND in THIS SEQUENCE ONLY!

  double o3 = 0.0;
  double dfo3 = 0.0;


  for (int i = m; i >= 1; i--)
  {
    int j1 = jlb(i);
    int jn = jub(i);
    for (int j = jn; j >= j1; j--)
    {
      //get input and output for each NN
      for(int s = 1; s <= num_nn; s++)
      {
        input_layer(s) =  param.get_input_layer(s, ym, i, j);
        output_layer(s) = param.nn(s).output_comp(input_layer(s), squash_flag(s) );
      }

      //put the <output_layer> ragged matrix  values into a vector
      nn_output = nn_cases<MATRIX,VECTOR>(num_nn,  output_layer);
      dfnn_output.initialize();  //this is neccessary

      //derivative computations
      if (m_ipar[80] == 0)
      {
        //scale the values to use in the main model
        if( param.m_ipar[77] == 0 )
          //sigma(i,j) = scale_fn(output_layer(3), sig_slope, sig_offset, minsig, maxsig);
          dfscale_fn(nn_output(3), param.sig_slope, param.sig_offset, dfnn_output(3),
                                 sig_slope, sig_offset, dfsigma(i,j),  minsig, maxsig);
        else if(param.m_ipar[77] == 1)
        {
          o3 = nn_output(3) * nn_output(3);
          //sigma(i,j) = scale_fn(o3, sig_slope, sig_offset, minsig, maxsig);
          dfscale_fn(o3, param.sig_slope, param.sig_offset, dfo3, sig_slope,
                                sig_offset, dfsigma(i,j), minsig, maxsig);

          // o3 = nn_output(3) * nn_output(3);
          dfnn_output(3) += 2.0*nn_output(3)*dfo3;
          dfo3 = 0.0;
        }
        else if(param.m_ipar[77] == 2)
        {
          //recompute
          double rs = scale_fn(nn_output(3), param.sig_slope, param.sig_offset, minsig, maxsig);

          double dfrs = 0.0;
          //sigma(i,j) = rs*rs;
          dfrs += 2.0*rs*dfsigma(i,j);
          dfsigma(i,j) = 0.0;

          //double rs = scale_fn(nn_ouput(3), sig_slope, sig_offset, min_sig, maxsig);
          dfscale_fn(nn_output(3), param.sig_slope, param.sig_offset, dfnn_output(3),
                  sig_slope, sig_offset, dfrs,  minsig, maxsig);
          dfrs = 0.0;

        }
        else if(param.m_ipar[77] == 3)
        { 
          //double dfuniform_sigma = 0;

          //sigma(i,j) = uniform_sigma;
          uniform_sigma += dfsigma(i, j);
          dfsigma(i, j) = 0;

          //dfuniform_sigma = 0;
        }
        else
        {
          cerr <<"illegal value of m_ipar[77]: " << m_ipar[77] << endl;
          cerr <<"this cannot happen. " << endl;
          exit(0);
        }
      }
      else // (m_ipar[80] == 1)
      {
        //do not scale the values to use in the main model
        if(m_ipar[77] == 0 )
        {
          //sigma(i,j) = nn_output(3);
          dfnn_output(3) += dfsigma(i,j);
          dfsigma(i,j) = 0.0;
        }
        else if(m_ipar[77] == 1)
        {
          //sigma(i,j) = o3;
          dfo3 += dfsigma(i,j);
          dfsigma(i,j) = 0.0;

          //o3 = nn_output(3) * nn_output(3);
          dfnn_output(3) += 2.0*nn_output(3)*dfo3;
          dfo3 = 0.0;
        }
        else if(m_ipar[77] == 2) // in this case same as (m_ipar[77] == 1)
        {
          // recompute
          double rs = nn_output(3);

          double dfrs = 0.0;
          //sigma(i,j) = rs * rs;
          dfrs += 2.0*rs*dfsigma(i,j);
          dfsigma(i,j) = 0.0;

          //double rs = nn_output(3);
          dfnn_output(3) += dfrs;
          dfrs = 0.0;
        }
        else if(param.m_ipar[77] == 3)
        { 
          //double dfuniform_sigma = 0;

          //sigma(i,j) = uniform_sigma;
          uniform_sigma += dfsigma(i, j);
          dfsigma(i, j) = 0;

          //dfuniform_sigma = 0;
        }
	else
        {
          cerr <<"illegal value of m_ipar[77]: " << m_ipar[77] << endl;
          cerr <<"this cannot happen. " << endl;
          exit(0);
        }
        //v(i,j) = nn_output(2);
        //u(i,j) = nn_output(1);
      }

      //scalefn(nn_output(2), v(i,j), v_slope, v_offset, minvel, maxvel);
      dfscalefn(nn_output(2), dfnn_output(2), dfv(i,j), param.v_slope, v_slope, param.v_offset, v_offset, minvel, maxvel);

      //scalefn(nn_output(1), u(i,j), u_slope, u_offset, minvel, maxvel);
      dfscalefn(nn_output(1), dfnn_output(1), dfu(i,j), param.u_slope, u_slope, param.u_offset, u_offset, minvel, maxvel);


       //now putback the values in <dfnnn_output> vector into
       //the  <dfoutput_layer> ragged matrix

       nn_dfcases(num_nn, dfnn_output, dfoutput_layer);

       for(int s = 1; s <= num_nn; s++)
       {
          //output_layer =  nn(s).output_comp(input_layer(s), squash_flag(s));
          dfoutput_comp(param.nn(s), this->nn(s), dfoutput_layer(s), squash_flag(s) );
       } //for s

    } //for i
  }  // for j
}
template void par_t_nn<d3_array,dmatrix,dvector,double>::dfuvs_comp(par_t_nn<d3_array,dmatrix,dvector,double>& param, dmatrix& dfu, dmatrix& dfv, dmatrix& dfsigma, year_month& ym);
template<> void par_t_nn<d3_array,dmatrix,dvector,double>::dfuvs_comp(par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>& param, dmatrix& dfu, dmatrix& dfv, dmatrix& dfsigma, year_month& ym)
{
}

void dfscale_fn(const double x, const double slope, const double offset, double& dfx, double& dfslope, double& dfoffset, double& dfy, const double min, const double max)
{
  double range = max - min;
  double arg = slope*(x-offset);

  double dfarg = 0.0;

  //double y = min + range*(atan(arg)/M_PI + 0.5);
  dfarg += range*dfy/M_PI*(1.0/(1.0+arg*arg));
  dfy = 0.0;

  //double arg = slope*(x-offset);
  //double arg = slope*x - slope*offset);
  dfslope  += dfarg*x;
  dfslope  -= dfarg*offset;
  dfx      += dfarg*slope;
  dfoffset -= dfarg*slope;
  dfarg = 0.0;
}
