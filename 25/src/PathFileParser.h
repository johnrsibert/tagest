//$Id: PathFileParser.h 2754 2011-01-02 20:57:07Z jsibert $
#ifndef __PathFileParser_h__
#define __PathFileParser_h__

#include <fvar.hpp>
#include <adstring.hpp>

//Started: 10.13.98
//Done: 10.13.98
//Note:
//  - Assumes path input contains no appended filename 
//  - Assumes file input contains no begining dir
//  - Assumes that all adstring input are in form of adstring ads(1, size)
//  - getPath() will return path with ending win "\" or unix "/"
//  - getRoot() will return root without rightmost .<ext> if it exist 
//Warnings:
//  - getPath(), getRoot(), getPathRoot() can return null;
//  - adsPath and adsRoot are shallow copies

class PathFileParser
{
public://constructors destructors
  PathFileParser() {}
  PathFileParser(const adstring& adsPathFile)
    { setPathFile(adsPathFile); }
  PathFileParser(const adstring& adsPath, const adstring& adsFile)
    { setPath(adsPath); setFile(adsFile); }
  PathFileParser(const PathFileParser& t); //{ adsPath(t.adsPath); 
                                           // adsRoot(t.adsRoot); } 
  ~PathFileParser() {}

public://methods
  void setPath(const adstring& ads);
  void setFile(const adstring& ads);
  void setPathFile(const adstring& adsPathFile)
    { separatePathFile(adsPathFile); }
  void setPathFile(const adstring& adsPath, const adstring& adsFile)
    { setPath(adsPath); setFile(adsFile); }
  adstring getRoot() 
    { return adsRoot; }
  adstring getPath() 
    { return adsPath; }
  adstring getPathRoot() 
    { return (adsPath+adsRoot); }

private://method
  //separatePathFile separates path and file then sets them
  void separatePathFile(const adstring& ads);

private://members
  adstring adsPath;
  adstring adsRoot;
};//class PathFileParser

#endif
