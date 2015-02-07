#include "par_treg.h"

extern ofstream clogf;

int max(const imatrix& m);

par_t_reg::par_t_reg(cifstream& prn): par_t(prn)
{
  //par_file_version = "#v20r\0";
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
      stratum_active[s][k] = 1;
      usergrid[s][k].u = 0.01;
      usergrid[s][k].v = 0.01;
      usergrid[s][k].sigma = 5000.0;
    }
  }
}//End Constructor
