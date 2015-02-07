#include <fstream.h>
#include "par_treg.h"
#include "tag_pack3.h"
#include "trace.h"
//#include "./worms.h"
#include <worms.h>


extern ofstream clogf;
extern int randn_count;
float ran1(int* idum);
double randn(long int& n);
int tag_packet::statics_set = 0;
int tag_packet::packet_size = 20;

par_t_reg*   tag_packet_list::pptr=0;
par_t_reg*   tag_packet::pptr=0;
double tag_packet::ymin, tag_packet::ymax;
double tag_packet::xmin, tag_packet::xmax;
double tag_packet::tau;
//double tag_packet::lambda;
double tag_packet::mu;
dvector tag_packet::sdx, tag_packet::ssdx, tag_packet::sdy, tag_packet::ssdy,
        tag_packet::nsdd;

double my_randn(long int& n)
{
   double r;
   r = randn(n);
   randn_count++;
   return(r);
}

void tag_packet::set_static_par(par_t_reg& p)
{
  TRACE(pptr)
  pptr = &p;
  TRACE(pptr)

  xmin = p.get_long_lat_long(); //index_to_long(1);
  ymin = p.get_long_lat_lat(); //index_to_lat(1);
  TTRACE(xmin,ymin)

  xmax = p.index_to_long(p.get_m()+1);
  ymax = p.index_to_lat(p.get_n()+1);
  TTRACE(xmax,ymax)
  /*
  const m = pptr->get_m();
  const n = pptr->get_n();
  TTRACE(m,n)

  if (!xmin)
    xmin.allocate(1,n);
  if (!xmax)
    xmax.allocate(1,n);
  if (!ymin)
    ymin.allocate(1,m);
  if (!ymax)
    ymax.allocate(1,m);

  for (int i = 1; i <= m; i++)
  {
    int kk = 1;
    for (int j = 1; (kk) && (j <= n) ; j++)
    {
      int k = pptr->get_gridmap(i,j);
      if (k > 0)
      {
        ymin(i) = i; //pptr->index_to_lat(i);
        kk = 0;
      }
    }
    kk = 1;
    for (int j = n; (kk) && (j >= 1); j--)
    {
      int k = pptr->get_gridmap(i,j);
      if (k > 0)
      {
        ymax(i) = i; //pptr->index_to_lat(i);
        kk = 0;
      }
    }
  }
  TRACE(ymin)
  TRACE(ymax)
  if (1)
    exit(1); 
  */
  if (p.m_ipar[37] <= 0)
  {
    clogf << "You must specify constant swimming speed in ipar[37]" << endl;
    exit(1);
  }

  //     m/sec       *     sec/mo    *   Nmi/m     = Nmi/mo
  //  ipar[37]/100         2073600     1/1852
  // "characteristic" rate, lambda/tau
  //mu = double(p.m_ipar[37])/100.0 * 2073600.0/1852.0; // Nmi/mo

  //     Nmi/da                   * da/mo
  mu = double(p.m_ipar[37])/100.0 * 30.0; // Nmi/mo

  //tau = 1.0/pptr->m_ipar[6]; // time steps / mo
  tau = 1.0/720.0; // 24*30 time steps per month
  TTRACE(mu,tau)

  int ngrid = pptr->get_ngrid();
  sdx.allocate(0,ngrid); sdx.initialize();
  sdy.allocate(0,ngrid); sdy.initialize();
  ssdx.allocate(0,ngrid); ssdx.initialize();
  ssdy.allocate(0,ngrid); ssdy.initialize();
  nsdd.allocate(0,ngrid); nsdd.initialize();
  statics_set = 1;
}

tag_packet::tag_packet(const tag_packet& t)
{
  //clogf << endl << "Called tag_packet::tag_packet(const tag_packet& t)"
  //      << endl;
  //exit(1);

  lambda = t.lambda;
  x = t.x;
  y = t.y;
  prevx = t.prevx;
  prevy = t.prevy;
  ntag = t.ntag;
}

/*
tag_packet::tag_packet(const double swlong, const double swlat)
{
  if (!statics_set)
  {
    clogf << "You must call tag_packet::set_static_par(par_t& p) "
	      "prior to declaring a variable\n"
	      "of type tag_packet" << endl;
    exit(1);
  }
  //rn = 0;
  x = 0;
  y = 0;
  prevx = 0;
  prevy = 0;
  //ntag = 0;
}
*/

tag_packet::tag_packet()
{
  if (!statics_set)
  {
    clogf << "You must call tag_packet::set_static_par(par_t& p) "
	      "prior to declaring a variable\n"
	      "of type tag_packet" << endl;
    exit(1);
  }
  //rn = 0;
  x = 0;
  y = 0;
  prevx = 0;
  prevy = 0;
  ntag = 0;
}

