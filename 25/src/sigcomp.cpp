//$Id: sigcomp.cpp 3205 2014-08-19 00:50:00Z eunjung $
#include "par_t_reg.h"
#include "trace.h"
#include "math.h"

int myisnan(const dvariable& x)
{
  double y = value(x);
  return(isnan(y));
}
int myisnan(const double x)
{
  return(isnan(x));
}

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::sigma_comp(MATRIX& sigma, year_month& ym)
{
  int season = get_season(ym);
  int t=ym-start_date+1;//t=1;
  //clogf << "sigma_comp for " << ym << "; season = " << season << endl;
  VECTOR& ug = usergridSigma[season];
  //sigma.initialize();
  for (int i = 1; i <= m; i++)
  {
    const int j1 = jlb(i);
    const int jn = jub(i);
    for (int j = j1; j <= jn; j++)
    {
      const int k = gridmap[i][j];
      //const int grid = gridmap[i][j];
      if (m_ipar(83)==1)
      {
      if (m_ipar(85)==1 && max_fad[k]>0 && fadmap(t,i,j)>0) // && fadmap(t,i,j)<11) //&& k==10) //fadmap[i][j]>1)
      {
        // int k = gridmap[i][j]; 
        // //sigma[i][j] = ug[k];      
        // //max_D[i][j] = sigma[i][j];
        VECTOR slope=exp(logslope);
        const int grid = gridmap[i][j];
        if (1 <= grid && grid  <= ngrid)
        {
             DOUBLE max_D = ug[grid];  //DOUBLE max_D=ug[k];
             DOUBLE A = 1/(1+mfexp(slope[grid]*(max_fad[grid]-inflection[grid])));  
             DOUBLE B = 1/(1+mfexp(slope[grid]*-inflection[grid])); 
             DOUBLE L = ((A*max_D)-(B*min_D[grid]))/(A-B); //(A*max_D-B*min_D[grid]/(A-B);
             DOUBLE U = ((min_D[grid]*(1-B))-(max_D*(1-A)))/(A-B); //ug[grid]*(1+mfexp(-slope[grid]*inflection[grid]));
             sigma[i][j]=L+(U-L)/(1+mfexp(slope[grid]*(fadmap(t,i,j)-inflection[grid])));
             /*DOUBLE inflection = (maxinflection[grid])*0.5;
	     DOUBLE A=1/(1+exp(slope[grid]*(max_fad[grid]-inflection)));
             DOUBLE B=1/(1+exp(slope[grid]*-inflection));
             DOUBLE L=(A*max_D-B*min_D[grid])/(A-B);
             DOUBLE U=(min_D[grid]*(1-B)-max_D*(1-A))/(A-B);
             sigma[i][j]=L+(U-L)/(1+exp(slope[grid]*((double)fadmap[i][j]-inflection)));*/
             TRACE(grid)
             TTRACE(i,j)
             TRACE(max_fad[grid])
             TRACE(min_D[grid])
             TTRACE(slope[grid],inflection[grid])
             TRACE(max_D)
             TTRACE(L,U)
             TRACE(sigma[i][j])
        
             //if (myisnan(A) || myisnan(B) || myisnan(L) || myisnan(U))  //if (myisnan(sigma[i][j]))
             if (myisnan(sigma[i][j]))
             {
               TRACE(max_D)
               TTRACE(i,j)
               TTRACE(grid,sigma[i][j])
               TTRACE(A,B)
               TTRACE(L,U)
               cout << "fadmap[" << i << "],[" << j << "]: " << fadmap(t,i,j) << endl;
               cout << "max_fad[" << grid << "]: " << max_fad[grid] << endl;
               cout << "max_D: " << max_D << endl;
               cout << "A: " << A << endl;
               cout << "B: " << B << endl;
               cout << "L: " << L << endl;
               cout << "U: " << U << endl;
               cout << "sigma: " << sigma[i][j] << endl;
               exit(1);
             }
        }//if(1<=grid && grid <=ngrid)
      }//if(m_ipar(85)==1)
      if(m_ipar(85)==2)
      {
         /*const int grid = gridmap[i][j];
         DOUBLE max_D=ug[grid];
         DOUBLE gamma=exp(loggamma);
         sigma[i][j]=max_D*exp(-gamma*fadmap(t,i,j)); 
         //cout << "max_D: " << max_D << endl;
	 //cout << "gamma: " << gamma << endl;*/
        VECTOR gamma=exp(loggamma);
        const int grid = gridmap[i][j];
        if (1 <= grid && grid  <= ngrid)
        {
           const int grid = gridmap[i][j];
           DOUBLE max_D=ug[grid];
           sigma[i][j]=max_D*exp(-gamma[grid]*fadmap(t,i,j)); 
           //cout << "max_D: " << max_D << endl;
	   //cout << "gamma: " << gamma << endl;
	 }
      }
      }//if(m_ipar(83)==1) 
      else
      {
        int grid = gridmap[i][j];  //k=gridmap[i][j]; 
        sigma[i][j] = ug[grid];    //sigma[i][j]=ug[k];  
      }
    } //for j
  } //for i 
         //cout << "sigma: " << sigma << endl;
}


template void par_t_reg<d3_array,dmatrix,dvector,double>::sigma_comp(dmatrix& sigma, year_month& ym);
template void par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>::sigma_comp(dvar_matrix& sigma, year_month& ym);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::sigma_comp(dvar_matrix &sigmaSeasonGrid, dvar_matrix& sigma, const int season) 
{
	for (int i = 1; i <= m; i++) {
		const int jmin = jlb(i);
		const int jmax = jub(i);
		for (int j = jmin; j <= jmax; j++) {
			const int grid = gridmap[i][j];
			if (1 <= grid && grid <= ngrid) {
				sigma[i][j] = sigmaSeasonGrid(season, grid);
			}
		}
	}
}


