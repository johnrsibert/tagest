include "par_t_nn.h"
#include "trace.h"

extern ofstream clogf;

void par_t_nn::input_layer_comp(dvector& input_layer, year_month& ym)
{
  int  knode = 0;
  if( m_ipar[151] )
  {
      knode ++; //TTRACE("yr ",knode)
      input_layer(knode) = scaled_year( ym.get_year() ); //linear scaled from -1.0 to +1.0
      //TTRACE(ym.get_year(),ym.get_month_value())
  }

  if(m_ipar[152])
  {
    knode++; //cout <<"mo " << mo << "  node " << knode <<  endl;
    input_layer(knode) = scaled_month( ym.get_month_value() );     // cosine scaled
    //TTRACE(input_layer(1),input_layer(2))
  }

  for (int i=1; i <= m; i++)
  {
    int  inode = knode;
    if(m_ipar[153])
    {
      inode ++; //TTRACE("lon ",inode)
      input_layer(inode) = scaled_longitude( i ); // longitude values scaled from -1.0 to +1.0
      //TTRACE(input_layer(3), index_to_long( i ) )
     }

    int j1 = jlb(i);
    int jn = jub(i);

    for (int j = j1; j <= jn; j++)
    {
      int jnode =  inode;
      if(m_ipar[154])
      {
        jnode++; //TTRACE("lat ", jnode )
        input_layer( jnode ) = scaled_latitude( j ); // latitude values scaled from -1.0 to +1.0
       //TTRACE(input_layer(4), index_to_lat(j) )
      }

      if( m_ipar[155] )
      {
        jnode++; //TTRACE("zdt ",jnode)
        input_layer(jnode) = depth( i,j ); //depth scaled from -1.0 to 1.0
      }

      if( m_ipar[156] )
      {
        jnode++; //TTRACE("fad ",jnode)
         input_layer(jnode) = numfad( i,j );  //numfad scaled from -1.0 to 1.0
      }

      if( m_ipar[157] )
      {
        jnode ++; //TTRACE("zdf ", jnode)
        input_layer( jnode ) = elevation(i,j); //elevation scaled from -1.0 to 1.0
      }


      if(m_ipar[158] )
      {
        jnode++; //TTRACE("dst ",jnode)
        input_layer(jnode) = distance(i,j);  //distance scaled from -1.0 from 1.0
      }

      if(m_ipar[159] ) 
      {
        if(( mo >= 1 && mo <= 4) || ( mo >= 10 && mo <= 12))  
        {
          jnode++; //cout << "wne " << jnode << endl;
          input_layer(jnode) = wstress_ne(i,j);  //ne_wind scaled from -1.0 from 1.0
        }
              
        if( mo >= 5 && mo <= 9 )
        {
          jnode++; //cout <<"wsw " << jnode << endl;
          input_layer(jnode) = wstress_sw(i,j); //se_wind scaled from -1.0 to + 1.0
        } 
      }
        
      if(m_ipar[160] )
      {
        jnode++;  //cout << "wsc " << jnode << endl;
        input_layer(jnode) = wstress_curl(i,j);  //windstress_curl -1.0 from 1.0
      }
