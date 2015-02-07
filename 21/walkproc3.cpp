#include "par_treg.h"
#include <jnigraphics2.h>
#include "trace.h"
#include "prnstrem.h"

#include "tag_pack3.h"

extern ofstream clogf;
extern adstring banner;
extern double sdx, ssdx, sdy, ssdy, nsdd;
int randn_count;

void par_t_reg::walk(void)
{
  randn_count = 0;
  double tcaps = 0;
  int rn = 1; /* tag release number */
  int ncohort = 0;

  char quit;

  dvector region_caps(1, ngrid);
  region_caps.initialize();
  HERE

  adstring capfile_name("walksim.tag");
  ofstream capfile(capfile_name);
  if ( !capfile )
  {
    cerr << "Cannot open file " << capfile_name << endl;
    exit(1);
  }
  capfile << "# " << banner << endl;
  capfile << "#\n";
  capfile << "# simulated tag recaptures\n";
  capfile << "# natural mortality = " << mort << endl;
  capfile << "# catchability coefficients (by fleet) = " << q << endl;
  capfile << "# random number seed = " << m_ipar[40] << endl;
  capfile << "# number of time steps per month = " << m_ipar[6] << endl;
  capfile << "# \n";
  capfile << "#         positions: release      recapture\n";
  capfile << "#   fleet year month cohort long   lat      tags" << endl;

  rn = 1;
  quit = '\0';


//  fmortq(param);
  dvector total_tags(0, nmonth);
  dmatrix total_check(1, 4, 0, nmonth);
  total_tags.initialize();
  total_check.initialize();
  year_month date = tr[1].date;

  int nstep = 30; // m_ipar[6];
  int seed = m_ipar[40];
  double time = 0;

  total_tags[0] = tr[1].tags;

  int max_month = tr[nrelease].date - tr[1].date;

  int start_month = 1;
  max_month += nmonth + 1;
  int max_days = max_month*nstep + 1;
  TRACE(max_days)
  int current_season = 0;

  TTRACE(pathname.getPath(),pathname.getRoot())

// > >	<*regiondef instance>.region.cstfile = <gmtfilename>.1.2.cst

#ifdef unix
  adstring coastline_file = "../"+pathname.getPathRoot()+".cst";
#else
  adstring coastline_file = "..\\"+pathname.getPathRoot()+".cst";
#endif
  TRACE(coastline_file)


  HERE
  tag_packet::set_static_par(*this);
  tag_packet_list tps(this);




  HERE
  jnigraphics2* jni = new jnigraphics2();
  TRACE(jni)
  if (!jni)
  {
      cerr << "Error creating instance of jnigraphics object. Exiting." << endl;
      exit(1);
  }

  worms dw(sw_coord.long_value(),sw_coord.lat_value(),5);
  worms mw(sw_coord.long_value(),sw_coord.lat_value());
  worms tw(sw_coord.long_value(),sw_coord.lat_value()); 
  //TTRACE(tw.size(),tw.getNumXYs())

  const int ntabs = 3;
  ivector tab_list(1,ntabs);
  int dp_plot = 0;
  int mp_plot = 0;
  int tp_plot = 0;

  jni->addTabs(ntabs, tab_list);
  int daily_tab = tab_list(1);
  int monthly_tab = tab_list(2);
  int track_tab = tab_list(3);
  HERE

  jni->setTabTitle("Daily", daily_tab);
  ivector dt_layout(1, 1);
  jni->addGridLayout(1, 1, dt_layout, daily_tab);
  dp_plot = dt_layout(1);
  HERE

  jni->setTabTitle("Monthly", monthly_tab);
  ivector mt_layout(1, 1);
  jni->addGridLayout(1, 1, mt_layout, monthly_tab);
  mp_plot = mt_layout(1);
  HERE

  jni->setTabTitle("Tracks", track_tab);
  ivector tt_layout(1, 1);
  jni->addGridLayout(1, 1, tt_layout, track_tab);
  tp_plot = tt_layout(1);
  HERE
  wormregiondef wrd(m, n);
  wrd.region.title = "Worm Tracks";
  wrd.region.setGridMap(gridmap);

  wrd.region.cstfile = coastline_file;
  jni->addWormRegion(wrd, mp_plot);
  jni->addWormRegion(wrd, dp_plot);
  jni->addWormRegion(wrd, tp_plot);
  jni->addStatusBar();

  HERE
  jni->layoutAll();
  HERE
  rn = 0;

  ofstream wh("wormhead.dat");

  int td = 0;
  char sbbuf[81];
  for (int month = start_month; ((month <= max_month) && (quit != 'Q')); month++)
  {
    /* check month of year for new season */

    if (current_season != get_season(date))
    {
      clogf <<  "Entered season " << get_season(date) << endl;
      current_season = get_season(date);
    }
    clogf << "Month " << month << ", " << date
          << ", Season " << current_season <<endl;
    cout  << "Month " << month << ", " << date
          << ", Season " << current_season <<endl;

    // check for new tag release
    while( (rn < nrelease) && (date == tr[rn+1].date) )
    {
      rn ++;
      tps.release_tags(tr[rn]);
      tps.add_worms(dw, tr[rn]);
      tps.add_worms(mw, tr[rn]);
      tps.add_worms(tw, tr[rn]);
      int npacket = tps.get_npacket();
      clogf << "tag release " << rn << " of "
	         << tr[rn].tags << " tags in "
	         << npacket << " packets." << endl;
      if (rn > nrelease) rn = nrelease;
    }

    for (int nn = 1; nn <= nstep; nn++)
    {
      td ++;
      /* 
      for (int h = 1; h <= 24; h++)
      {
        tps.walk(seed, date);
        time += tag_packet::get_tau();
      }
      */ 
      time ++;
      tps.norm_walk(seed, date);
      tps.update_worms(dw);
      jni->drawValuesWormRegion(dw, dp_plot);
     
      tps.update_worms(tw);
      jni->drawValuesWormRegion(tw, tp_plot);

      std::ostrstream sb(sbbuf,80);
      sb << date 
         << ", season" << setw(2) << current_season
         << ": month " << setw(3) << setfill(' ') << month
         << "; day" << setw(3) << nn
         << ends;
      adstring status(sbbuf);
      jni->drawStatusBar(status);
      if ( !(jni->paintAll()) )
      {
        quit='Q';
        break;
      }
        //exit(0);

      char bbuf[81];
      std::ostrstream bss(bbuf,80);
    #ifdef unix
      bss << "bitmaps/"
    #else
      bss << "bitmaps\\"
    #endif
          << "dw" << setw(4) << setfill('0') << td
                 << ".png" << ends;
          //     << ".ppm" << ends;
      //if (!(td % 5)) 
        jni->save_png(bbuf,640,480,dp_plot);
        //jni->saveppm(bbuf,640,480,dp_plot);

      wh << time;
      dw.printHeads(wh);
      wh << endl;
    } // nn loop
    tps.update_worms(mw);
    jni->drawValuesWormRegion(mw, mp_plot);
 
    ++ date;
    //cout << date << endl;
  } /* end month loop */

  TRACE(randn_count)

  char jbuf[81];
  std::ostrstream jss(jbuf,80);
#ifdef unix
  jss << "bitmaps/"
#else
  jss << "bitmaps\\"
#endif
      << "tw_final" 
    //<< "tw" << setw(2) << setfill('0') << date.get_year()
    //        << setw(2) << setfill('0') << date.get_month_value()
              << ".png" << ends;
  TRACE(jbuf)
  jni->save_png(jbuf,640,480,tp_plot);


  tag_packet::report_step_stats(clogf);

  /*
  for(month=0; month <= nmonth; month++)
  {
     clogf << setw(5) << month
	   << setw(10) << total_tags[month] << endl;
  }
  */

  capfile.close();
  // wait untile the graphics system is closed
  adstring done(sbbuf);
  done += " - done!";
  jni->drawStatusBar(done);
  while ( (jni->paintAll()) ) {}
  exit(0);
 } // end of walk_proc
