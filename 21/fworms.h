#ifndef __worms_h__
#define __worms_h__
#include <iostream>

#pragma warn -inl
#include "./jcalist.h"
using jca::list;

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
    list<float>::node* n = getNode(w);
    return n && n->next ? n->next->t : -1;
  }

  //[0, wormsize - 1] shift segments to left
  //-1 || i > wormsize add segment to worm
  void add(const double lon, const double lat, const int w, const int idx = -1)
  { 
    jca::list<list<float>::node*>::node* wn = myworms.getNode(w);
    if (wn)
    {
      list<float>::node* n = wn->t;
      n = n->next;

      if (0)//0 <= idx && idx < n->t)
      {
/*
FIX THIS should add not SET!
        int size = n->t - 1;

        //pull x0 and y0 two nodes and reset
        list<float>::node* x = xys.pulNode(n->next);   
        x->set((lon - swlon) * 60);
        list<float>::node* y = xys.pulNode(n->next);   
        y->set((lat - swlat) * 60);

        //insnodes at tail of worm
        if (wn->next)
        {
          list<float>::node* n2 = wn->next->t;
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
*/
      }
      else
      {
        //list<int>::node* x = new list<int>::node((int)(lon - swlon) * 60);
        //list<int>::node* y = new list<int>::node((int)(lat - swlat) * 60);
        list<float>::node* x = new list<float>::node((float)(lon - swlon) * 60);
        list<float>::node* y = new list<float>::node((float)(lat - swlat) * 60);
        if (wn->next)
        {
          list<float>::node* n = wn->next->t;
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

    float x = (float)(lon - swlon) * 60; 
    float y = (float)(lat - swlat) * 60; 
    xys.add(x);
    xys.add(y);

    return wormstotal() - 1;
  }

  void setrgb(const int w, const int rgb)
  {
    list<float>::node* n = getNode(w);
    if (n && n->next) n->next->t = rgb;
  }

  void setx(const double lon, const int w, const int idx)
  {
    list<float>::node* n = getNodex(w, idx);
    if (n) n->t = (float)(lon - swlon) * 60;
  }

  void sety(const double lat, const int w, const int idx)
  {
    list<float>::node* n = getNodey(w, idx);
    if (n) n->t = (float)(lat - swlat) * 60;
  }

  int getrgb(const int w) const
  {
    list<float>::node* n = getNode(w);
    return n ? n->t : -1;
  }

  float getx(const int w, const int idx) const
  {
    list<float>::node* n = getNodex(w, idx);
    return n ? n->t : -1;
  }

  float gety(const int w, const int idx) const
  {
    list<float>::node* n = getNodey(w, idx);
    return n ? n->t : -1;
  }

  void remove(const int w)
  { 
    list<float>::node* n = getNode(w);
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
    list<float>::node* n = xys.getHead()->next;
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

  jca::list<float>& getLst() { return xys; }
  jca::list<float>::node* getHead() const { return xys.getHead(); }

private:
  jca::list<float> xys;
  jca::list<list<float>::node*> myworms;
  double swlon, swlat;

private:
  list<float>::node* getNode(const int w) const
    { return myworms.get(w); }

  list<float>::node* getNodex(const int w, const int idx) const
  {
    list<float>::node* n = getNode(w);
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

  list<float>::node* getNodey(const int w, const int idx) const
  {
    list<float>::node* n = getNodex(w, idx);
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
