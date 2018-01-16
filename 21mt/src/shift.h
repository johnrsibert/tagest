#ifndef __SHIFT_H__
#define __SHIFT_H__
#include "geo_cord.h"
#include <iostream>

//class used to handle shifting coords//
class Shift
{
  public:
    Shift() { } 
    Shift(Geo_coord& g1)
    { island = g1; new_pos = g1; }
    Shift(Geo_coord& g1, Geo_coord& g2)
    { island = g1; new_pos = g2; }
    Shift& operator = (const Shift& g)
    { island = g.island; new_pos = g.new_pos; return(*this); }
    int operator == (const Shift& t);
    Geo_coord get_island() const { return(island); }
    Geo_coord get_new_pos() const { return(new_pos); }
    void set_island(Geo_coord& g) { island = g; }
    void set_new_pos(Geo_coord& g) { new_pos = g; }
    void readIn(istream& is);
  private:
    Geo_coord island;
    Geo_coord new_pos;
};

ostream& operator << (ostream& os, const Shift& s);
istream& operator >> (istream& is, Shift& s);
#endif // #ifndef __SHIFT_H__

