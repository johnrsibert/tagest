#include "par_t_nn.h"
#include "trace.h"

extern ofstream clogf;

/* encapsulated function to calculate and recalculate the input layer */
dvector par_t_nn::get_input_layer(year_month& ym, const int lon, const int lat)
{

  /************* 
      lon =  m  = i 
      lat =   n =  j 
  **************/
  
   ivector ncol = nn.get_ncol();
   dvector input_layer(1,ncol(1)); 
   input_layer.initialize();
     
   int mo = ym.get_month_value();
   int yr = ym.get_year(); 
   int  jnode = 0;
  
  if( m_ipar[151] )
    {
      jnode ++; //cout << "yrr " << jnode << endl;
      input_layer(jnode) = scaled_year( yr ); //linear scaled from -1.0 to +1.0
      //TTRACE(ym.get_year(),ym.get_month_value())
    }
  
  if(m_ipar[152])
    {
      jnode++; //cout << "mon  " << jnode << endl;
      input_layer(jnode) = scaled_month(mo );     // cosine scaled
      //TTRACE(input_layer(1),input_layer(2))
  }
  
  if(m_ipar[153])
    {
      jnode ++; //cout << "lon " << jnode << endl;
      input_layer(jnode) = scaled_longitude( lon  ); // longitude values scaled from -1.0 to +1.0
      //TTRACE(input_layer(3), index_to_long( lon ) )
  }
  
  if(m_ipar[154])
    {
      jnode++;  //cout << "lat  " << jnode << endl;
      input_layer( jnode ) = scaled_latitude( lat ); // latitude values scaled from -1.0 to +1.0
      //TTRACE(input_layer(4), index_to_lat( lat ) )
    }
  
  if( m_ipar[155] )
    {
      jnode++; //cout << "zdt  " << jnode << endl;
      input_layer(jnode) = depth( lon,lat ); //depth scaled from -1.0 to 1.0
    }
  
  if( m_ipar[156] )
    {
      jnode++; //cout << "fad  " << jnode << endl;
      input_layer(jnode) = numfad( lon,lat);  //numfad scaled from -1.0 to 1.0
    }
  
  if( m_ipar[157] )
    {
      jnode ++; //cout << "ele  " << jnode << endl;
      input_layer( jnode ) = elevation( lon,lat); //elevation scaled from -1.0 to 1.0
    }
  
  
  if(m_ipar[158] )
    {
      jnode++; //cout << "dst " << jnode << endl;
      input_layer(jnode) = distance(lon, lat);  //distance scaled from -1.0 from 1.0
    }
  
  if(m_ipar[159] ) 
    {
      if(( mo >= 1 && mo <= 4) ||  ( mo >= 10 && mo <= 12))  
        {
          jnode++; //cout << "wne " << jnode << endl;
          input_layer(jnode) = wstress_ne( lon,lat);  //ne_wind scaled from -1.0 from 1.0
        }
        
        if( mo >= 5 && mo <= 9 )
        {
          jnode++; //cout <<"wsw " << jnode << endl;
          input_layer(jnode) = wstress_sw(lon, lat); //se_wind scaled from -1.0 to + 1.0
        } 
   }
  
  if(m_ipar[160] )
    {
      jnode++;  //cout << "wsc " << jnode << endl;
      input_layer(jnode) = wstress_curl(lon, lat);  //windstress_curl -1.0 from 1.0
    }

  if(m_ipar[161] )
   {
     jnode++;  //cout <<"soi " << jnode << endl;
     input_layer(jnode) = soi_index(yr,mo);  //soi_index scaled from -1.0 to 1.0
    //clogf << yr << "  " << mo <<"   "<<  soi_index(yr,mo) << endl;
   }
  
  //cout << __FILE__<<":"<< input_layer << endl;
  return (input_layer);
  
 } // void par_t_nn::compute_input_layer(dvector& input_layer, year_month& ym)