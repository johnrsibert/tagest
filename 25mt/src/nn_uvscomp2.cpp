//$Id: nn_uvscomp2.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "par_t_nn.h"
#include "trace.h"

float ran1(int * idum);
void uvs_freq_comp(const dmatrix& unode, const dmatrix& vnode,
                   const dmatrix& snode, const imatrix& gridmap,
		   const int report);
dvector nn_cases(int num_nn, dmatrix output_layer);
extern ofstream clogf;
extern _global_report_flag;
ivector choose_nn_input_layer_display(imatrix &input_controls);  // in <nn_input_layer.cpp>

void par_t_nn::uvs_comp(dmatrix& u, dmatrix& v, dmatrix& sigma, year_month& ym)
{
  ivector display_nodes =  choose_nn_input_layer_display(input_controls);

  if(display_nodes(10)  )
       habitat_data(wsc_file_ext, ers_data_start_date, ym, wstress_curl);
  if(display_nodes(12)  )
       habitat_data(forage_file_ext, habitat_data_start_date, ym, forage);
  if(display_nodes(13)  )
       habitat_data(sst_file_ext, habitat_data_start_date, ym, sst);
  if(display_nodes(14)  )
       habitat_data(zonal_current_file_ext, habitat_data_start_date, ym, zonal_current);
  if(display_nodes(15)  )
       habitat_data(meridional_current_file_ext, habitat_data_start_date, ym, meridional_current);
  if(display_nodes(16)  )
       habitat_data(habitat_file_ext, habitat_data_start_date, ym, habitat);

  //iobjects for uvs_diagnose function
  dmatrix unode(1,m,1,n);  unode.initialize();
  dmatrix vnode(1,m,1,n);  vnode.initialize();
  dmatrix snode(1,m,1,n);   snode.initialize();


  //get stuff from neural_net class
  imatrix mncol(1,num_nn,1, num_levels-1);     mncol.initialize();
  ivector last_layer(1,num_nn);

  for(int s = 1; s <= num_nn; s++)
  {
    mncol(s) = nn(s).get_ncol();
    last_layer(s) = nn(s).get_nlayer();
  }
  ivector vinput(1,num_nn);
  ivector voutput(1,num_nn);
  ivector squash_flag(1,num_nn);

  for(int s =1; s <= num_nn; s++)
  {
    vinput(s) = mncol(s,1);
    voutput(s) = nodes_at_layer(s, last_layer(s));
    squash_flag(s) = nn(s).nn_ipar(3);
  }

  //ragged matrices
  dmatrix input_layer(1,num_nn,1,vinput);     input_layer.initialize();
  dmatrix output_layer(1,num_nn,1,voutput);     output_layer.initialize();

  dvector nn_output(1,3);   //for u, v, and sigma AND in THIS SEQUENCE ONLY!

  double o3 = 0.0;

  ivector ninput_layers(1,num_nn); ninput_layers.initialize();
  for(int s = 1; s <= num_nn; s++)
  {
    ninput_layers(s) = size_count(input_layer(s) );  // active input layers
  }

  for(int s = 1; s <= num_nn; s++)
  {
    if( nactive_input_layers(s)  != ninput_layers(s)  )
    {
      cerr <<"Error..!  nactive_input_layer and neural_net input layers does not match" <<
          "\nnactive_layers is " <<nactive_input_layers <<" while ninput_layers is " << ninput_layers <<"!!"<< endl;
          exit(1);
    }
  } //for s

  //loop through each grid in i,j plan
  for (int i=1; i <= m; i++)
  {
    int j1 = jlb(i);
    int jn = jub(i);
    for (int j = j1; j <= jn; j++)
    {
      for(int s = 1; s <= num_nn; s++)
      {
        input_layer(s)  = get_input_layer(s,  ym, i, j);
        output_layer(s) =  nn(s).output_comp(input_layer(s), squash_flag(s) );
        //HALT
      }
      //clogf <<"nnout:  ("<<i<<","<<j<<")   "<< input_layer(2,1) <<"   " << input_layer(2,2) <<"  " << input_layer(2,3) << "    " 
      //	    << input_layer(2,4) <<"   " << input_layer(2,5) <<"  " << input_layer(2,6) << endl; 
      //	                              << input_layer(2,5) <<"   " << input_layer(2,6) <<"  " << input_layer(2,7) << "   " 
      //	                              << input_layer(2,8) << endl; 
      
      //put the <output_layer> ragged matrix values into av vector
      nn_output = nn_cases(num_nn,  output_layer);

      if (m_ipar[80] == 0)
      {
        //scale the values to use in the main model
        u(i,j) = scale_fn2(nn_output(1), xmin_uv, xmax_uv, minvel, maxvel);
        v(i,j) = scale_fn2(nn_output(2), xmin_uv, xmax_uv, minvel, maxvel);
        if(m_ipar[77] == 0 )
        {
          sigma(i,j) = scale_fn2(nn_output(3), xmin_sig, xmax_sig, minsig, maxsig);
        }
        else if(m_ipar[77] == 1)
        {
          o3 = nn_output(3) * nn_output(3);
          sigma(i,j) = scale_fn2(o3, xmin_sig, xmax_sig, minsig, maxsig);
        }
        else if(m_ipar[77] == 2)
        {
          double rs = scale_fn2(nn_output(3), xmin_sig, xmax_sig, minsig, maxsig);
          sigma(i,j) = rs * rs;
        }
	  else
        {
          cerr <<"illegal value of m_ipar[77]: " << m_ipar[77] << endl;
          exit(0);
        }
      }
      else // (m_ipar[80] == 1)
      {
        //do not scale the values to use in the main model
        u(i,j) = nn_output(1);
        v(i,j) = nn_output(2);
        if(m_ipar[77] == 0 )
        {
          sigma(i,j) = nn_output(3);
          cerr << "\nsigma(" << i <<","<<  j <<") may be negative!  (" << sigma(i,j) << "!!)" << endl;
          exit(0);
        }
        else if(m_ipar[77] == 1)
        {
          o3 = nn_output(3) * nn_output(3);
          sigma(i,j) = o3;
        }
        else if(m_ipar[77] == 2) // in this case same as (m_ipar[77] == 1)
        {
          double rs = nn_output(3);
          sigma(i,j) = rs * rs;
        }
	   else
        {
          cerr <<"illegal value of m_ipar[77]: " << m_ipar[77] << endl;
          exit(0);
        }
      }

     // ---- > not part of the main logic .. for nn_vusfreq function. see below <-------//
     //NOTE  we cannot directly pass u,v,sigma because of the extra set of cells at the boundary!
     unode(i,j) = u(i,j);  //first node of the neural_net AFTER scaling
     vnode(i,j) = v(i,j);    //secong nodel of the NN AFTER scaling
     snode(i,j) = sigma(i,j);  //thirs node of the NN AFTER scaling


     if( _global_report_flag)
     {
       int seed = m_ipar[40];
       int save_ios = clogf.setf(ios::scientific,ios::floatfield);
       if (ran1(&seed) < 0.1)
         clogf <<"u_scale: " << nn_output(1) << "  "<< u(i,j) << endl;
       if (ran1(&seed) < 0.1)
         clogf <<"v_scale: " << nn_output(2) <<"   " <<v(i,j) << endl;

       if (ran1(&seed) < 0.1)
       {
         if( m_ipar[77]  == 0 )
           clogf <<"D_scale: "<< nn_output(3) << "  "<< sigma(i,j) << endl;
         else if(m_ipar[77] == 1)
           clogf <<"D_scale: " << o3 << "  "<< sigma(i,j) << endl;
         else if(m_ipar[77] == 2)
         {
           clogf <<"D_scale: " << nn_output(3)  << "  " << sigma(i,j) << endl;
         }
         }
         m_ipar[40] = seed;
         clogf.setf(save_ios);
       }
     //clogf << "nnout: " << u(i,j) << "    " << v(i,j) <<"   "<< sigma(i,j) <<  endl;
     } // for i
  }  // for j

  if( _global_report_flag)
       uvs_freq_comp(unode,vnode,snode,gridmap,0);
} //end of uvs_comp
