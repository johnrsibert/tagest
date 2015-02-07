//$Id: par_t_nn.cpp 3126 2013-12-04 17:52:52Z jsibert $
#include "par_t_nn.h"
#include "trace.h"

void read_ma_matrix(dmatrix& t, const char* filename);

extern ofstream clogf;

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
par_t_nn<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::par_t_nn(const par_t_nn<d3_array,dmatrix,dvector,double>&  t)
  : par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>( t ), nn(t.nn), num_nn(t.num_nn), num_levels(t.num_levels), nodes_at_layer( t.nodes_at_layer ),
    input_controls(t.input_controls), nactive_input_layers(t.nactive_input_layers),
    scaled_year( t.scaled_year), scaled_month( t.scaled_month),
    scaled_longitude( t.scaled_longitude ), scaled_latitude( t.scaled_latitude ),
    depth( t.depth ), numfad( t.numfad ), elevation( t.elevation ), distance( t.distance  ),
    wstress_ne( t.wstress_ne ), wstress_sw( t.wstress_sw ), wstress_curl( t.wstress_curl ),
     soi_index( t.soi_index), forage(t.forage), sst(t.sst), zonal_current(t.zonal_current),
     meridional_current(t.meridional_current), habitat(t.habitat)
{
   //HERE
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
  uniform_sigma = 0.0;

  min_weight = t.min_weight;
  max_weight = t.max_weight;
  min_bias = t.min_bias;
  max_bias = t.max_bias;
  min_slope = t.min_slope;
  max_slope = t.max_slope;
  min_offset = t.min_offset;
  max_offset = t.max_offset;

  min_diff_hwts = t.min_diff_hwts ;
  max_diff_hwts = t.max_diff_hwts;

  xmin_sig = t.xmin_sig;
  xmax_sig = t.xmax_sig;
  xmin_uv = t.xmin_uv;
  xmax_uv = t.xmax_uv;

  depth_file_ext = t.depth_file_ext;
  fads_file_ext = t.fads_file_ext;
  elev_file_ext = t.elev_file_ext;
  dist_file_ext  = t.dist_file_ext;
  wne_file_ext = t.wne_file_ext;
  wsw_file_ext = t.wsw_file_ext;
  wsc_file_ext = t.wsc_file_ext;
  soi_index_file_ext = t.soi_index_file_ext;
  forage_file_ext = t.forage_file_ext;
  sst_file_ext = t.sst_file_ext;
  zonal_current_file_ext = t.zonal_current_file_ext;
  meridional_current_file_ext = t.meridional_current_file_ext;
  habitat_file_ext = t.habitat_file_ext;

  total_wts_penalty = t.total_wts_penalty;
  total_wts_switch_penalty = t.total_wts_switch_penalty;

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
  nn.allocate(1,num_nn); //allocate memory to neural_net_vector
  for(int s = 1; s <= num_nn; s++)
    {
      nn(s).allocate(t.nn(s).get_layers()  );  // allocate memory to neural_net
      nn(s).nn_ipar = t.nn(s).nn_ipar;   // this is bloddy important!!!
    }
   nn.initialize();
   //HERE
}
template par_t_nn<d3_array,dmatrix,dvector,double>::par_t_nn(const par_t_nn<d3_array,dmatrix,dvector,double>&);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
par_t_nn<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::par_t_nn(const par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>&  t)
  : par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>( t ), nn(t.nn), num_nn(t.num_nn), num_levels(t.num_levels), nodes_at_layer( t.nodes_at_layer ),
    input_controls(t.input_controls), nactive_input_layers(t.nactive_input_layers),
    scaled_year( t.scaled_year), scaled_month( t.scaled_month),
    scaled_longitude( t.scaled_longitude ), scaled_latitude( t.scaled_latitude ),
    depth( t.depth ), numfad( t.numfad ), elevation( t.elevation ), distance( t.distance  ),
    wstress_ne( t.wstress_ne ), wstress_sw( t.wstress_sw ), wstress_curl( t.wstress_curl ),
     soi_index( t.soi_index), forage(t.forage), sst(t.sst), zonal_current(t.zonal_current),
     meridional_current(t.meridional_current), habitat(t.habitat)
{
   //HERE
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
  uniform_sigma = 0.0;

  min_weight = t.min_weight;
  max_weight = t.max_weight;
  min_bias = t.min_bias;
  max_bias = t.max_bias;
  min_slope = t.min_slope;
  max_slope = t.max_slope;
  min_offset = t.min_offset;
  max_offset = t.max_offset;

  min_diff_hwts = t.min_diff_hwts ;
  max_diff_hwts = t.max_diff_hwts;

  xmin_sig = t.xmin_sig;
  xmax_sig = t.xmax_sig;
  xmin_uv = t.xmin_uv;
  xmax_uv = t.xmax_uv;

  depth_file_ext = t.depth_file_ext;
  fads_file_ext = t.fads_file_ext;
  elev_file_ext = t.elev_file_ext;
  dist_file_ext  = t.dist_file_ext;
  wne_file_ext = t.wne_file_ext;
  wsw_file_ext = t.wsw_file_ext;
  wsc_file_ext = t.wsc_file_ext;
  soi_index_file_ext = t.soi_index_file_ext;
  forage_file_ext = t.forage_file_ext;
  sst_file_ext = t.sst_file_ext;
  zonal_current_file_ext = t.zonal_current_file_ext;
  meridional_current_file_ext = t.meridional_current_file_ext;
  habitat_file_ext = t.habitat_file_ext;

  total_wts_penalty = value(t.total_wts_penalty);
  total_wts_switch_penalty = value(t.total_wts_switch_penalty);

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
  nn.allocate(1,num_nn); //allocate memory to neural_net_vector
  for(int s = 1; s <= num_nn; s++)
    {
      nn(s).allocate(t.nn(s).get_layers()  );  // allocate memory to neural_net
      nn(s).nn_ipar = t.nn(s).nn_ipar;   // this is bloddy important!!!
    }
   nn.initialize();
   //HERE
}
template<> par_t_nn<d3_array,dmatrix,dvector,double>::par_t_nn(const par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>&) {}
template par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>::par_t_nn(const par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>&);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
par_t_nn<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::~par_t_nn()
{
  //destructor
  //cout << "par_t_nn::~par_t_nn()" << endl;
}
template par_t_nn<d3_array,dmatrix,dvector,double>::~par_t_nn();
template par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>::~par_t_nn();

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t_nn<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::habitat_data( const char* input_file_ext, year_month& current_month, dmatrix& hh)
{
  const adstring input_file(adstring("../") + pathname.getRoot() + input_file_ext);

  uistream ui(input_file);
  if (ui.is_open() == false)
  {
    cerr <<"Error: Unable to open file \""<< input_file <<"\". "<< endl;
    exit(1);
  }

  int data_start_date_year = 0;
  int data_start_date_month = 0;
  ui >> data_start_date_year;
  ui >> data_start_date_month;

  year_month data_start_date(data_start_date_year, data_start_date_month);
  const int month_index = current_month - data_start_date;
  if (month_index >= 0)
  {
    //file pointer for random access the extra 8 bytes are for the two integers (4 bytes x 2) for data time series start year and month that was saved during the binary file creation
    const int fp = (month_index * m * n * 8) + 8;
    ui.seekg(fp);
    ui >> hh;
  }

  ui.close();
}
template void par_t_nn<d3_array,dmatrix,dvector,double>::habitat_data(const char* input_file_ext, year_month& current_month, dmatrix& hh);
template void par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>::habitat_data(const char* input_file_ext, year_month& current_month, dmatrix& hh);

