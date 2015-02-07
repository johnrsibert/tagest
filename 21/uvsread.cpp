#include <iostream.h>
#include <fvar.hpp>
#include <adstring.hpp>

extern ofstream clogf;

void uvs_read(dmatrix& x, adstring& file_name, int m,
              ivector& jlb, ivector& jub)
{
  clogf << "uvs_read" << endl;
  dmatrix tmp(1,m,jlb,jub);
  ifstream tfile(file_name);
  tfile >> tmp;
  if (!tfile)
  {
    cerr << "Error reading " << file_name << " in uvs_read(...)" << endl;
    clogf << "Error reading " << file_name << " in uvs_read(...)" << endl;
    exit(1);
  }
  x.initialize();
  for (int i=1; i<=m; i++)
  {
    int j1 = jlb(i);
    int jn = jub(i);
    for (int j=j1; j<=jn; j++)
    {
      x(i,j) = tmp(i,j);
    }
  }
}//uvs_read(dmatrix&,adstring&,int,ivector&,ivector&)
