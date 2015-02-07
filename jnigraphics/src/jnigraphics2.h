#ifndef __JNIGRAPHICS2_H__
#define __JNIGRAPHICS2_H__

#include <stdlib.h>
#include <stdarg.h>

#include <jni.h>
#include <jcalist.h>

#include <fvar.hpp>
#include <fvar.hpp>
#include <adstring.hpp>


class regiondef
{
public:
  regiondef()
  {
    dx      = 60;
    dy      = 60;
    title   = 0;
    cstfile = 0;
  }

public:
  void setMN(const int m, const int n) {  regiondef::m = m; regiondef::n = n; }
  int getM() const { return m; }
  int getN() const { return n; }

  void setGridMap(const imatrix& im) 
  { 
    int m = im.rowmax();
    int n = im.colmax();
    gridmap.allocate(1, m, 1, n);
    gridmap.initialize();

    for (int i = 1; i <= m; i++)
      for (int j = 1; j <= n; j++)
        gridmap(i, j) = im(i, j);
  }
public:
  int   dx, dy;
  char* title;
  char* cstfile;
  
  imatrix gridmap;
private:
  int   m,  n;
};

class advectionregiondef
{
public:
  regiondef region;

public:
  advectionregiondef(const int i, const int j)
    { region.setMN(i, j); }
};

class movementregiondef
{
public:
  regiondef region;

public:
  movementregiondef(const int i, const int j)
    { region.setMN(i, j); }

  void setGridMap(const imatrix& im) { region.setGridMap(im); }
};

#include "worms.h"
class wormregiondef
{
public:
  regiondef region;

public:
  wormregiondef(const int i, const int j)
    { region.setMN(i, j); }
};

class legenddef
{
public:
  static const int UNIFORM = 0;
  static const int EXPONENTIAL = 1;

public:
  legenddef()
  {
    min       = 0;
    max       = 1;
    increment = 0;
    decimals  = 0;
    type      = 0; //legenddef::UNIFORM;
  }

public:
  double min, max;
  double increment;
  int    decimals;
  int    type;
};

class squareregiondef
{
public:
  regiondef region;
  legenddef legend;

public:
  squareregiondef(const int i, const int j)
    { region.setMN(i, j); }
};

class triangleregiondef
{
public:
  regiondef region;
  legenddef legend;

public:
  triangleregiondef(const int i, const int j)
    { region.setMN(i, j); }
};

class lineplotdef
{
public:
  lineplotdef(const int n)
  {
    title       = 0;
    numlines    = n;
    colors      = 0;

    //x and y are variable
    yaxis       = 0;
    ymax        = 0;
    yincrement  = 0;
    ydecimals   = 0;

    xaxis       = 0;
    xmax        = 0;
    xincrement  = 0;
  }
public:
  char*   title;
  int     getNumLines() const { return numlines; }
  int*    colors;

  char*   yaxis;
  double  ymax;
  double  yincrement;
  int     ydecimals;

  char*   xaxis;
  int     xmax;
  int     xincrement;

  ivector linesColors;
private:
  int     numlines;

public:
  void setColor(const int r, const int g, const int b, const int line = 0)
  { 
    int rgb = r << 16 | g << 8 | b;
    setColor(rgb, line);
  }
  void setColor(const int rgb, const int line = 0)
  {
    if (numlines > 0 && 0 <= line && line < numlines)
    {
      if (!linesColors)
      { 
        linesColors.allocate(1, numlines); 
        linesColors.initialize();
      }

      linesColors(line + 1) = rgb;
    }
  }
};

#include "jnigraphics3.h"

class jnigraphics2 : public jnigraphics3
{
public:
  jnigraphics2():jnigraphics3() {}
  jnigraphics2(const char* classpath):jnigraphics3(classpath) {}
  virtual ~jnigraphics2() {} 

public:
  void addGridLayout(const int m, const int n, ivector& iv, const int id = -1)
   { jnigraphics3::addGridLayout(m, n, iv, id); }

  //use id from addTabs
  void addTabs(const int num, ivector& iv, const int id = -1)
   { jnigraphics3::addTabs(num, iv, id); }

  void setTabTitle(const adstring& title, const int id)
   { jnigraphics3::setTabTitle(title, id); }

  int addRegion(const regiondef def, const int id)
  {
    regiondefs defs;
    defs.m = def.getM();
    defs.n = def.getN();
    defs.dx = def.dx;
    defs.dy = def.dy;
    defs.title = def.title;
    defs.coastlinesFile = def.cstfile;

    if (!def.gridmap) 
    {}
    else
    {
      int m = def.gridmap.rowmax();
      int n = def.gridmap.colmax();
      defs.gridmap.allocate(1, m, 1, n);
      defs.gridmap.initialize();

      for (int i = 1; i <= m; i++)
        for (int j = 1; j <= n; j++)
          defs.gridmap(i, j) = def.gridmap(i, j);
    }

    return jnigraphics3::addRegion(defs, id);
  }

  int addLegend(const legenddef def, const int id)
  {
    legenddefs defs;
    defs.min = def.min;
    defs.max = def.max;
    defs.increment = def.increment;
    defs.decimals = def.decimals;
    defs.type = def.type;
    return jnigraphics3::addLegend(defs, id);
  }

