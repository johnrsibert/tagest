//$Id: recap_ck3.cpp 3162 2014-04-26 01:29:31Z jsibert $
#include <iostream>
//#include <sstream>

#include "preproc3.h"
#include "longlat.h"
#include "longtude.h"
#include "latitude.h"
#include "trace.h"

//const int D_SLASH_POS = 2;
//const int CHAR_SIZE = 9;


extern ofstream clogf;

/** Parse tag recapture record.
\param[out] cap_date adstring containing the recapture date
\param[out] cap_lat adstring containing the recapture latitude
\param[out] cap_long adstring containing the recapture longitudd
\param[out] cap_gear adstring containing the recapture gear
\param[out] cap_fleet adstring containing the recapture flag
\param[out] cap_fleet_gear adstring containing the recapture flag/gear (fleet) 
\param[out] tag_number identification number of tag
\param[in] recap_record tag recapture input record
\return 1 for valid recapture record; 0 for invalid record
*/

int cap_rec_ok(year_month& cap_date, Latitude& cap_lat, Longitude& cap_long,
               adstring& cap_gear, adstring& cap_fleet, 
               adstring& cap_fleet_gear, //adstring& fleets_tmp, 
               adstring& tag_number, adstring& recap_record)
{
  //istrstream outs((char*)recap_record);
  istringstream outs((char*)recap_record);

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
  TRACE(state)
  outs >> tag_number;
  if( (!outs) || (tag_number == "NA"))
    state = 0;
  outs >> cap_date;
  if(!outs)
    state = 0;
  outs >> cap_lat;
  if(!outs)
    state = 0;
  outs >> cap_long;
  if (!outs)
    state = 0;
  outs >> cap_gear;
  if ((!outs) || (cap_gear == "NA") || (cap_gear == "OT") )
    state = 0;
  outs >> cap_fleet;
  if ((!outs) || (cap_fleet == "NA"))
    state = 0;

  if (state < 1)
    clogf << "\nstate 0 recap record: " <<recap_record << endl;

  TRACE(state)
  return(state); 
}
