#ifndef __worms_h__
#define __worms_h__
#include <iostream>

//#pragma warn -inl
#include <jcalist.h>

/*format 
 *
 *
 *   _|0,..., idx,... wormsize(w) - 1
 *   0|XXX
 *   .|XXXXX
 *   .|XXXXXX    getx(w, idx) gety(w, idx)
 *   .|XXX
 *   w|X
 *   .|XXX
 *   .|XXXXXX
 *   .|XX
 *   .|XXX
 *   totalworms() - 1
 */

class worms
{
public:
  worms() { set(0, 0); } 
  worms(const double swlong, const double swlat)
    { set(swlong, swlat); } 

public:
  int wormstotal() const
  {
    //int i = 0;
    //while (getNode(i)) { i++; }
    return myworms.size();
  }

  //assume format is correct
  int wormsize(const int w) const
  {
    jca::list<int>::node* n = getNode(w);
    return n && n->next ? n->next->t : -1;
  }

  //[0, wormsize - 1] shift segments to left
  //-1 || i > wormsize add segment to worm
  void add(const double lon, const double lat, const int w, const int idx = -1)
  { 
    jca::list<jca::list<int>::node*>::node* wn = myworms.getNode(w);
    if (wn)
    {
      jca::list<int>::node* n = wn->t;
      n = n->next;

      if (0 <= idx && idx < n->t)
      {
        int size = n->t - 1;

        //pull x0 and y0 two nodes and reset
        jca::list<int>::node* x = xys.pulNode(n->next);   
        x->set((lon - swlon) * 60);
        jca::list<int>::node* y = xys.pulNode(n->next);   
        y->set((lat - swlat) * 60);

        //insnodes at tail of worm
        if (wn->next)
        {
          jca::list<int>::node* n2 = wn->next->t;
          while (size > idx)
          {
            n2 = n2->prev;
            n2 = n2->prev;
          }
          xys.insNode(x, n2);
          xys.insNode(y, n2);
        }
        else
        {
          xys.addNode(x);
          xys.addNode(y);
        }
      }
      else
      {
        jca::list<int>::node* x = new jca::list<int>::node((lon - swlon) * 60);
        jca::list<int>::node* y = new jca::list<int>::node((lat - swlat) * 60);
        if (wn->next)
        {
          jca::list<int>::node* n = wn->next->t;
          xys.insNode(x, n);
          xys.insNode(y, n);
        }
        else
        {
          xys.addNode(x);
          xys.addNode(y);
        }

        n->t++;
      }
    }
  }

  int addWorm(const double lon, const double lat, const int rgb = 0)
  {
    xys.add(rgb);
    myworms.add(xys.getHead()->prev);
    xys.add(1);

    int x = (int)(lon - swlon) * 60; 
    int y = (int)(lat - swlat) * 60; 
    xys.add(x);
    xys.add(y);

    return wormstotal() - 1;
  }

  void setrgb(const int w, const int rgb)
  {
    jca::list<int>::node* n = getNode(w);
    if (n && n->next) n->next->t = rgb;
  }

  void setx(const double lon, const int w, const int idx)
  {
    jca::list<int>::node* n = getNodex(w, idx);
    if (n) n->t = (int)(lon - swlon) * 60;
  }

  void sety(const double lat, const int w, const int idx)
  {
    jca::list<int>::node* n = getNodey(w, idx);
    if (n) n->t = (int)(lat - swlat) * 60;
  }

  int getrgb(const int w) const
  {
    jca::list<int>::node* n = getNode(w);
    return n ? n->t : -1;
  }

  int getx(const int w, const int idx) const
  {
    jca::list<int>::node* n = getNodex(w, idx);
    return n ? n->t : -1;
  }

  int gety(const int w, const int idx) const
  {
    jca::list<int>::node* n = getNodey(w, idx);
    return n ? n->t : -1;
  }

  void remove(const int w)
  { 
    jca::list<int>::node* n = getNode(w);
    if (n) xys.remove(n->i, n->i + 1 + 1 + n->t * 2);
  }

  void removeAll()
  {
    const int size = wormstotal();
    for (int i = 0; i < size; i++)
      remove(0);
  }

  void printAll()
    { printAll(std::cout); }

  std::ostream& printAll(std::ostream& sout) const
  {
    jca::list<int>::node* n = xys.getHead()->next;
    while (n)
    {
      sout << n->t << ' ';
      n = n->next;
    }
    return sout;
  }

  void printSizes(std::ostream& sout) const
  {
    const int size = wormstotal();
    for (int i = 0; i < size; i++)
      { sout << wormsize(i) << ' '; }
  }

  void printHeads(std::ostream& sout) const
  {
    const int size = wormstotal();
    for (int i = 0; i < size; i++)
      { sout << " " << getx(i, 0) << " " << gety(i, 0); }
  }

public:
  void test(const int xmax, const int ymax);

  void set(const double swlong, const double swlat)
    { worms::swlon = swlong; worms::swlat = swlat; }

  jca::list<int>& getLst() { return xys; }
  jca::list<int>::node* getHead() const { return xys.getHead(); }

private:
  jca::list<int> xys;
  jca::list<jca::list<int>::node*> myworms;
  double swlon, swlat;

private:
  jca::list<int>::node* getNode(const int w) const
    { return myworms.get(w); }

  jca::list<int>::node* getNodex(const int w, const int idx) const
  {
    jca::list<int>::node* n = getNode(w);
    n = n->next ? n->next : 0; //skip rgb 

    if (n && 0 <= idx && idx < n->t)
    {
      n = n->next; //skip size
      for (int i = 0; i < idx; i++)
      {
        n = n->next;
        n = n->next;
      }

      return n;
    }

    return 0;
  }

  jca::list<int>::node* getNodey(const int w, const int idx) const
  {
    jca::list<int>::node* n = getNodex(w, idx);
    return n ? n->next : 0;
  }
};

/*
std::ostream& operator<<(std::ostream& os, const worms& w)
  { return w.printAll(os); } 

std::ostream& operator<<(std::ostream& os, const worms* w)
  { return w->printAll(os); } 
*/
#endif //define __worms_h__
