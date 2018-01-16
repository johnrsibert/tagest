#define HOME_VERSION
//COPYRIGHT (c) 1991 OTTER RESEARCH LTD
#undef HOME_VERSION
#include <fvar.hpp>

#ifdef __TURBOC__
  #pragma hdrstop
#endif

#include <string.h>
#include <iomanip>
ostream& setfixed(ostream& _s);
ostream& setscientific(ostream& _s);

void fmmdisp(_CONST dvector& x,_CONST dvector& g,
             const int& nvar, int scroll_flag, int noprintx)
{
  if (noprintx)
    return;

  int      headings = 3;     /* number of heading lines */
  int      cols = 3;     /* number of columns to display  */

  int      imax;         /* number of lines to display */
  int      wmax=22;         /* number of lines in current window */


  char colhead[] = "Var   Value    Gradient   ";

  int i1 = g.indexmin();
  int i2 = g.indexmax();
  ivector g_index(i1,i2);
  //dvector sorted_g = 
  sort(fabs(g),g_index);
 
  imax = nvar / cols;
  if (nvar % cols > 0) imax++;
  if ( (scroll_flag == 0) && (imax > wmax-headings) )
    imax = wmax - headings - 1;

  strcpy(colhead,"Var   Value    Gradient   ");
  cout << colhead << colhead << colhead << endl;
  cout.fill(' ');
 
  int rsij = i2+1;
  for (int i=1; i<=imax; i++)
  {
    for (int j=0; j<cols; j++)
    {
      int ij = cols*(i-1)+(j+1);
      rsij --;
      if (ij <= nvar)
      {
	      int sij = g_index(rsij);
        cout << setw(3) << sij
             << setw(9)  << setfixed      << setprecision(5) << x[sij]
             << setw(12) << setscientific << setprecision(4) << g[sij];

        if (j<cols-1)
        {
          cout << (" |");
        }
        else
        {
          cout << endl;
        }
      }
    } // j loop
  }  // i loop
  if (scroll_flag)
    cout << endl; // << endl;
  else
    cout.flush();
}

#undef HOME_VERSION
