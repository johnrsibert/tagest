//#include <adstring.hpp>
//#include "etopo5.h"
#include <fvar.hpp>
#include "neural_net.h"
#include <iostream>

std::ofstream clogf("neural_net.log");

int main(void)
{


ivector i(1,3);
i[1]= 3;
i[2]= 5;
i[3] = 3;

neural_net nn;
cout << (! nn) << endl;
nn.allocate(i);
cout << (! nn) << endl;

nn.make_random_weights(20);
nn.make_random_bias(100);

std::ofstream savewts("weights.dat");
nn.write_weights( savewts );



//cout << nn.get_nlayer() << endl; 
cout << nn.get_weights() << endl;
//cout << nn.get_layers() << endl;
cout << endl << endl;
cout << nn.get_bias(); 




/*
  adstring log_file_name("test.log");

  clogf.open(log_file_name);
  if ( !clogf )
  {
    cerr << "Cannot open file " << log_file_name << endl;
    exit(1);
  }
  cout << "Opened " << log_file_name << endl;
  etopo5_matrix gm;
 
  int e = gm(295,45);
  cout << e << endl;
  gm.test();
  cout << "finished gm.test()" << endl;

*/
  return 0;
}