tag_packet::tag_packet(const int n)
{
  if (n != 0)
  {
    clogf << "tag_packet::tag_packet(const int n) called with non zero argument" << endl;
    clogf << "  n = " << n << endl;
    exit(1);
  }
  tag_packet::tag_packet();
}

tag_packet::tag_packet(const double xlong, const double ylat,
                       const int ntp)
{
  if (!statics_set)
  {
    clogf << "You must call tag_packet::set_static_par(par_t& p) "
	      "prior to declaring a variable\n"
	      "of type tag_packet" << endl;
    exit(1);
  }
  x = xlong;
  y = ylat;
  prevx = x;
  prevy = y;
  ntag = ntp;
}

/*
tag_packet::tag_packet(int trn, int tntag)
{
  if (!statics_set)
  {
    clogf << "You must call tag_packet::set_static_par(par_t& p) "
	      "prior to declaring a variable\n"
	      "of type tag_packet" << endl;
    exit(1);
  }
  set(trn, tntag);
}
*/

/*
void tag_packet::set(int n, int tntag)
{
  int rn = n;
  x = pptr->index_to_long(pptr->get_tr_i(rn));
  y = pptr->index_to_lat(pptr->get_tr_j(rn));
  prevx = x;
  prevy = y;
  if (tntag)
    ntag = tntag;
  else
    ntag = int(pptr->get_tr_tags(rn));
}
*/

double tag_packet::set_direction(int& seed, double a)
{
  const double one = 1.0;
  double direction;
  float r = ran1(&seed);

  if (r < (float)a)
    direction = one;
  else
    direction = -one;

  return(direction);
}
    
double tag_packet::set_pos(double oldx, double dx, double xmin, double xmax,
                           int boundary)
{
  double newx = oldx + dx;
  if ( (boundary == RIGHT_CLOSED) && (newx > xmax) )
  {
    newx = xmax - (newx - xmax);
  }
  else if ( (boundary == LEFT_CLOSED) && (newx < xmin) )
  {
    newx = xmin + (xmin - newx);
  }
  return newx;
}

void tag_packet_list::walk(int& seed, year_month& date)
{
/*
  for (int i = 0; i < npacket; i++)
  {
    tag_packet t = packets.get(i);
    t.walk(seed, date);
    packets.set(t,i);
  }
*/

  //jca-2002-04-18: try avoiding using packets.get()
  jca::list<tag_packet*>::node* n = packets.getHead()->next;
  while (n)
  {
    n->t->walk(seed, date);
    n = n->next;
  }
}

void tag_packet_list::norm_walk(int& seed, year_month& date)
{
/*
  for (int i = 0; i < npacket; i++)
  {
    tag_packet t = packets.get(i);
    t.norm_walk(seed, date);
    packets.set(t,i);
  }
  */

  //jca-2002-04-18: try avoiding using packets.get()
  jca::list<tag_packet*>::node* n = packets.getHead()->next;
  while (n)
  {
    n->t->norm_walk(seed, date);
    n = n->next;
  }
}

void tag_packet_list::update_worms(worms& w)
{
/*
  for (int i = 0; i < npacket; i++)
  {
    tag_packet t = packets.get(i);
    w.add(t.x,t.y,i,nseg);
  }
*/
  //add jca 2001-06-06 to optimize a bit
  //use linklist directly
  jca::list<tag_packet*>::node* n = packets.getHead()->next;
  for (int i = 0; i < npacket; i++)
  {
    w.add(n->t->x,n->t->y,i);
    n = n->next;
  }
}

void tag_packet::walk(int& seed, year_month& date)
{
  double ax, ay;
  int k = acomp(ax, ay, date);

  double dx = lambda*set_direction(seed, ax);
  double dy = lambda*set_direction(seed, ay);

  const int m = pptr->get_m();
  const int n = pptr->get_n();
  int i = pptr->long_to_index(x);
  if (i < 1) 
    i = 0;
  if (i > m)
    i = m;

  int j = pptr->lat_to_index(y);
  if (j < 1)
    j = 1;
  if (j > n)
    j = n;

  prevx = x;
  prevy = y;
  bound  boundary;
  boundary = pptr->get_cell_type(i,j);

  double txmin = pptr->index_to_long(i);
  double txmax = pptr->index_to_long(i+1);
  double tymin = pptr->index_to_lat(j);
  double tymax = pptr->index_to_lat(j+1);
  
  x = set_pos(prevx, dx, txmin, txmax, boundary.edgex());
  y = set_pos(prevy, dy, tymin, tymax, boundary.edgey());

  add_step_stats(k, dx, dy);
}


