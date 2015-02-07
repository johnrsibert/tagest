#include <fvar.hpp>
#include "list2.cpp"
#include "shift.h"
#include <adstring.hpp>
#include <fstream.h>
  
extern ofstream clogf;
 

int shift_position(adstring& message, Longitude& i_long, Latitude& i_lat, List<Shift>& ls)
{
  unsigned list_count = ls.NumberofItemsinList(), count = 1;
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
  //Check if island long, lat was found in list//
  if (count == list_count)
  {
     cout << "\nIn function shift_position, enter new position, (eg 130E 25S):";
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
       //add new position to list//
       ls.add(tmp);
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

  //cout << "List: \n" << ls << endl; 

  i_long = gn.get_long();
  i_lat = gn.get_lat();

  return (ls.NumberofItemsinList());
}
