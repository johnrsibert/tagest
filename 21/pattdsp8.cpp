#include <iostream.h>
#include <fstream.h>
#include <adstring.hpp>
#include <fvar.hpp>
#include <cifstrem.h>
#include <jniGraphics2.h>
#ifdef __GNUC__
  #include <dirent.h>
#else
  #include <dir.h>
#endif
#include "prnstrem.h"

#include "par_treg.h"

#include "pattdisp.h"
#include "trace.h"

ofstream clogf;
void region_caps(par_t_reg& param, imatrix& returns, adstring& path, adstring& file);
int cohort_released(par_t& p, recaptype_& recapture_record);
void readrecaps(par_t& param, const char* root, recaptype_vector& recaps,
                int& pnrecs, const char* path = DEFAULT_PATH);

int main(void)
{
  adstring fit_name(1,12);

  clogf.open("pattdisp.log");
  /*
  if (argc > 1)
  {
    fit_name = adstring(argv[1]);
  }
  else
  {
    fit_name = "file_nam.tmp";
  }
  */
  cifstream fnt("file_nam.tmp");
  if ( !fnt)
  {
    cerr << "Cannot open file_name.tmp in " << __FILE__ << endl;
    exit(1);
  }
  char rootname[9];
  fnt >> rootname;
  fnt.close();
  cout << "Read '" << rootname << "' from file_nam.tmp" << endl;

  //TTRACE(path,file)
  //adstring filename = path + file + ".par";
  adstring filename = adstring(rootname) + ".par";
  TRACE(filename)
  par_t_reg param(filename);

  int nseason = param.get_nseason();
  int ngrid = param.get_ngrid();

  imatrix returns(1,nseason, 0,ngrid);
  region_caps(param, returns, ".\\", rootname);
 
  dmatrix peclet(1, nseason, 1, ngrid);
  dmatrix ikr(1, nseason, 1, ngrid);

  dmatrix obc(1, nseason, 1, ngrid);
  dmatrix cpeclet(1, nseason, 1, ngrid);
  dmatrix cikr(1, nseason, 1, ngrid);
  double deltax = param.get_deltax_eq();
  double deltay = param.get_deltay();
  for (int s = 1; s <= nseason; s++)
  {
    gridpartype_vector& ug = param.get_usergrid(s);
    for (int k = 1; k <= ngrid; k++)
    {
      double uu = ug[k].u*30.0;
      double vv = ug[k].v*30.0;
      double D =  ug[k].sigma+1e-10;

      ikr(s,k) = sqrt(uu*uu+vv*vv)*deltax/D;
      peclet(s,k) = dmax(fabs(uu)*deltax, fabs(vv)*deltay)/D;

      obc(s,k) = 0.5*(fabs(uu)*deltax+fabs(vv)*deltay);
      cikr(s,k) = sqrt(uu*uu+vv*vv)*deltax/(D+obc(s,k));
      cpeclet(s,k) = dmax(fabs(uu)*deltax, fabs(vv)*deltay)/(D+obc(s,k));

    }
  }

  {
    const path_len = 80;
    char cwd[path_len];
    getcwd(cwd,path_len);

    char fitc[4];
    ostrstream fits(fitc,4);
    fits << 'p' << setw(2) << setfill('0') << hex << param.m_ipar[8] << '\0';

    char css[80];
    ostrstream ss(css,80);
    ss << "peclet" << setw(2) << setfill('0') << hex << param.m_ipar[8]
	   << ".prn" << '\0';
    prnstream pp(css);

    //pp << cwd << param.filename << "fit" << fitc << PENDL;
    pp << cwd << (char*)param.pathname.getRoot() << "fit" << fitc << PENDL;
    pp << "  "     << "  "     << "tag"     << "u"  << "v"   << "D"  << "I-K"   << "Peclet"
       << "O'Brien" << "D"  << "I-K"   << "Peclet" << PENDL;
    pp << "season" << "region" << "returns" << "Nmi/day" << "Nmi/day" << "Nmi^2/mo" << "ratio" << "number"
       << "Correction" << "Nmi^2/mo" << "ratio" << "number" << PENDL;
    int nseason = param.get_nseason();
    for (int s = 1; s <= nseason; s++)
    {
      gridpartype_vector& ug = param.get_usergrid(s);
      for (int k = 1; k <= ngrid; k++)
      {
         pp << s << k << returns(s,k) << ug[k].u << ug[k].v << ug[k].sigma
                  << ikr(s,k) << peclet(s,k)
                  << obc(s,k) << (ug[k].sigma+obc(s,k))
                  << cikr(s,k) << cpeclet(s,k)
            << PENDL;
      }
    }
  }

  //  graphics stuff

  int m = param.get_m();
  int n = param.get_n();
  double swlong = param.get_long_lat_long();
  double swlat  = param.get_long_lat_lat();
  imatrix& gridmap = param.get_gridmap();

#ifdef unix
  jnigraphics2 jni("/home/aku/jancheta/jnigraphics3/g++");
#else
  jnigraphics2 jni("\\movemod\\jnigraphics3\\bcc55");
#endif
  HERE
  ivector tab_list(1,nseason);
  ivector plot_list(1,nseason);
  TRACE(plot_list)
  ivector tab_layout(1,1);
  jni.addTabs(nseason, tab_list);
  TRACE(tab_list)
  movementregiondef mrd(m, n);
  mrd.region.title ="Movement Pattern";
  mrd.setGridMap(gridmap);
  for (int s = 1; s <= nseason; s++)
  {
    jni.setTabTitle("Season "+str(s),tab_list(s));
    jni.addGridLayout(1, 1, tab_layout, tab_list(s));
    plot_list(s) = tab_layout(1);
    jni.addMovementRegion(mrd, plot_list(s));
  }
  TRACE(plot_list)
  jni.addStatusBar();
  HERE
  jni.layoutAll();
  HERE

  char buf[81];
  ostrstream ss(buf,80);
  ss << "     Fit p" << setw(2) << setfill('0') << hex << param.m_ipar[8] << '\0';
  jni.drawStatusBar(buf);
  TRACE(buf)
  
  for (int s = 1; s <= nseason; s++)
  {
    TRACE(s)
    TRACE(param.get_seasonal_u(s)) 
    TRACE(param.get_seasonal_v(s)) 
    jni.drawArrowsMovementRegion(param.get_seasonal_u(s),
                                 param.get_seasonal_v(s), plot_list(s));
    TRACE(param.get_seasonal_sigma(s)) 
    jni.drawCirclesMovementRegion(sqrt(param.get_seasonal_sigma(s)/M_PI), plot_list(s));
  }
 
  HERE
  // wait until the graphics system is closed
  while ( (jni.paintAll()) ) {}
    exit(0);
  HERE
  return (0);
}

void region_caps(par_t_reg& param, imatrix& returns,
                 adstring& path, adstring& file)
{
  HERE
  returns.initialize();

  const int MAXRETURNS = 12000;     // increase for more tag returns
  int  nrec = MAXRETURNS-1;         // used as check in readrecaps
  recaptype_vector recaps(1, MAXRETURNS);
  readrecaps(param, file, recaps, nrec);

  for (int k = 1; k <= nrec; k++)
  {
    int i = recaps[k].i;
    int j = recaps[k].j;

    int r = param.get_gridmap(i,j);
    int s = param.get_season(recaps[k].date);

    returns(s,r) += recaps[k].returns;
  }
}


