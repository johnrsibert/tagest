//$Id: tag_pack3.h 2754 2011-01-02 20:57:07Z jsibert $
#ifndef TAG_PACK3_H
#define TAG_PACK3_H
#include <fstream>
#include <fvar.hpp>
#include <adstring.hpp>
#include <jcalist.h>
#include <worms.h>
#include "yr_mon2.h"
#include "par_t_reg.h"
//#ifndef PAR_T_REG
//  class par_t_reg;
//#endif

class tag_packet
{
  static int statics_set;
  static par_t_reg<d3_array, dmatrix, dvector, double>* pptr;
  static double ymin, ymax;
  static double xmin, xmax;
  static double tau;
  //static
  double lambda;
  static double mu;
  static int packet_size;
  static dvector sdx, ssdx, sdy, ssdy, nsdd;

  int      cohort;  // tag release cohort
  int      ntag;    // number of tags in packet
  double   x;       // current position
  double   y;       // current position
  double   prevx;   // previous position
  double   prevy;   // previous position

  double set_direction(int& seed, double a);
  double set_pos(double oldx, double dx, double xmin, double xmax,
                 int boundary);
  int acomp(double& ax, double& ay, year_month& date);

public:
  static void set_static_par(par_t_reg<d3_array, dmatrix, dvector, double>& p);
  static int get_packet_size() {return packet_size;}

  tag_packet();
  //tag_packet(const double swlong, const double swlat);
  //tag_packet(int n, int tntag = 0);
  tag_packet(const int n);
  tag_packet(const double xlong, const double ylat, const int ntag, const int cohort);
  tag_packet(const tag_packet& t);
  ~tag_packet() {}

  static double get_tau(void) { return tau; }
  //int get_ntag(void) { return ntag; }

  //void walk(int& seed, year_month& date);
  int norm_walk(int& seed, year_month& date);
  //void set(int rn, int tntag = 0);
  //void add(tagreleasetype_& tr);

  void natural_mortality(int& seed);
  void fishing_mortality(int& seed, year_month& date, ofstream& f);
  void record_capture(ostream& f, adstring& fleet, year_month& date, int icap);

  double get_displacement(const trv_ptr& trv);

  void write_position(ostream& f);
  //int moveto(int color);
  //void set_at(int color);
  //void lineto(int color);
  //void recap_line(int color);
  ostream& operator << (ostream& s) const;
  int operator == (const tag_packet& t) const;
  tag_packet& operator = (const tag_packet& t);

  static void add_step_stats(const int k, const double dx, const double dy);
  static void report_all_step_stats(std::ostream& s);
  static void report_step_stats(const int k, std::ostream& s);
  friend class tag_packet_list;
  //friend class par_t_reg;
  friend ostream& operator << (ostream& s, const tag_packet& t);
};

class worms;

class tag_packet_list
{
private:
  int              npacket;
  jca::list<tag_packet*> packets;
  static par_t_reg<d3_array, dmatrix, dvector, double>* pptr;
public:
  tag_packet_list(par_t_reg<d3_array, dmatrix, dvector, double>* p);
  void release_tags(tagreleasetype_& tr);
  void add_worms(worms& w, tagreleasetype_& tr, const int color = 0);
  //void walk(int& seed, year_month& date);
  void norm_walk(int& seed, year_month& date);
  void update_worms(worms& w, const int nseg = -1);
  int get_npacket(void) {return(npacket);}
  dvector get_displacement(const trv_ptr& trv);
};


#endif // TAG_PACK3_H
