//$Id: xmparead.cpp 2951 2012-02-13 17:29:19Z jsibert $
#include "modlarea.h"
#include "bound.h"
#include <adstring.hpp>
#include "trace.h"

extern ofstream clogf;

Model_area::Model_area(const ADMB_XMLDoc& doc)
{
  cout << "Extracting XML Model_area data from " << doc.getFileName() << endl;
  clogf << "Extracting XML Model_area data from " << doc.getFileName() << endl;

  m = doc.getInt("m");
  n = doc.getInt("n");
  TTRACE(m,n)
  deltay = doc.getDouble("deltax");
  deltax = doc.getDouble("deltay");
  TTRACE(deltax,deltay)
  
  sw_coord.set((const char*)doc.getString("swlon"),(const char*)doc.getString("swlat"));
  TRACE(sw_coord)

  allocate();
  TTRACE(gridmap.rowmin(),gridmap.rowmax())
  TTRACE(gridmap.colmin(),gridmap.colmax())
  initialize();


  imatrix tmap = doc.getImatrix("gridmap");
  TRACE(tmap)
  TTRACE(tmap.rowmin(),tmap.rowmax())
  TTRACE(tmap.colmin(),tmap.colmax())
  non_land_cells = 0;
  for (int i = tmap.rowmin(); i <= tmap.rowmax(); i++)
  {
     for (int j = tmap.colmin(); j <= tmap.colmax(); j++)
     {
       gridmap(i,j) = tmap(i,j);
       if (gridmap(i,j))
          non_land_cells ++;
     }
  }
  clogf << "number of non-land cells in gridmap = " << non_land_cells << endl;

  clogf << "\nHuman-readable land mask:" << endl;
  for (int j=n; j>=1; j--)
  {
    for (int i=1; i<=m; i++)
    {
      clogf << setw(3) << gridmap(i,j);
    }
    clogf <<endl;
  }

  clogf << "boundary conditions prior to input:" << endl;
  clogf << "  " << west_bndry << "  " << east_bndry
        << "  " << south_bndry << "  " << north_bndry << endl;
  west_bndry = doc.getInt("west_bndry");
  east_bndry = doc.getInt("east_bndry");
  north_bndry = doc.getInt("north_bndry");
  south_bndry = doc.getInt("south_bndry");
  clogf << "boundary conditions:" << endl;
  clogf << "  " << west_bndry << "  " << east_bndry
        << "  " << south_bndry << "  " << north_bndry << endl;
 
  set_vector_bounds();
  set_cell_types();

  clogf << "Finished reading gridmap and boundary conditions." << endl;
  cout << "Finished reading gridmap and boundary conditions." << endl;

  clogf << "Fishished reading model area parameters." << endl;
  cout << "Fishished reading model area parameters." << endl;

  clogf << "\n Cell types:"<< endl;
  for (int j=n; j>=1; j--)
  {
    for (int i=1; i<=m; i++)
    {
      clogf << setw(4) << hex << (unsigned short)cell_type[i][j];
    }
    clogf << dec <<endl;
  }
  HERE
}
