#include "par_t_nn.h"
#include "bound.h"
#include "trace.h"

void linear_input_scale(dvector& v);
void linear_input_scale(dmatrix& t);

extern ofstream clogf;
extern uistream ui_wscf;

par_t_nn::par_t_nn(const char* name)
  : par_t(name)  //inherits from par_t class
{
  adstring fullname;

  cout << "Reading part_t_nn data for " << name << endl;
  clogf << "Reading part_t_nn data for " << name << endl;

  fullname = adstring(name);
  int dot_pos = fullname.pos(".");

  if (dot_pos == 0)
      fullname += ".par";

  cifstream dfile(fullname);  //create stream object for reading file
  if ( !dfile )
  {
      cerr << "Cannot reopen file again" << fullname << endl;
      exit(1);
  }

  //start where par_t_read finished
  dfile.seekg(sp);

  if (dfile_version != par_file_version)
  {
    cerr << "\nError: File signature difference for " << fullname << endl;
    cerr << "       Found " << dfile_version
         << " should have " << par_file_version << endl;
    cerr << "       Unable to interpret this file format." << endl;
    cerr << "       Error occured at line " << __LINE__
         << " in " << __FILE__ << endl;
    exit(1);
  }

  //WARNING!: Need to have values in the par file if these switches are made active!
  if ( (m_ipar[11] == 4) || (m_ipar[11] == 5) ||
       (m_ipar[11] ==14) || (m_ipar[11] ==15) )
  {
    dfile >> nb_par(1) >> nb_par(2);
    clogf << "negative binomial parameter = " << nb_par << endl;
    cout << "negative binomial parameter = " << nb_par << endl;
  }

  if ( (m_ipar[11] ==24) || (m_ipar[11] ==25) )
  {
    dfile >> nb_par;
    clogf << "negative binomial parameter = " << nb_par << endl;
    cout << "negative binomial parameter = " << nb_par << endl;
  }
   //end WARNING!
  
  dfile >> num_levels;
  TRACE(num_levels)
  nodes_at_layer.allocate(1,num_levels);
  dfile >> nodes_at_layer;
  TRACE(nodes_at_layer)
  
  //allocate memory to the class
  allocate();
  initialize();
  
  calc_nactive_layers(); //caculate nactive input layers
  TRACE(nactive_input_layers)

  linear_input_scale(scaled_longitude);  //linearly scales the longitude values
  linear_input_scale(scaled_latitude);     //linearly scales the laatitude values
  
  //conditional reading of data to save memory
  if(  m_ipar[155] )
  {
  char depth_file_ext[5] ={ ".zdt"};
  read_input_matrix_data( name, depth_file_ext, depth);  //read the depth data
  linear_input_scale(depth);   // linearly scale  depth data
  }

  if( m_ipar[156] )
  {
  char fads_file_ext[5] = {".fad"};
  read_input_matrix_data( name, fads_file_ext, numfad); //read fad data
  linear_input_scale( numfad );  //linearly scale fad data
  }

  if( m_ipar[157] )
  {
  char elev_file_ext[5] = {".zdf"};
  read_input_matrix_data( name, elev_file_ext, elevation); //read elevation data
  linear_input_scale(  elevation );  //linearly scale elevation data
  }
  

  if( m_ipar[158] )
  {
    char dist_file_ext[5] = {".dst"};
    read_input_matrix_data( name, dist_file_ext, distance); //read distance data
    linear_input_scale(  distance );  //linearly scale distance  data
  }

  if( m_ipar[159] )
  {
    char wne_file_ext[5] = {".wne"};
    read_input_matrix_data( name, wne_file_ext, wstress_ne); //read wstress_ne data 
    linear_input_scale( wstress_ne );  //linearly scale wstress_ne  data

    char wsw_file_ext[5] = {".wsw"};
    read_input_matrix_data( name, wsw_file_ext, wstress_sw); //read wstress_sw data 
    linear_input_scale( wstress_sw );  //linearly scale wstress_sw  data
  }

  if( m_ipar[160] )
  { 
      //do nothing here but give a warning that the data is active
      cout <<"WindStressCurl <wstress_curl> will be activated " << endl;
  }

  if( m_ipar[161] )
  {
     char soi_index_file_ext[5] = {".soi"};
     read_input_matrix_data(name, soi_index_file_ext, soi_index);
      linear_input_scale(soi_index);  //linearly scale soi_index data
  }
 
  int random_weights  = m_ipar[141];
  TRACE(random_weights)

  if (!dfile)
  {
    cerr << "Error reading random_weight flag" << endl;
    exit(1);
  }

  if( random_weights == 0)
  {
    nn.read_weights(dfile);  //read the initial values from from parfile
    if (!dfile)
    {
      cerr << "Error reading weights" << endl;
      exit(1);
    }
  }
  else
  {
    nn.make_random_weights(random_weights);  //get different initial values
    m_ipar[141] = 0; // reset to zero for subsequent read of new par file
  }
  TRACE(nn.get_weights())  
  

  int random_bias = m_ipar[142];
  TRACE(random_bias)
  if (!dfile)
  {
    cerr << "Error reading random_bias flag" << endl;
    exit(1);
  }

  if( random_bias == 0)
  {
    nn.read_bias(dfile);  //read the initial values from the par file
    if (!dfile)
    {
      cerr << "Error reading bias" << endl;
      exit(1);
    }    
  }
  else
  {
    nn.make_random_bias(random_bias);  //get different initial values
    m_ipar[142] = 0; // reset to zero for subsequent read of new par file
  }
  TRACE(nn.get_bias())

  // scaling parameters starting values
  dfile >> u_slope   >> u_offset;    TTRACE(u_slope, u_offset)
  dfile >> v_slope   >> v_offset;    TTRACE(v_slope, v_offset)
  dfile >> sig_slope >> sig_offset;  TTRACE(sig_slope, sig_offset)

   HERE
  //get the parameter ranges
   dfile  >> min_weight >> max_weight;   TTRACE(min_weight, max_weight)
   dfile  >> min_bias >> max_bias;           TTRACE(min_bias, max_bias)
   dfile  >> min_slope >> max_slope;        TTRACE(min_slope, max_slope)
   dfile  >> min_offset >> max_offset;      TTRACE(min_offset, max_offset)

  dfile.set_ignore_eof();
  dfile >> nvar;
  clogf << "file nvar: " << nvar << endl;
  if (!dfile)
      nvar = 0;
  clogf << "file nvar: " << nvar << endl;

  dfile >> objective_function_value;
  clogf << "objective_function_value: " <<  objective_function_value << endl;
  if (!dfile)
      objective_function_value = 0;
  clogf << "objective_function_value: " <<  objective_function_value << endl;

  dfile >> maximum_gradient;
  clogf << "maximum gradient magnitude: " << maximum_gradient << endl;
  if (!dfile)
      maximum_gradient = 0.0;
  clogf << "maximum gradient magnitude: " << maximum_gradient << endl;


  dfile.close();
  clogf << "Fishished reading par file." << endl;
  cout << "Fishished reading par file." << endl;
}

