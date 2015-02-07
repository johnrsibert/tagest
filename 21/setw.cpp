#include <iostream.h>
#include <iomanip.h>

int main(void)
{

  double x = 1.2345;
  char  y[]="2.3456";
  int   z =  345678;
  cout << x << endl;
  cout << y << endl;
  cout << z << endl;
  cout << "double: " << setw(15) << x << setw(15) << x << endl;
  cout << "char[]: " << setw(15) << y << setw(15) << y << endl;
  cout << "int:    " << setw(15) << z << setw(15) << z << endl;
  cout << "double: " << setw(15) << x << setfill('x') << setw(15) << x << endl;
  cout << "char[]: " << setw(15) << y << setfill('y') <<  setw(15) << y << endl;
  cout << "int:    " << setw(15) << z << setfill('z') << setw(15) << z << endl;
  return(0);
}
