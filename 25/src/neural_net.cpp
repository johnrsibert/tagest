//$Id: neural_net.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "neural_net.h"
#include "trace.h"

#include <iomanip>
using std::ios;
using std::setiosflags;
using std::setprecision;

extern ofstream clogf;
float ran1(int * idum);

//default constructor
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
neural_net<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::neural_net()
{
  nlayer = 0;
  nn_ipar.allocate(30);
}

/*
neural_net::neural_net(const neural_net& nn)
 : nn_ipar(nn.nn_ipar)
{
   nlayer = 0;
   //nn_ipar.allocate(30);
 }
*/

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
neural_net<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::~neural_net()
{
  /* leave empty */
  //cout << "neural_net::~neural_net()" << endl;
}

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void neural_net<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::initialize()
{
  nodes.initialize();             //just in case
  weights.initialize();           //just in case
  bias.initialize();              //just in case
  diff_hwts.initialize();
}
template void neural_net<d3_array,dmatrix,dvector,double>::initialize();
template void neural_net<dvar3_array,dvar_matrix,dvar_vector,dvariable>::initialize();

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void neural_net<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::set_nn( ivector _nodes_at_layers )
{
 TRACE(_nodes_at_layers);
 allocate( _nodes_at_layers );
}



template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void neural_net<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::allocate(const ivector& nodes_at_layers)
{
  nlayer = nodes_at_layers.size();
  nodes.allocate(1,nlayer,1,nodes_at_layers);

  nrow.allocate(1,nlayer-1);
  ncol.allocate(1,nlayer-1);

  //for declaring weigths
  for(int i = 1; i < nlayer; i++)
  {
     nrow(i) = nodes_at_layers(i+1);
     ncol(i) = nodes_at_layers(i);
  }
  weights.allocate(1,nlayer-1, 1,nrow, 1,ncol);

  //int r1 = nrow.indexmax();
  //int c1 = ncol.indexmax();
  //TTRACE(nrow(r1),ncol(c1))
  //TTRACE(nrow(nlayer-1),ncol(nlayer-1));
  //HALT
  diff_hwts.allocate(1,nrow(nlayer-1),1,ncol(nlayer-1));



  //for declaring bias
  ivector iv(1, nlayer-1);
  for(int i = 1; i < nlayer; i++)
  {
    iv(i) = nodes_at_layers(i+1);
  }
  bias.allocate(1,nlayer-1,1,iv);   //unique node to each hidden & output_layer's nodes
  layers.allocate(1,nlayer);
  layers = nodes_at_layers;
  nn_ipar.allocate(30);
}
template void neural_net<d3_array,dmatrix,dvector,double>::allocate(const ivector& nodes_at_layers);
template void neural_net<dvar3_array,dvar_matrix,dvar_vector,dvariable>::allocate(const ivector& nodes_at_layers);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void neural_net<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::make_random_weights(int& seed, int low, int high)
{
   int &tseed = seed;
   if( tseed == 1 )
       weights = 0.1;  //constant small weight for all
  else
   {
      int nwts = size_count(weights);
      dvector tmp(1,nwts);
      tmp.fill_randu(tseed);
      tmp = 2.0*tmp-1.0;  //change to -1 to +1

      int n = 0;
      for(int i = 1; i < nlayer; i++)
        {
           for(int j = 1; j <= nrow(i); j++)
           {
             for(int k =1; k <= ncol(i); k++)
             {
               n++;
               weights(i,j,k) = tmp(n) * 0.05;
             }
           } // for j
        } //for i
       init_hidden_output_weights(weights[nlayer-1], tseed,low,high);
     } //else

    seed = tseed;
   } // End func
