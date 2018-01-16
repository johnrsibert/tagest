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
  cout << "In Model_area::inii" << endl;

  //Default these values
  west_bndry  = 1;
  east_bndry  = 1;
  north_bndry = 1;
  south_bndry = 1;
  cout  << " out Model_area" << endl;
}//Model_area(cifstream&)
