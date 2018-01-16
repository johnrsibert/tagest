#include "modlarea.h"
#include "ipar.h"

ostream& operator << (ostream& ost, ipar_t& t)
{
  int n = t[0];
  if (Model_area::get_par_file_version() == adstring("#v20r"))
    n = 150;
  const int w = 6;
  const int maxline = 80;

  adstring cc = "#    ";
  adstring ss = "|     ";
//               123456
  int ii;
  int j1, j2;

  adstring head; //(1,maxline);

  j2 = 0;

  ost << cc << t[0] << " control flags" << endl;

  for (int i = 1; i <= n; i++)
  {
     ii = (i % 10);

     if (ii == 1)
     {
       j1 = j2 + 1;
       j2 = j1 + 9;
       ost << "#" << endl;
       ost << cc << "flags " << j1 << " - " << j2 << endl;
       ost << "#" << endl;
     }

     head = cc;
     if (ii != 1)
     {
       int smax = ii;
       if (smax == 0) smax = 10;
       for (int s = 1; s < smax; s++)
       {
	 head += ss;
       }
     }

     int hlen = head.size();
     adstring tname = t.get_name(i);
     int namelen = tname.size();


     if ( (hlen + namelen) <= maxline)
     {
       ost << head << tname << endl;
     }
     else
     {
       int p1 = 1;
       int p2 = maxline - hlen;
       while (p1 < namelen)
       {
	 while ((p2 < namelen) && (tname[p2--] != ' '));
	 ost << head << tname(p1, p2) << endl;
	 p1 = p2 + 2;
	 p2 = p1 + maxline;
	 if (p2 > namelen)
	   p2 = namelen;
       }
     }

     if (ii == 0)
     {
       head += ss;
       ost << head << endl;
       for (int j = j1; j <= j2; j++)
       {
	      ost << setw(w) << t[j];
       }
       ost << endl;
    }
  }

  return ost;
}

