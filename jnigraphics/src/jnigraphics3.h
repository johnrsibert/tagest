#ifndef __JNIGRAPHICS3_H__
#define __JNIGRAPHICS3_H__

#include <stdlib.h>
#include <stdarg.h>

#include <jni.h>
#include <jcalist.h>

#include <fvar.hpp>
#include <adstring.hpp>
#include "worms.h"

class regiondefs
{
public:
  regiondefs() 
  {
    m = n = 0;
    dx = dy = 0;
    title = 0;
    coastlinesFile = 0;
  }
  regiondefs(const regiondefs&)
    { std::cerr << __FILE__ << ':' << __LINE__ << '\n'; exit(1); }
  ~regiondefs() {}

public:
  int m, n;
  int dx, dy;

  char* title;
  //int   titleColor;
  //char* titleFont;

  char* coastlinesFile;
  int   coastlinesColorLand;
  int   coastlinesColorEdge;

  imatrix gridmap;
};

class legenddefs
{
public:
  legenddefs() 
  {
    min = 0;
    max = 0;
    increment = 0;
    decimals = 0;
    type = legenddefs::NONE;
  }
  legenddefs(const legenddefs&)
    { std::cerr << __FILE__ << ':' << __LINE__ << '\n'; exit(1); }
  ~legenddefs() {/*DoNothing*/}
public:
  double min;
  double max;
  double increment;
  double decimals;
  int type;

  static const int NONE = -1;
  static const int UNIFORM = 0;
  static const int EXPONENTIAL = 1;
};

class lineplotdefs
{
public:
  lineplotdefs() 
  {
    title = 0;
    linesTotal = 0;

    yaxisTitle = 0;
    yaxisMin = 0;
    yaxisMax = 0;
    yaxisIncrement = 0;
    yaxisDecimals = 0;

    xaxisTitle = 0;
    xaxisMax = 0;
  }
  lineplotdefs(const lineplotdefs&)
    { std::cerr << __FILE__ << ':' << __LINE__ << '\n'; exit(1); }
  ~lineplotdefs() {}
public:
  char* title;

  int     linesTotal;
  ivector linesColors;

  char*  yaxisTitle;
  double yaxisMin;
  double yaxisMax;
  double yaxisIncrement;
  int    yaxisDecimals;

  char* xaxisTitle;
  //int xaxisMin; //will be always zero
  int xaxisMax;
  int xaxisIncrement;

public:
  void setColor(const int r, const int g, const int b, const int line = 0)
  { 
    int rgb = r << 16 | g << 8 | b;
    setColor(rgb, line);
  }
  void setColor(const int rgb, const int line = 0)
  {
    if (linesTotal > 0 && 0 <= line && line < linesTotal)
    {
      if (!linesColors)
      { 
        linesColors.allocate(1, linesTotal); 
        linesColors.initialize();
      }

      linesColors(line + 1) = rgb;
    }
  }
};

class histogramdefs
{
public:
  histogramdefs()
  {
    title = 0;

    yaxisTitle = 0;
    yaxisMin = 0;
    yaxisMax = 0;
    yaxisIncrement = 0;
    yaxisDecimals = 0;

    barTitle = 0;
    barTotal = 0;
    barStack = 0;
    barSpacing = 0;
    barWidth = 0;
    barLabels = 0;
  }
  histogramdefs(const histogramdefs&)
    { std::cerr << __FILE__ << ':' << __LINE__ << '\n'; exit(1); }
  ~histogramdefs() { /*DoNothing*/ }
public:
  char* title;

  char*  yaxisTitle;
  double yaxisMin;
  double yaxisMax;
  double yaxisIncrement;
  int    yaxisDecimals;

  char*   barTitle;
  int     barTotal;
  int     barStack;
  ivector barColors;
  int     barSpacing;
  int     barWidth;
  char**  barLabels;

public:
  void setColor(const int r, const int g, const int b, const int stack = 0)
  { 
    int rgb = r << 16 | g << 8 | b;
    setColor(rgb, stack);
  }
  void setColor(const int rgb, const int stack = 0)
  {
    if (barTotal > 0 && 0 <= stack && stack < barStack)
    {
      if (!barColors)
      { 
        barColors.allocate(1, barTotal); 
        barColors.initialize();
      }

      barColors(stack + 1) = rgb;
    }
  }
  void setLabel(const char* ca, const int bar)
  {
    if (barTotal > 0 && 0 <= bar && bar < barTotal)
    {
      if (barLabels == 0)
      {
        barLabels = new char*[barTotal];
        for (int i = 0; i < barTotal; i++)
          { barLabels[i] = 0; }
      }
    
      int size = strlen(ca);
      char* ca2 = new char[size];
      strcpy(ca2, ca);
      barLabels[bar] = ca2;
    }
  }
};