template void neural_net<d3_array,dmatrix,dvector,double>::make_random_weights(int& seed, int low, int high);
template void neural_net<dvar3_array,dvar_matrix,dvar_vector,dvariable>::make_random_weights(int& seed, int low, int high);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void neural_net<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::make_random_bias(int& seed)
{
   if( seed == 1)
     bias = 0.0;
   else
   {
    long tseed = seed;
    bias.fill_randu(tseed);
    bias = 2.0* bias -1.0;
    seed = tseed;
  }
}
template void neural_net<d3_array,dmatrix,dvector,double>::make_random_bias(int& seed);
template void neural_net<dvar3_array,dvar_matrix,dvar_vector,dvariable>::make_random_bias(int& seed);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
VECTOR neural_net<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::output_comp(dvector& input, const int squash_flag)
{
  VECTOR output(1,layers(nlayer));
  nodes(1) = input;

  for(int i = 1; i < nlayer; i++)
  {
    for(int j = 1; j <= nrow(i); j++ )
    {
      nodes(i+1,j) = 0.0;
      for(int k = 1; k <= ncol(i); k++)
      {
        nodes(i+1,j) += weights(i,j,k) * nodes(i,k);
      }
       nodes(i+1,j) += bias(i,j); //because  bias's row is indexed from (1,nlayer-1)
    }

    if( (squash_flag == 1) &&  (i < nlayer-1)  )
          sigmoid_squash_func(nodes[i+1]);  //apply the scaling function to each step forward
    else if( (squash_flag == 2) &&  (i < nlayer-1) )
            linear_squash_func(nodes[i+1]);  //apply the scaling function to each step forward
    else if( (squash_flag == 3 ) &&  (i < nlayer-1) )
           logistic_squash_func (nodes[i+1] ); // -- ditto --

   //sigmoid_squash_func(nodes[i+1]);   //apply function including ouputlayer

  }

/*//compact version
  for(int i = 1; i < nlayer; i++)
  {
    nodes[i+1] = weights[i] * nodes[i] + bias[i];
    nodes(i+1) = squash_func(nodes[i+1]); //apply the scaling function to each step forward
  }
*/

  output = nodes(nlayer);
  return output;
}
template dvector neural_net<d3_array,dmatrix,dvector,double>::output_comp(dvector& input, const int squash_flag);
template dvar_vector neural_net<dvar3_array,dvar_matrix,dvar_vector,dvariable>::output_comp(dvector& input, const int squash_flag);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void neural_net<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::sigmoid_squash_func(VECTOR& v)
{
  //returns the nodes in the range -0.5 and +0.5
  int min =  v.indexmin();
  int max = v.indexmax();

  for(int k = min; k <= max; k++)
   {
     DOUBLE tmp = (1.0 / M_PI ) * atan( v(k) );   // -1 and + 1
     v(k)  = tmp;
   }

}
template void neural_net<d3_array,dmatrix,dvector,double>::sigmoid_squash_func(dvector& v);
template void neural_net<dvar3_array,dvar_matrix,dvar_vector,dvariable>::sigmoid_squash_func(dvar_vector& v);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void neural_net<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::logistic_squash_func(VECTOR& v)
{
  //returns nodes in the range 0 and 1
  //int min = v.indexmin();
  const int max = v.indexmax();

  for(int k =1; k <= max; k++)
   {
      DOUBLE tmp = 1.0 / (1.0 + exp(-v(k)) );
      v(k) = tmp;
   }
}
template void neural_net<d3_array,dmatrix,dvector,double>::logistic_squash_func(dvector& v);
template void neural_net<dvar3_array,dvar_matrix,dvar_vector,dvariable>::logistic_squash_func(dvar_vector& v);

//linear squashing function
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void neural_net<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::linear_squash_func(VECTOR& v)
{
  //returns the nodes in the range -0.5 and +0.5
  int min =  v.indexmin();
  int max = v.indexmax();

  for(int k = min; k <= max; k++)
   {
     DOUBLE tmp = (0.05 * v(k) ) * 1.0;
     v(k)  = tmp;
   }

} //end squash_func(dvector& v)
template void neural_net<d3_array,dmatrix,dvector,double>::linear_squash_func(dvector& v);
template void neural_net<dvar3_array,dvar_matrix,dvar_vector,dvariable>::linear_squash_func(dvar_vector& v);

//writing weights in the par_file
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
int neural_net<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::write_weights(ofstream& ostr)
{
  //HERE
  for(int i = 1; i < nlayer; i++)
  {
    ostr <<"# layer " << i << endl;
    for( int j = 1; j <= nrow(i); j++)
    {
      ostr << setw(25)  << setprecision(16) <<  weights(i,j) << endl;
    }
  }

  if(!ostr)
    return(1);
  else
    return(0);
}
template int neural_net<d3_array,dmatrix,dvector,double>::write_weights(ofstream& ostr);
template int neural_net<dvar3_array,dvar_matrix,dvar_vector,dvariable>::write_weights(ofstream& ostr);

//reading weights from the parfile
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
int neural_net<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::read_weights(cifstream& istr)
{
  for(int i = 1; i < nlayer; i++)
  {
    for( int j = 1; j <= nrow(i); j++)
    {
      istr >>  weights(i,j) ;
    }
  }

  if(!istr)
    return(1);
  else
    return(0);
}
template int neural_net<d3_array,dmatrix,dvector,double>::read_weights(cifstream& istr);
template int neural_net<dvar3_array,dvar_matrix,dvar_vector,dvariable>::read_weights(cifstream& istr);

//writing bais terms to the par_file
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
int neural_net<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::write_bias(ofstream& ostr)
{
  ostr <<"# bias "  << endl;
 for(int i = 1; i < nlayer; i++)
   {
     ostr << setw(25)  << setprecision(16) << bias[ i ] << endl;
   }

  if(!ostr)
    return(1);
  else
    return(0);
}
template int neural_net<d3_array,dmatrix,dvector,double>::write_bias(ofstream& ostr);
template int neural_net<dvar3_array,dvar_matrix,dvar_vector,dvariable>::write_bias(ofstream& ostr);

