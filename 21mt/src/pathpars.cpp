#include <fvar.hpp>
#include <iostream>
#include <stdlib.h>
#include "PathFileParser.h"
#include "trace.h"
void PathFileParser::setPath(const adstring& ads)
{
  adsPath = "";
  int size = ads.size();
  ASSERT((size >= 0));

  if (size == 0)
#ifdef unix
    adsPath = "./";
#else
    adsPath = ".\\";
#endif
  else 
  {
#ifdef unix
    if (ads(size) != '/')
      adsPath = ads + "/";
#else
    if (ads(size) != '\\') 
      adsPath = ads + "\\";
#endif
    else
      adsPath = ads;
  }
}//setPath

void PathFileParser::setFile(const adstring& ads)
{
  adsRoot = "";

  int size = ads.size(); 
  ASSERT((size > 0));

  int i = size;
  while(i > 0)
#ifdef unix
    if (ads(i--) == '/')
#else
    if (ads(i--) == '\\')
#endif
    {
      cerr << "Error: PathFileParser::setFile(" << ads << ") "
           << "directory not allowed\n"
           << "       line " << __LINE__ << " file " << __FILE__ << "\n";
      return;
    }

  i = size;
  while(i > 0)
    if (ads(i--) == '.') break;

  if (i == 0)
    adsRoot = ads;
  else
  {
    for (int j = 1; j <= i; j++)
      adsRoot += ads(j);
  }
}//setFile

void PathFileParser::separatePathFile(const adstring& ads)
{
  adstring adsPath;
  adstring adsFile;
  int size = ads.size();
  int i = size;
  while (i > 0)
  {
#ifdef unix
    if (ads(i) == '/') break; 
#else
    if (ads(i) == '\\') break; 
#endif
    i--;
  }

  int j;
  for (j = 1; j <= i; j++)
    adsPath += ads(j);
  for (j = i + 1; j <= size; j++)
    adsFile += ads(j);

  setPath(adsPath);
  setFile(adsFile);
}//separatePathFile

/*
int main()
{
  cout << "hello" << endl;
  adstring ads;
  PathFileParser p;
  cout << "Start p()" << endl;
  cout << " " << ads << "  -> " << p.getRoot() << endl;
  cout << " " << ads << "  -> " << p.getPath() << endl;
  ads = "one";
  p.setPath(ads);
  ads = "/one";
  cout << " " << ads << "  -> " << p.getPath() << endl;
  ads = "one/";
  p.setPath(ads);
  cout << " " << ads << "  -> " << p.getPath() << endl;
  ads = "/one/";
  p.setPath(ads);
  cout << " " << ads << "  -> " << p.getPath() << endl;
  ads = "/one/two";
  p.setPath(ads);
  cout << " " << ads << "  -> " << p.getPath() << endl;
  ads = "one/two/";
  p.setPath(ads);
  cout << " " << ads << "  -> " << p.getPath() << endl;
  ads = "/one/two/";
  p.setPath(ads);
  cout << " " << ads << "  -> " << p.getPath() << endl;
  ads = "filename.rtf";
  p.setFile(ads);
  cout << " " << ads << "  -> " << p.getRoot() << endl;
  ads = "fil.ename.rtf";
  p.setFile(ads);
  cout << " " << ads << "  -> " << p.getRoot() << endl;
  ads = "/filename.rtf";
  p.setFile(ads);
  cout << " " << ads << "  -> " << p.getRoot() << endl;
  ads = "/filename.rtf";
  p.setPathFile(ads);
  cout << " " << ads << "  P-> " << p.getPath() << endl;
  cout << " " << ads << "  R-> " << p.getRoot() << endl;
  ads = "one/fil.ename..rtf";
  p.setPathFile(ads);
  cout << " " << ads << "  P-> " << p.getPath() << endl;
  cout << " " << ads << "  R-> " << p.getRoot() << endl;
  ads = "/tto/on e /filename rtf";
  p.setPathFile(ads);
  cout << " " << ads << "  P-> " << p.getPath() << endl;
  cout << " " << ads << "  R-> " << p.getRoot() << endl;
  cout << "p()" << endl;
}//main
*/
