#include "par_t_nn.h"
#include "trace.h"

float ran1(int * idum);
void uvs_freq_comp(const dmatrix& unode, const dmatrix& vnode, const dmatrix& snode, const imatrix& gridmap, const int report);
extern ofstream clogf;
extern _global_report_flag;

void par_t_nn::uvs_comp(dmatrix& u, dmatrix& v, dmatrix& sigma, year_month& ym)
{
 
  
 //because binary data is read on the fly
 if( m_ipar[160] )
 {
   get_wstress_curl(ym, wstress_curl); 
  }
  
  //iobjects for uvs_diagnose function
  dmatrix unode(1,m,1,n);  unode.initialize();
  dmatrix vnode(1,m,1,n);  vnode.initialize();
  dmatrix snode(1,m,1,n);   snode.initialize();


   //get stuff from neural_net class
   ivector ncol = nn.get_ncol();
   dvector input_layer(1,ncol(1)); 
   input_layer.initialize(); 
   
   ivector nodes_at_layers = nn.get_layers();
   int last_layer = nn.get_nlayer();
   dvector output_layer(1,nodes_at_layers(last_layer));
   output_layer.initialize();
   
   double o3 = 0.0;
   int squash_flag = m_ipar[71]; // choice of squashing function 
   int  ninput_layers = size_count(input_layer);     // active input layers
   //TTRACE(ninput_layers, nactive_input_layers)

  if( nactive_input_layers != ninput_layers )
  {
    cerr <<"Error..!  nactive_input_layer and neural_net input layers does not match" <<
    "\nnactive_layers is " <<nactive_input_layers <<" while ninput_layers is " << ninput_layers <<"!! "<<   endl;
    exit(1);
  }
    
    for (int i=1; i <= m; i++)
    {
      int j1 = jlb(i);
      int jn = jub(i);
      for (int j = j1; j <= jn; j++)
      {
         //get an input_layer (or an input_pattern!) for this ordered triplet [t,i,j] 
         input_layer = get_input_layer(ym, i, j);

        //calculate neural_net out for the above input layer
         output_layer =  nn.output_comp(input_layer, squash_flag);

         // ---------> note this is not part of main logic.. done only for use in vus_diagnose. see below! < --- //
        //unode(i,j) = output_layer(1);     //first node of the neural_net BEFORE scaled!
        //vnode(i,j) = output_layer(2); 	//second node of the neural_net BEFORE scaled!
        //snode(i,j) = output_layer(3) ;     //third node of the neural_net BEFORE scaled!
            

       //scale the values to use in the main model
       u(i,j)        = scale_fn(output_layer(1), u_slope, u_offset, minvel, maxvel);
       v(i,j)        = scale_fn(output_layer(2), v_slope, v_offset, minvel, maxvel);
       if( m_ipar[72] == 0) 
           sigma(i,j) = scale_fn(output_layer(3), sig_slope, sig_offset, minsig, maxsig); 
       else {  
           o3 = output_layer(3) * output_layer(3);
           sigma(i,j) = scale_fn(o3, sig_slope, sig_offset, minsig, maxsig);
          // HERE
       }
        
        // ----------- > not part of the main logic .. done use only for nn_vusfreq function. see below <-------//
        //NOTE  we cannot directly pass u,v,sigma because of the extra set of cells at the boundary!
        unode(i,j) = u(i,j);  //first node of the neural_net AFTER scaling
        vnode(i,j) = v(i,j); 	 //secong nodel of the NN AFTER scaling
        snode(i,j) = sigma(i,j);  //thirs node of the NN AFTER scaling
         
      if( _global_report_flag)
      {
        int seed = m_ipar[40];
        if (ran1(&seed) < 0.1)
          clogf <<"u_scale: " << output_layer(1) << "  "<< u(i,j) << endl;
        if (ran1(&seed) < 0.1)
          clogf <<"v_scale: " << output_layer(2) <<"   " <<v(i,j) << endl;
           
        if (ran1(&seed) < 0.1)
        {
          if(m_ipar[72] == 0 )
            clogf <<"D_scale: "<< output_layer(3) << "  "<< sigma(i,j) << endl;
          else
            clogf <<"D_scale: " << o3 << "  "<< sigma(i,j) << endl;
        }
        m_ipar[40] = seed;
      }
       
  
      } // for i
   }  // for j


 //uvs_diagnose(unode, vnode, snode, u,v,sigma,".uvs");  //for diagnostics


 if( _global_report_flag)
   {
    //uvs_freq_comp(unode,vnode,snode,gridmap,0);
    uvs_freq_comp(unode,vnode,snode,gridmap,0);
   }
 

 //HERE
} //end of uvs_comp

