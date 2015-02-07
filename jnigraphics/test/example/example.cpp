#include <iostream>
#include <fvar.hpp>
#include <stdlib.h>
#include "jnigraphics3.h"

//#ifdef unix
int random(const int i)
  { return 3; }//random() % i; }
//#endif

int main()
{
  //jnigraphics3 jni;
  //jnigraphics3 jni("../g++");
  jnigraphics3 jni;

  int* grid53 = jni.addGridLayout(1, 2);
  int* tabs5 = jni.addTabs(5, grid53[1]);
  jni.setTabTitle("test", tabs5[3]);


  jni.addStatusBar();
  //TEST: tabs and gridlayouts
/*
  jni.layoutAll();
  while (1)
  	jni.paintAll(4000);
*/

  lineplotdefs pdef;
  pdef.linesTotal = 5;
  pdef.yaxisTitle = "yaxis";
  pdef.yaxisIncrement = 10;
  pdef.xaxisTitle = "xaxis";
  pdef.xaxisIncrement = 1;
  pdef.setColor(0, 0, 255, 0);
  jni.addLineplot(pdef, grid53[0]);

  regiondefs rdef;
  rdef.title = "region";
  rdef.m = 145;
  rdef.n = 55;
  rdef.dx = 60;
  rdef.dy = 60;
  //rdef.coastlinesFile = "..\\a3.cst";
  rdef.coastlinesFile = "atlantic_c.cst";
  jni.addRegion(rdef, grid53[0]);

  legenddefs ldef;
  ldef.min = 1;
  ldef.max = 100;
  ldef.type = legenddefs::UNIFORM;
  //ldef.type = legenddefs::EXPONENTIAL;
  jni.addLegend(ldef, grid53[0]);

  histogramdefs hdef;
  hdef.title = "histograms";
  hdef.yaxisTitle = "yaxisTitle";
  hdef.yaxisMax = 3000;
  hdef.yaxisMin = -3000;
  hdef.yaxisIncrement = 100;
  hdef.barSpacing = 4;
  hdef.barWidth = 10;
  hdef.barTitle = "barTitle";
  hdef.barTotal = 5;
  hdef.barStack = 3;
//  hdef.setColor(200, 0, 0, 0);
  hdef.setColor(0, 200, 0, 1);
//  hdef.setColor(0, 0, 200, 2);
//  hdef.setLabel("Label2", 2);
  jni.addHistogram(hdef, grid53[0]);

  jni.layoutAll();

  dvector dv(1, pdef.linesTotal);
  dvector dv2(1, hdef.barTotal);
  dvector dv3(1, hdef.barTotal);
  dvector dv4(1, hdef.barTotal);
  dmatrix dmX(1, rdef.m, 1, rdef.n);
  dmatrix dmY(1, rdef.m, 1, rdef.n);

  
  worms ws(0, 0);
  ws.addWorm(5,5);
  ws.addWorm(5,5);
  ws.addWorm(5,5);

  int idx = 0;
  while (1)//jni.paintAll(2000))
  { 
    if (idx % 2 == 0)
      { jni.drawStatusBar("test"); }
    else
      { jni.drawStatusBar("    TEST"); }

    dv.initialize();
    for (int i = 1; i <= pdef.linesTotal; i++)
    {
      if (random(10) > 5)
        dv(i) = -1 * random(1000);
      else
        dv(i) = random(1000);
    }

    dv2.initialize();
    for (int i = 1; i <= hdef.barTotal; i++)
    {
      if (random(10) > 5)
        dv2(i) = -1 * random(1000);
      else
        dv2(i) = random(1000);
    }
    jni.drawHistogram(dv2, grid53[0]);

    dv3.initialize();
    for (int i = 1; i <= hdef.barTotal; i++)
    {
      if (random(10) > 5)
        dv3(i) = -1 * random(1000);
      else
        dv3(i) = random(1000);
    }
    jni.drawHistogram(dv3, 1, grid53[0]);
    dv4.initialize();
    for (int i = 1; i <= hdef.barTotal; i++)
    {
      if (random(10) > 5)
        dv4(i) = -1 * random(1000);
      else
        dv4(i) = random(1000);
    }
    jni.drawHistogram(dv4, 2, grid53[0]);

    dmX.initialize();
    dmY.initialize();
    for (int i = 1; i <= rdef.m; i++)
      for (int j = 1; j <= rdef.n; j++)
      {
        if (random(10) > 5)
          dmX(i, j) = -1 * random(3);
        else
          dmX(i, j) = random(3);

        if (random(10) > 5)
          dmY(i, j) = -1 * random(3);
        else
          dmY(i, j) = random(3);
      }

    jni.drawSquares(dmX, grid53[0]);
    jni.drawTriangles(dmX, dmY, grid53[0]);
    jni.drawOvals(dmX, dmY, 1, 1, grid53[0]);
    //jni.drawArrows(dmX, dmY, 20, 20, grid53[0]);
    jni.drawLineplots(dv, grid53[0]);


    int x = random(rdef.m);
    int y = random(rdef.n);
    int w = random(3);
    //std::cout << "xyw " << x * rdef.dx << ' ' << y * rdef.dy << ' ' << w << std::endl;
    ws.add(x, y, w);
    //std::cout << ws << std::endl;
    //ws.printSizes(std::cout);
    //std::cout << std::endl;
    //ws.printHeads(std::cout);
    //std::cout << std::endl;
    //jni.drawWorms(ws, grid53[0]);

    jni.save_png("test.png", 1600, 1400, grid53[0]);

  
    jni.paintAll(2000);
    if (idx == 5) exit(1);
    idx++;
  }

  return 0;
}
