//$Id: ruvscomp.cpp 3127 2013-12-05 22:40:36Z jsibert $
#include "par_t_reg.h"
//#include <strstream>

void uvs_read(dmatrix& x, adstring& file_name, int m,
              ivector& jlb, ivector& jub);
void rowcol_average(dmatrix& x, const int npoint);
void rowcol_average(dvar_matrix& x, const int npoint);
//template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
/*void par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::read_fadmap(dmatrix& fadmap, const int year, const int month)
{
   if (m_ipar[83] ==1)
   {
     cout << "ipar[83] is " << m_ipar[83] << endl;
     std::ostrstream ostr;
     ostr << "../FAD_maps/fadmap_" << year << "_" 
          << std::setw(2) << std::setfill('0') << month << ".fad" << std::ends;
     cout << "the string name is "<< ostr.str() << endl;
     cifstream ifs(ostr.str());
     if (!ifs.is_open())
     {
       cerr << "no FADmap available at " << year << "_" << month << endl;
       exit(1);
     }
     fadmap.allocate(1,get_m(),1,get_n());
     fadmap.initialize();
     max_fad.allocate(0,ngrid);
     max_fad.initialize();
     //cout << "fadmap" << fadmap << endl;
     //cout << "max_fad" << max_fad << endl;
     while(!ifs.eof())
     {
       double lon;
       double lat;
       int FADno;
       ifs >> lon >> lat >> FADno;
       if(ifs.eof())
       {
          break;
       }
       int i = long_to_index(lon);
       int j = lat_to_index(lat);
       if(i<=0 || m<i || j<=0 || n<j)
       {
          continue;
       }
       fadmap(i,j) = FADno;
       int g=gridmap(i,j);
       if(gridmap(i,j)==0)
       {
         fadmap(i,j)=0;
       }
       if(fadmap(i,j)>max_fad[g])
       {
         max_fad[g]=fadmap(i,j);
       }
     }
     //max_fad = max(fadmap);  
     maxinflection.allocate(1,ngrid);
     maxinflection.initialize();
     maxinflection=max_fad(1,ngrid);
     cout << "maxinflection: " << maxinflection << endl;
     gx_fadmap.allocate(1,get_m(),1,get_n());
     gx_fadmap.initialize();
     gy_fadmap.allocate(1,get_m(),1,get_n());
     gy_fadmap.initialize();
     FADGradComp();
     max_gx=max(gx_fadmap);
     max_gy=max(gy_fadmap);
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
     cout << "max_gy: "<< max_gy << endl;*/

//     ifs.close();
     //exit(1);

//   }

//}
//extern ofstream clogf;
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::uvs_comp(MATRIX& u, MATRIX& v, MATRIX& sigma, year_month& ym)
{
  //clogf << "uvs_comp for " << ym << endl;
  const int year = ym.get_year();
  const int month = ym.get_month_value();
  //read_fadmap(fadmap,year,month);
  //int time_step = ym-start_date+1;
  //cout << "time_step: " << time_step << endl;
  //cout << "fadmap_t: " << fadmap_t(time_step) << endl;
  //exit(1);
  if(m_ipar[83] ==1)
  {
  FADGradComp();
  }
  uvinterpolate(u, v, ym);
  sigma_comp(sigma, ym);
  rowcol_average(sigma, m_ipar(36));
}
template void par_t_reg<d3_array,dmatrix,dvector,double>::uvs_comp(dmatrix& u, dmatrix& v, dmatrix& sigma, year_month& ym);
template void par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>::uvs_comp(dvar_matrix& u, dvar_matrix& v, dvar_matrix& sigma, year_month& ym);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::uvs_comp(dvar_matrix &uSeasonGrid, dvar_matrix &vSeasonGrid, dvar_matrix &sigmaSeasonGrid, dvar_matrix &u, dvar_matrix &v, dvar_matrix &sigma, const int season) {
	uvinterpolate(uSeasonGrid, vSeasonGrid, u, v, season);
	sigma_comp(sigmaSeasonGrid, sigma, season);
	rowcol_average(sigma, m_ipar(36));
}
