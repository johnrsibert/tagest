//$Id: nn_inputlayer.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "par_t_nn.h"
#include "trace.h"

extern ofstream clogf;

/* encapsulated function to calculate and recalculate the input layer */
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
dvector par_t_nn<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::get_input_layer( int _nn, year_month& ym, const int lon, const int lat)
{
 //TTRACE(_nn,ym)
 //TTRACE(lon,lat)

 /*************
  lon =  m = i
  lat =  n = j
 **************/

  ivector ncol = nn(_nn).get_ncol();
  dvector input_layer(1,ncol(1));
  input_layer.initialize();

  int mo = ym.get_month_value();
  int yr = ym.get_year();
  int  jnode = 0;

  if(input_controls(_nn,1) )
    {
      jnode ++;
       input_layer(jnode) = scaled_year( yr ); //linear scaled from -1.0 to +1.0
       //input_layer(jnode) = scaled_year( yr ) * scaled_year( yr); 
       //input_layer(jnode) =  yr; 
       //clogf << "yrr " << scaled_year(yr) << endl;
    }

  if(input_controls(_nn,2) )
    {
      jnode++;
      input_layer(jnode) = scaled_month(mo );     // cosine scaled
      //input_layer(jnode) = scaled_month(mo ) * scaled_month(mo);
      //clogf << "mon  " << scaled_month(mo)  << endl;
  }

  if(input_controls(_nn,3) )
    {
      jnode ++;
      input_layer(jnode) = scaled_longitude( lon  ); // longitude values scaled from -1.0 to +1.0
      //input_layer(jnode) = scaled_longitude( lon  ) * scaled_longitude( lon);
      //input_layer(jnode) =  lon;
      //clogf  << "lon " << scaled_longitude(lon) << endl;
  }

  if(input_controls(_nn,4) )
    {
      jnode++;
      input_layer( jnode ) = scaled_latitude( lat ); // latitude values scaled from -1.0 to +1.0
      //input_layer( jnode ) = scaled_latitude( lat ) * scaled_latitude( lat);
      //input_layer( jnode ) = lat; 
      //clogf<< "lat  " << scaled_latitude(lat) << endl;
    }

  if(input_controls(_nn,5) )
    {
      jnode++;
      input_layer(jnode) = depth( lon,lat ); //depth scaled from -1.0 to 1.0
      //clogf << "zdt  " << depth(lon,lat) << endl;
    }

  if(input_controls(_nn,6) )
    {
      jnode++;
      input_layer(jnode) = numfad( lon,lat);  //numfad scaled from -1.0 to 1.0
      //clogf << "fad  " << numfad(lon,lat) << endl;
    }

  if(input_controls(_nn,7) )
    {
      jnode ++;
      input_layer( jnode ) = elevation( lon,lat); //elevation scaled from -1.0 to 1.0
      //clogf <<"ele  " << elevation(lon,lat) << endl;
    }


  if(input_controls(_nn,8) )
    {
      jnode++;
      input_layer(jnode) = distance(lon, lat);  //distance scaled from -1.0 from 1.0
      //clogf << "dst " << distance(lon,lat)  << endl;
    }

  if(input_controls(_nn,9) )
    {
      if(( mo >= 1 && mo <= 4) ||  ( mo >= 10 && mo <= 12))
        {
          jnode++;
          input_layer(jnode) = wstress_ne( lon,lat);  //ne_wind scaled from -1.0 from 1.0
          //clogf << "wne " << wstress_ne(lon,lat) << endl;
        }

        if( mo >= 5 && mo <= 9 )
        {
          jnode++; ;
          input_layer(jnode) = wstress_sw(lon, lat); //se_wind scaled from -1.0 to + 1.0
         //clogf <<"wsw " << wstress_sw(lon,lat) << endl;
        }
   }

  if(input_controls(_nn,10) )
    {
      jnode++;
      input_layer(jnode) = wstress_curl(lon, lat);  //windstress_curl -1.0 from 1.0
      //clogf  << "wsc " << wstress_curl(lon,lat)  << endl;
    }

  if(input_controls(_nn,11) )
   {
     jnode++;
     input_layer(jnode) = soi_index(yr,mo);  //soi_index scaled from -1.0 to 1.0
     //input_layer(jnode) = soi_index(yr,mo) * soi_index(yr,mo) ;
     //clogf <<"soi  " << soi_index(yr,mo) << endl;
   }

  if(input_controls(_nn,12) )
   {
     jnode++;
     input_layer(jnode) = forage(lon,lat);  
     //clogf <<"forage(" <<lon <<","<<lat<<"): " << forage(lon,lat) << endl;
   }

  if(input_controls(_nn,13) )
   {
     jnode++;  //cout <<"sst " << jnode << endl;
     input_layer(jnode) = sst(lon,lat);  
     //input_layer(jnode) = sst(lon,lat) * sst(lon,lat); 
   }

  if(input_controls(_nn,14) )
   {
     jnode++;  //cout <<"usc " << jnode << endl;
     input_layer(jnode) = zonal_current(lon,lat);  
   }

  if(input_controls(_nn,15) )
   {
     jnode++;  //cout <<"vsc " << jnode << endl;
     input_layer(jnode) = meridional_current(lon,lat);  
   }

  if(input_controls(_nn,16) )
   {
     jnode++;  //cout <<"hab " << jnode << endl;
     input_layer(jnode) = habitat(lon,lat);  //habitat scaled from -1.0 to 1.0
   }


  //cout << __FILE__<<": line "<< __LINE__<< "   nn["<<_nn <<"]:  " << input_layer << endl;
  //cout <<__FILE__<< endl; exit(1);
  return ( input_layer );

}
template dvector par_t_nn<d3_array,dmatrix,dvector,double>::get_input_layer( int _nn, year_month& ym, const int lon, const int lat);
template dvector par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>::get_input_layer( int _nn, year_month& ym, const int lon, const int lat);

ivector choose_nn_input_layer_display(imatrix &input_controls)
{
  /* Purpose: This function takes the  imatrix of <input_controls>
  to get a vector of active display nodes- Shiham 7/27/2002 */

  int n1 = input_controls.colmin();
  int n2 = input_controls.colmax();
  //TTRACE(n1,n2)


  int num_nn = input_controls.rowmax();
 // TRACE(num_nn)

  ivector display_node(n1,n2); display_node.initialize();

  for(int t = n1; t <= n2; t++)
  {
    for(int s = 1; s <= num_nn; s++)
    {
        display_node(t) = input_controls(s,t);
        if (display_node(t) > 0) break;
    } //s
  } //t

//clogf << "display_nodes vector" << endl;
//clogf << display_node << endl;
//cout <<__LINE__<<" line: " <<__LINE__ << endl; exit(1);

return (display_node);

}
