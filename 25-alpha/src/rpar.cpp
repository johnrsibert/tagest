//$Id: rpar.cpp 2948 2012-02-11 01:30:26Z eunjung $
#include "par_t_reg.h"

extern ofstream clogf;

int max(const imatrix& m);

template <>
par_t_reg<d3_array,dmatrix,dvector,double>::par_t_reg(cifstream& prn): par_t<d3_array,dmatrix,dvector,double>(prn)
{
  //Default value
  nseason = 1;

  //Find max cohort in the gridmap 
  ngrid = max(gridmap);
  clogf << "ngrid: " << ngrid << endl;
 
  allocate();
  initialize();
 
  //Defaults
  for (int s = 1; s <= nseason; s++)
  {
    for (int k = 1; k <= ngrid; k++)
    {
      stratum_active[s][k] = 11;
      usergridU[s][k] = 0.01;
      usergridV[s][k] = 0.01;
      usergridSigma[s][k] = 5000.0;
    }
  }

  par_file_version = "#v20r";
}//End Constructor
