//$Id: savetags.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include <fvar.hpp>
#include "par_t.h"
#include "yr_mon2.h"
#include "trace.h"

extern ofstream clogf;


void save_tags(par_t& param, dvar_matrix& releases, year_month& date, 
               ofstream& savefile)
{
  int _m = param.get_m();
  ivector& _jlb = param.get_jlb();
  ivector& _jub = param.get_jub();

  savefile<< "# year/month" << endl;
  savefile<< date << endl;
  savefile<< "# long" << "\tlat" << "\treleases" << endl;
  for (int i=1; i <= _m; i++)
  {
    int j1 = _jlb(i);
    int jn = _jub(i);
    for (int j=j1; j <= jn;j++)
    {
      if (param.m_ipar[83] == 1) //real numbers
      {
        double drel = value(releases[i][j]);
        if (drel > 0)
        {
          savefile << param.index_to_long(i); //a_move_corner(i, param.get_long_lat_long()) << "\t";
          savefile << param.index_to_lat(j); //a_move_corner(j, param.get_long_lat_lat()) << "\t";
          savefile << drel << endl;
        }//if drel
      }//if m_ipar
      else // whole numbers
      {
        int irel;
        if (releases[i][j] < 1e-10)
           irel = 0;
        else
           irel = int(value(releases[i][j]) + 0.5);
        if ( irel >= 1 )
        {
          savefile << param.index_to_long(i); //a_move_corner(i, param.get_long_lat_long()) << "\t";
          savefile << param.index_to_lat(j); //a_move_corner(j, param.get_long_lat_lat()) << "\t";
          savefile << irel << endl;
        }//if irel
      }//else
    }//for j
  }//for i
}//End save_tags
