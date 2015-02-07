#include <fvar.hpp>

double non_zero_matrix_average(dmatrix& x)
{
  const double eps = 1.0e-5;
  double sum = 0.0;
  int n = 0;
  int i1 = x.rowmin();
  int i2 = x.rowmax();
  for (int i = i1; i <= i2; i++)
  {
    int j1 = x(i).indexmin();
    int j2 = x(i).indexmax();
    for (int j = j1; j <= j2; j++)
    {
      if (fabs(x(i,j)) > eps)
      {
	n++;
	sum += x(i,j);
      }
    }
  }
  double average = 0.0;
  if (n > 0)
    average = sum/double(n);

  return(average);
}
