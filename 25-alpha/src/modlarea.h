//$Id: modlarea.h 3125 2013-11-19 02:44:01Z jsibert $
#ifndef M_AREA_H
#define M_AREA_H
#if (__GNUC__ >=3)
  #include <iomanip>
#else
  #include <iomanip.h>
#endif
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


class Shift;

/** Variables and functions specific to model domain.
Base class for various model parameter classes.
*/
class Model_area
{
  //friend class coff_t;
  friend class ipar_t;
public://Variables
  int non_land_cells; ///< number of cells in model domain that are not on land
  int m;              ///< number of intervals in ew direction 
  int n;              ///< number of intervals in ns direction
  int west_bndry;     ///< type of boundary at the four borders
  int east_bndry;     ///< see enum boundary_types
  int north_bndry;
  int south_bndry;
  double deltax;      ///< dx, a constant
  double deltay;      ///< dy, a constant
  dvector del_x;      ///< dx, changes with lat
  imatrix gridmap;    ///< array mappping small grid to larger world
  imatrix cell_type;  ///< specifies type of cell: boundary, center ...
  ivector ilb;        ///< lower and upper bounds for ragged model
  ivector iub;
  ivector jlb;
  ivector jub;
  Geo_coord sw_coord;  ///< longitude & latitude of south west corner of grid
  List<XY_pair> lxy;   ///< Linked list of coast line data

  static ivector di;
  static ivector dj;

  dmatrix fadmap;          ///< matrix of fadmap
  dmatrix gx_fadmap;        ///< gradient matrix of fadmap in x
  dmatrix gy_fadmap;        ///< gradient matrix of fadmap in y
 
 
  //After new constructors made dump this
#ifdef __ZTC__
  long  sp;          ///< position in .par file
#else
  streampos  sp;     ///< position in .par file
#endif
  adstring dfile_version;  // holds first line of .par file
protected:
  adstring par_file_version;  // holds first line of .par file
public:
  adstring getParFileVersion() const { return par_file_version; }

public://Variables
  PathFileParser pathname;
public://Functions
  Model_area();                     //modlarea.cpp
  Model_area(cifstream& prn);       //mpar.cpp
  Model_area(const Model_area&);          //modlarea.cpp
  Model_area(const char * name, const adstring parversion); //mparead.cpp

  virtual ~Model_area();            //modlarea.cpp

  //inside.cpp
  int inside(const Longitude& lng, const Latitude& lat);
  int is_land(const Longitude& lng, const Latitude& lat);

  int long_to_index(const double lng);
  int long_to_index(const Longitude& lng);
  int lat_to_index(const double lat);
  int lat_to_index(const Latitude& lat);
  double index_to_long(const int i) const;
  double index_to_lat(const int j) const;

  int shift_position(adstring& message, Longitude& i_long, Latitude& i_lat, List<Shift>& ls);


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
  //void FADGradComp(void);

private:
  //modlarea.cpp
  void      make_del_x();
  //setvbnds.cpp
  void      set_vector_bounds();

  // translates longitude or latitude end equator-dateline system to subscript
  int move_corner(const double x, const double dx, const int edge);
  // inverse translation
  double a_move_corner(const int i, const double dx, const int edge) const;

public://Access Functions
  void	set_n(int nv) { n = nv; }
  void	set_m(int mv) { m = mv; }
  void	set_deltax(dvector& dex) { del_x = dex; }
  void	set_deltay(double dv) { deltay = dv; }
  void  set_west_bndry(int t) { west_bndry = t; }
  void  set_east_bndry(int t) { east_bndry = t; }
  void  set_north_bndry(int t) { north_bndry = t; }
  void  set_south_bndry(int t) { south_bndry = t; }

  int get_non_land_cells() { return(non_land_cells); } const
  int get_n() const { return n; }
  int get_m() const { return m; }
  int get_cell_type(int i, int j) { return cell_type[i][j]; }
  int get_jlb(int i) const { return jlb(i); }
  int get_jub(int i) const { return jub(i); }
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
  Geo_coord get_sw_coord() const { return sw_coord; }
};//class Model_area
#endif //#ifdef M_AREA_H
