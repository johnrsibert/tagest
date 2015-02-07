//$Id: nn_paread.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "par_t_nn.h"
#include "bound.h"
#include "par_t.h"
#include "trace.h"

void linear_input_scale(dvector& v);
void linear_input_scale(dmatrix& t);

double scale_fn2_minxy(int nwts, double dhwts, double bias);

extern ofstream clogf;
extern uistream ui_wscf;
extern uistream uf_frg;

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
par_t_nn<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::par_t_nn(const char* name)
        : par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(name, adstring("#v25n"))
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

    if (!(dfile_version == par_file_version))
    {
        cerr << "\nError: File signature difference for " << fullname << endl;
        cerr << "       Found " << dfile_version
        << " should have " << par_file_version << endl;
        cerr << "       Unable to interpret this file format." << endl;
        cerr << "       Error occured at line " << __LINE__
        << " in " << __FILE__ << endl;
        exit(1);
    }

    /* Section that reads negative binomial parameters moved to pparead.cpp  */

    // Neural Networks Scaling Function Parameters
    dfile >> u_slope   >> u_offset;
    TTRACE(u_slope, u_offset)
    dfile >> v_slope   >> v_offset;
    TTRACE(v_slope, v_offset)
    dfile >> sig_slope >> sig_offset;
    TTRACE(sig_slope, sig_offset)
    dfile >> uniform_sigma;

    //Neural Networks Scaling Function Parameters
    dfile  >> min_weight >> max_weight;
    TTRACE(min_weight, max_weight)
    dfile  >> min_bias >> max_bias;
    TTRACE(min_bias, max_bias)
    dfile  >> min_slope >> max_slope;
    TTRACE(min_slope, max_slope)
    dfile  >> min_offset >> max_offset;
    TTRACE(min_offset, max_offset)

    dfile >> min_diff_hwts >> max_diff_hwts;
    TTRACE(min_diff_hwts, max_diff_hwts)
    //dfile >> xmin >> xmax; TTRACE(xmin,xmax)


    dfile >> num_nn;       //number of neural_nets
    TRACE(num_nn)
    nn.allocate(1,num_nn); //allocate memory to nn_vector

    num_levels.allocate(1,num_nn);
    dfile >> num_levels;  //vector of num_levels
    TRACE(num_levels)
    nodes_at_layer.allocate(1,num_nn,1,num_levels);

    //get the input_data file extensions
    allocate_input_data_file_extensions();



    for (int s = 1; s <= num_nn; s++)
    {
        dfile >> nodes_at_layer(s);
        TRACE(nodes_at_layer(s))
        nn(s).allocate(nodes_at_layer[s] );             //allocate memory for nn
        nn(s).initialize();                                           //initialize the nn
        nn(s).set_nn_ipar_names(nn(s).nn_ipar, s);   //set the nn_ipar name
        nn(s).read_nn_ipar(dfile, nn(s).nn_ipar, s);    //read the nn_ipars

        int random_weights  = nn(s).nn_ipar[1];
        TRACE(random_weights)

        if (!dfile)
        {
            cerr << "Error reading random_weight flag in nn(" << s <<")" << endl;
            exit(1);
        }

        if ( random_weights == 0)
        {
            nn(s).read_weights(dfile);  //read the initial values from from parfile
            if (!dfile)
            {
                cerr << "Error reading weights" << endl;
                exit(1);
            }
            TRACE(nn(s).get_weights() );
        }
        else
        {
            nn(s).make_random_weights(random_weights, int(min_diff_hwts), int(max_diff_hwts));  //get different initial values
            TRACE(nn(s).get_weights());
            nn(s).nn_ipar[1] = 0; // reset to zero for subsequent reading from parfile
        }


        int random_bias = nn(s).nn_ipar[2];
        TRACE(random_bias)
        if (!dfile)
        {
            cerr << "Error reading random_bias flag" << endl;
            exit(1);
        }

        if ( random_bias == 0)
        {
            nn(s).read_bias(dfile);  //read the initial values from the par file
            if (!dfile)
            {
                cerr << "Error reading bias" << endl;
                exit(1);
            }
            TRACE(nn(s).get_bias() );
        }
        else
        {
            nn(s).make_random_bias(random_bias);  //get different initial values
            TRACE(nn(s).get_bias());
            nn(s).nn_ipar[2] = 0; // reset to zero for subsequent read of new par file
        }
    }//for s


    //allocate memory to the class
    allocate();
    initialize();

    int index_of_last_level = 0;
    int num_output_nodes = 0;
    for (int s = 1; s <= num_nn; s++)
    {
        index_of_last_level = size_count(nodes_at_layer(s));
        num_output_nodes +=  nodes_at_layer(s,index_of_last_level);

        /*
                if (num_nn == 1)
                {
                  cout <<"There is only one neural net..  "<< endl;
                  cout << "Cannot initialize the variable 'xmin_uv' and 'xmax_uv' " << endl;
                  cout <<"Bailing out in " <<__FILE__<<": " <<__LINE__<< endl;
                  exit(1);
                }
        */
        if (s == num_nn)
        {
            int nhidden = nodes_at_layer(s,num_levels(s)-1);
            xmin_sig =  - scale_fn2_minxy(nhidden, max_diff_hwts, max_bias);
            xmax_sig =    scale_fn2_minxy(nhidden, max_diff_hwts, max_bias);
        }
        else
        {
            int nhidden = nodes_at_layer(s,num_levels(s)-1);
            xmin_uv   =  - scale_fn2_minxy(nhidden, max_diff_hwts, max_bias);
            xmax_uv =     scale_fn2_minxy(nhidden, max_diff_hwts, max_bias);
        }
        nn(s).init_diff_hwts();  //initialize the diff_hwts variable from weights in hidden_layer
        TRACE(nn(s).get_diff_hwts());
    } //for s

    TTRACE(xmin_uv,xmax_uv)
    TTRACE(xmin_sig,xmax_sig)

    /*
          if(num_output_nodes != 3)
          {
             cerr <<"Total number of output_nodes is not equal to 3.." << endl;
             exit(1);
          }
    */


    for (int s =1; s <= num_nn; s++)
    {
        cout <<"Reading input_layer data fo neural_net("<< s <<")" <<endl;
        if (nn(s).nn_ipar[15] )
        {
            read_input_matrix_data( name, depth_file_ext, depth);  //read the depth data
            linear_input_scale(depth);   // linearly scale  depth data
        }

        if (nn(s).nn_ipar[16] )
        {
            read_input_matrix_data( name, fads_file_ext, numfad); //read fad data
            linear_input_scale( numfad );  //linearly scale fad data
        }

        if ( nn(s).nn_ipar[17] )
        {
            read_input_matrix_data( name, elev_file_ext, elevation); //read elevation data
            linear_input_scale(  elevation );  //linearly scale elevation data
        }

        if (nn(s).nn_ipar[18] )
        {
            read_input_matrix_data( name, dist_file_ext, distance); //read distance data
            linear_input_scale(  distance );  //linearly scale distance  data
        }

        if (nn(s).nn_ipar[19] )
        {
            read_input_matrix_data( name, wne_file_ext, wstress_ne); //read wstress_ne data
            linear_input_scale( wstress_ne );  //linearly scale wstress_ne  data

            read_input_matrix_data( name, wsw_file_ext, wstress_sw); //read wstress_sw data
            linear_input_scale( wstress_sw );  //linearly scale wstress_sw  data
        }

        if (nn(s).nn_ipar[20] )
        {
            //do nothing here but give a warning that the data is active
            cout <<"Will read .. " <<name << wsc_file_ext << endl;
        }

        if (nn(s).nn_ipar[21] == 1) {
            read_input_matrix_data(name, soi_index_file_ext, soi_index);
            linear_input_scale(soi_index);
        } else if (nn(s).nn_ipar[21] == 2) {
            int readSouthernOscillationIndexFile(const int beginYear, const int endYear, dmatrix& soidata);
            const int beginYear = start_date.get_year();
            readSouthernOscillationIndexFile(beginYear, 2006, soi_index);
            linear_input_scale(soi_index);
        }

        if (nn(s).nn_ipar[22] )
        {
            cout <<"Will read .. " << name << forage_file_ext << endl;
        }

        if (nn(s).nn_ipar[23] )
        {
            cout <<"Will read .. " << name << sst_file_ext << endl;
        }

        if (nn(s).nn_ipar[24] )
        {
            cout <<"Will read .. " << name << zonal_current_file_ext << endl;
        }

        if (nn(s).nn_ipar[25] )
        {
            cout <<"Will read .. " << name << meridional_current_file_ext << endl;
        }

        if (nn(s).nn_ipar[26] )
        {
            cout <<"Will read .. " <<  name << habitat_file_ext <<  endl;
        }


    } // data reading loop


    //puts the input_layer flags into input_controls for the model
    for (int s = 1; s <= num_nn; s++)
    {
        for (int t = 1; t <= 20; t++)
        {
            input_controls(s,t) = nn(s).nn_ipar(t+10);
        }
    }

    calc_nactive_layers(); //caculate nactive input layers
    TRACE(nactive_input_layers)

    linear_input_scale(scaled_longitude);  //linearly scales the longitude values
    linear_input_scale(scaled_latitude);     //linearly scales the latitude values


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
template par_t_nn<d3_array,dmatrix,dvector,double>::par_t_nn(const char* name);
template par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>::par_t_nn(const char* name);

double scale_fn2_minxy(int nwts, double dhwts, double bias)
{
    /* Calculates  xmin and xmax to initialize
    xmin_sig, xmax_sig,  xmin_uv, xmax_uv */
    double tmp = 0.0;
    double tmp2 = 0.0;
    for (int i = 1; i <= nwts; i++)
    {
        tmp +=  dhwts;
        tmp2 += tmp;
    }
    double xval = tmp2 * 0.5 + bias;
    //double xval = tmp2  * 1.0  + bias;
    return (xval);
}
