//$Id: modlarea.cpp 2845 2011-08-22 23:21:22Z jsibert $
#include "modlarea.h"

ivector Model_area::di;
ivector Model_area::dj;

Model_area::Model_area()
{
  //cout << "Top of Model_area null constructor" << endl;
  //cout << "Bottom of Model_area null constructor" << endl;
}//Model_area()

Model_area::~Model_area()
{
  //cout << "Top of Model_area destructor" << endl;
  //cout << "Bottom of Model_area destructor" << endl;
}//~Model_area()

void Model_area::allocate()
{
  //cout << "allocate" << endl;
  gridmap.allocate(1, m, 1, n);   gridmap.initialize();
  cell_type.allocate(1, m, 1, n); cell_type.initialize();
  ilb.allocate(1,n); ilb.initialize();
  iub.allocate(1,n); iub.initialize();
  jlb.allocate(1,m); jlb.initialize();
  jub.allocate(1,m); jub.initialize();
  if (!di)
  {
     di.allocate(1,8);
     di.fill("{-1, 1, 0, 0, 1, 1,-1,-1}");
  }
  if (!dj)
  {
     dj.allocate(1,8);
     dj.fill("{ 0, 0,-1, 1, 1,-1, 1,-1}");
  }
  //cout << "out allocate" << endl;
/*
  for (int i=1; i<=m; i++)
    for (int j=1; j<=n; j++)
      cell_type[i][j] = NONE;
*/
}//allocate()

void Model_area::initialize()
{
  make_del_x();
}//initialize()

void Model_area::make_del_x()
{
  del_x.allocate(1, n);
  //del_x = deltay;
  double lat_val = get_long_lat_lat();
  const double R = 3437.746771; //Nm

  //double PI = 3.14159265358979323846;
  for (int i=1; i <= n; i++)
  {
    del_x[i] = R * cos( fabs(lat_val)*M_PI/180.0 ) * (M_PI / 180.0 ) ;
    lat_val += 1.0;
  }
}//make_del_x()


//initialize model_area object to another model_area object
Model_area::Model_area(const Model_area& t)
: m(t.m), n(t.n),
  west_bndry(t.west_bndry), east_bndry(t.east_bndry),
  north_bndry(t.north_bndry), south_bndry(t.south_bndry),
  deltax(t.deltax), deltay(t.deltay), del_x(t.del_x),
  gridmap(t.gridmap), cell_type(t.cell_type),
  ilb(t.ilb), iub(t.iub), jlb(t.jlb), jub(t.jub),
  dfile_version(t.dfile_version)
{
  //cout << "Top of Model_area copy constructor" << endl;

  sw_coord = (Geo_coord)t.sw_coord;
  lxy = t.lxy;
  sp = t.sp;

  //for (int i = 0; i < SZ; i++)
  //  filename[i] = t.filename[i];
  //pathname(t.pathname);
  pathname = t.pathname;

  //for (int i = 0; i < VSZ; i++)
  //  dfile_version[i] = t.dfile_version[i];
  //cout << "Bottom of Model_area copy constructor" << endl;
}
