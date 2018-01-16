//$Id: ipar.cpp 2949 2012-02-13 02:04:24Z jsibert $
#include "ipar.h"

extern ofstream clogf;


adstring ipar_t::get_name(const int i)
{
  int n = (*this)[0];
  if ((i<0) || (i>n))
  {
    cerr << "Index out of range in adstring ipar_t::get_name(int i)" << endl;
    cerr << "  Valid range is 0 .. " << n << "; you had " << i << endl;
    exit(1);
  }

  return name[i];
}

void ipar_t::set_name(const int i, const char* c)
{
  int n = (*this)[0];
  if ((i<0) || (i>n))
  {
    cerr << "Index out of range in adstring ipar_t::set_name()" << endl;
    cerr << "  Valid range is 0 .. " << n << "; you had " << i << endl;
    exit(1);
  }

  //adstring s = c;
  //nameptr[i] = s;
  name(i) = adstring(c);
}

ipar_t::ipar_t(const int n)
{
  allocate(n);
}

ipar_t::ipar_t(const ipar_t& t) :v(t.v),name(t.name)
{ cout << "in ipar_t::ipar_t(const ipar_t& t)" << endl; }

ipar_t::ipar_t() { }

/*
ipar_t::~ipar_t()
{
  //int n = (*this)[0];
  //delete [n+1]nameptr;
	if (nameptr != NULL)
    delete [] nameptr;
	if (lvptr != NULL)
    delete lvptr;
}
*/

void ipar_t::allocate(const int n)
{
  v.allocate(0,n);
  v[0] = n;

  name.allocate(0,n);



  set_name(0, "Number of ipars");

  for (int i = 1; i <= n; i++)
  {
    v(i) = 0;
    set_name(i, "Unused");
  }
}

ipar_t& ipar_t::operator = (const ipar_t& t)
{
  /*
  v = t.v;
  name = t.name;
  */
  for (int i = indexmin(); i<= indexmax(); i++)
  {
    v(i) = t.v(i);
    name(i) = t.name(i);
  }
  return *this;
}

