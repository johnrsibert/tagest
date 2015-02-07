#include <fvar.hpp>
#include <adstring.hpp>

ofstream clogf("make_par.log");
adstring make_banner(const adstring& program_name);
void rpreproc_main(char c, const adstring& path);
adstring banner;
int main(int argc, char ** argv)
{
  banner = make_banner(adstring(argv[0])+": initial par file generation utility\n");  
  cout << banner << endl;
  clogf << banner << endl;
  adstring adsPath(".\\");
  rpreproc_main('p', adsPath);
  exit(0);
}