//reading  bais terms from the par_file
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
int neural_net<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::read_bias(cifstream& istr)
{
 for(int i = 1; i < nlayer; i++)
  {
    istr >> bias[ i ];
  }

  if(!istr)
   return(1);
  else
   return(0);
}
template int neural_net<d3_array,dmatrix,dvector,double>::read_bias(cifstream& istr);
template int neural_net<dvar3_array,dvar_matrix,dvar_vector,dvariable>::read_bias(cifstream& istr);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void neural_net<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::init_hidden_output_weights(MATRIX& tmp, int& nseed, int low, int high)
{
  //will only change values at the last level of the d3_array weights
  int nlen = nrow(nlayer-1) * ncol(nlayer-1);
  dvector tr(1,nlen);
  tr.fill_randu(nseed);  //fill  with random nos

  low = int(0.01); high = int(1.0);
  tr = (double)low + ((double)high - (double)low ) * tr; //scale between low and high
  tr = sort(tr);  // sort it in ascending order

  int ii = 0;
  for(int i = 1; i <= nrow(nlayer-1); i++)
  {
    for(int j = 1; j <= ncol(nlayer-1); j++)
     {
       ii ++;
       tmp(i,j) = tr(ii);
      }
  }
} //end func
template void neural_net<d3_array,dmatrix,dvector,double>::init_hidden_output_weights(dmatrix& tmp, int& nseed, int low, int high);
template void neural_net<dvar3_array,dvar_matrix,dvar_vector,dvariable>::init_hidden_output_weights(dvar_matrix& tmp, int& nseed, int low, int high);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void neural_net<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::read_nn_ipar(cifstream& istr, ipar_t& ipar, int n)
{
 //HERE
  cout <<ipar[0] << " nn_ipars read for neural_net(" << n << ")" <<  endl;
  for (int i = 1; i <= ipar[0]; i++)
  {
    istr >> ipar(i);
    if (!istr)
    {
      cerr << "Error reading nn_ipar[" << i << "]" << endl;
      exit(1);
    }
  }
}
template void neural_net<d3_array,dmatrix,dvector,double>::read_nn_ipar(cifstream& istr, ipar_t& ipar, int n);
template void neural_net<dvar3_array,dvar_matrix,dvar_vector,dvariable>::read_nn_ipar(cifstream& istr, ipar_t& ipar, int n);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
 void neural_net<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::set_nn_ipar_names(ipar_t& t, int n)
 {
   //HERE
    //input_layer switches
    cout <<"Setting nn_ipar names for neural_net(" << n <<")" << endl;
    t.set_name(1, "INIT. WEIGHTS: 0=> from par file, 1=> with  0.01; other =>  random seed");
    t.set_name(2, "INIT. BIAS: 0=> from par file, 1=> with  0.01; other =>  random seed");
    t.set_name(3, "Squash func: 0 => none; 1 => sigmoid; 2=> linear; 3 => logistic");
    t[4] = 0;
    t.set_name(5, "Weights: 1 => active , 0 => inactive");
    t.set_name(6, "Bias_nodes: 1 => active , 0 => inactive");

     t[7] = 0;
     t[8] = 0;
     t[9] = 0;
     t[10] = 0;

    t.set_name(11, "Year_flag:   1= active; 0  = inactive");
    t.set_name(12, "Month_flag");
    t.set_name(13, "Longitude_flag");
    t.set_name(14, "Latitude_flag");
    t.set_name(15, "Depth_flag");
    t.set_name(16, "Numfads_flag");
    t.set_name(17, "Elevation_flag");
    t.set_name(18, "Distance_flag");
    t.set_name(19, "Seasonal WStressCurl");
    t.set_name(20, "Spatio-Temporal WStressCurl");
    t.set_name(21, "SOI Index: 1=> active, 0 = inactive");
    t.set_name(22, "Tuna forage(x0.4gWW/m^2)");
    t.set_name(23, "SST field");
    t.set_name(24, "zonal current speed [u]");
    t.set_name(25, "meridional current speed [v]");
    t.set_name(26, "Skipjack habitat field");
    t[27] = 0;
    t[28] = 0;
    t[29]  =0;
    t[30] = 0;
}
template void neural_net<d3_array,dmatrix,dvector,double>::set_nn_ipar_names(ipar_t& t, int n);
template void neural_net<dvar3_array,dvar_matrix,dvar_vector,dvariable>::set_nn_ipar_names(ipar_t& t, int n);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void neural_net<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::init_diff_hwts()
{
  //Make diff_hwts from the hidden layer weights
  int nout = nrow(nlayer-1);
  int nhidden = ncol(nlayer-1);
  if(nhidden < 2 ) { cerr <<" Nodes in hidden layer less than 2. Cannot initialize 'diff_hwts' " << endl;   exit(1); }

  for(int j = 1; j <= nout; j++)
  {
    diff_hwts(j,1) = weights(nlayer-1,j,1);
     for(int k = 2; k <= nhidden; k++)
     {
        diff_hwts(j,k) = weights(nlayer-1,j,k) - weights(nlayer-1,j,k-1);
      }
   }
 } //end
template void neural_net<d3_array,dmatrix,dvector,double>::init_diff_hwts();
template void neural_net<dvar3_array,dvar_matrix,dvar_vector,dvariable>::init_diff_hwts();
