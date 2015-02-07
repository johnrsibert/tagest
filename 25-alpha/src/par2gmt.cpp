//$Id: par2gmt.cpp 2904 2011-12-20 23:40:00Z jsibert $
/** \file par2gmt.cpp
Process par file to produce:
- coordinates of region boundaries
- coordinates of model domain
- cordinates of region centroids with region numbers and recaptures
- table of advection and diffuision estimates with direction and magnitude of
resultant advection
for plotting by GMT
\ingroup preproc
\ingroup postproc
*/
#include <fvar.hpp>
#include <adstring.hpp>
#include <cifstrem.h>
#include "par_t_reg.h"
#include "recap_t.h"
#include "trace.h"
ofstream clogf;
adstring make_banner(const adstring& program_name);
int _global_report_flag;
template <typename D3_ARRAY,typename MATRIX,typename VECTOR,typename DOUBLE>
void readrecaps(par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, const char* root, recaptype_vector& recaps,
                int& pnrecs, const char* path, indexed_regional_fishery_record& irfr);


extern "C"  {
  void ad_boundf(int i)
  {
    // so we can stop here
    exit(i);
  }
} 

int main(int argc, char ** argv)
{
  adstring banner = make_banner(adstring(argv[0])+": region boundary generator.");
  cout << banner << endl;

  adstring log_file_name("par2gmt.log");

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
     cerr << "Must specifify file name root of .par file on command line, e.g.\n"
          << "  par2gmt skj16" << endl;
     exit(1);
  }
  strcpy(root_filename,argv[1]);
  #endif // USE_FILE_NAM_TMP

  adstring pfile = adstring(root_filename);
  TRACE(pfile)

  par_t_reg<d3_array,dmatrix,dvector,double> ma(pfile);
  TTRACE(ma.m,ma.n)
  TTRACE(ma.deltax,ma.deltay)
  int nregion = max(ma.gridmap);
  TRACE(nregion)

  adstring gmtname(adstring(root_filename) + adstring("_regions.dat"));
  ofstream gmt(gmtname);
  if (!gmt)
  {
     cerr << "Unable to open " << gmtname << " for output." << endl;
     exit(1);
  }
  clogf << "Opened " << gmtname << " for output." << endl;
  cout  << "Opened " << gmtname << " for output." << endl;
  gmt.precision(2);
  gmt.setf(ios::fixed);

  double dx = ma.deltax/60.0;
  double dy = ma.deltay/60.0;


  //nregion = 2;
  double prevx = ma.index_to_long(1);
  double prevy = ma.index_to_lat(ma.n);
  gmt << "> y direction:" << endl;
  for (int k = 1; k <= nregion; k++)
  //int k = 16;
  {
     for (int j = ma.n; j >= 1; j--)
     {
       //int np = 0;
       for (int i = 2; i <= ma.m; i++)
       {
          if (ma.gridmap(i-1,j) != 0 && 
              ma.gridmap(i,j) == k &&
              ma.gridmap(i-1,j) != ma.gridmap(i,j))
          {
             double x = ma.index_to_long(i);;
             double y = ma.index_to_lat(j);
             /*
             if (np == 0)
             {
               prevx = x;
               prevy = y;
             }
             */
             if (x != prevx)
             {
                //gmt << prevx << "  " << prevy << endl;
                gmt << "> k = " << k << endl;
                gmt << x << "  " << (y + dy) << endl;
                //np = 0;
             }
             gmt << x << "  " << y << endl;
             //np ++;
             prevx = x;
             prevy = y;
          }
       }
     }        
  }
  //
  gmt << ">\n> x direction:" << endl;
  for (int k = 1; k <= nregion; k++)
  {
     for (int i = 1; i <= ma.m; i++)
     {
       //int np = 0;
       for (int j = ma.n-1; j >= 1; j--)
       {
         if (ma.gridmap(i,j+1) != 0 && 
              ma.gridmap(i,j) == k &&
              ma.gridmap(i,j+1) != ma.gridmap(i,j))
          {
             double x = ma.index_to_long(i);
             double y = ma.index_to_lat(j)+dy;
             if (y != prevy)
             {
                gmt << (prevx+dx) << "  " << prevy << endl;
                gmt << "> k = " << k << endl;
                //np = 0;
             }
             gmt << x << "  " << y << endl;
             //np ++;
             prevx = x;
             prevy = y;
          }
       }
     }
   }
   gmt.close();

   double xmin = ma.index_to_long(1);
   double xmax = ma.index_to_long(ma.m) + dx;
   double ymin = ma.index_to_lat(1);
   double ymax = ma.index_to_lat(ma.n) + dy;

   adstring domname(adstring(root_filename) + adstring("_domain.dat"));
   ofstream dom(domname);
   if (!dom)
   {
     cerr << "Unable to open " << domname << " for output." << endl;
     exit(1);
   }
   clogf << "Opened " << domname << " for output." << endl;
   cout  << "Opened " << domname << " for output." << endl;
   dom.precision(2);
   dom.setf(ios::fixed);
   //dom << "> -W3p/0/128/0" << endl;
   dom << xmin << " " << ymin << endl;
   dom << xmin << " " << ymax << endl;
   dom << xmax << " " << ymax << endl;
   dom << xmax << " " << ymin << endl;
   dom << xmin << " " << ymin << endl;

   dom.close();

   dvector xsum(0,nregion); xsum.initialize();
   dvector ysum(0,nregion); ysum.initialize();
   ivector rsum(0,nregion); rsum.initialize();

   for (int i = 1; i <= ma.m; i++)
   {
     for (int j = ma.n; j >= 1; j--)
     {
        int k = ma.gridmap(i,j);
        xsum(k) += ma.index_to_long(i);
        ysum(k) += ma.index_to_lat(j);
        rsum(k) ++;
     }
   }

   indexed_regional_fishery_record ifr;
   ifr.read(root_filename,"../");

   const int MAXRETURNS = 82000;     // increase for more tag returns
   int  nrec = MAXRETURNS-1;
   recaptype_vector recaps(1, MAXRETURNS);
   readrecaps(ma, root_filename, recaps, nrec, "../", ifr);
   TRACE(nrec) 
   ivector RegionCaps(0,ma.ngrid);
   RegionCaps.initialize();
   for (int k = 1; k <= nrec; k++)
   {
     const int i = recaps[k].i;
     const int j = recaps[k].j;
     const int r = ma.gridmap(i,j);
     RegionCaps(r) += recaps[k].returns;
   }
 
   adstring labname(adstring(root_filename) + adstring("_region_numbers.dat"));
   ofstream lab(labname);
   if (!lab)
   {
     cerr << "Unable to open " << labname << " for output." << endl;
     exit(1);
   }
   clogf << "Opened " << labname << " for output." << endl;
   cout  << "Opened " << labname << " for output." << endl;
   lab.precision(3);
   lab.setf(ios::fixed);
   clogf << "\nregion centroids:" << endl;
   clogf << "  k   n" << setw(11) << "x" << setw(11) << "y" << endl;

   for (int k = 1; k <= nregion; k++)
   {
      double cx = xsum(k)/(double)rsum(k);
      double cy = ysum(k)/(double)rsum(k) + 0.5*dy;
      clogf << setw(3) << k << setw(6) << rsum(k) 
            << setw(12) << setprecision(3) << cx
            << setw(12) << setprecision(3) << cy 
            << setw (6) << setprecision(0) << RegionCaps(k) << endl;

     lab << setprecision(3) << cx << " " << cy << "  8 0 0 CM " 
         << setprecision(0) << k << " " << RegionCaps(k) << endl;
 
   }

   adstring uvDname(adstring(root_filename) + adstring("_uvD.dat"));
   ofstream uvD(uvDname);
   if (!uvD)
   {
     cerr << "Unable to open " << uvDname << " for output." << endl;
     exit(1);
   }
   clogf << "Opened " << uvDname << " for output." << endl;
   cout  << "Opened " << uvDname << " for output." << endl;
   uvD.precision(3);
   uvD.setf(ios::fixed);

   for (int s = 1; s <= ma.nseason; s++)
   {
      for (int k = 1; k <= nregion; k++)
      {
         double cx = xsum(k)/(double)rsum(k);
         double cy = ysum(k)/(double)rsum(k) + 0.5*dy;
         double mag = sqrt(square(ma.usergridU(s,k))+square(ma.usergridV(s,k)));
         double dir = atan2(ma.usergridV(s,k),ma.usergridU(s,k));
         // degrees counter-clockwise from horizontal for psxy -Sv
         dir = 360.0*dir/(2.0*M_PI);
         uvD << setprecision(0) << s << " " << k << "  "
             << setprecision(3) << cx << " " << cy << "  "
             << ma.usergridU(s,k) << " " << ma.usergridV(s,k) << " " 
             << dir << " " << mag << " " 
             << ma.usergridSigma(s,k) << endl;
 
      }
 
   }

 
   cout << "Finished." << endl;
}
 
