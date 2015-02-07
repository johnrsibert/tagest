//$Id: bound.h 2754 2011-01-02 20:57:07Z jsibert $
#ifndef BOUND_H
#define BOUND_H
 
#include "modlarea.h"
#define SHORT short
 
union boundary_union
{
  unsigned SHORT int bb;
  struct
  {
    char x;
    char y;
  }edge;
};
 
class bound
{
 
  public:
    bound& operator = (int t)
    {
      b.bb = (unsigned SHORT)t;
      return(*this);
    }

    operator int () const {return((int)b.bb);}
 
    int edgey() const {return (int)b.edge.y;}
    int edgex() const {return (int)b.edge.x;}
 
    void set_edgey(const boundary_types t)  {b.edge.y = (char)t;}
    void set_edgex(const boundary_types t)  {b.edge.x = (char)t;}
 
  private:
    boundary_union b;
};

#endif //#ifdef BOUND_H