void tag_packet::norm_walk(int& _seed, year_month& date)
{
  long seed = _seed;
  double dx, dy;

  const int m = pptr->get_m();
  int k = 0;
  while (k == 0) // keep jumping until curren position is in the ocean
  {
    int i = pptr->long_to_index(x);
    if (i>m)
      i = m;
    else if (i<=0)
      i = 1;
  
    const int jlb = pptr->get_jlb(i);
    const int jub = pptr->get_jub(i);
    int j = pptr->lat_to_index(y);
    if (j>jub)
      j = jub;
    else if (j<jlb)
      j = jlb;

    k = pptr->get_gridmap(i,j);
    if (k > 0) 
    {
      // retain the last legit position
      prevx = x;
      prevy = y;
      int s = pptr->get_season(date);
      const gridpartype_vector& ug = pptr->get_usergrid(s);
  
      double sigma = sqrt(ug[k].sigma/15.0); // i.e. sqrt(2*D/30)
      double edx = ug[k].u;
      double edy = ug[k].v;
      dx = (sigma * my_randn(seed) + edx)/60.0;
      dy = (sigma * my_randn(seed) + edy)/60.0;
      if (nsdd(k) < 1.0)
      {
        TTRACE(k,sigma)
        TTRACE(edx,dx)
        TTRACE(edy,dy)
      }
      bound boundary;
      boundary = pptr->get_cell_type(i,j);
    
      double txmin = pptr->index_to_long(i);
      double txmax = pptr->index_to_long(i+1);
      double tymin = pptr->index_to_lat(j);
      double tymax = pptr->index_to_lat(j+1);
    
      x = set_pos(prevx, dx, txmin, txmax, boundary.edgex());
      y = set_pos(prevy, dy, tymin, tymax, boundary.edgey());
    }
    else
    {
      // reset and try again
      x = prevx;
      y = prevy;
    }
  }

  add_step_stats(k, dx, dy);
}

void tag_packet::add_step_stats(const int k, const double dx, const double dy)
{
  sdx(k) += dx;
  ssdx(k) += dx*dx;
  sdy(k) += dy;
  ssdy(k) += dy*dy;
  nsdd(k) += 1.0;
}


int tag_packet::acomp(double& ax, double& ay, year_month& date)
{
  int m = pptr->get_m();

  int i = pptr->long_to_index(x);
  if (i>m)
    i = m;
  else if (i<=0)
    i = 1;

  int jlb = pptr->get_jlb(i);
  int jub = pptr->get_jub(i);
  int j = pptr->lat_to_index(y);
  if (j>jub)
    j = jub;
  else if (j<jlb)
    j = jlb;

  int k = pptr->get_gridmap(i,j);
  if (k > 0)
  {
    int s = pptr->get_season(date);
    const gridpartype_vector& ug = pptr->get_usergrid(s);

    lambda = 0.5*ug[k].sigma/mu;

    double epsx = 30.0*ug[k].u/mu;
    double epsy = 30.0*ug[k].v/mu;

    double bx = (1.0 - epsx)/2.0;
    double by = (1.0 - epsy)/2.0;

    ax = 1.0 - bx;
    ay = 1.0 - by;
  }
  else
  {
    ax = 0.0;
    ay = 0.0;
  }
  return(k);
}

tag_packet& tag_packet::operator = (const tag_packet& t)
{
  lambda = t.lambda;
  x = t.x;
  y = t.y;
  prevx = t.prevx;

  prevy = t.prevy;
  ntag = t.ntag;

  return (*this);
}


void tag_packet::write_position(ostream& f)
{
  f << " " << prevx << " " << prevy;
}

void tag_packet_list::release_tags(tagreleasetype_& tr)
{
  int nr = 0;
  int cr = 0;
  double x = pptr->index_to_long(tr.i);
  double y = pptr->index_to_lat(tr.j);

  while (nr < tr.tags)
  {
    cr = tag_packet::get_packet_size();
    if ( (cr + nr) > tr.tags)
      cr = (int)tr.tags - nr;

    tag_packet* t = new tag_packet(x, y, cr);
    packets.add(t);
    npacket++;
    nr += cr;
  }
}

void tag_packet_list::add_worms(worms& w, tagreleasetype_& tr)
{
  int nr = 0;
  int cr = 0;
  double x = pptr->index_to_long(tr.i);
  double y = pptr->index_to_lat(tr.j);
  while (nr < tr.tags)
  {
    cr = tag_packet::get_packet_size();
    if ( (cr + nr) > tr.tags)
      cr = (int)tr.tags - nr;

    w.addWorm(x, y);

    nr+=cr;
  }
}

tag_packet_list::tag_packet_list( par_t_reg* p)
{
  TTRACE(pptr,npacket)
  pptr = p;
  npacket = 0;
  TTRACE(pptr,npacket)
}

