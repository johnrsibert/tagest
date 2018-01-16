#include "etopo_area.h"
#include <values.h>
#include <fvar.hpp>
#include "trace.h"

extern ofstream clogf;
/*
 * Note: etopo5 matrix [0..2160]x[0..4319] matrix in C++ term
 * Note: need to close file in Model_area destructor
 */
//===================================================================
//======================  CONSTRUCTORS ==============================
//===================================================================
/*
etopo_area::~etopo_area()
{
  cout << "in etopo_area::~etopo_area() em.trace_v() = " 
       << em.trace_v() << endl;
}
*/

etopo_area::etopo_area(const char* filename)
: Model_area(filename)
{
  em.allocate();
  TRACE(em.trace_v())
  threshold_elevation = 0;
  if ((60 % (int)deltax) == 0)
    delx = deltax / 5;
  else
  {
    cerr << "Error: deltax " << deltax << " is unable to partition"
            "the one degree square evenly." << endl;
    exit(1);
  }  

  if ((60 % (int)deltay) == 0)
    dely = deltay / 5;
  else
  {
    cerr << "Error: deltay " << deltay << " is unable to partition"
            "the one degree square evenly." << endl;
    exit(1);
  }

  TRACE(em.trace_v())
}//etopo_area(adstring)

int etopo_area::land(Longitude _long, Latitude _lat)
{
  int i0 = xindex(_long);
  int j0 = yindex(_lat);

  int number_5min_land_squares = 0;

  for (int j = j0; j > j0 - dely; j--)
  {
    for (int i = i0; i < i0 + delx; i++)
    {
      //cout << operator()(i, j) << " ";
      //if (operator()(i, j) >= threshold_elevation)
      if (em(i, j) >= threshold_elevation)
        number_5min_land_squares++;
    }
    //cout << endl;
  }

  return number_5min_land_squares;
}//End land

int etopo_area::min_elevation(Longitude _long, Latitude _lat)
{
  int i0 = xindex(_long);
  int j0 = yindex(_lat);
  double minElevation = MAXINT;

  for (int i = i0; i < i0 + delx; i++)
    for (int j = j0; j > j0 - dely; j--)
      if (em(i, j) < minElevation)
        minElevation = em(i, j);

  return (int)minElevation;
}//End min_elevation

int etopo_area::max_elevation(Longitude _long, Latitude _lat)
{
  int i0 = xindex(_long);
  int j0 = yindex(_lat);
  int maxElevation = -MAXINT;

  for (int i = i0; i < i0 + delx; i++)
    for (int j = j0; j > j0 - dely; j--)
      if (em(i, j) > maxElevation)
        maxElevation = em(i, j);

  return maxElevation;
}//End max_elevation

/*
double etopo_area::avg_elevation(Longitude _long, Latitude _lat)
{
  char buffer[80];
  long_block(_long, buffer);
  int i0 = xindex(buffer);
  lat_block(_lat, buffer);
  int j0 = yindex(buffer);
  int avgElevation = 0;

  for (int i = i0; i < i0 + delx_; i++)
    for (int j = j0; j > j0 - dely_; j--)
      avgElevation += em(i, j);
      //avgElevation += operator()(i, j);

//  return (float)(avgElevation / (delx_ * dely_));
  return avgElevation / (delx_ * dely_);
}//End avg_elevation
*/

void etopo_area::insert_land_gridmap(const double _percent_blocks_above_elvation)
{
  double ncell = delx * dely;
  int min = (int)(ncell * _percent_blocks_above_elvation);
  for (int j = 1; j <= n; j++)
  {
    double dLat = index_to_lat(j);
    Latitude lLat(dLat);
    for (int i = 1; i <= m; i++)
    {
      double dLong = index_to_long(i);
      Longitude lLong(dLong);

      int total = land(lLong, lLat);
      if (total >= min)
        gridmap(i, j) = 0;
    }//for j
  }//for i
}//End insert_land_gridmap

//Convert to index of etopo5_matrix (*12 for 5 min)
int etopo_area::xindex(const Longitude& _long)
  { return (float)(_long.value() * 12.0); }
int etopo_area::yindex(const Latitude& _lat)
  { return (float)((90.0 - _lat.value()) * 12.0); }

int etopo_area::write_matrix(ostream& os, const imatrix& t)
{
  for (int j = n; j >= 1; j--)
  {
     for (int i = 1; i <= m; i++)
       os << " " << t(i,j);
     os << "\n";
  }//for j
  return(os.rdstate());
} 

ostream& operator << (ostream& os, etopo_area& gm)
{
  imatrix& gridmap = gm.get_gridmap();
  int m = gm.get_m();
  int n = gm.get_n();

  if (!gridmap)
   return os;

  for (int j = n; j >= 1; j--)
  {
     for (int i = 1; i <= m; i++)
       os << " " << gridmap(i, j);
//       os << setw(3) << gridmap(i, j);
     os << "\n";
  }//for j

  return os;
}//operator<<(ostream&,etopo_area&)

//===================================================================
/*
void etopo_area::test_1()
{
  //Test if i = f(inverse f) = i
  for (int i = 0; i < 4320; i++)
      if (i != xindex(xlong(i)))
      cout << "Error: " << i << " " << xlong(i)
           << " != " << xindex(xlong(i)) <<  " debug long time.\n";

  //Test if i = f(inverse f) = i
  for (int j = 0; j <= 2160; j++)
    if (j != yindex(ylat(j)))
      cout << "Error: " << j << " " << ylat(j) << "  debug lat time.\n";

  //check coordinates in gridmap
  for (int m = sw_x_; i <= m_; i++)
    cout << i << " " << long_gridmap(i) << endl;
  for (int n = 1; j <= n_; j++)
    cout << j << " " << lat_gridmap(j) << endl
}//End test_1
*/
//End of File

