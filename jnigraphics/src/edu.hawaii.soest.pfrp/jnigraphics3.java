/*
 */

import java.awt.*;

public class jnigraphics3 extends Container
{
private int uniqueid = 0;
//private java.util.Vector vec = new java.util.Vector(0, 0);
private java.util.Vector<java.lang.Object> vec = new java.util.Vector<java.lang.Object>();
private Thread[] ta = null;

public jnigraphics3()
{
  addComponentListener(new java.awt.event.ComponentAdapter()
    {
      public void componentResized(java.awt.event.ComponentEvent ce)
      {
        //System.out.println(getSize());
        if (vec == null) return;

        if (ta != null)
        {
          int size = ta.length;
          for (int i = 0; i < size; i++)
            ta[i].interrupt();

          for (int i = 0; i < size; i++)
          {
            try
              { ta[i].join(); }
            catch (InterruptedException ie)
              { }
          }
          ta = null;
        }

        paintAll();
      }
    });
}

public int[] addGridLayout(int m, int n, int id)
{
  int mn = m * n;
  glc myglc = new glc(m, n);

  myglc.id = id < 0 ? uniqueid++ : id;

  myglc.cid = new int[mn];
  for (int i = 0; i < mn; i++)
    myglc.cid[i] = uniqueid++;

  vec.addElement(myglc);

  return myglc.cid;
}

public int[] addTabs(int num, int id)
  { return addTabs(num, null, id); }

private tabs theTabs = null;

public int[] addTabs(int num, String[] sa, int id)
{
  tabs mytabs = new tabs(num);

  mytabs.id = id < 0 ? uniqueid++ : id;

  mytabs.cid = new int[num];
  for (int i = 0; i < num; i++)
    mytabs.cid[i] = uniqueid++;

  mytabs.setTitles(sa);
  vec.add(mytabs);

theTabs = mytabs;

  return mytabs.cid;
}

public void setTabTitle(String s, int cid)
{
  int size = vec.size();
  int index = 0;
  for (index = 0; index < size; index++)
  {
    Object obj = vec.get(index);
    if (obj instanceof tabs)
    {
      tabs mytabs = (tabs)obj;
      int num = mytabs.cid.length;
      for (int i = 0; i < num; i++)
      {
        if (mytabs.cid[i] == cid)
          { mytabs.setTitle(i, s); }
      }
    }
  }
}

public region add(int id)
{
  region r = getRegion(id);
  if (r == null)
  {
    r = new region();
    r.id = id;
    vec.add(r);
  }
  return r;
}

public void addRegion(regiondef def, int id)
{
  region r = add(id);
  r.addRegion(def);
}

public void addLegend(legenddef def, int id)
{
  region r = add(id);
  r.addLegend(def);
}

public void addLineplot(lineplotdef def, int id)
{
  region r = add(id);
  r.addLineplot(def);
}

public void addHistogram(histogramdef def, int id)
{
  region r = add(id);
  r.addHistogram(def);
}

private int build(int id)
{
  int size = vec.size();
  int index = 0;
  for (index = 0; index < size; index++)
  {
    Object obj = vec.get(index);
    if (obj instanceof tabs)
    {
      tabs mytabs = (tabs)obj;
      if (mytabs.id == id)
      {
        int num = mytabs.numtabs; 
        Container[] ca = new Container[num]; 
        for (int t = 0; t < num; t++) 
        {
          int tindex = build(mytabs.cid[t]);
          if (tindex >= size)
          {
            ca[t] = new glc(1, 1);
            ca[t].add(new javax.swing.JButton("TAB" + t));
          }
          else
          {
            Object o = vec.get(tindex);
            if (!(o instanceof Container))
            {
              System.out.println("ERROR: need to setGridLayout to tab " + t
                + " before adding objects");
              System.exit(1);
            }
            ca[t] = (Container)o;
          }
        }

        mytabs.addContainers(ca);
        break;
      }
    }//tabs
    else if (obj instanceof glc)
    {
      glc myglc = (glc)obj;
      if (myglc.id == id)
      {
        GridLayout gl = (GridLayout)myglc.getLayout();
        int mn = gl.getColumns() * gl.getRows();
        for (int g = 0; g < mn; g++)
        {
          int gindex = build(myglc.cid[g]);
          if (gindex >= size)
          {
            myglc.add(new javax.swing.JButton("GRID" + g), g);
          }
          else
          {
            Object o = vec.get(gindex); 
            assert o instanceof Component;
            myglc.add((Component)o, g); 
          }
        } 
        break;
      }
    }//glc
    else if (obj instanceof region)
    {
      region r = (region)obj;
      if (r.id == id)
        { break; }
    }//dbc
  }//forindex

  return index;
}

public void layoutAll()
  { layoutAll(0, 0); }

private statusbar sb = null;

public void addStatusBar()
  { sb = new statusbar(); }

public void layoutAll(int w, int h)
{
  assert vec != null;

  if (vec.size() > 0)
  {
    setLayout(new BorderLayout());

    int index = build(0);
    assert index == 0;
    add((Component)vec.get(index), "Center");

    //remove all containers not needed anymore
    while (index < vec.size())
    {
      if (vec.get(index) instanceof dbc) 
        index++;
      else
        vec.removeElementAt(index);
    }
  }

  if (getParent() == null)
  {
    Dimension d = Toolkit.getDefaultToolkit().getScreenSize();
    myframe frame = new myframe(getClass().getName());
    if (w > 0 && h > 0)
      frame.setSize(w, h);
    else
      frame.setSize(d.width * 3 / 4, d.height * 3 / 4);
    frame.add(this, "Center");

    if (sb != null)
      frame.add(sb, "South");

    frame.invalidate();
    frame.validate();
    frame.setVisible(true);
  }
  else
  {
  }
}

private region getRegion(int id)
{
  int size = vec.size();
  for (int i = 0; i < size; i++)
  {
    Object obj = vec.get(i);
    if (obj instanceof region)
    {
      region r = (region)obj;
      if (r.id == id)
        { return r; }
    }
  }

  return null;
}

public void drawStatusBar(String s)
{ 
  if (sb != null)
    sb.setString(s); 
}

public int save_png(String filename, int w, int h, int id)
{
  Dimension d = Toolkit.getDefaultToolkit().getScreenSize();
  if (d == null) System.out.println("Error: save_png_aa");

  int w2 = w <= 0 ? d.width * 3 / 4 : w;
  int h2 = h <= 0 ? d.height * 3 / 4 : h;

  if (filename != null && w2 > 0 && h2 > 0)
  {
    region r = getRegion(id);
    if (r != null)
    {
      java.awt.image.BufferedImage img = new java.awt.image.BufferedImage(w2, h2, java.awt.image.BufferedImage.TYPE_INT_RGB);
      int height = r.drawAll(img);

      if (height > 0)
      {
        img = img.getSubimage(0, 0, w2, height);
      }
  
      Graphics2D g2 = img.createGraphics();
      FontMetrics fm = g2.getFontMetrics();
      int sw = fm.stringWidth(filename);
      int ascent = fm.getAscent(); 
      g2.setColor(Color.black);
      g2.drawString(filename, img.getWidth() - sw - 4, img.getHeight() - ascent);
      g2.dispose();

      try
      {
System.out.println("filename " + filename);
        java.io.FileOutputStream outfile = new java.io.FileOutputStream(filename);
        java.awt.image.RenderedImage rimg = (java.awt.image.RenderedImage)img;
        com.sun.media.jai.codec.ImageEncoder enc = 
          com.sun.media.jai.codec.ImageCodec.createImageEncoder("png", 
                                                                outfile, 
                 com.sun.media.jai.codec.PNGEncodeParam.getDefaultEncodeParam(rimg));

        try { enc.encode(img); } catch (java.io.IOException ie)
                                   { System.out.println("Error: save_png_a"); }
        try { outfile.close(); } catch (java.io.IOException ie)
                                   { System.out.println("Error: save_png_b"); }
      } catch (java.io.FileNotFoundException ie) 
          { System.out.println("Error: save_png_c"); }

      img = null;
    }
  }

  return 0;
}

public int paintAll(int time)
{
  if (time > 0)
  {
    try
      { Thread.sleep(time); }
    catch (InterruptedException ie)
      { return 1; }
  }

  return paintAll();
}

public synchronized int paintAll()
{
  if (theTabs != null) {
	theTabs.paintTabs();
  }

  if (sb != null)
  {
    sb.drawStatusBar();
    sb.switchBuffers();
    sb.repaint();
  }

  int size = vec == null ? 0 : vec.size();
  if (size <= 0) return 1; //nothing to paint

  Thread[] ta = new Thread[size];
  
  for (int i = 0; i < size; i++)
  {
    assert vec.get(i) instanceof Runnable;

    ta[i] = new Thread((Runnable)vec.get(i));
    ta[i].setPriority(Thread.MAX_PRIORITY);
    try
    {
      ta[i].start();
    }
    catch (IllegalThreadStateException ie)
    {
      //ie.printStack();
      System.exit(1);
    }
  }//fori 

  for (int i = 0; i < size; i++)
  {
    try
      { ta[i].join(); }
    catch (InterruptedException ie)
    {
      System.out.println("THREADINTERURUPED " + i);
      return 1;
    }
  }
  ta = null;

  for (int i = 0; i < size; i++)
  {
    Object o = vec.get(i);
    assert o instanceof dbc;

    dbc mydbc = (dbc)o;
    if (mydbc.isShowing())
    {
      mydbc.switchBuffers();
      mydbc.repaint();
    }
  }

//System.out.println("Total Memory: "+Runtime.getRuntime().totalMemory() / 1024);
//System.out.println("Free Memory: "+Runtime.getRuntime().freeMemory() / 1024);

  if (isShowing())
    return 1; 
  else
    return 0; 
}

public static void main(String[] sa)
{
  java.util.Random r = new java.util.Random();

  jnigraphics3 jni = new jnigraphics3();
  int ia12[] = jni.addGridLayout(1, 2, -1);
  int region0 = ia12[0];

//jni.layoutAll();
//jni.paintAll(2000);


  regiondef rdef = new regiondef();
  rdef.title = "region";
  rdef.coastlinesFile = "yft16.cst";
  rdef.m = r.nextInt(50) + 1;
  rdef.n = r.nextInt(50) + 1;
  rdef.dx = 60;
  rdef.dy = 60;

  legenddef ldef = new legenddef();
  ldef.min = 0;
  ldef.max = 100;
  ldef.increment = 10;
  ldef.type = legenddef.UNIFORM;

  lineplotdef pdef = new lineplotdef();
  pdef.linesTotal = 5;
  pdef.linesColors = new Color[5];
  pdef.linesColors[0] = Color.pink;
  pdef.linesColors[1] = Color.blue;
  pdef.linesColors[2] = Color.green;
  pdef.linesColors[3] = Color.black;
  pdef.linesColors[4] = Color.red;
    
  pdef.title = "title";
  pdef.xaxisTitle = "xtitle";
  pdef.xaxisIncrement = 10;
  pdef.yaxisTitle = "ytitle";
  pdef.yaxisIncrement = 100;

  histogramdef hdef = new histogramdef();
  hdef.title = "title";
  hdef.yaxisTitle = "ytitle";
  hdef.yaxisIncrement = 100;
  hdef.barTitle = "barTitle";
  hdef.barTotal = 5;
  
  jni.addRegion(rdef, region0);
  jni.addLegend(ldef, region0);
  jni.addHistogram(hdef, region0);
  //jni.addLineplot(pdef, region0);
  jni.layoutAll();

  int i = 0;
  while (true)
  {
    region myregion = jni.getRegion(region0);

    double iaSquares[] = new double[rdef.m * rdef.n];
    for (int j = 0; j < rdef.m * rdef.n; j++)
    {
      int value = r.nextInt(75);
      iaSquares[j] = value;
    }
    myregion.setSquares(iaSquares);

    double iaUpper[] = new double[rdef.m * rdef.n];
    double iaLower[] = new double[rdef.m * rdef.n];
    for (int j = 0; j < rdef.m * rdef.n; j++)
    {
      int value = r.nextInt(75);
      iaUpper[j] = value;
      value = r.nextInt(75);
      iaLower[j] = value;
    }

    myregion.setTriangles(iaUpper, iaLower);

    int sizeEllipses = r.nextInt(20) * 6;
    int iaEllipses[] = new int[sizeEllipses];
    for (int c = 0; c < sizeEllipses; c += 6)
    {
      Color cc = new Color(r.nextInt(255), r.nextInt(255), r.nextInt(255));
      iaEllipses[c] = cc.getRGB();

      iaEllipses[c + 1] = 1;

      iaEllipses[c + 2] = r.nextInt(rdef.m * rdef.dx);
      iaEllipses[c + 3] = r.nextInt(rdef.n * rdef.dy);
      iaEllipses[c + 4] = r.nextInt(rdef.m * rdef.dx);
      iaEllipses[c + 5] = r.nextInt(rdef.n * rdef.dy);
    }
    myregion.setOvals(iaEllipses);
    myregion.setArrows(iaEllipses);

    //java.util.Vector vec = new java.util.Vector();
	java.util.Vector<java.lang.Object> vec = new java.util.Vector<java.lang.Object>();
    int total = r.nextInt(10);
    for (int p = 0; p < total; p++)
    {
      Color cc = new Color(r.nextInt(255), r.nextInt(255), r.nextInt(255));
      int rgb = cc.getRGB();
      vec.add(new Integer(rgb)); 

      int total2 = r.nextInt(10) + 1;
      vec.add(new Integer(total2)); 
      for (int t = 0; t <= total2; t++)
      {
        vec.add(new Integer(r.nextInt(rdef.m * rdef.dx)));
        vec.add(new Integer(r.nextInt(rdef.m * rdef.dx)));
      }
    }

    int iaWorms[] = new int[vec.size()];
    for (int ee = 0; ee < vec.size(); ee++)
    {
      Integer myint = (Integer)vec.get(ee);
      int value = myint.intValue();
      //System.out.print(" " + value);
      iaWorms[ee] = value; 
    }
    myregion.setWorms(iaWorms);
    
    double daLineplots[] = new double[pdef.linesTotal];
    for (int lines = 0; lines < pdef.linesTotal; lines++)
    {
      daLineplots[lines] = r.nextInt(9020) + r.nextDouble();
      if (r.nextInt(10) > 5)
        daLineplots[lines] *= -1;
    }
    //myregion.addLineplots(daLineplots);

    double daLineplots2[] = new double[pdef.linesTotal];
    for (int lines = 0; lines < pdef.linesTotal; lines++)
    {
      daLineplots2[lines] = r.nextInt(9020) + r.nextDouble();
      if (r.nextInt(10) > 5)
        daLineplots2[lines] *= -1;
    }
    myregion.setHistograms(daLineplots2, 0);

    jni.drawStatusBar(String.valueOf(i++));
    jni.paintAll(2000);
  }
}
}//class jnigraphics3

class glc extends java.awt.Container
{
public glc()
{
  //leave null
  //setGridLayout(1, 1);
}

public glc(int m, int n)
{
  setGridLayout(m, n);
}

public void setGridLayout(int m, int n)
{
  setLayout(new java.awt.GridLayout(m, n));
}

public int id = -1; 
public int[] cid = null; 
}//class glc
