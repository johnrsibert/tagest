#include <iostream>
#include <fstream>
#include <sstream>
/** \file gmt2cst.cpp
Souce code for gmt2cst() and supporting functions.
*/
using std::istringstream;
using std::ostream;
using std::istream;
using std::ios;
using std::cerr;
using std::endl;

#include <jnigraphics3.h>
#include "jcalist.h"
using jca::list;

void read_gmt(const char* gmtfile, list<list<char*>*>& lst, std::ofstream& ofs, const double swlon, const double swlat);
void write(std::ofstream& ofs, list<char*>* segment, const double swlon, const double swlat);
void write(std::ofstream& ofs, list<list<char*>*>& lst, const double swlon, const double swlat);
void clean(list<list<char*>*>& lst);
void clean(list<char*>* segment);

void connect_america(list<list<char*>*>& lst)
{
  list<list<char*>*>::node* west = 0;
  list<list<char*>*>::node* east = 0;
  std::string dummy;
  list<list<char*>*>::node* n = lst.getHead()->next;
  while (n)
  {
    list<char*>* segment = n->t;
    if (segment && segment->size() > 0)
    {
      list<char*>::node* s = segment->getHead(); 
      int shore_bin_number = 0;
      int level = 0;
      istringstream iss(s->t, istringstream::in);
      iss >> dummy >> dummy >> dummy >> dummy >> shore_bin_number >> dummy >> dummy >> level;
      if (shore_bin_number == 68 && level == 1)
      {
        west = n;
      }
      else if (shore_bin_number == 85 && level == 1)
      {
        east = n;
      }
    }
    n = n->next;
  }
  if (west && east)
  {
    east = lst.pulNode(east);
    list<char*>* segment_east = east->t;
    list<char*>* segment_west = west->t;
    while (segment_east->getHead()->next)
    {
      segment_west->addNode(segment_east->pulNode(segment_east->getHead()->next)); 
    }
    clean(segment_east);
    delete segment_east;
    delete east;
  }
}
void connect_africa(list<list<char*>*>& lst)
{
  std::string dummy;
  list<list<char*>*>::node* n = lst.getHead()->next;
  while (n)
  {
    list<char*>* segment = n->t;
    if (segment && segment->size() > 0)
    {
      list<char*>::node* s = segment->getHead(); 
      int shore_bin_number = 0;
      int level = 0;
      istringstream iss(s->t, istringstream::in);
      iss >> dummy >> dummy >> dummy >> dummy >> shore_bin_number >> dummy >> dummy >> level;
      if (shore_bin_number == 72 && level == 1)
      {
        list<list<char*>*>::node* africa = n;
        list<char*>* segment_africa = africa->t;
        list<char*>::node* n2 = new list<char*>::node(0);
        char* ca2 = new char[20];
        strcpy(ca2, "50 -10");
        n2->t = ca2;
        segment_africa->addNode(n2);
        list<char*>::node* n3 = new list<char*>::node(0);
        char* ca3 = new char[20];
        strcpy(ca3, "50 45");
        n3->t = ca3;
        segment_africa->addNode(n3);
        return;
      }
    }
    n = n->next;
  }
}
/**
Converts file dumped by GMT pscoast into format for jnigraphics display.
Parameters list corresponds to first line of a movemod .par file.
\param infile Input file name. 
Character string contating for the name of the file containing output from GMT pscoast extraction procedure
\param swlon double containing the southwest longitude of the model domain.
\param swlat double containing the southwest latitude of the model domain.
\param m int containing the number of 1 degree (?) cells to the east of swlon.
\param n int containing the number of 1 degree (?) cells to the north of swlat.
\parm graphics boolean 1 (TRUE) to display graphics.
\ingroup preproc
*/
int gmt2cst(const char* infile, const double swlon, const double swlat, const int m, const int n, const bool graphics)
{
  std::string outfile(infile);
  outfile += ".cst";

  //write cst header
  std::ofstream ofs(outfile.c_str(), std::ios::out|std::ios::binary);
  int version = 3;
  ofs.write((char*)&version, sizeof(short int));

  //read all in
  list<list<char*>*> lst;
  read_gmt(infile, lst, ofs, swlon, swlat);

std::cout << "Got " << lst.size() << " unclosed segments." << std::endl;

  connect_africa(lst);
  connect_america(lst);

  write(ofs, lst, swlon, swlat);

  ofs.close();

  clean(lst);

  if (graphics)
  {
    jnigraphics3 jni;
    int* ia = jni.addGridLayout(1, 1);
    int id = ia[0];
    delete ia;
    jni.add(id);

    regiondefs rdef;
    rdef.title = (char*)outfile.c_str();
    rdef.m = m;
    rdef.n = n;
    rdef.dx = 60;
    rdef.dy = 60;
    rdef.coastlinesFile = (char*)outfile.c_str();
    jni.addRegion(rdef, id);

    jni.layoutAll(800, 600);

    while (jni.paintAll(1000));
  }

  return 0;
}
list<list<char*>*>::node* add(list<char*>* segment, list<list<char*>*>& lst)
{
  if (segment == 0 || segment->size() == 0)
  {
    return 0;
  }
  char* segmenthead = segment->getHead()->next->t;
  char* segmenttail = segment->getHead()->prev->t;

  list<list<char*>*>::node* n = lst.getHead()->next;
  while (n)
  {
    list<char*>* tracer = n->t;
    if (tracer->size() > 0) 
    {
      char* tracerhead = tracer->getHead()->next->t;
      char* tracertail = tracer->getHead()->prev->t;
      if (strcmp(segmenthead, tracerhead) == 0)
      {
        list<char*>::node* head = segment->getHead();
        while (head->next)
        {
          list<char*>::node* pulled = segment->pulNode(head->next);
          tracer->insNode(pulled, 0);
        }
        return n;
      }
      else if (strcmp(segmenthead, tracertail) == 0)
      {
        list<char*>::node* head = segment->getHead();
        while (head->next)
        {
          list<char*>::node* pulled = segment->pulNode(head->next);
          tracer->addNode(pulled);
        }
        return n;
      }
      else if (strcmp(segmenttail, tracerhead) == 0)
      {
        list<char*>::node* head = segment->getHead();
        while (head->prev)
        {
          list<char*>::node* pulled = segment->pulNode(head->prev);
          tracer->insNode(pulled, 0);
        }
        return n;
      }
      else if (strcmp(segmenttail, tracertail) == 0)
      {
        list<char*>::node* head = segment->getHead();
        while (head->prev)
        {
          list<char*>::node* pulled = segment->pulNode(head->prev);
          tracer->addNode(pulled);
        }
        return n;
      }
    }
    n = n->next;
  }
  return 0;
}
bool is_closed(list<char*>* segment)
{
  if (segment && segment->size() > 0)
  {
    char* head = segment->getHead()->next->t;
    char* tail = segment->getHead()->prev->t;
    if (strcmp(head, tail) == 0)
    {
      return true;
    }
  }
  return false;
}
list<list<char*>*>::node* combine_segments(list<list<char*>*>::node* n, list<list<char*>*>& lst, std::ofstream& ofs, const double swlon, const double swlat)
{
  list<list<char*>*>::node* from = lst.pulNode(n);
  list<list<char*>*>::node* to = add(from->t, lst);
  if (to == 0)
  {
    if (is_closed(from->t))
    {
      write(ofs, from->t, swlon, swlat);
      clean(from->t);
      delete from->t;
      from->t = 0;

      delete from;
      from = 0;
    }
    else
    {
      lst.addNode(from);
    }
  }
  else
  {
    clean(from->t);
    delete from->t;
    from->t = 0;

    delete from;
    from = 0;
  }
  return to;
}
void read_gmt(const char* gmtfile, list<list<char*>*>& lst, std::ofstream& ofs, const double swlon, const double swlat)
{
std::cout << "Reading...";
  std::ifstream ifs(gmtfile);
  if (!ifs)
  {
    std::cerr << "Error: unable to open " << gmtfile << " for input\n";
    exit(1);
  }
  list<char*>* segment = segment = new list<char*>();
  const int maxlinesize = 100;
  char* line = new char[maxlinesize];
  ifs.getline(line, maxlinesize); //skip header line
  int shore_bin_number = 0;
  int level = 0;
  int filelines = 3;
  while (ifs)
  {
    if (line[0] == '>')
    {
      std::string dummy;
      istringstream iss(line, istringstream::in);
      iss >> dummy >> dummy >> dummy >> dummy >> shore_bin_number >> dummy >> dummy >> level;
      if (is_closed(segment))
      {
        write(ofs, segment, swlon, swlat);
      }
      else
      {
        //save segment to list
        list<list<char*>*>::node* n = new list<list<char*>*>::node(0);
        n->t = segment;
        lst.addNode(n);
        segment = 0;

        while (n)
        {
          n = combine_segments(n, lst, ofs, swlon, swlat);
        }
      }
      if (segment == 0)
      {
        segment = new list<char*>();
      }
      else
      {
        clean(segment);
      }
      segment->getHead()->t = line;
      line = 0;
    }
    else
    {
      //allow only main coastlines
      if (level == 1)
      {
        list<char*>::node* segmentnode = new list<char*>::node(0);
        segmentnode->t = line;
        segment->addNode(segmentnode);
      }
    }
    line = new char[maxlinesize];
    ifs.getline(line, maxlinesize);
    filelines++;
  }
  ifs.close();
  if (segment && segment->size() > 0)
  {
    list<list<char*>*>::node* n = new list<list<char*>*>::node(0);
    n->t = segment;
    lst.addNode(n);
    segment = 0;
  }
  if (line)
  {
    delete line;
    line = 0;
  }
  if (segment)
  {
    clean(segment);
    delete segment;
    segment = 0;
  }
std::cout << "done." << std::endl;
cout << "Total lines is " << filelines << endl;
}
void remove_duplicates(list<char*>* segment, const double swlon, const double swlat)
{
  int prevlon = 0;
  int prevlat = 0;
  list<char*>::node* segmentnode = segment->getHead()->next;
  while (segmentnode)
  {
    char* cahead = segmentnode->t;
    char* endptr;
    double dlon = strtod(cahead, &endptr);
if (swlon > 200)
{
  dlon += 360.0;
}
    double dlat = strtod(endptr, 0);
    int lon = int(60.0 * (dlon - swlon));
    int lat = int(60.0 * (dlat - swlat));
    if (prevlat == lat && prevlon == lon)
    {
      list<char*>::node* prev = segment->pulNode(segmentnode->prev);
      if (prev->t)
      {
        delete prev->t;
        prev->t = 0;
      }
      delete prev;
      prev = 0;
    }
    prevlon = lon;
    prevlat = lat;
    segmentnode = segmentnode->next;
  }
}
void write(std::ofstream& ofs, list<char*>* segment, const double swlon, const double swlat)
{
  //remove_duplicates(segment, swlon, swlat);

  const int sizeofint = sizeof(int);
  int size = segment->size();
  ofs.write((char*)&size, sizeofint);
  list<char*>::node* segmentnode = segment->getHead()->next;
  while (segmentnode)
  {
    char* cahead = segmentnode->t;
    char* endptr;
    double dlon = strtod(cahead, &endptr);
if (swlon > 200)
{
  dlon += 360.0;
}
    double dlat = strtod(endptr, 0);
    int lon = int(60.0 * (dlon - swlon));
    int lat = int(60.0 * (dlat - swlat));
    ofs.write((char*)&lon, sizeofint);
    ofs.write((char*)&lat, sizeofint);
    segmentnode = segmentnode->next;
  }
}
void check(list<list<char*>*>& lst)
{
  int index = 0;
  list<list<char*>*>::node* n = lst.getHead()->next;
  while (n)
  {
    list<char*>* segment = n->t; 
    if (segment && segment->size() > 0)
    {
      cout << "Index: "<< index << endl;
      cout << "Size: "<< segment->size() << endl;
      list<char*>::node* s = segment->getHead(); 
      cout << "Head: "<< s->t << endl;
      cout << "Next: "<< s->next->t << endl;
      cout << "Prev: "<< s->prev->t << endl;
      cout << endl;
    }
    n = n->next;
    index++;
  }
}
void write(std::ofstream& ofs, list<list<char*>*>& lst, const double swlon, const double swlat)
{
  check(lst);
  //int index = 0;
  list<list<char*>*>::node* lstnode = lst.getHead()->next;
  while (lstnode)
  {
    write(ofs, lstnode->t, swlon, swlat);
    //index++;
    lstnode = lstnode->next;
  }

/*
  int size = 4;
  ofs.write((char*)&size, sizeofint);

  int lon0 = 0;
  int lat0 = 0;
  ofs.write((char*)&lon0, sizeofint);
  ofs.write((char*)&lat0, sizeofint);

  int lon1 = 0;
  int lat1 = 200;
  ofs.write((char*)&lon1, sizeofint);
  ofs.write((char*)&lat1, sizeofint);

  int lon2 = 200;
  int lat2 = 200;
  ofs.write((char*)&lon2, sizeofint);
  ofs.write((char*)&lat2, sizeofint);

  int lon3 = 200;
  int lat3 = 0;
  ofs.write((char*)&lon3, sizeofint);
  ofs.write((char*)&lat3, sizeofint);
*/
}
void clean(list<char*>* segment)
{
  list<char*>::node* n = segment->getHead()->next;
  while (n)
  {
    if (n->t)
    {
      delete n->t;
      n->t = 0;
    }
    n = n->next;
  }
  segment->removeAll();
}
void clean(list<list<char*>*>& lst)
{
  list<list<char*>*>::node* n = lst.getHead()->next;
  while (n)
  {
    if (n->t)
    {
      clean(n->t);
    }
    n = n->next;
  }
  lst.removeAll();
}
