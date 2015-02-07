//$Id: mymanip.cpp 2842 2011-08-15 22:35:14Z jsibert $
#if (__GNUC__ >=3)
  #include <iostream>
  using namespace std;
#else
  #include <iostream>
#endif

ostream& setscientific(ostream& _s)
{
  ostream& s=(ostream&)(_s);
  s.setf(ios::scientific,ios::floatfield);
  return s;
}

ostream& setfixed(ostream& _s)
{
  ostream& s=(ostream&)(_s);
  s.setf(ios::fixed,ios::floatfield);
  return s;
}
