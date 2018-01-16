//$Id: pareof.cpp 2842 2011-08-15 22:35:14Z jsibert $
#if (__GNUC__ > 3)
  #include <iostream>
  #include <fstream>
  using namespace std;
#else
    #include <iostream>
    #include <fstream.h>
#endif

#if defined(__SC__) || defined(__BCPLUSPLUS__)
#include "list2.cpp"
#else
#include "list2.cpp"
#endif //__SC__
#include "shift.h"

extern ofstream clogf;

void par_eofile(List<Shift>& s_pos, fstream& psf)
{
	cout << "Shift position list: " << endl;
	cout << s_pos;
	clogf << "Shift position list: " << endl;
	clogf << s_pos;
        psf.setf(ios::fixed);
	psf << s_pos;
}

