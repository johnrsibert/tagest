//$Id: mpar.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "modlarea.h"

Model_area::Model_area(cifstream& prn)
{
  cout << "In Model_area" << endl;
  if (!prn) cout << "bad " << endl;
  prn >> m >> n >> deltax >> deltay >> sw_coord;
  cout << m << " " << n << " " << sw_coord << endl;
  allocate();
  cout << "In Model_area::alllocate" << endl;
  initialize();
  cout << "In Model_area::initialize" << endl;

  // set to nonsens to verify input
  west_bndry  = -1;
  east_bndry  = -1;
  north_bndry = -1;
  south_bndry = -1;
  cout  << "Finished Model_area::Model_area(cifstream& prn)" << endl;
}//Model_area(cifstream&)