void tag_packet::report_step_stats(ostream& s)
{
  const ngrid = pptr->get_ngrid();
  /*
  0        0              0              0              0              0
  1     2872      0.0940895       0.867168     -0.0379636       0.903552
  2    15870      0.0928079       0.527861     0.00118793       0.533234
  3        8    -0.00497196        0.42561       0.223498       0.313847
  */
  s << endl;
  s << "                  ---------- dx ----------     ---------- dy ----------" << endl;
  s << "reg        n           mean        std dev           mean       std dev" << endl;
  for (int k = 0; k <= ngrid; k++)
  {
    report_step_stats(k, s);
  }


}

void tag_packet::report_step_stats(const int k, ostream& s)
{
  double mdx = 0.0;
  double mdy = 0.0;
  double vdx = 0.0;
  double vdy = 0.0;
  if (nsdd(k) > 2.0)
  {
    mdx = sdx(k)/nsdd(k);
    mdy = sdy(k)/nsdd(k);
    vdx = (ssdx(k) - sdx(k)*sdx(k)/nsdd(k))/(nsdd(k) - 1.0);
    vdy = (ssdy(k) - sdy(k)*sdy(k)/nsdd(k))/(nsdd(k) - 1.0);
  }
  s << setw(3) << k
    << setw(9) << nsdd(k)
    << setw(15) << mdx << setw(15) << sqrt(vdx)
    << setw(15) << mdy << setw(15) << sqrt(vdy)
    << endl;
}

ostream& operator << (ostream& s, const tag_packet& t)
{
  s << " " << &t;
}

/*
void tag_packet::natural_mortality(int& seed)
{
  float nmort = pptr->get_mort()*tau;
  int n = 0;
  while  (n < ntag)
  {
    float r = ran1(&seed);
    if (r <= nmort)
    {
      ntag --;
    }
    n++;
  }
}

void tag_packet::fishing_mortality(int& seed, year_month& date, ofstream& tf)
{
  int s = pptr->get_season(date);
  gridpartype_vector ug = pptr->get_usergrid(s);

  int i = x/pptr->get_deltax_eq() + 1;
  int j = y/pptr->get_deltay() + 1;
  //int k = pptr->gridmap[i][j];
  //double fmort = pptr->q*ug[k].efrt*tau;

  ivector effort_occured(1,pptr->get_nfleet());
  effort_occured.initialize();

  d3_array effort = make_d3_array(1, pptr->get_nfleet(), 1, pptr->get_m(), pptr->jlb, pptr->jub);
  read_effort_files(*pptr, effort, date, effort_occured);

  for (int f = 1; f <= pptr->nfleet; f++)
  {
   // f_mort(f) = param.q(f)*effort(f);
    double fmort = pptr->q(f)*effort(f,i,j)*tau;

    int ncap = 0;
    int n = 0;
    while  (n < ntag)
    {
      float r = ran1(&seed);
      if (r <= fmort)
      {
	     ntag --;
	     ncap ++;
      }
      n++;
    }
    if (ncap > 0)
    {
      record_capture(tf, pptr->fleet_names(f), date, ncap);
      //recap_line(_WHITE);
    }
  }
}


void tag_packet::record_capture(ostream& f, adstring& fleet,
				year_month& date, int icaps)
{
  recaptype_ recapture_record;

  recapture_record.date = date;
  int pool_tags = pptr->m_ipar[13];
  if (pool_tags)
    recapture_record.cohort = 0;
  else
    recapture_record.cohort = pptr->get_tr_cohort(rn);

  //float  rx = pptr->deltax * (pptr->tr[rn].i - .5);
  //float  ry = pptr->deltay * (pptr->tr[rn].j - .5);

  //float  cx = x;
  //float  cy = y;
  int i = x/pptr->get_deltax_eq() + 1;
  int j = y/pptr->get_deltay() + 1;

  //strcpy(recapture_record.fleet, param.fleet_names(f));
  strcpy(recapture_record.fleet, fleet);

  recapture_record.i = i; //a_move_corner(i, pptr->sw_long);
  recapture_record.j = j; //a_move_corner(j, pptr->sw_lat);
  recapture_record.returns = icaps;
//  float  dx = rx - cx;
//  float  dy = ry - cy;
//  dist = sqrt(dx*dx + dy*dy);
//  azim = azimuth(dx,dy);

  //f << recapture_record << endl;

  f << setprecision(0)
//#ifdef __ZTC__
    << setfixed
//#endif
    << setw(7) << month
    << setprecision(0) << setw(6) << rn    //cohort
    << setprecision(0) << setw(7) << rx
    << setprecision(0) << setw(7) << ry
    << setprecision(2) << setw(11) << cx    //<< "("<<i<<")"
    << setprecision(2) << setw(13) << cy    //<< "("<<j<<")"
    << setw(7) << icaps
    << endl;

  if (!f)
  {
    cerr << "Error writing recapture file." << endl;
    clogf << "Error writing recapture file." << endl;
    exit(1);
  }
} // record_capture
*/
