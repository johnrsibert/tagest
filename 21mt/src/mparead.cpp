#include "modlarea.h"
#include "bound.h"
#include <adstring.hpp>

extern ofstream clogf;

Model_area::Model_area(const char* name)
{
  adstring fullname;

  cout << "Reading Model_area data for " << name << endl;
  clogf << "Reading Model_area data for " << name << endl;

  fullname = adstring(name);
  int dot_pos = fullname.pos(".");
  if (dot_pos == 0)
  {
    fullname += ".par";
  }
  cifstream dfile(fullname);
  if ( !dfile )
  {
    cerr << "Cannot open file " << fullname << endl;
    exit(1);
  }
  dfile_version = dfile.signature();
  //par_file_version = "#v21n";   // msa 3/18/2002 3:28PM - jrs belongs in savefit
  cout << "File version " << dfile_version << endl;
  if (dfile_version != par_file_version)
  {
    cerr << "\nWarning: File signature difference for " << fullname << endl;
    cerr << "         Found " << dfile_version 
         << " should have " << par_file_version << endl;
    cerr << "         This could cause errors reading this file." << endl;
    exit(1);
  }
  dfile >> m >> n >> deltax >> deltay >> sw_coord;

  if ( !dfile )
  {
    cerr << "Error reading sizes in file " << fullname << endl;
    exit(1);
  }
 
  allocate();
  initialize();
 
  //strcpy(filename,name);
  pathname.setPathFile(name);

  read_gridmap(dfile);   //Also, reads the boundary conditions
  cout << "Finished reading gridmap and boundary conditions." << endl;
 
  //return current position in file
  sp = dfile.tellg();
  dfile.close();
  clogf << "Fishished reading model area parameters." << endl;
  cout << "Fishished reading model area parameters." << endl;

  for (int j=n; j>=1; j--)
  {
    for (int i=1; i<=m; i++)
    {
      clogf << setw(4) << hex << (unsigned short)cell_type[i][j];
    }
    clogf << dec <<endl;
  }
}


void Model_area::read_gridmap(cifstream& istr)
{
  non_land_cells = 0;
  for (int j=n; j>=1; j--)
  {
    //cout << "row " << j << ": ";
    for (int i=1; i<=m; i++)
    {
      istr >> gridmap[i][j];
      if (!istr)
      {
       cerr << "Error reading gridmap: row " << j
       << ", column " << i
       << endl;
       cerr << "  value: " << gridmap[i][j] << endl;
       cerr << "  stream state: " << istr.rdstate() << endl;
       exit(1);
      }
      if (gridmap(i,j))
        non_land_cells ++;
    }
  }
  clogf << "number of cells in gridmap = " << non_land_cells << endl;
 
  istr >> west_bndry >> east_bndry >> south_bndry >> north_bndry;
  clogf << "boundary conditions:" << endl;
  clogf << "  " << west_bndry << "  " << east_bndry
        << "  " << south_bndry << "  " << north_bndry << endl;
  if (!istr)
  {
    cerr << "**  error reading boundary conditions" << endl;
    exit(1);
  }
 
  set_vector_bounds();
  set_cell_types(); // moved to read_gridmap()
}

void Model_area::set_cell_types()
{
  bound boundary;
 
  int i, j;
 
  boundary = NONE;
  for (i = 1; i <= m; i++)
  {
    for (j = 1; j <= n; j++)
    {
      cell_type[i][j] = boundary;
    }
  }
 
  i = 1;
  boundary.set_edgey(NONE);
  if (!west_bndry)
    boundary.set_edgex(LEFT_OPEN);
  else
    boundary.set_edgex(LEFT_CLOSED);

  for (j = 1; j <= n; j++)
  {
    cell_type[i][j] = boundary;
  }
 
  i = m;
  boundary.set_edgey(NONE);
  if (!east_bndry)
    boundary.set_edgex(RIGHT_OPEN);
  else
    boundary.set_edgex(RIGHT_CLOSED);
  for (j = 1; j <= n; j++)
  {
    cell_type[i][j] = boundary;
  }
 
  j = 1;
  boundary.set_edgex(NONE);
  if (!south_bndry)
    boundary.set_edgey(LEFT_OPEN);
  else
    boundary.set_edgey(LEFT_CLOSED);
  for (i = 1; i <= m; i++)
  {
    cell_type[i][j] = boundary;
  }
 
  j = n;
  boundary.set_edgex(NONE);
  if (!north_bndry)
    boundary.set_edgey(RIGHT_OPEN);
  else
    boundary.set_edgey(RIGHT_CLOSED);
  for (i = 1; i <= m; i++)
  {
    cell_type[i][j] = boundary;
  }
 
  // corners
  for (i = 1; i <= m; i+=(m-1))
  {
    for (j = 1; j <= n; j+=(n-1))
    {
      if (j == 1)
      {
        if (!south_bndry)
          boundary.set_edgey(LEFT_OPEN);
        else
          boundary.set_edgey(LEFT_CLOSED);
      }
      else // j = n
      {
        if (!north_bndry)
          boundary.set_edgey(RIGHT_OPEN);
        else
          boundary.set_edgey(RIGHT_CLOSED);
      }
 
      if (i == 1)
      {
        if (!west_bndry)
          boundary.set_edgex(LEFT_OPEN);
        else
          boundary.set_edgex(LEFT_CLOSED);
      }
      else // i = n;
      {
        if (!east_bndry)
          boundary.set_edgex(RIGHT_OPEN);
        else
          boundary.set_edgex(RIGHT_CLOSED);
      }
 
      cell_type[i][j] = boundary;
    }
  }
  // islands
  for (i = 1; i <= m; i++)
  {
    for (j = 1; j <= n; j++)
    {
      if (!gridmap[i][j])
      {
        if (i > 1)
        {
          boundary = cell_type[i-1][j];
          if (boundary.edgex() == NONE)
          {
            boundary.set_edgex(RIGHT_CLOSED);
            cell_type[i-1][j] = boundary;
          }
          else if (boundary.edgex() == LEFT_CLOSED)
          {
            boundary.set_edgex(CHANNEL);
            cell_type[i-1][j] = boundary;
          }
       }
       if (i < m)
       {
         boundary = cell_type[i+1][j];
         if (boundary.edgex() == NONE)
         {
           boundary.set_edgex(LEFT_CLOSED);
           cell_type[i+1][j] = boundary;
         }
         else if (boundary.edgex() == RIGHT_CLOSED)
         {
           boundary.set_edgex(CHANNEL);
           cell_type[i+1][j] = boundary;
         }
       }
       if (j > 1)
       {
         boundary = cell_type[i][j-1];
         if (boundary.edgey() == NONE)
         {
           boundary.set_edgey(RIGHT_CLOSED);
           cell_type[i][j-1] = boundary;
         }
         else if (boundary.edgey() == LEFT_CLOSED)
         {
           boundary.set_edgey(CHANNEL);
           cell_type[i][j-1] = boundary;
         }
       }
       if (j < n)
       {
         boundary = cell_type[i][j+1];
         if (boundary.edgey() == NONE)
         {
           boundary.set_edgey(LEFT_CLOSED);
           cell_type[i][j+1] = boundary;
         }
         else if (boundary.edgey() == RIGHT_CLOSED)
         {
           boundary.set_edgey(CHANNEL);
           cell_type[i][j+1] = boundary;
         }
       }
       boundary.set_edgex(ISLAND);
       boundary.set_edgey(ISLAND);
       cell_type[i][j] = boundary;
      }
    } // j loop
  } // i loop
}
