#include "modlarea.h"
#include <fstream>

#include "trace.h"

void Model_area::set_vector_bounds()
{
  int i, j;

/*jca Optimization
  for (j = 1; j <= n; j++)
  {
    i = 1;
    while (gridmap(i++, j) == 0);
    ilb(j) = i;
  }//for j
*/
  for (j = 1; j <= n; j++)
  {
    i = 1;
    ilb(j) = i;
    while ( (i<m) && (gridmap(i,j) == 0) )
    {
      i++;
      ilb(j) = i;
    }
  }
  //TRACE(ilb)

  for (j = n; j >= 1; j--)
  {
    i = m;
    iub(j) = i;
    while ( (i<m) && (gridmap(i,j) == 0) )
    {
      iub(j) = i;
      i--;
    }
  }
  //TRACE(iub)

  for (i = 1; i <= m; i++)
  {
    j = 1;
    jlb(i) = j;
    while ( (j<n) && (gridmap(i,j) == 0) )
    {
      j++;
      jlb(i) = j;
    }
  }
  //TRACE(jlb)

  for (i = m; i >= 1; i--)
  {
    j = n;
    jub(i) = j;
    while ( (j<n) && (gridmap(i,j) == 0) )
    {
      j--;
      jub(i) = j;
    }
  }
  //TRACE(jub)

  for (j=1; j<=n; j++)
  {
    int lb = ilb(j);
    int ub = iub(j);
    for (i = lb; i <= ub; i++)
    {
      if (jlb(i) > j)
      {
	jlb(i) = j;
      }
      if (jub(i) < j)
      {
	jub(i) = j;
      }
    }
  }
  TRACE(jlb) TRACE(jub)

  for (i=1; i<= m; i++)
  {
    int lb = jlb(i);
    int ub = jub(i);
    for (j = lb; j <= ub; j++)
    {
      if (ilb(j)> i)
      {
	ilb(j) = i;
      }
      if (iub(j)< i)
      {
	iub(j) = i;
      }
    }
  }
  TRACE(ilb) TRACE(iub)
  cout << "Finished void Model_area::set_vector_bounds()" << endl;
}
