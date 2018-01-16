//$Id: xrparead.cpp 2951 2012-02-13 17:29:19Z jsibert $
#include "par_t_reg.h"
#include "bound.h"
#include "trace.h"

extern ofstream clogf;

//template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
//par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::par_t_reg(const char* name): par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(name,"#v20r")
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::par_t_reg(const ADMB_XMLDoc& doc): par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(doc)
{
  adstring fullname;
  seasons.allocate(1, 12);
 
  cout << "Extracting XML par_t_reg data from " << doc.getFileName() << endl;
  clogf << "Extracting XML par_t_reg data from " << doc.getFileName() << endl;

  /*
  fullname = adstring(name);
cout << "fullname: " << fullname << endl;
  int dot_pos = fullname.pos(".");


  if (dot_pos == 0)
      fullname += ".par";

  cifstream dfile(fullname);
  if ( !dfile )
  {
      cerr << "Cannot reopen file again" << fullname << endl;
      exit(1);
  }
 
  //start where par_t_read finished
  dfile.seekg(sp);

  dfile  >> ngrid >> nseason; 
  if ( !dfile )
  {
      cerr << "Error reading sizes in file " << fullname << endl;
      exit(1);
  }
  */

  ngrid = doc.getInt("ngrid");
  nseason = doc.getInt("nseason");
  TTRACE(ngrid,nseason)

  allocate();
  initialize();

  #define XML_TEST 
  #ifndef XML_TEST 
  read_seasons(dfile);
  clogf <<"Finished reading seasons." << endl;
  cout << "Finished reading seasons." << endl;

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
  clogf << "Finished reading par file." << endl;
  cout << "Fishished reading par file." << endl;
  #endif // XML_TEST 
}
template par_t_reg<d3_array,dmatrix,dvector,double>::par_t_reg(const ADMB_XMLDoc& doc);
template par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>::par_t_reg(const ADMB_XMLDoc& doc);

  #define XML_TEST 
  #ifndef XML_TEST 
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::read_seasons(cifstream& istr)
{
  double uscale, vscale, sigmascale;
  if (m_ipar[66] == 1)
  {
      //read in scale factors from file "scale.dat"
      ifstream scale("scale.dat");
      if (!scale)
      {
          clogf << "Unable to open scale.dat" << endl;
          exit(1);
      }
      scale >> uscale >> vscale >> sigmascale;
  }
	
  for (int k = 1; k <= nseason; k++)
  {
      istr >> startmonth[k];
      if (startmonth[k] <= 0)
      {
          cerr << "Starting month for season " << k << " not specified." << endl;
          exit(1);
      }

      usergridU[k][0] = 0;
      usergridV[k][0] = 0;
      usergridSigma[k][0] = 0;
      for (int i = 1; i <= ngrid; i++)
      {
          clogf << "season " << k << ", region " << i;
          int j;
          istr >> j;
          clogf << setw(10) << j;

          istr >> stratum_active(k, j);
          clogf << setw(5) << stratum_active(k, j);


          istr >> usergridU[k][j];
          if (m_ipar[66] == 1)
              usergridU[k][j] *= uscale;
          clogf << setw(10) << usergridU[k][j];

          istr >> usergridV[k][j];
          if (m_ipar[66] == 1)
              usergridV[k][j] *= vscale;
          clogf << setw(10) << usergridV[k][j];

          istr >> usergridSigma[k][j];
          if (m_ipar[66] == 1)
              usergridSigma[k][j] *= sigmascale;
          clogf << setw(10) << usergridSigma[k][j] << endl;

          if (!istr)
          {
	     cerr << "**  error reading season " << j
	          << " (season record " << i << ")" << endl;
	     exit(1);
          }
      }
  }

  set_seasons();
  HERE
  cout << "ipar[83] is " << m_ipar[83] << endl;
  HERE
  //FADmodel 
  if (m_ipar[83] == 1)
  {
     cout << "ipar[83] is " << m_ipar[83] << endl;
     cifstream ifs1("../FAD_table.csv");

     if (!ifs1.is_open())
     {
       cerr << "no data" << endl;
       exit(1);
     }
     fadmap.allocate(1,get_m(),1,get_n());
     fadmap.initialize();
     max_fad.allocate(0,ngrid);
     max_fad.initialize();
     while(!ifs1.eof())
     {
       double lon;
       double lat;
       int FADno;
       ifs1  >> lon >> lat >> FADno;
       if(ifs1.eof())
       {
         break;
       }
       int i = long_to_index(lon);
       int j = lat_to_index(lat);
       if (i <=0 || m < i || j <= 0 || n < j)
       {
         continue;
       }
       fadmap(i,j) = FADno;
       int g=gridmap(i,j);
       if (gridmap(i,j)==0)
       {
         fadmap(i,j) = 0;
       }
       if (fadmap(i,j) > max_fad[g])
       {
         max_fad[g]=fadmap(i,j);
       }
     }
     //max_fad = max(fadmap);
     TRACE(max_fad)
     cout << "max_fad:" << max_fad << endl;
     cout << "fadmap:" << fadmap << endl;
     maxinflection.allocate(1,ngrid);
     maxinflection.initialize();
     maxinflection = max_fad(1,ngrid);
    
     TRACE(maxinflection) 
     cout << "max_fad:" << max_fad << endl;
     cout << "maxinflection:" << maxinflection << endl;
     /*maxinflection.allocate(0,ngrid);
     maxinflection.initialize();
     for (int g = 1; g <= ngrid; g++)
     {
        maxinflection(g) = max_fad(g);     
     }

     cout << "max_fad(1)" << max_fad(1) << endl;    
     cout << "maxinflection: " << maxinflection << endl;
     exit(1);*/

     //y is latitude. latitude is N. 
     gx_fadmap.allocate(1,get_m(),1,get_n());
     gx_fadmap.initialize();
     double dx = get_deltax_eq();
     for (int i=1; i<=m; i++)
     {
        for (int j=1; j<=n; j++)
        {
          if (1 < i && i < m)
          {
             gx_fadmap(i,j) = (fadmap(i+1,j)-fadmap(i-1,j))/(2.0*dx);
          }
          /* 
          else if (i==1)
          {
             gx_fadmap(i,j) =  fadmap(i+1,j)-fadmap(i,j)/(1.0*dx);;
          }
          else if (i==m)
          {
             gx_fadmap(i,j) =  fadmap(i,j)-fadmap(i-1,j)/(1.0*dx);
          }
          */
        }
     } 
  
     gy_fadmap.allocate(1,get_m(),1,get_n());
     gy_fadmap.initialize();
     double dy = get_deltay();
     for (int i=1; i<=m; i++)
     {
        for (int j=1; j<=n; j++)
        {
           if (1 < j && j < n)
           {
               gy_fadmap(i,j) = (fadmap(i,j+1)-fadmap(i,j-1))/(2.0*dy);
           }
           /*
           else if (j==1)
           {
              gy_fadmap(i,j) =  fadmap(i,j+1)-fadmap(i,j)/(1.0*dy);;
           }
           else if (j==n)
           {
              gy_fadmap(i,j) =  fadmap(i,j)-fadmap(i,j-1)/(1.0*dy);
           }
           */
        }
     } 
     ifs1.close();
    
  
      if (m_ipar[84] == 1)
      {
        cout << "ipar[84] is 1" << endl;
        istr >> mean_size;
        istr >> sus_spd;
       //cout << "mean size: " << mean_size << endl;
       //cout << "sustainable speed: " << sus_spd << endl;
      }  //if(m_ipar[84])
      else
      {
        cerr << "ipar[84] should be 1 when ipar[83]=1" << endl;
        exit(1);
      } 
     
      fadmap_stratum_active_d.allocate(1,ngrid);
      fadmap_stratum_active_d.initialize();
     // min_D.allocate(1, ngrid);
     // min_D.initialize();
      slope.allocate(1,ngrid);
      slope.initialize();
      inflection.allocate(1,ngrid);
      inflection.initialize();
      if (m_ipar[85] == 1)
      {
          for (int g = 1; g <= ngrid; g++)
          {
             int h;
             //istr >> h >> fadmap_stratum_active_d(g) >> min_D(g) >> slope(g) >> inflection(g);
             istr >> h >> fadmap_stratum_active_d(g) >> slope(g) >> inflection(g);
             cout  << "grid: " << h << " " << "fadmap_stratum_active_d: " << fadmap_stratum_active_d(g) << " " << //"min_D: " << min_D(g) << 
             " " <<"slope: " << slope(g) << " " << "inflection: " << inflection(g) <<endl;
          }
      }  //if(m_ipar[85])
      else
      {
        cerr << "ipar[85] should be 1 when ipar[83]=1" << endl;
        exit(1);
      } 
   }   //if(m_ipar[83]) 
   HERE
}
template void par_t_reg<d3_array,dmatrix,dvector,double>::read_seasons(cifstream& istr);
template void par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>::read_seasons(cifstream& istr);
  #endif // XML_TEST 
