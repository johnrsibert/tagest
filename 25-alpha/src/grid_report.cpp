//$Id: grid_report.cpp 3156 2014-03-14 01:01:14Z eunjung $
#include <fvar.hpp>

void grid_report(dmatrix& t, ofstream& c, const char* name)
{
  double sum = 0;
  int negatives = 0;
  double tmin = 0;
  double tmax = 0;

  int i1=t.rowmin();
  int i2=t.rowmax();
  for (int i=i1; i<=i2; i++)
  {
    int j1 = t(i).indexmin();
    int j2 = t(i).indexmax();
    for (int j=j1; j<=j2; j++)
    {
      sum += t[i][j];
      if (t[i][j] > tmax)
	tmax = t[i][j];
      if (t[i][j] < 0)
      {
	 negatives ++;
	 if (t[i][j] < tmin)
	   tmin = t[i][j];
      }
    }
  }

  c << "sum of " << name <<" cells = " << sum
    << "; the maximum was " << tmax << endl;
  if (negatives > 0)
  {
    c << "  there were " << negatives << " negatives" << endl;
    c << "  the minimum was " << tmin << endl;
  }
}