/* sigmoid scaling function: scales the output layer for u, v and sigma */
template <typename DOUBLE>
DOUBLE scale_fn(const DOUBLE x, const DOUBLE slope, const DOUBLE offset, const double min, const double max)
{
  const double range = max - min;
  DOUBLE arg = slope*(x-offset);
  DOUBLE y = min + range*(atan(arg)/M_PI + 0.5);
  return y;
}
template double scale_fn<double>(const double x, const double slope, const double offset, const double min, const double max);
template dvariable scale_fn<dvariable>(const dvariable x, const dvariable slope, const dvariable offset, const double min, const double max);

/* linear scaling function */
double scale_fn2(const double x, const double xmin, const double xmax, const double ymin, const double ymax)
{
   double slope =  (ymax - ymin) / (xmax- xmin);
   double intercept = ymin -  (slope * xmin);
   double y = slope*x + intercept;
   return (y);
 }


////////////////// read and initialize the saled year vector /////////////////////
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t_nn<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::read_initialize_scaled_year()
{
  //HERE
   int  yr1 = start_date.get_year();  //start year in model
  year_month final_date = get_tr_date(nrelease) + nmonth;
  int  yr2 = final_date.get_year();  //end year in model

  //TTRACE(yr1,yr2)
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

// TRACE(scaled_year)

 for(int j = yr1;  j <= yr2; j++)
 {
  scaled_year( j ) =  yr_m * scaled_year( j )  +  yr_c;  //scaled from -1.0 to +1.0
 }

// TRACE(scaled_year)
} //end read_initialize_scaled_year


/////////////////// read  matrix data for neural_net input layers ///////////////////////////
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t_nn<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::read_input_matrix_data(const char* FILE, const char* file_extension, dmatrix& t, const char* path )
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
}
template void par_t_nn<d3_array,dmatrix,dvector,double>::read_input_matrix_data(const char* FILE, const char* file_extension, dmatrix& t, const char* path);
template void par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>::read_input_matrix_data(const char* FILE, const char* file_extension, dmatrix& t, const char* path);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t_nn<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::allocate()
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
  forage.allocate(1,m,1,n);
  sst.allocate(1,m,1,n);
  zonal_current.allocate(1,m,1,n);
  meridional_current.allocate(1,m,1,n);
  habitat.allocate(1,m,1,n);

  input_controls.allocate(1,num_nn,1,20);
  nactive_input_layers.allocate(1,num_nn);
}
template void par_t_nn<d3_array,dmatrix,dvector,double>::allocate();
template void par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>::allocate();


