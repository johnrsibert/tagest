#include "modlarea.h"
#include "par_t_reg.h"
#include "bound.h"
#include "trace.h"

void bad_cell_type(const char* function, int boundary); // coffcmp6.cpp
/** Compute FAD density gradients. Uses cell_type matrix to handle boundaries
consistently with the algrorithm use to compute tridiagonal coefficients.
*/
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::FADGradComp(void)
{
     const double dx = get_deltax_eq();
     const double dy = get_deltay();
     //TTRACE(fadmap.rowmin(),fadmap.rowmax())
     year_month final_date = get_tr_date(nrelease) + nmonth;
     int tmn = final_date-start_date+1;   
     int t = 1;
     //cout << "fadmap: " << fadmap << endl;
     for (year_month ym = start_date; ym <= final_date; ym++)
     {
        for (int i=1; i<=m; i++)
        {
           //TTRACE(fadmap(i).indexmin(),fadmap(i).indexmax())
           for (int j=1; j<=n; j++)
           {
              bound boundary;
              boundary = get_cell_type(i,j);

              switch (boundary.edgex())
              {
                 case NONE:
                 gx_fadmap(t,i,j) = (fadmap(t,i+1,j)-fadmap(t,i-1,j))/(2.0*dx);
                 break;
                 case LEFT_OPEN:
                 case LEFT_CLOSED:
                 gx_fadmap(t,i,j) = (fadmap(t,i+1,j)-fadmap(t,i,j))/(1.0*dx);
                 break;
                 case RIGHT_OPEN:
                 case RIGHT_CLOSED:
                 gx_fadmap(t,i,j) = (fadmap(t,i,j)-fadmap(t,i-1,j))/(1.0*dx);
                 break;
                 case ISLAND:
                 case CHANNEL:
                 gx_fadmap(t,i,j) = 0.0;
                 break;
                 default:
                 bad_cell_type("FADGradComp gx", boundary.edgex());
                 //if(gx_fadmap(t,i,j) > max_gx)
                 //{
                 //   max_gx = gx_fadmap(t,i,j);
                 //}
               }
           
               switch (boundary.edgey())
               {
                case NONE:
                  gy_fadmap(t,i,j) = (fadmap(t,i,j+1)-fadmap(t,i,j-1))/(2.0*dy);
                  break;
                case LEFT_OPEN:
                case LEFT_CLOSED:
                  gy_fadmap(t,i,j) = (fadmap(t,i,j+1)-fadmap(t,i,j))/(1.0*dy);
                  break;
                case RIGHT_OPEN:
                case RIGHT_CLOSED:
                  gy_fadmap(t,i,j) = (fadmap(t,i,j)-fadmap(t,i,j-1))/(1.0*dy);
                  break;
                case ISLAND:
                case CHANNEL:
                  gx_fadmap(t,i,j) = 0.0;
                  break;
                default:
                  bad_cell_type("FADGradComp gy", boundary.edgey());
                 //if(gy_fadmap(t,i,j) > max_gy)
                 //{
                 //   max_gy = gy_fadmap(t,i,j);
                 //}
               }
           } //for(j)
        } //for(i)
        t++;
     } //for(year_month)
     //cout << "gx_fadmap: " << gx_fadmap << endl;
     //cout << "gy_fadmap: " << gy_fadmap << endl;
}
template void par_t_reg<d3_array,dmatrix,dvector,double>::FADGradComp(void);
template void par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>::FADGradComp(void);

