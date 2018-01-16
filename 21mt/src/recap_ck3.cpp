#include <iostream>
#ifdef unix
  #include <strstream.h>
#else
  #include <strstrea.h>
#endif

#include "preproc3.h"
#include "longlat.h"
#include "longtude.h"
#include "latitude.h"
#include "trace.h"

const int D_SLASH_POS = 2;
const int CHAR_SIZE = 9;


extern ofstream clogf;

int cap_rec_ok(year_month& cap_date, Latitude& cap_lat, Longitude& cap_long,
               adstring& cap_gear, adstring& cap_fleet, 
               adstring& cap_fleet_gear, //adstring& fleets_tmp, 
               adstring& tag_number, adstring& recap_record)
{
  istrstream outs((char*)recap_record);

  /* Examples of recapture records
                     AY02118                U
                     AY02992 77/11          PL SB
                     SK18090       17S 179E PL FJ
                     AB07070 77/10 08S 155E PL
                     AB07401 78/12 10S 159E PL SB
0000000001111111111222222222233333333334444444444555555
1234567890123456789012345678901234567890123456789012345
  */
  int state = 1;  
  outs >> tag_number;
  if(!outs)
    state = 0;
  outs >> cap_date;
  if(!outs)
    state = 0;
  outs >> cap_lat;
  if(!outs)
    state = 0;
  outs >> cap_long;
  if(!outs)
    state = 0;
  outs >> cap_gear;
  if(!outs)
    state = 0;
  outs >> cap_fleet;
  if(!outs)
    state = 0;

  return(state); 
}
