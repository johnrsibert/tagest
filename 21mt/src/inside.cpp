#include "modlarea.h"
#include "longtude.h"
#include "latitude.h"
#include "trace.h"

int Model_area::inside(const Longitude& lng, const Latitude& lat)
{
  int state = 0;
  //TTRACE(lng.value(),lat.value())
  int i = long_to_index(lng);
  int j = lat_to_index(lat);
  //TTRACE(i,j)

  if ( (i >= 1) && (i <= m) )
  {
    if ( (j >= jlb(i)) && (j <= jub(i)) )
    {
      state = 1;
    }
  }
  return(state);
}

int Model_area::is_land(const Longitude& lng, const Latitude& lat)
{
  int state = 1;
  //TTRACE(lng.long_lat_value(), lat.long_lat_value())
  int i = long_to_index(lng);
  int j = lat_to_index(lat);
  //TTRACE(i,j)

  if ( (i >= 1) && (i <= m) )
  {
    if ( (j >= jlb(i)) && (j <= jub(i)) )
    {
      if (gridmap(i,j) >0)
      {
        state = 0;
       }
     }
  }
  return(state);
}
