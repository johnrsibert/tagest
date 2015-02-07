#ifndef NEURAL_NET_H
#define NEURAL_NET_H

#include <fvar.hpp>
#include <cifstrem.h>

class neural_net
{
 public:
  //constructors
  neural_net(); 
  neural_net(const neural_net& nn);  //declared but not defined
  neural_net(const ivector& nodes_at_layers) { allocate(nodes_at_layers); }
  ~neural_net(); 
   
   //Functions
  void allocate(const ivector& nodes_at_layers);
  dvector output_comp(dvector& input, const int squash_flag);
  void sigmoid_squash_func(dvector& v); 
  void linear_squash_func(dvector& v);
 
  void set_nn( ivector ); 
  int operator !() const { return (nlayer == 0);   }
  void initialize();

  // access functions
  int get_nlayer(void) const { return nlayer; } 
  ivector get_nrow(void) const { return nrow; } 
  ivector get_ncol(void) const { return ncol; } 
  ivector get_layers(void) const {return layers; } 
  dmatrix get_nodes(void) const { return nodes; }
  dvector get_bias(void) const { return bias; }
  d3_array  get_weights(void) const { return weights; } 
  
  
  dvector& get_bias(void) { return bias; }
  d3_array&  get_weights(void) { return weights; } 
  

  void make_random_weights(int& seed); //filling weights with random nos.
  void make_random_bias(int& seed);       //filling bias with random nos.
  
  int write_weights(ofstream& ostr);          //write the weights to an outputfile
  int read_weights(cifstream& istr);  		//not defined

  int write_bias(ofstream& ostr);    		//write bias
  int read_bias(cifstream& istr);				//read bias

 protected:
  int nlayer;           	//no. of layers 
  ivector nrow;        	//no. of rows in the ragged array
  ivector ncol;         	//no. of cols in the ragged array
  ivector layers;       //no. of nodes at layers
  d3_array weights;  //d3_array of weights of the nueral_net
  dmatrix nodes;        //matrix array of nodes
  dvector bias;         	//bias nodes

  friend void dfoutput_comp(neural_net& nn, neural_net& dfnn, dvector& output, const int squash_flag);
};

#endif