//initialzie function
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t_nn<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::initialize()
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
  forage.initialize();
  sst.initialize();
  zonal_current.initialize();
  meridional_current.initialize();
  habitat.initialize();

   input_controls.initialize();
   nactive_input_layers.initialize();

   xmin_sig = xmax_sig = 0.0;
   xmin_uv = xmax_uv = 0.0;
   total_uv_penalty = 0.0;
   total_sigma_penalty = 0.0;
   total_negative_tag_penalty = 0.0;
   total_total_tag_penalty = 0.0;
   total_bounds_penalty = 0.0;
   total_wts_penalty = 0.0;
   total_wts_switch_penalty = 0.0;

} //end initialize()
template void par_t_nn<d3_array,dmatrix,dvector,double>::initialize();
template void par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>::initialize();

//calculate the number of active_input_layers
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t_nn<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::calc_nactive_layers(void)
{
   for(int s = 1; s <= num_nn; s++) {
       int nactive = 0;
       for(int k = 1; k <= 20; k++) {
         if (input_controls(s,k) == 1 || input_controls(s,k) == 2)
         nactive++;
        }
       nactive_input_layers(s) = nactive;
     }
}
template void par_t_nn<d3_array,dmatrix,dvector,double>::calc_nactive_layers(void);
template void par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>::calc_nactive_layers(void);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t_nn<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::allocate_input_data_file_extensions(void)
{
    //spatial data
   depth_file_ext = (char*)".zdt";   //water depth
   fads_file_ext = (char*)".fad";  // number of FADs
   elev_file_ext =  (char*)".zdf";  //elevation (bathymetry)
   dist_file_ext = (char*)".dst";   //distance
   soi_index_file_ext = (char*)".soi";   //monthly ENSO index

   //spatio-temporl data (read on the fly from binary files)
   wne_file_ext = (char*)".wne";   //curl of the windstress, NE monsoon
   wsw_file_ext = (char*)".wsw";  //curl of the windstress, SW monsoon
   wsc_file_ext = (char*)".wsc";    //curl of the windstress (spatio_temporal)
   forage_file_ext = (char*)".frg";  //forage field (Patrick Lehodey)
   sst_file_ext = (char*)".sst";       //SST
   zonal_current_file_ext  = (char*)".usc";   //zona current speed
   meridional_current_file_ext =  (char*)".vsc"  ;  //meridional current speed
   habitat_file_ext = (char*)".hab";          //habitat index
}
template void par_t_nn<d3_array,dmatrix,dvector,double>::allocate_input_data_file_extensions(void);
template void par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>::allocate_input_data_file_extensions(void);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
DOUBLE par_t_nn<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::scalefn(const DOUBLE x, const DOUBLE slope, const DOUBLE offset, const double min, const double max)  const
{
        const double mid = (max - min) * 0.5;
	const DOUBLE arg = slope * (x - offset);
	DOUBLE y = min + mid * (1 + (2.0 / M_PI) * atan(arg));
	return y;
}
template double par_t_nn<d3_array,dmatrix,dvector,double>::scalefn(const double x, const double slope, const double offset, const double min, const double max)  const;
template dvariable par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>::scalefn(const dvariable x, const dvariable slope, const dvariable offset, const double min, const double max)  const;

/*
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t_nn<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::scalefn(const double x, double &y, const dvariable slope, const dvariable offset, const double min, const double max)  const
{
	scalefn(x, y, value(slope), value(offset), min, max);
}
template void par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>::scalefn(const double x, double &y, const dvariable slope, const dvariable offset, const double min, const double max)  const;
*/

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t_nn<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::dfscalefn(const double x, double &dfx, double &dfy, const double slope, double &dfslope, const double offset, double &dfoffset, const double min, const double max) const
{
        const double mid = (max - min) * 0.5;
	const double arg = slope * (x - offset);

	double dfarg(0);
	//y = min + mid * (1 + (2.0 / M_PI) * atan(arg));
	dfarg += dfy * mid * (2.0 / M_PI) * (1.0 / (1.0 + arg * arg));
	dfy = 0;

	//const double arg = slope * (x - offset);
	dfx += dfarg * slope;
	dfslope += dfarg * (x - offset);
	dfoffset -= dfarg * slope;
	dfarg = 0;
}
template void par_t_nn<d3_array,dmatrix,dvector,double>::dfscalefn(const double x, double &dfx, double &dfy, const double slope, double &dfslope, const double offset, double &dfoffset, const double min, const double max) const;
template void par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>::dfscalefn(const double x, double &dfx, double &dfy, const double slope, double &dfslope, const double offset, double &dfoffset, const double min, const double max) const;
