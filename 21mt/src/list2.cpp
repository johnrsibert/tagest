#ifndef __LIST2_CPP__
#define __LIST2_CPP__
 
#include <iostream>
#include <stdio.h> //needed for NULL mnemonic//
 
template <class LType> class List
{
  public:
    List() { head = NULL; ItemsinList = 0; }
    List(const List<LType>& l);
    ~List();
    void add(LType d);
    int isEmpty() const;
    int NumberofItemsinList() const { return ItemsinList; }
    int find(LType d) const;
    LType getItem(int ith) const;
    void print(ostream& os) const;
    void readIn(istream& is);
    List<LType>& operator = (const List<LType>& l);
  private:
    class ListElement
    {
      friend class List<LType>;
      public:
        LType data;
      private:
        ListElement* next; //link to next item//
    };
    ListElement* head; //start of list//
    unsigned ItemsinList;
};
 
template <class LType> ostream& operator << (ostream& os, const List<LType>& l);
template <class LType> istream& operator >> (istream& is, List<LType>& l);
#include <stdlib.h>
 
template <class LType> void List<LType>::add(LType d)
{
  ListElement* new_ptr; //pointer to next item in list//
  new_ptr = new ListElement;
  if(!new_ptr)
  {
    cerr << "\nAllocation error in List class!\n" << endl;
    exit(1);
  }
  new_ptr->data = d; //store data//
  //make next pointer point to first element//
  new_ptr->next = head;
  //change head to point to new element//
  head = new_ptr;
  ItemsinList++;
}
 
template <class LType> List<LType>::List(const List<LType>& l)
:ItemsinList(l.ItemsinList)
{
  //LType temp;
  int count = l.ItemsinList;
  while (count >= 1)
  {
    LType temp = l.getItem(count);
    add(temp);
    count--;
  }
}

template <class LType> List<LType>& List<LType>::operator =(const List<LType>& l)
{
  if (this == &l)
    return *this;
  ListElement* current_ptr;
  current_ptr = head;
  while (head != NULL)
  {
    head = head->next;
    delete current_ptr;
    current_ptr = head;
  }
  ItemsinList = 0;
  //current_ptr = l.head;
  //LType temp;
  int count = l.ItemsinList;
  while (count >= 1)
  {
    LType temp = l.getItem(count);
    add(temp);
    count--;
  }
  return(*this);
}

//returns a 1 if there are no items in list//
template <class LType> int List<LType>::isEmpty() const
{
  if (head == NULL)
    return 1; //empty list//
  return 0; //some items in list//
}
 
//returns 1 if item found, 0 if item not found//
template <class LType> int List<LType>::find(LType d) const
{
  ListElement* current_ptr;
  current_ptr = head;
  while (current_ptr != NULL)
  {
    if (current_ptr->data == d)
      break;
    current_ptr = current_ptr->next;
  }
  //if current_ptr == NULL, reached end of list and didn't find data//
  return (current_ptr != NULL);
}
 
//returns the ith  item, if ith less than 1 then returns first item//
//if ith greater then ItemsinList then returns last item//
//remember this a LIFO(Last In First Out)//
template <class LType> LType List<LType>::getItem(int ith) const
{
  ListElement* current_ptr;
  current_ptr = head;
  int counter = 1;
  if (ith < 1)
    ith = 1;
  if ((unsigned)ith > ItemsinList)
    ith = ItemsinList;
  while(counter < ith)
  {
    current_ptr = current_ptr->next;
    counter++;
  }
  return (current_ptr->data);
}
 
template <class LType> ostream& operator << (ostream& os, const List<LType>& l)
{
  l.print(os);
  return os;
}

template <class LType> void List<LType>::print(ostream& os) const
{
  ListElement* current_ptr;
  current_ptr = head;
 
  if (os)
  {
    while (current_ptr != NULL)
    {
      os << current_ptr->data << endl;
      current_ptr = current_ptr->next;
    }
  }
}
 
template <class LType> istream& operator >> (istream& is, List<LType>& l)
{
  l.readIn(is);
 
  return is;
}
 
template <class LType> void List<LType>::readIn(istream& is)
{
  LType temp;
 
  if (is)
  {
    while (!is.eof())
    {
      is >> temp;
      //extra check to prevent eof error causing extra read//
      if (is)
      {
        add(temp);
      }
    }
  }
}
 
template <class LType> List<LType>::~List()
{
  ListElement* current_ptr;
  current_ptr = head;
  while (head != NULL)
  {
    head = head->next;
    delete current_ptr;
    current_ptr = head;
  }
  delete head;
  //cout << "\nFinished with list destructor!" << endl;
}
 
#endif //#ifdef __LIST_H__ //
