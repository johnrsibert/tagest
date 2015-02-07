//$Id: neural_net.h 2754 2011-01-02 20:57:07Z jsibert $
#ifndef NEURAL_NET_H
#define NEURAL_NET_H

#include <fvar.hpp>
#include <cifstrem.h>
#include "ipar.h"

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
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
  VECTOR output_comp(dvector& input, const int squash_flag);
  void sigmoid_squash_func(VECTOR& v);
  void logistic_squash_func(VECTOR& v);
  void linear_squash_func(VECTOR& v);
  void init_hidden_output_weights(MATRIX& t, int& nseed, int low, int high);
  void init_diff_hwts(); //neural_net.cpp

  void set_nn( ivector );
  int operator !() const { return (nlayer == 0);   }
  void initialize();

  ipar_t nn_ipar;
  void set_nn_ipar_names(ipar_t & t, int n);
  void read_nn_ipar(cifstream& istr, ipar_t& ipar, int n);

  // access functions
  int get_nlayer(void) const { return nlayer; }
  ivector get_nrow(void) const { return nrow; }
  ivector get_ncol(void) const { return ncol; }
  ivector get_layers(void) const {return layers; }
  MATRIX get_nodes(void) const { return nodes; }
  MATRIX get_bias(void) const {return bias;}
  D3_ARRAY  get_weights(void) const { return weights; }
  MATRIX get_diff_hwts(void) const {return diff_hwts; }

  //dfparam access functions
  MATRIX& get_bias(void) { return bias; }
  D3_ARRAY&  get_weights(void) { return weights; }
  MATRIX& get_diff_hwts(void) {return diff_hwts; }

  void make_random_weights(int& seed, int low, int high); //filling weights with random nos.
  void make_random_bias(int& seed);     //filling bias with random nos.

  int write_weights(ofstream& ostr);    //write the weights to an outputfile
  int read_weights(cifstream& istr);  		//not defined

  int write_bias(ofstream& ostr);    		//write bias
  int read_bias(cifstream& istr);				//read bias

public:
  int nlayer;           	//no. of layers
  ivector nrow;        	//no. of rows in the ragged array
  ivector ncol;         	//no. of cols in the ragged array
  ivector layers;       //no. of nodes at layers
  D3_ARRAY weights;  //d3_array of weights of the nueral_net
  MATRIX nodes;        //matrix array of nodes
  MATRIX bias;            //bias nodes
  MATRIX diff_hwts;    //differences in hidden layer weights

  //friend void dfoutput_comp(neural_net& nn, neural_net& dfnn, dvector& output, const int squash_flag);
};

#endif
