#include "par_t_nn.h"
#include "trace.h"

extern ofstream clogf;

void par_t_nn::dfuvs_comp(par_t_nn& param, dmatrix& dfu, dmatrix& dfv, dmatrix& dfsigma, year_month& ym)
{
  //because binary data is read on the fly
  if( m_ipar[160] )
   {
     get_wstress_curl(ym, param.wstress_curl); 
   }

  dvector    output_layer(1,nn.get_layers()(nn.get_nlayer()));
  dvector dfoutput_layer(1,nn.get_layers()(nn.get_nlayer()));

  output_layer.initialize();
  dfoutput_layer.initialize();
  
  double o3 = 0.0;
  double dfo3 = 0.0;
  int squash_flag = m_ipar[71];  // choice of squashing function 

  // to recompute nodes
  ivector ncol = nn.get_ncol();
  dvector input_layer(1,ncol(1) );
  input_layer.initialize();

  int  ninput_layers = size_count(input_layer);     // # inputlayers
  //TTRACE(ninput_layers, param.nactive_input_layers)
  
  //we should not be needing this as this will be taken caref of uvs_comp, msa  4/11/2002 9:53AM
  if( param.nactive_input_layers != ninput_layers )
  {
    cerr <<"Error..!  nactive_input_layer and neural_net input layers does not match" <<
    "\nnactive_layers is " <<param.nactive_input_layers <<" while ninput_layers is " << ninput_layers <<"!! "<<   endl;
    exit(1);
  }
  
 for (int i = m; i >= 1; i--)
  {
    int j1 = jlb(i);
    int jn = jub(i);
    for (int j = jn; j >= j1; j--)
     {  
   
      //get the  input_layer starting from reverse
      input_layer =  get_input_layer(ym, i, j);
    

     //recomputer  output layer for above
     output_layer = param.nn.output_comp(input_layer, squash_flag);
 
      //derivative computations
      //sigma(i,j) = scale_fn(output_layer(3), sig_slope, sig_offset, minsig, maxsig);
      if( m_ipar[72] == 0 )
              dfscale_fn(output_layer(3), param.sig_slope, param.sig_offset, dfoutput_layer(3), 
                                sig_slope, sig_offset, dfsigma(i,j),  minsig, maxsig);
      else
        {
              o3 = output_layer(3) * output_layer(3);
             //sigma(i,j) = scale_fn(o3, sig_slope, sig_offset, minsig, maxsig);
             dfscale_fn(o3, param.sig_slope, param.sig_offset, dfo3, sig_slope, 
                            sig_offset, dfsigma(i,j), minsig, maxsig);

              //double o3 = output_layer(3)*output_layer(3);
              dfoutput_layer(3) += 2.0*output_layer(3)*dfo3;
              dfo3 = 0.0;  
              //HERE
         }

      //v(i,j) = scale_fn(output_layer(2), v_slope, v_offset, minvel, maxvel);
       dfscale_fn(output_layer(2), param.v_slope, param.v_offset,
               dfoutput_layer(2), v_slope, v_offset, dfv(i,j),
                minvel, maxvel);
   
      //u(i,j) = scale_fn(output_layer(1), u_slope, u_offset, minvel, maxvel);
      dfscale_fn(output_layer(1), param.u_slope, param.u_offset,
                 dfoutput_layer(1), u_slope, u_offset, dfu(i,j),
                 minvel, maxvel);
                 
      //output_layer =  nn.output_comp(input_layer, squash_flag);
      dfoutput_comp(param.nn, this->nn, dfoutput_layer, squash_flag);  
      
  
     } //for i
 }  // for j
      
//HERE
} //end of dfuvs_comp



void dfscale_fn(const double x, const double slope, const double offset,
                  double& dfx, double& dfslope, double& dfoffset, double& dfy,
                  const double min, const double max)
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

} // end of dfscale_fn

