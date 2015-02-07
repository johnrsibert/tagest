//$Id: rparead.cpp 3205 2014-08-19 00:50:00Z eunjung $
#include "par_t_reg.h"
#include "bound.h"
#include "trace.h"

extern ofstream clogf;

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::par_t_reg(const char* name): par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(name,"#v20r")
{
  adstring fullname;
  seasons.allocate(1, 12);
 
  cout << "Reading part_t_reg data for " << name << endl;
  clogf << "Reading part_t_reg data for " << name << endl;
 
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
  clogf << "Seasons: " << nseason << endl;

  allocate();
  initialize();

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
}
template par_t_reg<d3_array,dmatrix,dvector,double>::par_t_reg(const char* name);
template par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>::par_t_reg(const char* name);

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
  //FADmodel 
  if (m_ipar[83] == 1)
  {
     cout << "ipar[83] is " << m_ipar[83] << endl;
     year_month final_date = get_tr_date(nrelease) + nmonth; //set final_date
     int tmn=final_date-start_date+1;   //total months to allocate fadmap
     fadmap.allocate(1,tmn,1,get_m(),1,get_n());
     fadmap.initialize();
     max_fad.allocate(0,ngrid);
     max_fad.initialize();
     //int yr1 = start_date.get_year();
     //int mn1 = start_date.get_month_value();
     //year_month final_date = get_tr_date(nrelease) + nmonth;
     //int  yr2 = final_date.get_year();  //end year in model
     //int  mn2 = final_date.get_month_value();  //end month in model
     int t = 1;
     for (year_month ym = start_date; ym <= final_date; ym++)
     {
        ostringstream ostr;
        if (ym.get_month_value() < 10)
        {
           ostr << "../FAD_maps/fadmap_" << ym.get_year() << "_0" << ym.get_month_value() << ".fad";
        }
        else
        {
           ostr << "../FAD_maps/fadmap_" << ym.get_year() << "_" << ym.get_month_value() << ".fad";
        }
        ifstream ifs1(ostr.str().c_str());
        if(!ifs1)
        {
           cerr << "Cannot find FAD map file!" << endl;
           exit(1);
        }
        while(!ifs1.eof())
        {
           double lon;
           double lat;
           int FADno;
           ifs1 >> lon >> lat >> FADno;
           if (ifs1.eof())
           {
              break;
           }
           int i = long_to_index(lon);
           int j = lat_to_index(lat);
           if (i <= 0 || m < i || j <= 0 || n < j)
           {
              continue;
           }
           fadmap(t,i,j) = FADno;
           int g=gridmap(i,j);
           if (gridmap(i,j)==0)
           {
              fadmap(t,i,j) = 0;
           }
           if (fadmap(t,i,j) > max_fad[g])
           {
              max_fad[g]=fadmap(t,i,j);
              //max_fad[g]=11;
           }
       }
       maxinflection.allocate(1,ngrid);
       maxinflection.initialize();
       maxinflection = max_fad(1,ngrid);   //maxinflection one per ngrid? or should it change by index(time?)
       t ++;
     }
     // moving average for fadmap
     cout << "tmn: " << tmn  << endl;
     cout << "t: " << t  << endl;
     cout << "get_m: " << get_m()  << endl;
     cout << "get_n: " << get_n()  << endl;
     clogf << "fadmap[original]: " << fadmap << endl;
     clogf << "maxinflection[original]: " << maxinflection << endl;
     max_fad.allocate(0,ngrid);
     max_fad.initialize();
     int windowSize=6;
     double sum=0.0;
     double count=0.0;
     for (int a = 1; a <= tmn; a++)
     {
       sum=0.0;
       for (int i = 1; i <=get_m(); i++)
         {
         sum=0.0;
         for (int j =1; j <=get_n(); j++)
         {
           sum=0.0;
           if (a < tmn-windowSize+2)
           {
             for (int b=a; b< a+windowSize; b++)
             {
               sum +=fadmap(b,i,j);
             }  
             fadmap(a,i,j)=sum/windowSize;
           }
           else
           {
             count =0.0;
             count = tmn-a+1;
             for (int b=a; b<= a+count-1; b++)
             {
               sum +=fadmap(b,i,j); 
             }
             fadmap(a,i,j)=sum/count;
           }
           int g= gridmap(i,j);
           if (fadmap(a,i,j) > max_fad[g])
           {
              max_fad[g]=fadmap(a,i,j);
           }
         } // j
         } // i
     }
     maxinflection.allocate(1,ngrid);
     maxinflection.initialize();
     maxinflection = max_fad(1,ngrid);   //maxinflection one per ngrid? or should it change by index(time?)
     clogf << "maxinflection[after_moving]: " << maxinflection << endl;
     clogf << "fadmap[moving]: " << fadmap << endl;
     // end of moving average
     gx_fadmap.allocate(1,tmn,1,get_m(),1,get_n());
     gx_fadmap.initialize();
     gy_fadmap.allocate(1,tmn,1,get_m(),1,get_n());
     gy_fadmap.initialize();
     FADGradComp();
     max_gx = 0.0;
     max_gy = 0.0;
     int l=tmn;
     for(l = 1; l <= tmn; l++)
     {
        max_gx = max(max_gx,max(gx_fadmap(l)));
        max_gy = max(max_gy,max(gy_fadmap(l)));
     }
    max_g = max(max_gx,max_gy);
    cout << "max_g: " << max_g << endl;
    /*
     cifstream ifs1("../FAD_table.csv");
     if (!ifs1.is_open())
     {
       cerr << "no FADmap data" << endl;
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
       if (i <= 0 || m < i || j <= 0 || n < j)
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

     gx_fadmap.allocate(1,get_m(),1,get_n());
     gx_fadmap.initialize();
     gy_fadmap.allocate(1,get_m(),1,get_n());
     gy_fadmap.initialize();
     FADGradComp();
    */ 
     
     /*max_gx.allocate(0,ngrid);
     max_gx.initialize();
     cout << "gx_fadmap: "<< gx_fadmap << endl;
     cout << "gridmap: "<< gridmap << endl;
     int i = m;
     int j = n;
     int g = gridmap(i,j);         
     for (i = 1; i <= m; i++)
     {
        for (j = 1; j <= n;j++)
        {
           for (g = 1; g <= ngrid; g++)
           {
              if ( gridmap(i,j)==g)
              {
                 if (gx_fadmap(i,j) > max_gx(g))
                 {
                    max_gx(g) = gx_fadmap(i,j);
                 }}}}}
    
     max_gy.allocate(0,ngrid);
     max_gy.initialize();
     for (i = 1; i <= m; i++)
     {
        for (j = 1; j <= n;j++)
        {
           for (g = 1; g <= ngrid; g++)
           {
              if ( gridmap(i,j)==g)
              {
                 if (gy_fadmap(i,j) > max_gy(g))
                 {
                    max_gy(g) = gy_fadmap(i,j);
                 }}}}}

     cout << "max_gx: "<< max_gx << endl;
     cout << "max_gy: "<< max_gy << endl;
     //exit(1);*/ 
     
     //max_gx=max(gx_fadmap);
     //max_gy=max(gy_fadmap);
     
     /*
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
           
          else if (i==1)
          {
             gx_fadmap(i,j) = (fadmap(i+1,j)-fadmap(i,j))/(1.0*dx);
          }
          else if (i==m)
          {
             gx_fadmap(i,j) = (fadmap(i,j)-fadmap(i-1,j))/(1.0*dx);
          }
           if (gx_fadmap(i,j)<0.0)
           {
              TTRACE(i,j)
              //TRACE(gx_fadmap(i,j) )
              //gx_fadmap(i,j) *= 100.0;
              TRACE(gx_fadmap(i,j) )
           } 
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
           else if (j==1)
           {
              gy_fadmap(i,j) = (fadmap(i,j+1)-fadmap(i,j))/(1.0*dy);
           }
           else if (j==n)
           {
              gy_fadmap(i,j) = (fadmap(i,j)-fadmap(i,j-1))/(1.0*dy);
           }
           //if (gy_fadmap(i,j)<0.0)
           //   gy_fadmap(i,j) *= 10.0;
        }
     }*/ 
     
     //ifs1.close(); 
     
  
      if (m_ipar[84] == 1)
      {
        cout << "ipar[84] is 1" << endl;
        //istr >> mean_size;
        //istr >> sus_spd;
        istr >> chi;
       //cout << "mean size: " << mean_size << endl;
       //cout << "sustainable speed: " << sus_spd << endl;
      }  //if(m_ipar[84])
     
      fadmap_stratum_active_d.allocate(1,ngrid);
      fadmap_stratum_active_d.initialize();
      min_D.allocate(1, ngrid);
      min_D.initialize();
      logslope.allocate(1,ngrid);
      logslope.initialize();
      inflection.allocate(1,ngrid);
      inflection.initialize();
      loggamma.allocate(1,ngrid);
      loggamma.initialize();
      if (m_ipar[85] == 1)
      {
          for (int g = 1; g <= ngrid; g++)
          {
             int h;
             //istr >> h >> fadmap_stratum_active_d(g) >> min_D(g) >> slope(g) >> inflection(g);
             istr >> h >> fadmap_stratum_active_d(g) >> min_D(g) >> logslope(g) >> inflection(g);
             cout  << "grid: " << h << " " << "fadmap_stratum_active_d: " << fadmap_stratum_active_d(g) << " " << "min_D: " << min_D(g) << 
             " " <<"logslope: " << logslope(g) << " " << "inflection: " << inflection(g) <<endl;
          }
      }  //if(m_ipar[85]==1)
      else if (m_ipar[85] == 2)
      {
          for (int g = 1; g <= ngrid; g++)
          {
            istr >> loggamma(g);
            cout << "loggamma: " << loggamma(g) << endl;
          }
      } //else if(m_ipar[85]==2)
  }   //if(m_ipar[83]) 
  else
  {
      cout << "Both ipar [84][85] inactive unless ipar[83] = 1" << endl; 
  }
}
template void par_t_reg<d3_array,dmatrix,dvector,double>::read_seasons(cifstream& istr);
template void par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>::read_seasons(cifstream& istr);
