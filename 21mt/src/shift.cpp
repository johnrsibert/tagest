#include "shift.h"

ostream& operator << (ostream& os, const Shift& s)
{
  if (os)
  {
    os << s.get_island() << " " << s.get_new_pos();
  }
  return(os);
}

istream& operator >> (istream& is, Shift& s)
{
	s.readIn(is);
	return (is);
}

void Shift::readIn(istream& is)
{
	if (is)
	{
  	is >> island;
	  is >> ws;
	  is >> new_pos;
	}
}

int Shift::operator == (const Shift& t)
{
//  if ( ((Shift)t.island == island) && ((Shift)t.new_pos == new_pos) )
  if ( (t.island == island) && (t.new_pos == new_pos) )
    return 1;

  return 0;
}

