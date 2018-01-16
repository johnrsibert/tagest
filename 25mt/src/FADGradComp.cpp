#include "modlarea.h"
#include "bound.h"
#include "trace.h"

void bad_cell_type(const char* function, int boundary); // coffcmp6.cpp
/** Compute FAD density gradients. Uses cell_type matrix to handle boundaries
consistently with the algrorithm use to compute tridiagonal coefficients.
*/

void Model_area::FADGradComp(void)
{
     const double dx = deltax;
     const double dy = deltay;
     for (int i=1; i<=m; i++)
     {
        for (int j=1; j<=n; j++)
        {
           bound boundary;
           boundary = get_cell_type(i,j);

           switch (boundary.edgex())
           {
             case NONE:
               gx_fadmap(i,j) = (fadmap(i+1,j)-fadmap(i-1,j))/(2.0*dx);
               break;
             case LEFT_OPEN:
             case LEFT_CLOSED:
               gx_fadmap(i,j) = (fadmap(i+1,j)-fadmap(i,j))/(1.0*dx);
               break;
             case RIGHT_OPEN:
             case RIGHT_CLOSED:
               gx_fadmap(i,j) = (fadmap(i,j)-fadmap(i-1,j))/(1.0*dx);
               break;
             case ISLAND:
             case CHANNEL:
               gx_fadmap(i,j) = 0.0;
               break;
             default:
               bad_cell_type("FADGradComp gx", boundary.edgex());
           }
           
           switch (boundary.edgey())
           {
             case NONE:
               gy_fadmap(i,j) = (fadmap(i,j+1)-fadmap(i,j-1))/(2.0*dy);
               break;
             case LEFT_OPEN:
             case LEFT_CLOSED:
               gy_fadmap(i,j) = (fadmap(i,j+1)-fadmap(i,j))/(1.0*dy);
               break;
             case RIGHT_OPEN:
             case RIGHT_CLOSED:
               gy_fadmap(i,j) = (fadmap(i,j)-fadmap(i,j-1))/(1.0*dy);
               break;
             case ISLAND:
             case CHANNEL:
               gx_fadmap(i,j) = 0.0;
               break;
             default:
               bad_cell_type("FADGradComp gy", boundary.edgey());
           }
        }
     } 
}
