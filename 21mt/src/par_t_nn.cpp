#include "par_t_nn.h"
#include "trace.h"

void read_ma_matrix(dmatrix& t, const char* filename);

extern uistream ui_wscf;
extern ofstream clogf;

par_t_nn::par_t_nn(const par_t_nn&  t)
  : par_t( t ), num_levels(t.num_levels), nodes_at_layer( t.nodes_at_layer ),
    scaled_year( t.scaled_year), scaled_month( t.scaled_month),
    scaled_longitude( t.scaled_longitude ), scaled_latitude( t.scaled_latitude ),
    depth( t.depth ), numfad( t.numfad ), elevation( t.elevation ), distance( t.distance  ),
    wstress_ne( t.wstress_ne ), wstress_sw( t.wstress_sw ), wstress_curl( t.wstress_curl ),
    wsc_start_date( t.wsc_start_date ), soi_index( t.soi_index)
{
 // HERE
  nactive_input_layers = 0;

  // The copy constructor is used to generate an object that contains the
  // derivative contributions of the "active" parameters. Therefore, we do not
  // want to initialize the derivative contributions to the parameter values.
  // They should be set to zero. Refer to the version of January 4, 2002.

  u_slope = 0.0; //t.u_slope;
  u_offset = 0.0; //t.u_offset;
  v_slope = 0.0; //t.v_slope;
  v_offset = 0.0; //t.v_offset;
  sig_slope = 0.0; //t.sig_slope;
  sig_offset = 0.0; //t.sig_offset;

  min_weight = t.min_weight;
  max_weight = t.max_weight;
  min_bias = t.min_bias;
  max_bias = t.max_bias;
  min_slope = t.min_slope;
  max_slope = t.max_slope;
  min_offset = t.min_offset;
  max_offset = t.max_offset;


  nb_par.initialize();
  mort  = 0.0;
  q.initialize();
  report_rates.initialize();
  afm.initialize();

 //nn stuff
 if(t.nn.operator!() )
 {
   cerr << "Unallocated neural_net object passed to <par_t_nn::par_t_nn(const par_t_nn&  t) > "  << endl;
   exit (1 );
  }
  nn.allocate(t.nn.get_layers()  );
  nn.initialize();
 // HERE
} //end of copy constructor!


par_t_nn::~par_t_nn()
{
  //destructor
  //cout << "par_t_nn::~par_t_nn()" << endl;
}


void par_t_nn::get_wstress_curl( year_month& current_month, dmatrix& wstress_curl)
{
  
  int month_index = ( current_month - wsc_start_date ) + 1 ;
  int fp =( ( month_index  - 1 ) *  ( m * n ) * 8) + 8;    //file pointer for random access
  //the extra 8  is for the two integers (time_series start year and  month) 
  //saved during creating the binary file <see erswind.cp>
  
  //TTRACE(month_index,fp) 
  //TTRACE(month_index,current_month) 
  
  ui_wscf.seekg( fp );   // seek and get the pointer in the file
  ui_wscf >> wstress_curl;  //refresh the matrix with correct month's wind stress  data
  
//  cout << wstress_curl << endl;

}  


/* scaling function: scales the output layer for u, v and sigma */
double scale_fn(const double x, const double slope, const double offset,
                const double min, const double max)
{
  double range = max - min;
  double arg = slope*(x-offset);
  double y = min + range*(atan(arg)/M_PI + 0.5);
  return(y);
}


