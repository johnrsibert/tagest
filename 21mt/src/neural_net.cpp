#include "neural_net.h"
#include "trace.h"

#include <iomanip>
using std::ios;
using std::setiosflags;
using std::setprecision;

extern ofstream clogf;


//default constructor
neural_net::neural_net()
{
  nlayer = 0;
}


neural_net::~neural_net()
{
  /* leave empty */
  //cout << "neural_net::~neural_net()" << endl;
}

void neural_net::initialize()
{
  nodes.initialize();             //just in case
  weights.initialize();           //just in case
  bias.initialize();              //just in case
}

void neural_net::set_nn( ivector _nodes_at_layers )
{
 TRACE(_nodes_at_layers);
 allocate( _nodes_at_layers );
}



void neural_net::allocate(const ivector& nodes_at_layers)
{
  nlayer = nodes_at_layers.size();
  nrow.allocate(1,nlayer-1);
  ncol.allocate(1,nlayer-1);

  for(int i = 1; i < nlayer; i++)
  {
     nrow(i) = nodes_at_layers(i+1);
     ncol(i) = nodes_at_layers(i);
  }

  weights.allocate(1,nlayer-1, 1,nrow, 1,ncol);
  nodes.allocate(1,nlayer,1,nodes_at_layers);
  bias.allocate(1,nlayer-1);       //one bias node in the input layer & hidden layers
  layers.allocate(1,nlayer);
  layers = nodes_at_layers;
}


void neural_net::make_random_weights(int& seed)
{
   if( seed == 1 )
   {
      for(int n = 1; n <= nlayer; n++)
         weights(n) = 0.01; 
   }
   else
   {
    long tseed = seed;
    weights.fill_randu(tseed);
    seed = tseed;
  }
}


void neural_net::make_random_bias(int& seed)
{
   if( seed == 1)
     bias = 0.0;
   else 
   {
    long tseed = seed;
    bias.fill_randu(tseed);
    seed = tseed;
  }
}

dvector neural_net::output_comp(dvector& input, const int squash_flag)
{
  dvector output(1,layers(nlayer));
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
       nodes(i+1,j) += bias(i);
    }

   if( squash_flag == 1)
          sigmoid_squash_func(nodes[i+1]);  //apply the scaling function to each step forward
   else if( squash_flag == 2 )
           linear_squash_func(nodes[i+1]);  //apply the scaling function to each step forward
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



void neural_net::sigmoid_squash_func(dvector& v)
{
  //returns the nodes in the range -0.5 and +0.5
  int min =  v.indexmin();
  int max = v.indexmax();
  
  double tmp = 0.0;
  for(int k = min; k <= max; k++)
   {
     tmp = (1.0 / M_PI ) * atan( v(k) );  
     v(k)  = tmp;
   }
   
} //end squash_func(dvector& v)


//linear squashing function
void neural_net::linear_squash_func(dvector& v)
{
  //returns the nodes in the range -0.5 and +0.5
  int min =  v.indexmin();
  int max = v.indexmax();
  
  double tmp = 0.0;
  for(int k = min; k <= max; k++)
   {
      tmp = (0.05 * v(k) ) * 1.0;
     v(k)  = tmp;
   }
   
} //end squash_func(dvector& v)



//writing weights terms to the par_file
int neural_net::write_weights(ofstream& ostr)
{
  HERE
  for(int i = 1; i < nlayer; i++)
  {
    ostr <<"#layer " << i << endl;
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


//reading weights from the parfile
int neural_net::read_weights(cifstream& istr)
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



//writing bais terms to the par_file
int neural_net::write_bias(ofstream& ostr)
{

  for(int i = 1; i < nlayer; i++)
  {
    ostr << setw(25)  << setprecision(16) << bias[ i ] << endl;
  }

  if(!ostr)
    return(1);
  else
    return(0);
}


//reading  bais terms from the par_file
int neural_net::read_bias(cifstream& istr)
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