class jnigraphics3
{
public:
  jnigraphics3(): northwest_origin_inputs(false)
  { 
    char* classpath = getenv("JNIGRAPHICS_PATH");
    if (classpath)
    {
      invoke(classpath);
    }
    else
    {
      jnivm = 0;
      jnienv = 0; 
    }
  }
  jnigraphics3(const char* classpath) { invoke(classpath); }
  ~jnigraphics3() { unvoke(); }

public: //setup
  void set_northwest_origin_inputs(const bool b) { northwest_origin_inputs = b; }
  void addGridLayout(const int m, const int n, ivector& iv, const int id = -1);
  void addTabs(const int num, ivector& iv, const int id = -1);
  void setTabTitle(const char* title, const int id);

  int add(const int id);
  int addRegion(const regiondefs& region, const int id);
  int addLegend(const legenddefs& legend, const int id);
  int addLineplot(const lineplotdefs& lineplot, const int id);
  int addHistogram(const histogramdefs& histogramdefs, const int id);
  int addStatusBar();
  int layoutAll(const int w = 0, const int h = 0);
public: //add data
  int drawArrows(const dvector& dvX, const dvector& dvY, 
                 const double scaleX, const double scaleY, 
                 const int id);

  int drawArrows(const dmatrix& dmX, const dmatrix& dmY, 
                 const double scaleX, const double scaleY, 
                 const int id);

  int drawOvals(const dvector& dvX, const dvector& dvY, 
                const double scaleX, const double scaleY, 
                const int id);

  int drawOvals(const dmatrix& dmX, const dmatrix& dmY, 
                const double scaleX, const double scaleY, 
                const int id);

  int drawTriangles(const dmatrix& dmU, const dmatrix& dmL, const int id);

  int drawSquares(const dmatrix& dm, const int id);

  int drawWorms(const worms& ws, const int id);

  int drawLineplot(const double d, const int id);
  int drawLineplots(const dvector& dv, const int id);

  int drawHistogram(const dvector& dv, const int id)
    { return drawHistogram(dv, 0, id); }
  int drawHistogram(const dvector& dv, const int stack, const int id);

  int drawStatusBar(const char* statusbar);

  int save_png(const char* filename, const int id)
    { return save_png(filename, 0, 0, id); }
  int save_png(const char* filename, const int w, const int h, const int id);

  int paintAll(const int milliseconds = 1000);

public://for gmt2cst only
  int drawArrows(const int* ia, const int size, const int row,
                 const int xmins, const int ymins, const int id);
  int drawCst(const char* filename, 
              const int xmins, const int ymins, const int id);
  void test();
public:
  static JavaVM* jnivm;
  static JNIEnv* jnienv;

  JavaVMOption jvmoptions[4];

  jclass  jnigraphics3_cls;
  jobject jnigraphics3_obj;

public:
  int* addTabs(const int num, const int id);
  int* addGridLayout(const int rows, const int cols, const int id = -1);

protected:
  void invoke(const char* classpath);
  void unvoke();

  jobject newRegiondef(const regiondefs& region);
  jobject newLegenddef(const legenddefs& legend);
  jobject newLineplotdef(const lineplotdefs& lineplot);
  jobject newHistogramdef(const histogramdefs& histogram);

  jdoubleArray dvector2jdoubleArray(const dvector& dv) const;
  jdoubleArray dmatrix2jdoubleArray(const dmatrix& dm, 
                                    const int m, const int n, 
                                    const double min = 0) const;
  jintArray ivector2jintArray(const ivector& iv) const;

  int drawStatusBar(jstring js);
  bool check_exceptions();

  bool northwest_origin_inputs;
};
#endif //__JNIGRAPHICS3_H__