////////////////// read and initialize the saled year vector /////////////////////
void par_t_nn::read_initialize_scaled_year()
{
  HERE
   int  yr1 = start_date.get_year();  //start year in model
  year_month final_date = get_tr_date(nrelease) + nmonth;
  int  yr2 = final_date.get_year();  //end year in model

  TTRACE(yr1,yr2)
  for(int k = yr1;  k <= yr2; k++)
  {
     scaled_year( k ) = k;
   }

  double yr_c;		//year intercept
  double yr_m;		//year slope

  //initialize the scaling stuff!
  if ((yr2 - yr1) > 0.0)
  {
    yr_c =  - (1.0 + ( (2.0 * yr1) / (yr2 - yr1) )  );    //year intercept
    yr_m = 2.0/(yr2 - yr1);                      //year slope
  }
  else
  {
    // if only one year scaled value = 0.0
    yr_c = 0.0;
    yr_m = 0.0;
  }

 TRACE(scaled_year)

 for(int j = yr1;  j <= yr2; j++)
 {
  scaled_year( j ) =  yr_m * scaled_year( j )  +  yr_c;  //scaled from -1.0 to +1.0
 }

 TRACE(scaled_year)
} //end read_initialize_scaled_year


/////////////////// read  matrix data for neural_net input layers ///////////////////////////
void par_t_nn::read_input_matrix_data(const char* FILE, const char* file_extension, dmatrix& t, const char* path )
{
   adstring filename = adstring(path ) +  adstring( FILE ) + adstring( file_extension );
   cout << "Reading  "<< filename << endl;
   cifstream ifs( filename );
   if ( !ifs )
  {
    cerr << "Cannot open : " << filename << endl;
    exit(1);
  }
   read_ma_matrix( t,filename);  //read the data

}  // end read_input_matrix_data



//allocate function
void par_t_nn::allocate()
{
  int  yr1 = start_date.get_year();  //start year in model
  year_month final_date = get_tr_date(nrelease) + nmonth;
  int  yr2 = final_date.get_year();  //end year in model

  scaled_year.allocate(yr1,yr2);
  scaled_month.allocate(1,12);

  scaled_longitude.allocate(1,m);
  scaled_latitude.allocate(1,n);

  depth.allocate(1,m,1,n);            //allocate memory for depth matrix
  numfad.allocate(1,m,1,n);         //allocate memory for numfad
  elevation.allocate(1,m,1,n);      //allocate memory for elevation matrix
  distance.allocate(1,m,1,n);         //allocate memory for distance matrix
  wstress_ne.allocate(1,m,1,n);
  wstress_sw.allocate(1,m,1,n);
  wstress_curl.allocate(1,m,1,n);
  soi_index.allocate(1,99,1,12);  //specific to the dat set
  nn.allocate( nodes_at_layer  );  // allocate memory to neural_net

}


//initialzie function
void par_t_nn::initialize()
{
   read_initialize_scaled_year();          //reads and initialize and scale the year vector
   scaled_month = ("{0.86603,0.50000,0.00000,-0.50000,-0.86603,-1.00000,"
                   "-0.86603,-0.50000,0.00000,0.50000,0.86603,1.00000 }");

   for(int i = 1; i <= m; i++)
  {
    scaled_longitude( i )  = index_to_long( i );  //cout << index_to_long(i) << " ";
    int j1 = jlb(i);
    int jn = jub(i);
    for (int j = j1; j <= jn; j++)
    {
        scaled_latitude( j ) = index_to_lat( j );  //cout << index_to_lat(j) << "  " ;
    }
  }

  depth.initialize();
  numfad.initialize();
  elevation.initialize();
  distance.initialize();
  wstress_ne.initialize();
  wstress_sw.initialize();
  wstress_curl.initialize();
  soi_index.initialize();

  nn.initialize();  //initialize the neural_net
   
   int  wsc_yr;
   int  wsc_mo;
  
   if(! ui_wscf)
   {
     cerr <<"Could not open the binary file " << endl;
     exit(1);
   }
   ui_wscf >> wsc_yr >> wsc_mo;
   wsc_start_date.set(wsc_yr,wsc_mo);
   TRACE(wsc_start_date)
   
 } //end initialize()



 //calculate the number of active_input_layers
  void par_t_nn::calc_nactive_layers(void)
  {
    int nactive = 0;
    for(int k = 151; k <= 170; k++)
    {
      if(m_ipar[k] == 1 )
      nactive++;
    }

    TRACE(nactive);
    nactive_input_layers = nactive;
    
  }
