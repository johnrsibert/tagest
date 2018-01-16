/**********************************************************************
* xypair - A class to store an x, y pair of values.
*        - RDB(11/20/95)
**********************************************************************/
#ifndef __xypair_h__
#define __xypair_h__

#include <iostream>
#include <stdlib.h>

class cifstream;

class XY_pair
{
  public:
    XY_pair() { x = -999; y= -999; }
    XY_pair(const double _x, const double _y) { x = _x; y = _y; }
    XY_pair(const XY_pair& xy);
    ~XY_pair() { }
    double get_x() { return x; }
    double get_y() { return y; }
    void print(ostream& os) const;
    void readIn(istream& is);
    XY_pair& operator=(XY_pair& xy);
    XY_pair& operator=(int ixy);
    int operator == (XY_pair& xy);
    int operator != (XY_pair& xy) { return(!(*this==xy)); }
  private:
    double x;
    double y;

  friend   cifstream& operator >> (cifstream& is, XY_pair& xy);


};

ostream& operator << (ostream& os, const XY_pair& xy);
istream& operator >> (istream& is, XY_pair& xy);
#endif
