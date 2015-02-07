#ifndef M_AREA_H
#define M_AREA_H
#include <iomanip.h>
#include <fvar.hpp>
#include <adstring.hpp>
#include <cifstrem.h>
 
#include "list2.cpp"
#include "geo_cord.h"
#include "xypair.h"
#include "PathFileParser.h"

enum boundary_types
{
  ISLAND = -1,
  NONE = 0,
  LEFT_OPEN,
  RIGHT_OPEN,
  LEFT_CLOSED,
  RIGHT_CLOSED,
  CHANNEL
};

//forward declaration tells compiler of subsequent definition
class coff_t;

const int SZ = 80;
//const int VSZ = 5;

/* holds model area generic parameters */
class Model_area
{
  friend class coff_t;
  friend class ipar_t;
protected://Variables
  int non_land_cells; // number of cells in model domain that are not on land
  int m;              /* number of intervals in ew direction */
  int n;              /* number of intervals in ns direction */
  int west_bndry;     /* type of boundary at the four borders */
  int east_bndry;     /* see enum boundary_types */
  int north_bndry;
  int south_bndry;
  double deltax;      /* dx, a constant */
  double deltay;      /* dy, a constant */
  dvector del_x;      /* dx, changes with lat */
  imatrix gridmap;    /* array mappping small grid to larger world */
  imatrix cell_type;  /* specifies type of cell: boundary, center ... */
  ivector ilb;        /* lower and upper bounds for ragged model */
  ivector iub;
  ivector jlb;
  ivector jub;
  Geo_coord sw_coord;  /* longitude & latitude of south west corner of grid */
  List<XY_pair> lxy;   /* Linked list of coast line data*/
 
  //After new constructors made dump this
#ifdef __ZTC__
  long  sp;          /* position in .par file */
#else
  streampos  sp;     /* position in .par file */
#endif
  //Not really needed dump laters
  //char      dfile_version[VSZ+1];  /* holds first line of .par file */
  adstring dfile_version;  // holds first line of .par file
  static adstring par_file_version;  // holds first line of .par file

public://Variables
  //char    filename[SZ+1]; /* file name root for data files */
  PathFileParser pathname;
public://Functions
  Model_area();                     //modlarea.cpp
  Model_area(cifstream& prn);       //mpar.cpp
  Model_area(const Model_area&);          //modlarea.cpp
  Model_area(const char * name); //, adstring& par_file_ver); //mparead.cpp
  virtual ~Model_area();            //modlarea.cpp

  //inside.cpp
  int inside(const Longitude& lng, const Latitude& lat);
  int is_land(const Longitude& lng, const Latitude& lat);

  //int long_to_int(const double lng);
  //int lat_to_int(const double lat);
  //double int_to_long(const int i);
  //double int_to_lat(const int j);

  int long_to_index(const double lng);
  int long_to_index(const Longitude& lng);
  int lat_to_index(const double lat);
  int lat_to_index(const Latitude& lat);
  double index_to_long(const int i);
  double index_to_lat(const int j);


#ifndef __JAVA__
  //colormap.cpp
  void color_map(ivector& cv, const dvector& cscale,
                 const dmatrix& x, const char* title, const int ipar64);
  void color_map(ivector& cva, ivector& cvb, const dvector& cscale,
                 const dmatrix& a, const dmatrix& b, const char* title, 
                 const int ipar64);
#endif

  //cst_line.cpp
  void coast_line(int view, double mile_scale, int pasy, int ipar64);

protected://Functions
  //modlarea.cpp
  void      allocate(); 
  void      initialize();

  //modlread.cpp
  void  set_cell_types();
  void  read_gridmap(cifstream& istr);

private:
  //modlarea.cpp
  void      make_del_x();
  //setvbnds.cpp
  void      set_vector_bounds();

  // translates longitude or latitude end equator-dateline system to subscript
  int move_corner(const double x, const double dx, const int edge);
  // inverse translation
  double a_move_corner(const int i, const double dx, const int edge);

public://Access Functions
  void	set_n(int nv) { n = nv; }
  void	set_m(int mv) { m = mv; }
  void	set_deltax(dvector& dex) { del_x = dex; }
  void	set_deltay(double dv) { deltay = dv; }
  void  set_west_bndry(int t) { west_bndry = t; }
  void  set_east_bndry(int t) { east_bndry = t; }
  void  set_north_bndry(int t) { north_bndry = t; }
  void  set_south_bndry(int t) { south_bndry = t; }

  static adstring& get_par_file_version() { return par_file_version; } const
  int get_non_land_cells() { return(non_land_cells); } const
  int get_n() { return n; }
  int get_m() { return m; }
  int get_cell_type(int i, int j) { return cell_type[i][j]; }
  int get_jlb( int a ) { return jlb(a); }
  int get_jub( int b ) { return jub(b); }
  int get_gridmap(int i, int j) { return gridmap[i][j]; }
  int get_west_bndry() { return west_bndry; }
  int get_east_bndry() { return east_bndry; }
  int get_north_bndry() { return north_bndry; }
  int get_south_bndry() { return south_bndry; }
  double get_deltax(int j) { return (del_x(j)); }
  double get_deltax(double j);
  //double get_deltax(Long_lat& l);
  double get_deltay() { return deltay; }
  double get_deltax_eq() { return deltax; }
  double get_deltax_prn() { return deltax; }
  double get_long_lat_long() { return sw_coord.long_value(); }
  double get_long_lat_lat() { return sw_coord.lat_value(); }
  imatrix& get_cell_type() { return cell_type; }
  imatrix& get_gridmap() { return gridmap; }
  dvector& get_deltax() { return (del_x); }
  ivector& get_ilb() { return ilb; }
  ivector& get_iub() { return iub; }
  ivector& get_jlb() { return jlb; }
  ivector& get_jub() { return jub; }
  Geo_coord get_sw_coord() { return sw_coord; }
};//class Model_area
#endif //#ifdef M_AREA_H
