#ifndef __JCALIST_H__
#define __JCALIST_H__

#include <iostream>
#include <stdlib.h>

//#pragma option -w-inl-

namespace jca {

template <class mytype> class list
{
public:
  list(); 
  list(const list& l) { head = l.getHead(); }
  virtual ~list();

public:
  class node
  {
  public:
    node()
      { init(); }

    node(mytype t) 
      { init(); set(t); }
    virtual ~node() {}

  public:
    std::ostream& printAll(std::ostream& os)
    {
      os << "list<mytype>::node i " << i << '\n';
      os << "list<mytype>::node t " << t << '\n';
      return os;
    }

    void set(mytype t)
      { node::t = t; }

    void init()
      { prev = next = 0; i = 0; } //i in head carries size

  public:
    node* prev;
    node* next;

    int i;
    mytype t;
  };

public:
  mytype operator() (const int i) const
    { return get(i); }

  mytype operator[] (const int i) const
    { return get(i); }

  bool operator== (const list<mytype>& lst) const
    { return equal(lst); }

  bool operator!= (const list<mytype>& lst) const
    { return !equal(lst); }

public:
  int size() const { return head->i; }

  mytype get(const int i) const
  { 
    node* n = getNode(i);

    if (!n)
    {
      std::cerr << "Error: list::get(" << i << ") not in range[0, " 
                << head->i << ")\n";
      exit(1);
    }

    //if (n->i != i)
    //  std::cerr << "Error: n->i = " << n->i << " i = " << i << '\n';

    return n->t; 
  }

  mytype* getAll();

  void set(const mytype t, const int i) 
  { 
    node* n = getNode(i);
    if (n) n->t = t; 
  }

  // 'a' 'b' 'd' -> add('c', 1) -> 'a' 'b' 'c' 'd'
  int add(const mytype t, const int i = -1) 
  { 
    node* n = new node(t);
    if (addNode(n, i))
      numbernodes(n->next, n->i + 1);
    else 
      delete n;
 
    return n ? n->i : -1; 
  }

  // 'a' 'b' 'd' -> insert('c', 2) -> 'a' 'b' 'c' 'd'
  int insert(const mytype t, const int i) 
  { 
    node* n = new node(t);
    if (insNode(n, i)) 
      numbernodes(n->next, n->i + 1);
    else
      delete n; 

    return n ? n->i : -1;
  }

  void remove(const int i) 
  { 
    node* n = getNode(i);
    if (n)
    {
      numbernodes(n->next, i);
      pulNode(n);
      delete n;
    }
  }

  void remove(const int i0, const int i2)
    { for (int i = i0; i <= i2; i++) remove(i0); }

  void removeAll();

  //first occurrance of t from 0 to size - 1
  node* find(const mytype t) const;

  int index(const mytype t) const
  { 
    node* n = find(t);
    return n ? n->i : -1;
  }

  bool inlist(const mytype t) const
    { return exist(t); }

  bool exist(const mytype t) const
    { return index(t) >= 0 ? true : false; }

  void move(const int i0, const int i2)
  { 
    node* n = getNode(i0);
    if (n && i0 != i2)
    {
      if (i0 < i2)
      {
        node* n2 = n->next;
        n = pulNode(n);
        addNode(n, i2 - 1);
        numbernodes(n2, i0);
      }
      else
      {
        n = pulNode(n);
        node* n2 = insNode(n, i2);
        numbernodes(n2, i2);
      }
    }
  }

  bool equal(const list<mytype>& lst) const;

public:
  std::istream& inputAll(std::istream& is, list<mytype>& l);
  std::istream& inputAll(std::istream& is, list<mytype>* l);
  std::ostream& printAll(std::ostream& os = std::cout) const;
  void debugPrint() const;

public:
  node* getNode(const int i) const;

  node* insNode(list<mytype>::node* n, const int i)
  { 
    node* in = getNode(i); 
    if (in)
    {
      n->i;
      return insNode(n, in);
    }
    return in;
  }
  node* insNode(list<mytype>::node* n, list<mytype>::node* in);

  node* addNode(list<mytype>::node* n, const int i = -1)
  {
    if (i < 0 || i == head->i - 1)
    {
      n->i = head->i;
      return addNode(n, head->prev ? head->prev : head);
    }

    n->i = i + 1;
    return addNode(n, getNode(i));
  }
  node* addNode(list<mytype>::node* n, list<mytype>::node* in);

  node* pulNode(const int i) 
    { return pulNode(getNode(i)); }
  node* pulNode(list<mytype>::node* n);
  node* delNode(list<mytype>::node* n)
  {
    pulNode(n);
    delete n;
    n = 0;

    return n; 
  }

  inline node* getHead() const { return head; }
  inline void numbernodes() { numbernodes(head->next, 0); }

private:
  //set head->prev
  void numbernodes(node* n, int i)
  {
    while (n)
    {
      n->i = i++;
      n = n->next;
    } 
  }

  node* head;
};

//#include "jcalist.cpp"
template <class mytype>
list<mytype>::list() 
{
  head = new node();
  head->i = 0;
  head->prev = 0; //will point to last node
  head->next = 0;
}

template <class mytype>
list<mytype>::~list() 
{
  removeAll();
  if (head) delete head;
}

template <class mytype>
bool list<mytype>::equal(const list<mytype>& lst) const
{
  int size = lst.size();
  if (head->i != size) return false;

  node* n0 = head->next;
  node* n2 = lst.getHead()->next;

  for (int i = 0; i < size; i++)
  {
    if (n0->t != n2->t) 
      return false;

    n0 = n0->next;
    n2 = n2->next;
  }
  
  return true;
}

template <class mytype>
typename list<mytype>::node* list<mytype>::insNode(list<mytype>::node* n, list<mytype>::node* in)
{
  if (in)
  {
    head->i++;
    in->prev->next = n;
    n->prev = in->prev;
    n->next = in;
    in->prev = n;
  }
    
  return n;
}

template <class mytype>
typename list<mytype>::node* list<mytype>::addNode(list<mytype>::node* n, list<mytype>::node* in)
{
  if (in->next)
    insNode(n, in->next);
  else
  {
    node* tail;
    head->i++;
    if (head->prev)
    {
      tail = head->prev;
      n->i = tail->i + 1;
    }
    else
    {
      tail = head;
      n->i = 0;
    }
  
    tail->next = n;
    n->prev = tail;
    head->prev = n;
  }

  return n;
}

//ASSUME head->i contains correct total of heads
//here there is two options start from the front
//or start from the rear.
template <class mytype>
typename list<mytype>::node* list<mytype>::getNode(const int i) const
{
  node* current = head->next;
  if (i == head->i - 1)
    current = head->prev;
  else
    for (int n = 0; n < i; n++) current = current->next;
/*
  bool back = head->i / 2 < i ? true : false;
  list<type>::node* current;
 
  if (back)
  {
    current = head->prev;
    for (int n = current->i; n > i; n--) current = current->prev;
  }
  else
  {
    current = head->next;
    for (int n = 0; n < i; n++) current = current->next;
  }
*/
  return current;
}

template <class mytype>
typename list<mytype>::node* list<mytype>::pulNode(list<mytype>::node* n)
{
  if (n)
  {
    head->i--;
 
    //deleting from tail
    if (!n->next)
    {
      head->prev = n->prev != head ? n->prev : 0;
      n->prev->next = 0;
      n->prev = 0;
    }
    //middle of lst
    else
    {
      n->next->prev = n->prev;
      n->prev->next = n->next;
      n->next = 0;
      n->prev = 0;
    }
  }

  return n; 
}

template <class mytype>
mytype* list<mytype>::getAll()
{
  int s = size();
  mytype* ta = new mytype[s];

  for (int i = 0; i < s; i++)
    ta[i] = get(i);

  return ta;
}

template <class mytype>
void list<mytype>::removeAll()
{
  node* tracer = head->next;
  while (tracer)
  {
    node* node = tracer;
    tracer = tracer->next;

    node->prev = 0;
    node->next = 0;
    delete node;
  }

  if (head)
  {
    head->i = 0;
    head->prev = 0;
    head->next = 0;
  }
}

template <class mytype>
typename list<mytype>::node* list<mytype>::find(const mytype t) const
{
  node* tracer = head->next;
  while (tracer)
  {
    if (t == tracer->t)
      return tracer;
    tracer = tracer->next;
  }
  return 0; //not found
}

template <class mytype>
std::istream& list<mytype>::inputAll(std::istream& is, list<mytype>& l)
{
  //null the lst
  removeAll();

  mytype t;
  int size;
  is >> size;
  for (int i = 0; is && i < size; i++)
  {
    is >> t;
    l.add(t);
  }
  return is;
}

template <class mytype>
std::istream& list<mytype>::inputAll(std::istream& is, list<mytype>* l)
{
  //null the lst
  removeAll();

  mytype t;
  int size;
  is >> size;
  for (int i = 0; is && i < size; i++)
  {
    is >> t;
    l->add(t);
  }
  return is;
}

template <class mytype>
std::ostream& list<mytype>::printAll(std::ostream& os) const
{
  node* tracer = head->next;
  while (tracer)
  {
    os << tracer->t << ' ';
    tracer = tracer->next;
  }
  return os;
}
  
template <class mytype>
void list<mytype>::debugPrint() const
{
  std::cout << '\n' << "total nodes " << head->i 
            << " header is at " << head 
            << " next " << head->next
            << " prev " << head->prev;

  node* tracer = head->next;
  while (tracer)
  {
    std::cout << "\n[" << tracer->i << "]\t" << tracer->t
              << "\tat " << tracer << "\tnext " << tracer->next
                                   << "\tprev " << tracer->prev;
    tracer = tracer->next;
  }
  std::cout << std::endl;
}

template <class mytype> 
std::ostream& operator<< (std::ostream& os, const list<mytype>& l)
  { return l.printAll(os); }

template <class mytype> 
std::ostream& operator<< (std::ostream& os, const list<mytype>* l)
  { return l->printAll(os); }

template <class mytype> 
std::istream& operator>> (std::istream& is, list<mytype>& l)
  { return l.inputAll(is, l); }

template <class mytype> 
std::istream& operator>> (std::istream& is, list<mytype>* l)
  { return l->inputAll(is, l); }

template <class mytype> 
std::ostream& operator<< (std::ostream& os, const typename list<mytype>::node& l)
  { return l.printAll(os); }

template <class mytype> 
std::ostream& operator<< (std::ostream& os, const typename list<mytype>::node* l)
  { return l->printAll(os); }

}//namspace jca
#endif
