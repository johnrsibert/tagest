/**********************************************************************
* XY_pair - various member functions of XY_pair class
*         - RDB(11/20/95)
***********************************************************************/

#include "xypair.h"
#include <cifstrem.h>

ostream& operator << (ostream& os, const XY_pair& xy)
{
  xy.print(os);
  return (os);
}

void XY_pair::print(ostream& os) const
{
  if (os)
  {
    os << x << "\t" << y; // << endl;
  }
}

istream& operator >> (istream& is, XY_pair& xy)
{
  xy.readIn(is);
  return (is);
}

void XY_pair::readIn(istream& is)
{
  if (is)
  {
    is >> x >> ws >> y;
  }
}
 
//overloaded assignment operator//
XY_pair& XY_pair::operator=(XY_pair& xy)
{
  if (this == &xy)
    return *this;
  x = xy.x;
  y = xy.y;
  return *this;
}

//overloaded assignment operator//
XY_pair& XY_pair::operator=(int ixy)
{
  x = ixy;
  y = ixy;
  return *this;
}


// if you don't want this called; the linker will know
// but it is used in the linked list template
int XY_pair::operator==(XY_pair& xy)
{
  /*
  if (1)
  {
    cerr << "Error: XY_pair::operator==(XY_pair&) called\n";
    exit(1);
  }
  */
  //return xy.get_x() == x && xy.get_y() == y;
  return ((xy.x==x) && (xy.y==y));
  //return 0;
}

//copy constructor initializes object to another//
XY_pair::XY_pair(const XY_pair& xy)
{
  x = xy.x;
  y = xy.y;
}

cifstream& operator >> (cifstream& is, XY_pair& xy)
{
  is >> xy.x >> xy.y;
  return(is);
}
