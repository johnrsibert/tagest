//$Id: prn2poly.cpp 2886 2011-11-23 23:51:34Z jsibert $
/** \file prn2poly.cpp
Process land mask from .prn file to produce closed polygons
or plotting by GMT
\ingroup preproc
*/
#include <fvar.hpp>
#include <adstring.hpp>
#include <cifstrem.h>
#include "par_t_reg.h"
#include "trace.h"
ofstream clogf;
adstring make_banner(const adstring& program_name);

void addToPoly(imatrix& poly, const int i, const int j, int& p)
{
   p ++;
   if (p > poly.rowmax())
   {
      cerr << "Need to increase number of points per polygon." << endl;
      exit(1);
   }
   poly(p,1) = i;
   poly(p,2) = j;
}

int main(int argc, char ** argv)
{
  adstring banner = make_banner(adstring(argv[0])+": region polygon generator.");
  cout << banner << endl;

  adstring log_file_name("prn2poly.log");

  clogf.open(log_file_name);
  if ( !clogf )
  {
    cerr << "Cannot open file " << log_file_name << endl;
    exit(1);
  }
  cout << "Opened " << log_file_name << endl;
  clogf << banner << endl;

  char root_filename[50];
  #undef USE_FILE_NAM_TMP
  #ifdef USE_FILE_NAM_TMP
  //Open file_nam.tmp to read names of files for input
  cifstream fnt("file_nam.tmp");
  if ( !fnt)
  {
    cerr << "Error: Cannot open file_nam.tmp";
    exit(1);
  }
  fnt >> root_filename;
  fnt.close();

  #else
  if (argc < 2)
  {
     cerr << "Must specifify file name root of .prn file on command line, e.g.\n"
          << "  prn2poly skj16" << endl;
     exit(1);
  }
  strcpy(root_filename,argv[1]);
  #endif // USE_FILE_NAM_TMP

  adstring pfile = adstring(root_filename) + adstring(".prn");
  TRACE(pfile)

  cifstream prnfile(pfile);
  if (!prnfile)
  {
    cerr << "Error: Cannot open " << pfile << " in " << __FILE__;
    exit(1);
  }

  Model_area ma(prnfile);
  int  nmonth,  start_year,  start_month,  nrelease,  nfleet;
  prnfile >>  nmonth >> start_year >> start_month >> nrelease >> nfleet;
  //prnfile >> ma.gridmap;
  for (int j = ma.n; j >= 1; j--)
     for (int i = 1; i <= ma.m; i++)
        prnfile >> ma.gridmap(i,j);

  prnfile.close();
  TTRACE(ma.m,ma.n)
  TTRACE(ma.deltax,ma.deltay)
  int nregion = max(ma.gridmap);
  TRACE(nregion)

  adstring polyname(adstring(root_filename) + adstring("_polygons.dat"));
  ofstream poly(polyname);
  if (!poly)
  {
     cerr << "Unable to open " << polyname << " for output." << endl;
     exit(1);
  }
  clogf << "Opened " << polyname << " for output." << endl;
  cout  << "Opened " << polyname << " for output." << endl;
  poly.precision(2);
  poly.setf(ios::fixed);

  const int npts = 500;
  imatrix polygon(1,npts,1,2);
  polygon.initialize();
  int ppp = 0;

  ivector jmin(1,ma.m);
  ivector jmax(1,ma.m);
  ivector imin(1,ma.m);
  ivector imax(1,ma.m);
  int lastJ = 0;
  int lastI = 0;
  //poly << "> -W-" << endl;
  for (int k = 1; k <= nregion; k++)
  //int k = 15;
  {
     clogf << "\n> " << k << endl;
     poly << "> -W- " << k << endl;
     jmax = 0;
     jmin = ma.n+1;
     imax = 0;
     imin = ma.m+1;
     ppp = 0;

     //poly << "> -W2p/black" << endl;
     // move eastward along northern boundary of region
     for (int i = 1; i <= ma.m; i++)
     {
       for (int j = 1; j <= ma.n; j++)
       {
          if (ma.gridmap(i,j) == k)
          {
             if (j > jmax(i))
             {
                jmax(i) = j;
             }
             if (i > imax(j))
                imax(j) = 1;
          }
       }
       if (jmax(i) > 0)
       {
         // add one to index to get the point on the north and east side of cell
         addToPoly(polygon,i,jmax(i)+1,ppp);
         addToPoly(polygon,i+1,jmax(i)+1,ppp);
         lastI = i;
         lastJ = jmax(i);
         double lon = ma.index_to_long(i);
         double lat =  ma.index_to_lat(jmax(i)+1);
         //poly << lon << " " << lat << endl;

         lon = ma.index_to_long(i+1);
         lat =  ma.index_to_lat(jmax(i)+1);
         //poly << lon << " " << lat << endl;
       }
     }        
     TTRACE(lastI,lastJ)

     //poly << "> -W2p/green" << endl;
     for (int j = lastJ; j >= 1; j--)
     {
       for (int i = 1; i <= ma.m; i++)
       {
          if (ma.gridmap(i,j) == k)
          {
             if (i > imax(j))
             {
                imax(j) = i;
             }
          }
        } 
        if (imax(j) > 0)
        {
           addToPoly(polygon,imax(j)+1,j,ppp);
           lastI = imax(j);
           lastJ = j;
           double lon = ma.index_to_long(imax(j)+1);
           double lat =  ma.index_to_lat(j);
           //poly << lon << " " << lat << endl;
        // lon = ma.index_to_long(imax(j)+1);
        // lat =  ma.index_to_lat(j+1);
        // poly << lon << " " << lat << endl;
        }
     }
     TTRACE(lastI,lastJ)

     //poly << "> -W2p/blue" << endl;
     for (int i = lastI; i >= 1; i--)
     {
       for (int j = 1; j <= ma.n; j++)
       {
          if (ma.gridmap(i,j) == k)
          {
             if (j < jmin(i))
             {
                jmin(i) = j;
             }
          }
       }
       if (jmin(i) < ma.n+1)
       {
         addToPoly(polygon,i,jmin(i),ppp);
         lastI = i;
         lastJ = jmin(i);
         double lon = ma.index_to_long(i);
         double lat =  ma.index_to_lat(jmin(i));
         //poly << lon << " " << lat << endl;
       }
     }        
     TTRACE(lastI,lastJ)

     //poly << "> -W2p/red"  << endl;
     for (int j = lastJ; j <=ma.n; j++)
     {
       for (int i = 1; i <= ma.m; i++)
       {
          if (ma.gridmap(i,j) == k)
          {
             if (i < imin(j))
             {
                imin(j) = i;
             }
          }
        } 
        if (imin(j) < ma.m+1)
        {
           addToPoly(polygon,imin(j),j,ppp);
           addToPoly(polygon,imin(j),j+1,ppp);
           lastI = imin(j);
           lastJ = j;
           double lon = ma.index_to_long(imin(j));
           double lat =  ma.index_to_lat(j);
           //poly << lon << " " << lat << endl;
           //lon = ma.index_to_long(imin(j));
           //lat =  ma.index_to_lat(j+1);
           //poly << lon << " " << lat << endl;
        }
     }
     TTRACE(lastI,lastJ)

    for (int p = 1; p <= ppp; p++)
    {
       double lon = ma.index_to_long(polygon(p,1));
       double lat =  ma.index_to_lat(polygon(p,2));
       poly << lon << " " << lat << endl;
       clogf << p << " " << polygon(p) << " " << lon << " " << lat << endl;
    } 

  }

  poly.close();
 
  cout << "Finished." << endl;
}
 
