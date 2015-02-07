#include <iostream>
#include <stdlib.h>
/** \file main.cpp
main() for gmt2cst.
\ingroup preproc
*/

int gmt2cst(const char* infile, const double swlon, const double swlat, const int m, const int n, const bool graphics);

/*
\ingroup preproc
Calls gmt2cst.
\param argc
\param argv Argument vector containing
     <gmt file> <sw longitude> <sw latitude> <m>  <n> [<graphics off>];
   <gmt file>: ascii coastlines from gmt pscoast program;
 <sw longitude>: southwesti corner longitude with range [0, 360) degress;
 <sw latitude>: southwest corner latitude with range [-90, -90] degrees;
 <m>: size of the grid in the x direction;
<n>: size of the grid in the y direction;
<graphics off>: 0 to not display graphics.
*/
int main(int argc, char** argv)
{
  if (!(argc == 6 || argc == 7))
  {
    std::cerr << "Usage: " << argv[0] << " <gmt file> <sw longitude> <sw latitude> <m>  <n> [<graphics off>]\n";
    std::cerr << "<gmt file>: ascii coastlines from gmt pscoast program.\n";
    std::cerr << "<sw longitude>: southwesti corner longitude with range [0, 360) degress.\n";
    std::cerr << "<sw latitude>: southwest corner latitude with range [-90, -90] degrees.\n";
    std::cerr << "<m>: size of the grid in the x direction.\n";
    std::cerr << "<n>: size of the grid in the y direction.\n";
    std::cerr << "<graphics off>: 0 to not display graphics.\n";
    exit(1);
  }

  return gmt2cst(argv[1], strtod(argv[2], 0), strtod(argv[3], 0), atoi(argv[4]), atoi(argv[5]), argc == 7 ? false : true);
}
