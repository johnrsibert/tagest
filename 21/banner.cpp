#include <fvar.hpp>
#include <adstring.hpp>
#ifdef unix
#include <strstream>
#else
#include <strstrea.h>
#endif
#include <iomanip.h>

adstring make_banner(const adstring& program_name)
{
  int version = 0;
  adstring banner(1,80);
  banner = program_name + " (";
#if defined(__BCPLUSPLUS__)
  banner += "Borland C++";
  version = __BORLANDC__;
#elif defined (_MSC_VER)
  banner += "Microsoft Visual C++";
  version = _MSC_VER;
#elif defined (__SC__)
  banner += "Symantec C++";
  version = __SC__;
#elif defined (__ZTC__)
  banner += "Zortech C++";
  version = __ZTC__;
#elif defined(__GNUC__)
  banner += "Gnu C++";
 // cout << "__GNUC__ = " << __GNUC__ << endl;
  version = __GNUC__;
//version = __VERSION__;
#elif defined(sun)
  banner += "Sun C++";
  #ifdef __SUNPRO_CC
    version = __SUNPRO_CC;
  #endif
#elif defined(__NDPX__)
  #ifdef __i860
    banner += "(NDP i860 C++ version)";
  #else
    banner += "(NDP C++ version)";
  #endif
#else
  banner += "Unknown Compiler";
#endif
  if (version != 0)
  {
    char s[80];
    std::ostrstream ss(s,80);
    ss << hex << version << '\0';
    banner += ", v" + adstring(s);
  }

#if defined (__ZTC__) || defined (__SC__)
  #ifdef DOS386
    banner += ", protected mode";
  #endif
#endif

  banner += ") ";

  banner += adstring(__PWD__);

//adstring spath(__PWD__);
//int n = spath.size();
//int m = n;
//while ( (n > 1) && (spath(--n) != '/') );
//dirname = spath(n+1,m);


  return(banner);
}

/*
int main(void)
{
   cout << make_banner(__FILE__) << endl;
   return(0);
}
*/
