#include <adstring.hpp>
#include <fvar.hpp>
#include <cifstrem.h>
#include <math.h>
#include "trace.h"

/***************************************************************************
 *   This program calculates distance from  sea-grids to their neartest land-grid                        *
 *   Input file for this program is generated  from  makemap with ".map" extension                   *
 *   Output file is written with ".dst" extension:                                                                           *
 ***************************************************************************/

adstring make_banner(const adstring& program_name);

void read_ma_matrix(imatrix& t, const char* filename);
void write_ma_matrix(const dmatrix& t, const char* filename);

ofstream clogf;

int main(int argc, char ** argv)
{
  adstring banner = make_banner(adstring(argv[0])+": Minimum distance calculator ");
  cout << banner << endl;
  adstring log_file_name("distance.log");

 clogf.open(log_file_name);
  if ( !clogf )
  {
    cerr << "Cannot open file " << log_file_name << endl;
    exit(1);
  }
  cout << "Opened " << log_file_name << endl;
  clogf << banner << endl;


  cifstream fnt("file_nam.tmp");
  if ( !fnt)
  {
    cerr << "Cannot open file_nam.tmp in main()" << endl;
    exit(1);
  }

  char filename[9];
  fnt >> filename;
  fnt.close();
  clogf << "Read  '"  << filename << "' from file_nam.tmp" << endl;
  cout << "Read  '"   << filename << "' from file_nam.tmp" << endl;

  adstring input_file = adstring(filename)+adstring(".map");
  cifstream  ifs(input_file);

  if ( !ifs )
  {
    cerr << "Cannot input open file: " << input_file << endl;
    exit(1);
  }

  clogf << "Read  '"  << input_file << "' for input" << endl;


  int plon;        //number of longitude  grids = m
  int plat;       //number of and latitude grids = n
  double dx;  //geographic distance between grids
  double dy;  //geographic distance between grids

  //read data
  ifs >> plon >> plat;         TTRACE(plon,plat)
  ifs >> dx >> dy;              TTRACE(dx,dy)

  imatrix gridmap(1,plon,1,plat);

  double dist;
  int i,j, ii, jj;

  //ifs.seekg(sp);  need to pass sp as an argument!
  //read_ma_matrix(gridmap, ifs);


  for ( j = plat; j >= 1; j--)
  {
    for ( i = 1; i <= plon; i++)
      ifs >> gridmap(i,j);
  }//for j

  if ( !ifs )
  {
    cerr << "Error reading file: " << input_file << endl;
    exit(1);
  }


clogf << gridmap << endl;

dmatrix  min_dist(1,plon,1,plat);

for( i = 1; i <= plat; i++)
{
   for( j = 1; j <= plon; j++)
   {
      double smallest = (plon + 1) * dx;

      for( ii = 1; ii <= plat; ii++)
      {
         for( jj = 1; jj <= plon; jj++)
         {
            if( gridmap(jj,ii) == 0)
            {
               double a = (ii - i) * dx;
               double b = (jj - j) * dy;
               double dist = sqrt( a*a + b*b );
               if(dist < smallest )
               {
                  smallest = dist;
               }
               else
               {
                  smallest = smallest;   //swap
               }
            }  //end if

         } //for jj
      }  //for ii

     min_dist(j,i) = smallest;  //swap again


   }  //for j
}  //for i


  //for output
  adstring distname= adstring(filename)+adstring(".dst");
  ofstream df(distname);

  write_ma_matrix(min_dist, distname);  //write output

  clogf << "\nMinimum distance file written to file " << distname << endl;
  cout << "Minimum distance file written to file " << distname << endl;
  clogf <<"pau...!" << endl;
  cout <<"pau..! " << endl;

return (0);
}