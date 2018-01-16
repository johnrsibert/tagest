#ifndef TAG_PACK3_H
#define TAG_PACK3_H
#include <fstream>
#include <adstring.hpp>
#include <jcalist.h>
#include <worms.h>
#include "yr_mon2.h"
#ifndef PAR_T_REG
  class par_t_reg;
#endif

class tag_packet
{
  static int statics_set;
  static par_t_reg* pptr;
  static double ymin, ymax;
  static double xmin, xmax;
  static double tau;
  //static
  double lambda;
  static double mu;
  static int packet_size;
  static dvector sdx, ssdx, sdy, ssdy, nsdd;

  //int      rn;      // tag release cohort
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
  static void set_static_par(par_t_reg& p);
  static int get_packet_size() {return packet_size;}

  tag_packet();
  //tag_packet(const double swlong, const double swlat);
  //tag_packet(int n, int tntag = 0);
  tag_packet(const int n);
  tag_packet(const double xlong, const double ylat, const int ntag);
  tag_packet(const tag_packet& t);
  ~tag_packet() {}

  static double get_tau(void) { return tau; }
  //int get_ntag(void) { return ntag; }

  void walk(int& seed, year_month& date);
  void norm_walk(int& seed, year_month& date);
  //void set(int rn, int tntag = 0);
  //void add(tagreleasetype_& tr);

  void natural_mortality(int& seed);
  void fishing_mortality(int& seed, year_month& date, ofstream& f);
  void record_capture(ostream& f, adstring& fleet, year_month& date, int icap);

  void write_position(ostream& f);
  //int moveto(int color);
  //void set_at(int color);
  //void lineto(int color);
  //void recap_line(int color);
  ostream& operator << (ostream& s) const;
  int operator == (const tag_packet& t) const;
  tag_packet& operator = (const tag_packet& t);

  static void add_step_stats(const int k, const double dx, const double dy);
  static void report_step_stats(const int k, ostream& s);
  friend class tag_packet_list;
  friend class par_t_reg;
  friend ostream& operator << (ostream& s, const tag_packet& t);
};

class worms;

class tag_packet_list
{
public:
  virtual ~tag_packet_list()
  {
    //jca2002-04-18: clean up memory
    jca::list<tag_packet*>::node* n = packets.getHead()->next;
    while (n)
    {
      if (n->t) delete n->t;

      n = n->next;
    }
  }

private:
  int              npacket;
  jca::list<tag_packet*> packets;
  static par_t_reg* pptr;
public:
  tag_packet_list(par_t_reg* p);
  void release_tags(tagreleasetype_& tr);
  void add_worms(worms& w, tagreleasetype_& tr);
  void walk(int& seed, year_month& date);
  void norm_walk(int& seed, year_month& date);
  void update_worms(worms& w);
  int get_npacket(void) {return(npacket);}
};


#endif // TAG_PACK3_H
