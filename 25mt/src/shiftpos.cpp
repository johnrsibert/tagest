//$Id: shiftpos.cpp 2845 2011-08-22 23:21:22Z jsibert $
/** \file shiftposs.cpp
Position shifting.
*/
#include "modlarea.h"
#include "list2.cpp"
#include "shift.h"
#include <fvar.hpp>
#include <adstring.hpp>
#include "trace.h"
  
extern ofstream clogf;

int Model_area::shift_position(adstring& message, Longitude& i_long, Latitude& i_lat, List<Shift>& ls)
{
  unsigned list_count = ls.NumberofItemsinList();
  unsigned count = 1;
  Geo_coord gc(i_long, i_lat);
  Geo_coord gi, gn;
  Shift ts;
  while (count <= list_count)
  {
    ts = ls.getItem(count);
    gi = ts.get_island();
    //island position in list, so fetch new values//
    if ( gc == gi)
    {
       gn = ts.get_new_pos();
       break;
    }
    count++;
  }
  count--;

  if (count == list_count)
  {
     // came to end of list without finding match
     
     // look in the 8 nearby cells
     int water = 0;
     int k = 0;
     const int ii = long_to_index(i_long);
     const int jj = lat_to_index(i_lat);
     while (!water && k < 8)
     {
        k ++;
        const int i = ii + di(k);
        const int j = jj + dj(k);
        if (i >= 1 && i <= m && j >= 1 && j <= n)
        {
           water = (gridmap(i,j) > 0);
           if (water)
           {
              Longitude o_long(index_to_long(i));
              Latitude  o_lat(index_to_lat(j));
              gn.set(o_long, o_lat);
              Shift tmp(gc, gn);
              //add new position to list
              ls.add(tmp);
              cout << "  Land found in nearby cell." << endl;
              clogf << "  Land found in nearby cell." << endl;
           }
        }
     }
     if (!water)
     {
       //no neaby land cells, do it the tedious way
        cout << "  Unable to find land in nearby cells." << endl;
        cout << "  Manually shifting geographic position; enter new position, (eg 140E 15S):";
        char ch;  //peek character//
        if ( (ch = cin.peek()) == '\n' )
        {
          cin.get();  //read and discard//
          //if return pressed then ignore changing new_pos//
          Shift tmp(gc);
          gn = gc;
          //add new position to list//
          ls.add(tmp);
        }
        else
        {
          cin >> gn;
          cin.get();
          Shift tmp(gc, gn);
          //add new position to list
          ls.add(tmp);
        }
     }
  }
  
  cout << message
       << " position " << gc
       << " shifted to " << gn
       << endl;

  clogf << message
       << " position " << gc
       << " shifted to " << gn
       << endl;

  i_long = gn.get_long();
  i_lat = gn.get_lat();

  return (ls.NumberofItemsinList());
}
