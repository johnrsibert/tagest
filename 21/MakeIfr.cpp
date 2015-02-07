#include <iostream.h>
#include <adstring.hpp>

ofstream clogf("makeifr.log");
void rpreproc_main(char c, const adstring& path);

int main(void)
{
  adstring adsPath(".\\");
  rpreproc_main('e', adsPath);
  return(0);
}