  //returns id
  int addWormRegion(const wormregiondef def, const int id)
  {
    addRegion(def.region, id);
    return 0;
  }
  int addAdvectionRegion(const advectionregiondef def, const int id)
  {
    addRegion(def.region, id);
    return 0;
  }
  int addMovementRegion(const movementregiondef def, const int id)
  {
    addRegion(def.region, id);
    return 0;
  }
  int addTriangleRegion(const triangleregiondef def, const int id)
  {
    addRegion(def.region, id);
    addLegend(def.legend, id);
    return 0;
  }
  int addSquareRegion(const squareregiondef def, const int id)
  {
    addRegion(def.region, id);
    addLegend(def.legend, id);
    return 0;
  }

  int addLinePlot(const lineplotdef def, const int id)
  {
    lineplotdefs defs;
    defs.title = def.title; 
    defs.linesTotal = def.getNumLines();
    defs.yaxisTitle = def.yaxis;
    defs.yaxisMin = 0; 
    defs.yaxisMax = def.ymax;
    defs.yaxisIncrement = def.yincrement;
    defs.yaxisDecimals = def.ydecimals;

    defs.xaxisTitle = def.xaxis;
    defs.xaxisMax = def.xmax;
    defs.xaxisIncrement = def.xincrement;

    if (!def.linesColors)
      {}
    else
    {
      int min = def.linesColors.indexmin();
      int max = def.linesColors.indexmax();
      if (min != max && min < max)
      {
        defs.linesColors.allocate(min, max);
        defs.linesColors.initialize();

        for (int i = min; i <= max; i++)
          { defs.linesColors(i) = def.linesColors(i); }
      }
    }

    return jnigraphics3::addLineplot(defs, id);
  }

  void addStatusBar()
    { jnigraphics3::addStatusBar(); }
  void layoutAll(const int w = 0, const int h = 0)
    { jnigraphics3::layoutAll(w, h); }

  void drawValuesWormRegion(const worms& ws, const int id)
    { jnigraphics3::drawWorms(ws, id); }

  //dvx(i) dvy(i) is +/- length in minutes
  void drawArrowsAdvectionRegion(const dmatrix& dmx, const dmatrix& dmy,
                                 const int id)
    { jnigraphics3::drawArrows(dmx, dmy, 1, 1, id); }

  //movementregion
  //dvx(i) dvy(i) is +/- length in minutes
  void drawArrowsMovementRegion(const dvector& dvx, const dvector& dvy,
                                const int id)
  { 
    int x0 = dvx.indexmin();
    int x2 = dvx.indexmax();
    dvector dvx2(x0, x2);
    {
      for (int i = x0; i <= x2; i++)
        { dvx2(i) = dvx(i) * 60.0; }
    }

    int y0 = dvy.indexmin();
    int y2 = dvy.indexmax();
    dvector dvy2(dvy.indexmin(), dvy.indexmax());
    {
      for (int i = y0; i <= y2; i++)
        { dvy2(i) = dvy(i) * 60.0; }
    }

    jnigraphics3::drawArrows(dvx2, dvy2, 1, 1, id); 
  }

  void drawEllipsesMovementRegion(const dvector& dvx, const dvector& dvy, 
                                  const int id)
  { 
    int x0 = dvx.indexmin();
    int x2 = dvx.indexmax();
    dvector dvx2(x0, x2);
    {
      for (int i = x0; i <= x2; i++)
        { dvx2(i) = sqrt(dvx(i) / 3.14) * 60.0; }
    }

    int y0 = dvy.indexmin();
    int y2 = dvy.indexmax();
    dvector dvy2(dvy.indexmin(), dvy.indexmax());
    {
      for (int i = y0; i <= y2; i++)
        { dvy2(i) = sqrt(dvy(i) / 3.14) * 60.0; }
    }

    jnigraphics3::drawOvals(dvx2, dvy2, 1, 1, id); 
  }

  //dv(i) is radius in minutes
  void drawCirclesMovementRegion(const dvector& dv, const int id)
    { drawEllipsesMovementRegion(dv, dv, id); }

  //squareregion
  void drawSquareRegion(const dmatrix& dm, int id)
    { jnigraphics3::drawSquares(dm, id); }
  
  //triangleregion
  void drawTriangleRegion(const dmatrix& dmU, const dmatrix& dmL,
                          const int id)
    { jnigraphics3::drawTriangles(dmU, dmL, id); }

  //lineplot
  void drawLinePlot(const double d, const int id)
    { jnigraphics3::drawLineplot(d, id); }

  void drawLinePlot(const dvector& dv, const int id)
    { jnigraphics3::drawLineplots(dv, id); }

  void drawStatusBar(const adstring& ads)
    { jnigraphics3::drawStatusBar(ads); }

  void saveppm(const adstring& ads, const int w, const int h, const int id)
    {}
  void savejpeg(const adstring& ads, const int id)
    {}
  int paintAll(int milliseconds = 1000)
    { return jnigraphics3::paintAll(milliseconds); }
};
#endif //__jnigraphics2_h__
