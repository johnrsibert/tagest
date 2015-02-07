/* region.javaG
 * Started: 1999.06.04
 *
 * Usage:
 * Note:
 */

import java.awt.*;
import java.awt.image.*;
import java.util.Vector;

public class region extends dbc implements Runnable
{
private static boolean loadLibrary = false;
static 
{ 
  try
  { 
    Runtime.getRuntime().loadLibrary("coastlines"); 
    loadLibrary = true;
  }
  catch (Error e)
  {
    e.printStackTrace();
    System.out.println("Warning: unable to load coastlines");
    loadLibrary = false;
  }
}

public region() { /*DoNothing*/ } 
public region(defs ds)
{
  addRegion(ds.region);
  addLegend(ds.legend);
  addLineplot(ds.lineplot);
  addHistogram(ds.histogram);
}

public defs alldefs = new defs();
public void addRegion(regiondef def)
  { alldefs.region = def != null ? new regiondef(def) : null; }

public void addLegend(legenddef def)
  { alldefs.legend = def != null ? new legenddef(def) : null; }

public void addLineplot(lineplotdef def)
{
  alldefs.lineplot = def != null ? new lineplotdef(def) : null;

  if (alldefs.lineplot != null && alldefs.lineplot.linesTotal > 0) 
  {
    lineplots = new Vector[alldefs.lineplot.linesTotal];
    for (int i = 0; i < alldefs.lineplot.linesTotal; i++)
      { lineplots[i] = new Vector(); }
  }
}

public void addHistogram(histogramdef def)
{ 
  alldefs.histogram = def != null ? new histogramdef(def) : null; 
  int size = alldefs.histogram.barTotal * alldefs.histogram.barStack; 
  if (size > 0)
    { histograms = new double[size]; }
}

public void run() 
{ 
	allocateBufferedImages();

	int w = getBounds().width;
	int h = getBounds().height;
	if (w > 0 && h > 0) {
		allocate(imgs, w, h);
		if (imgs[1] != null && sw != null && pix != null) {
			drawLayer1(imgs[1], sw, pix, scalesLegend, colorsGradiants);
		}
	}

	drawAll(); 
	drawLayers(); 
}


/*
 * Format:
 *   0: title, box [legend]
 *   0: user drawings
 *   2: title box [legend] areas coastlines 
 */
private int imgsSize = 3; //should be const
private BufferedImage imgs[] = new BufferedImage[imgsSize]; //container

public void drawLayers()
{
  if (imgs != null)
  {
    clear();

    for (int i = 0; i < imgsSize; i++)
    { 
      if (imgs[i] != null)
      {
        paint(imgs[i]);
        imgs[i].flush();
      }
    }
  }
}

private xy sw = null;
private xy pix = null;

Color backgroundColor = new Color(204, 204, 204, 5);
private void allocate(BufferedImage imgs[], int w, int h)
{
	if (!isShowing()) {
		for (int i = 0; i < imgsSize; i++) {
			if (imgs[i] != null) {
				imgs[i].flush();
				imgs[i] = null;
			}
		}

		return;
	}

	if (alldefs.region != null && alldefs.region.m > 0 && alldefs.region.n > 0) {
		pix = getPix(w, h);

		if (pix != null) {
			int swx = getXOffset(w, pix.x);
			int swy = getYOffset(h, pix.y);
			sw = new xy(swx, swy);
		} else {
			sw = null;
		}

		if (pix != null && sw != null) {
			if (alldefs.legend != null) {
				scalesLegend = setLegendScales(pix.x * alldefs.region.m);
				colorsGradiants = setLegendGradiants(pix.x * alldefs.region.m);
			}
		}
	}

	for (int i = 0; i < imgsSize; i++) {
		if (imgs[i] != null)
			{ imgs[i].flush(); }
	}

	if (imgs[0] != null && (w != imgs[0].getWidth() || h != imgs[0].getHeight())) {
		imgs[0] = null;
	}

	if (imgs[0] == null) {
		imgs[0] = new BufferedImage(w, h, BufferedImage.TYPE_INT_ARGB);
		Graphics g = imgs[0].getGraphics();
		g.setColor(backgroundColor);
		g.fillRect(0, 0, w, h);
		g.dispose();
		imgs[0].flush();
	}


	if (imgs[1] != null && (w != imgs[1].getWidth() || h != imgs[1].getHeight())) {
		imgs[1] = null;
	}

	if (imgs[1] == null) {
		imgs[1] = new BufferedImage(w, h, BufferedImage.TYPE_INT_ARGB);
		Graphics g = imgs[1].getGraphics();
		g.setColor(backgroundColor);
		g.fillRect(0, 0, w, h);
		g.dispose();

		imgs[1].flush();
	}
    
	if (imgs[2] != null && (w != imgs[2].getWidth() || h != imgs[2].getHeight())) {
		imgs[2] = null;
	}

	if (imgs[2] == null) {
		imgs[2] = new BufferedImage(w, h, BufferedImage.TYPE_INT_ARGB);
		Graphics g = imgs[2].getGraphics();
		g.setColor(backgroundColor);
		g.fillRect(0, 0, w, h);
		g.dispose();

		imgs[2].flush();
	}
}

public int drawAll(BufferedImage img)
{
  if (img != null) 
  {
    xy pix = null;
    xy sw = null;
    double scales[] = null;
    Color gradiants[] = null;

    int w = img.getWidth();
    int h = img.getHeight();

    if (alldefs.region != null && alldefs.region.m > 0 && alldefs.region.n > 0)
    {
      pix = getPix(w, h);

      if (pix != null)
      {
        int swx = getXOffset(w, pix.x);
        int swy = getYOffset(h, pix.y);

        sw = new xy(swx, swy);

        if (alldefs.legend != null)
        {
          scales = setLegendScales(pix.x * alldefs.region.m);
          gradiants = setLegendGradiants(pix.x * alldefs.region.m);
        }
      }
    }

    Graphics2D g2 = img.createGraphics();
    g2.setBackground(backgroundColor);
    g2.clearRect(0, 0, w, h);
    img.flush();
    g2.dispose();
    
    drawLayer0(img, sw, pix, scales, gradiants); 
    int h2 = drawLayer1(img, sw, pix, scales, gradiants);
    drawLayer2(img, sw, pix);
 
    return h2;
  }
  return 0;
}

private void drawLayer0(BufferedImage img,
                        xy sw, xy pix, 
                        double[] scales, Color[] gradiants)
{
  if (img != null)
  {
    int w = img.getWidth();
    int h = img.getHeight();

    Graphics2D g2 = img.createGraphics();
    img.flush();

    if (histograms != null)
      drawHistograms(g2, 0, h, w, h);

    if (lineplots != null)
      drawLineplot(g2, 0, h, w, h);

    //user stuff
    if (sw != null && pix != null)
    {
      if (daSquares != null)
        drawSquares(g2, sw.x + 1, sw.y - pix.y, pix.x, pix.y, scales, gradiants);

      if (upperTriangles != null && lowerTriangles != null)
        drawTriangles(g2, sw.x + 1, sw.y - pix.y, pix.x, pix.y, scales, gradiants);
    }

    g2.dispose();
    img.flush();
  }
}

private int drawLayer1(BufferedImage img, 
                        xy sw, xy pix, 
                        double[] scales, Color[] gradiants)
{
  if (img != null && sw != null && pix != null)
  {
    Graphics2D g2 = img.createGraphics();

    img.flush();

    if (alldefs.region.iaAreas != null)
      drawAreas(g2, sw.x + 1, sw.y, pix.x, pix.y);

    if (alldefs.region.coastlinesFile != null)
      drawCst(g2, sw.x + 1, sw.y, pix.x, pix.y);

    int asc = getFontMetrics(getFont()).getAscent();

    if (alldefs.region.title != null) 
      drawTitle(g2, sw.x - 1, 1 + asc);

    drawBox(g2, 
            sw.x, 
            1
            + asc
            + 2,
            0     //line           
            + alldefs.region.m * pix.x 
            + 0,  //line
            0     //line           
            + alldefs.region.n * pix.y 
            + 0); //line
    int height = 1
                 + asc
                 + 2
                 + alldefs.region.n * pix.y;
    if (alldefs.legend != null)
    {
      drawLegend(g2, 
                 sw.x + 1, sw.y, alldefs.region.m * pix.x, 1 + asc + 1 + 1 + pix.y + 1,
                 scales, gradiants);
      height += 1 + asc + 2 + pix.y + 2 + 2 * asc + 2;
    }
    g2.dispose();
    img.flush();

    return height;
  }
  return 0;
}

private void drawLayer2(BufferedImage img, xy sw, xy pix)
{
  if (img != null && sw != null && pix != null)
  {
    int w = img.getWidth();
    int h = img.getHeight();

    Graphics2D g2 = img.createGraphics();
    img.flush();

    if (segmentedOvals != null)
      drawOvals(g2, sw.x + 1, sw.y - 1, pix.x, pix.y);

    if (segmentedArrows != null)
      drawArrows(segmentedArrows, g2, sw.x + 1, sw.y - 1, pix.x, pix.y);

    if (segmentedWorms != null)
      { drawArrows(segmentedWorms, g2, sw.x + 1, sw.y - 1, pix.x, pix.y); } 

    g2.dispose();
    img.flush();
  }
}
 
public void drawAll() //{ drawUser(imgs, sw, pix); }
{
  if (imgs[0] != null)
  {
    int w = imgs[0].getWidth();
    int h = imgs[0].getHeight();

    Graphics2D g0 = imgs[0].createGraphics();
    g0.setBackground(backgroundColor);
    g0.clearRect(0, 0, w, h);
    imgs[0].flush();
    g0.dispose();

    drawLayer0(imgs[0], sw, pix, scalesLegend, colorsGradiants); 
  }

  if (imgs[2] != null)
  {
    int w = imgs[2].getWidth();
    int h = imgs[2].getHeight();

    Graphics2D g2 = imgs[2].createGraphics();
    g2.setBackground(backgroundColor);
    g2.clearRect(0, 0, w, h);
    imgs[2].flush();
    g2.dispose();

    drawLayer2(imgs[2], sw, pix);
  }
}

private void drawUser(BufferedImage imgs[], xy sw, xy pix)
{
  if (imgs[0] != null)
  {
    BufferedImage img0 = imgs[0];
    int w = img0.getWidth();
    int h = img0.getHeight();

    Graphics2D g2 = img0.createGraphics();
    g2.setBackground(backgroundColor);
    g2.clearRect(0, 0, w, h);
    img0.flush();

    if (histograms != null)
      drawHistograms(g2, 0, h, w, h);

    if (lineplots != null)
      drawLineplot(g2, 0, h, w, h);

    //user stuff
    if (sw != null && pix != null)
    {
/*
      if (daSquares != null)
        drawSquares(g2, sw.x + 1, sw.y - pix.y, pix.x, pix.y);

      if (upperTriangles != null && lowerTriangles != null)
        drawTriangles(g2, sw.x + 1, sw.y - pix.y, pix.x, pix.y);
*/
    }

    g2.dispose();
    img0.flush();
  }

  if (imgs[2] != null)
  {
    BufferedImage img2 = imgs[2];
    int w = img2.getWidth();
    int h = img2.getHeight();

    //for some reason Graphics2D works better here?
    Graphics2D g2 = img2.createGraphics();
    g2.setBackground(backgroundColor);
    g2.clearRect(0, 0, w, h);
    img2.flush();
    if (sw != null && pix != null)
    {
      if (segmentedOvals != null)
        drawOvals(g2, sw.x + 1, sw.y - 1, pix.x, pix.y);

      if (segmentedArrows != null)
        drawArrows(segmentedArrows, g2, sw.x + 1, sw.y - 1, pix.x, pix.y);

      if (segmentedWorms != null)
        { drawArrows(segmentedWorms, g2, sw.x + 1, sw.y - 1, pix.x, pix.y); } 
    }

    g2.dispose();
    img2.flush();
  }
}

private void drawRegion(BufferedImage imgs[], xy sw, xy pix)
{ 
  assert imgs != null && sw != null && pix != null;

  BufferedImage img1 = imgs[1];
  assert img1.getGraphics() != null;

  int w = img1.getWidth();
  int h = img1.getHeight();
  Graphics g = img1.getGraphics();
  g.setColor(backgroundColor);
  g.fillRect(0, 0, w, h);

  if (alldefs.region.iaAreas != null)
    drawAreas(g, sw.x + 1, sw.y, pix.x, pix.y);

  if (alldefs.region.coastlinesFile != null)
    drawCst(g, sw.x + 1, sw.y, pix.x, pix.y);

  int asc = getFontMetrics(getFont()).getAscent();

  if (alldefs.region.title != null) 
    drawTitle(g, sw.x - 1, 1 + asc);

  drawBox(g, 
          sw.x, 
          1
          + asc
          + 2,
          0     //line           
          + alldefs.region.m * pix.x 
          + 0,  //line
          0     //line           
          + alldefs.region.n * pix.y 
          + 0); //line

  if (alldefs.legend != null)
    drawLegend(g, sw.x + 1, sw.y, alldefs.region.m * pix.x, 1 + asc + 1 + 1 + pix.y + 1);

  g.dispose();

  img1.flush();
}

private void drawTitle(Graphics g, int x0, int y0)
{  
  g.setColor(alldefs.region.colorLine);
  g.drawString(alldefs.region.title, x0, y0); 
}

private void drawBox(Graphics g, int x0, int y0, int w, int h)
{ 
  g.setColor(alldefs.region.colorLine);
  g.drawRect(x0, y0, w, h);
}

//-------------begin draw coastlines ----------------
public void drawArrows2(int[] ia, int idx, int xmins, int ymins)
{
  if (imgs != null && imgs[0] != null)
  {
    int w = imgs[0].getWidth();
    int h = imgs[0].getHeight();
    Graphics2D g = imgs[0].createGraphics();

      double xpixm = (double)(w - 200) / (double)xmins;
      double ypixm = (double)(h - 200) / (double)ymins;

      double pixm = xpixm < ypixm ? xpixm : ypixm;
      int myx = (int)(pixm * xmins);
      int myy = (int)(pixm * ymins);

      int x0 = 50;//(w - myx) / 2;
      int y0 = h - (h - myy) / 2;

      ypixm = pixm;
      xpixm = pixm;

    g.setColor(Color.black);

    //actual map
    g.drawLine(x0, 1, x0, h);
    g.drawLine(w - x0, 1, w - x0, h);
    g.drawLine(0, y0, w, y0);
    g.drawLine(0, h - y0, w, h - y0);

    //outer
    g.drawLine(0, 1, w, 1);
    g.drawLine(0, h - 1, w, h - 1);
    g.drawLine(w - 1, 1, w - 1, h - 1);
    g.drawLine(0, 1, 0, h - 1);

    double headsize = 9 * xpixm + 1;

    int size = ia.length;
    int i = 0;
    while (i < size)
    {
      g.setColor(new Color(ia[i++]));

      int numsegments = ia[i++];
      if (numsegments > 0)
      {
        int x1 = x0 + (int)(ia[i++] * xpixm);
        int y1 = y0 - (int)(ia[i++] * ypixm);
        g.drawString("T" + idx, x1, y1);
        for (int n = 0; n < numsegments - 1; n++) 
        {
          int x2 = x0 + (int)(ia[i++] * xpixm);
          int y2 = y0 - (int)(ia[i++] * ypixm);

          g.drawLine(x1, y1, x2, y2);

          x1 = x2;
          y1 = y2;
        }

        int x2 = x0 + (int)(ia[i++] * xpixm);
        int y2 = y0 - (int)(ia[i++] * ypixm);
        g.drawLine(x1, y1, x2, y2);

        double atan = Math.atan2((double)(y2  - y1), (double)(x2 - x1));
    
        double angle = atan + Math.PI / 4.0;
        int x = (int)(headsize * Math.cos(angle));
        int y = (int)(headsize * Math.sin(angle));
        g.drawLine(x2, y2, x2 - x, y2 - y);
              
        angle = atan - Math.PI / 4.0;
        x = (int)(headsize * Math.cos(angle));
        y = (int)(headsize * Math.sin(angle));
        g.drawLine(x2, y2, x2 - x, y2 - y);

        g.drawString("H" + idx, x2, y2);
      }
    }
  }
}

public void drawCst2(String filename, int xmins, int ymins)
{
  if (imgs != null && imgs[0] != null && xmins > 0 && ymins > 0)
  {
    int w = imgs[0].getWidth();
    int h = imgs[0].getHeight();

    if (w > 0 && h > 0)
    {
      Graphics2D g2d = imgs[0].createGraphics();
      g2d.setBackground(backgroundColor);
      g2d.clearRect(0, 0, w, h);

      defs defs2 = new defs();
      regiondef rdefs2 = new regiondef();

      double xpixm = (double)(w - 200) / (double)xmins;
      double ypixm = (double)(h - 200) / (double)ymins;

      double pixm = xpixm < ypixm ? xpixm : ypixm;
      int myx = (int)(pixm * xmins);
      int myy = (int)(pixm * ymins);

      int x0 = 50;//(w - myx) / 2;
      int y0 = h - (h - myy) / 2;

      drawCst(filename,
              g2d, 
              rdefs2.coastlinesBoundaryColor, 
              rdefs2.coastlinesFillColor, 
              x0, y0,
              pixm,
              pixm);

      g2d.dispose();
      g2d = null;
    }
  }
}

public native void drawCst(String sCstFile, Graphics2D g2d, Color coastline, Color land, int x0, int y0, double xpixm, double ypixm);

private void drawCst(Graphics g, int x0, int y0, int xpix, int ypix)
{
  int w = alldefs.region.m * xpix;
  int h = alldefs.region.n * ypix;
  BufferedImage img = new BufferedImage(w, h, BufferedImage.TYPE_INT_ARGB);

  double xpixelsperminute = (double)xpix / (double)alldefs.region.dx;
  double ypixelsperminute = (double)ypix / (double)alldefs.region.dy;

  Graphics2D g2d = img.createGraphics();
  g2d.setColor(backgroundColor);
  g2d.fillRect(0, 0, w, h);

  if (true == loadLibrary)
  {
    drawCst(alldefs.region.coastlinesFile, 
            g2d, 
            alldefs.region.coastlinesBoundaryColor, 
            alldefs.region.coastlinesFillColor, 
            0, 
            alldefs.region.n * ypix, 
            xpixelsperminute, 
            ypixelsperminute);
  }

  g2d.dispose();
  g2d = null;

  g.drawImage(img, x0, y0 - h, this);
  img.flush();
  img = null;
}
//-------------end draw coastlines ----------------

private void drawAreas(Graphics g, int x0, int y0, int xpix, int ypix)
{
  int m = alldefs.region.m;
  int n = alldefs.region.n;

  g.setColor(alldefs.region.colorLine);
  int idx = 0;
  int x = x0;
  for (int i = 0; i < m; i++)
  {
    int y = y0;
    for (int j = 0; j < n; j++)
    {
      int v = alldefs.region.iaAreas[idx];
      if (v == 0)
      {
        g.setColor(Color.white);
        g.fillRect(x, y - ypix, xpix, ypix);
        g.setColor(alldefs.region.colorLine);
      }
  
      if (j + 1 < n && v != alldefs.region.iaAreas[idx + 1])
      {
        g.drawLine(x, y - ypix, x + xpix - 1, y - ypix);
      }

      if (i + 1 < m && v != alldefs.region.iaAreas[idx + n])
      {
        g.drawLine(x + xpix - 1, y - ypix, x + xpix - 1, y);
      }

      y -= ypix;
      idx++;
    }
  
    x += xpix;
  }
}

//SW pixel coordinate
public int getXOffset(int w, int xpix)
{ 
  int x0 = (w - alldefs.region.m * xpix) / 2;
  return x0 + 1;
}

public int getYOffset(int h, int ypix)
{ 
  int y0 = 1     //space
           + getFontMetrics(getFont()).getAscent() //title
           + 1   //space 
           + 1   //line
           + alldefs.region.n * ypix //inside box
           + 1   //line
           + 1;  //space 
  return y0 - 2;
}

//returns null is unable to divide w and h into even squares
//xy(xpix, ypix) otherwise
public xy getPix(int w, int h)
{
  int asc = getFontMetrics(getFont()).getAscent();
  int w2 = w;
  w2 += - 1  //line
        - alldefs.region.m * 0
        - 1; //line

  int h2 = h;
  h2 += - 1  //space
        - asc
        - 1  //space
        - 1  //line
        - alldefs.region.n * 0
        - 1  //line
        - 1  //space
        - asc
        - 1  //space
        - 1  //line
        - asc
        - 1; //line

  //hard code legend
  h2 -= 1     //space
        + asc //font
        + 1   //space
        + 1   //line
        + asc //legend height
        + 1;  //line

  //case a: w < h
  int xpixa = w2 / alldefs.region.m;
  int ypixa = 
    (int)((double)xpixa / (double)alldefs.region.dx * (double)alldefs.region.dy);
  boolean valida = 
    xpixa > 0 && ypixa > 0 && ypixa * alldefs.region.n <= h2 ? true : false;
         
  //case a: w > h
  int ypixb = h2 / alldefs.region.n;
  int xpixb = (int)((double)ypixb / (double)alldefs.region.dx * (double)alldefs.region.dy);
  boolean validb = xpixb > 0 && ypixb > 0 && xpixb * alldefs.region.m <= w2 ? true : false;

  if (valida && validb)
    //{ valida = xpixa + ypixa >= xpixb + ypixb ? true : false; }
    { return xpixa + ypixa >= xpixb + ypixb ? new xy(xpixa, ypixa) : new xy(xpixb, ypixb); }

  if (validb)
    { return new xy(xpixb, ypixb); }
  else if (valida)
    { return new xy(xpixa, ypixa); }

  return null;
}

private void drawLegend(Graphics g, 
                        int swx, int swy, int w, int h, //box
                        double[] scales, Color[] gradiants)
{
  FontMetrics fm = getFontMetrics(getFont());
  int ascent = fm.getAscent(); 
  if (ascent > 0)
  {
    drawLegendGradiants(g, swx, swy + ascent, w, h - 1 - ascent - 1, gradiants);
    drawLegendStrings(g, swx, swy + ascent - 1, w, scales);
    drawBox(g, swx, swy + ascent, w, h - 1 - ascent - 1);
  }
}

private void drawLegend(Graphics g, int swx, int swy, int w, int h)
{
  FontMetrics fm = getFontMetrics(getFont());
  int ascent = fm.getAscent(); 
  assert ascent > 0;

  setScalesLegend(w);
  drawLegendStrings(g, swx, swy + ascent - 1, w);
  drawLegendGradiants(g, swx, swy + ascent, w, h - 1 - ascent - 1);
  drawBox(g, swx, swy + ascent, w, h - 1 - ascent - 1);
}

private double[] setLegendScales(int w)
{
  double[] scales = null;

  if (alldefs.legend != null 
      && w > 0 
      && alldefs.legend.max > alldefs.legend.min)
  {
    if (alldefs.legend.type == legenddef.UNIFORM)
    {
      scales = new double[w];

      double pix = (alldefs.legend.max - alldefs.legend.min) / w;

      double v0 = alldefs.legend.min; 
      for (int i = 0; i < w; i++)
      {
        v0 += pix; 
        scales[i] = v0;
      }
    }
    else if (alldefs.legend.type == legenddef.EXPONENTIAL 
             && alldefs.legend.max > 0
             && alldefs.legend.min > 0)
    {
      scales = new double[w];

      double pix = 
        Math.exp(Math.log(alldefs.legend.max/alldefs.legend.min) / w);

      double v0 = alldefs.legend.min; 
      for (int i = 0; i < w; i++)
      {
        v0 *= pix;
        scales[i] = v0;
      }
    }
  }

  return scales;
}

private double scalesLegend[] = null;
private void setScalesLegend(int w)
{
  scalesLegend = null;

  if (alldefs.legend != null 
      && w > 0 
      && alldefs.legend.max > alldefs.legend.min)
  {
    if (alldefs.legend.type == legenddef.UNIFORM)
    {
      scalesLegend = new double[w];

      double pix = (alldefs.legend.max - alldefs.legend.min) / w;

      double v0 = alldefs.legend.min; 
      for (int i = 0; i < w; i++)
      {
        v0 += pix; 
        scalesLegend[i] = v0;
      }
    }
    else if (alldefs.legend.type == legenddef.EXPONENTIAL 
             && alldefs.legend.max > 0
             && alldefs.legend.min > 0)
    {
      scalesLegend = new double[w];

      double pix = 
        Math.exp(Math.log(alldefs.legend.max/alldefs.legend.min) / w);

      double v0 = alldefs.legend.min; 
      for (int i = 0; i < w; i++)
      {
        v0 *= pix;
        scalesLegend[i] = v0;
      }
    }
  }
}

private void drawLegendStrings(Graphics g, int x0, int y0, int w, double[] scales)
{
  FontMetrics fm = g.getFontMetrics();

  double max = scales[w - 1];
  double min = scales[0];
  assert min < max;

  g.setColor(alldefs.region.colorLine);

  //drawMax
  String s2 = format.doublewithdecimals(max, alldefs.legend.decimals);
  int sw2 = fm.stringWidth(s2);
  int i2 = x0 + w - sw2;
  g.drawString(s2, i2,  y0);

  //drawMin
  String s0 = format.doublewithdecimals(min, alldefs.legend.decimals);
  int sw0 = fm.stringWidth(s0);
  int i0 = x0 + sw0 + 1;

  if (i0 < i2)
  {
    g.drawString(s0, x0,  y0);

    if (alldefs.legend.increment > 0)
    {
      double v = 0;

      //move to first increment after min
      if (min >= 0)
        while (v <= min)
          v += alldefs.legend.increment;
      else
        while (v - alldefs.legend.increment > min)
          v -= alldefs.legend.increment;

      java.util.Vector<String> svector = new java.util.Vector<String>();
      java.util.Vector<Integer> ivector = new java.util.Vector<Integer>();
      int idx = 0;
      while (v < max)
      {
        while (v > scales[idx])
          { idx++; }
        ivector.add(new Integer(idx));

        String s = format.doublewithdecimals(v, alldefs.legend.decimals);
        svector.add(s);

        if (alldefs.legend.type == legenddef.UNIFORM)
          v += alldefs.legend.increment;
        else if (alldefs.legend.type == legenddef.EXPONENTIAL && min >= 1)
          v *= 2;
      }

      int factor = 1; //do all increments

      boolean done = false;
      while (!done 
             && factor <= svector.size()) //prevent infiteloop
      {
        boolean overlap = false;
        int idx0 = sw0;
        for (int i = factor - 1; 
             i < svector.size() && !overlap; 
             i+=factor)
        {
          String s = (String)svector.get(i);
          int sw = fm.stringWidth(s);

          Integer integer = (Integer)ivector.get(i);
          int pix = integer.intValue();

          if (idx0 + 2 > pix - sw)
            overlap = true;
          else
            idx0 = pix + 2;
        }

        if (!overlap && idx0 < w - sw2) //check if second to last okay
          done = true;
        else
          factor++;
      }

      for (int i = factor - 1; i < svector.size(); i+=factor)
      {
        String s = (String)svector.get(i);
        Integer integer = (Integer)ivector.get(i);
        int pix = integer.intValue();

        int fmsw = fm.stringWidth(s);
        int idx2 = x0 + pix;

        if (i0 < idx2 - fmsw  && idx2 < i2) //ensure does not overlap min and max
          g.drawString(s, idx2 - fmsw, y0);
      }

      //do some forceful cleanup
      svector = null;
      ivector = null;
    }
  }
}

private void drawLegendStrings(Graphics g, int x0, int y0, int w)
{
  FontMetrics fm = g.getFontMetrics();

  double max = scalesLegend[w - 1];
  double min = scalesLegend[0];
  assert min < max;

  g.setColor(alldefs.region.colorLine);

  //drawMax
  String s2 = format.doublewithdecimals(max, alldefs.legend.decimals);
  int sw2 = fm.stringWidth(s2);
  int i2 = x0 + w - sw2;
  g.drawString(s2, i2,  y0);

  //drawMin
  String s0 = format.doublewithdecimals(min, alldefs.legend.decimals);
  int sw0 = fm.stringWidth(s0);
  int i0 = x0 + sw0 + 1;

  if (i0 < i2)
  {
    g.drawString(s0, x0,  y0);

    if (alldefs.legend.increment > 0)
    {
      double v = 0;

      //move to first increment after min
      if (min >= 0)
        while (v <= min)
          v += alldefs.legend.increment;
      else
        while (v - alldefs.legend.increment > min)
          v -= alldefs.legend.increment;

      java.util.Vector<String> svector = new java.util.Vector<String>();
      java.util.Vector<Integer> ivector = new java.util.Vector<Integer>();
      int idx = 0;
      while (v < max)
      {
        while (v > scalesLegend[idx])
          { idx++; }
        ivector.add(new Integer(idx));

        String s = format.doublewithdecimals(v, alldefs.legend.decimals);
        svector.add(s);

        if (alldefs.legend.type == legenddef.UNIFORM)
          v += alldefs.legend.increment;
        else if (alldefs.legend.type == legenddef.EXPONENTIAL && min >= 1)
          v *= 2;
      }

      int factor = 1; //do all increments

      boolean done = false;
      while (!done 
             && factor <= svector.size()) //prevent infiteloop
      {
        boolean overlap = false;
        int idx0 = sw0;
        for (int i = factor - 1; 
             i < svector.size() && !overlap; 
             i+=factor)
        {
          String s = (String)svector.get(i);
          int sw = fm.stringWidth(s);

          Integer integer = (Integer)ivector.get(i);
          int pix = integer.intValue();

          if (idx0 + 2 > pix - sw)
            overlap = true;
          else
            idx0 = pix + 2;
        }

        if (!overlap && idx0 < w - sw2) //check if second to last okay
          done = true;
        else
          factor++;
      }

      for (int i = factor - 1; i < svector.size(); i+=factor)
      {
        String s = (String)svector.get(i);
        Integer integer = (Integer)ivector.get(i);
        int pix = integer.intValue();

        int fmsw = fm.stringWidth(s);
        int idx2 = x0 + pix;

        if (i0 < idx2 - fmsw  && idx2 < i2) //ensure does not overlap min and max
          g.drawString(s, idx2 - fmsw, y0);
      }

      //do some forceful cleanup
      svector = null;
      ivector = null;
    }
  }
}

private void drawLegendStrings2(Graphics g, int x0, int y0, int w)
{
  g.setColor(alldefs.region.colorLine);

  FontMetrics fm = g.getFontMetrics();

  //drawMax
  String s = 
    format.doublewithdecimals(alldefs.legend.max, alldefs.legend.decimals);
  int ilast = x0 + w - fm.stringWidth(s);
  g.drawString(s, ilast,  y0);

  double pix;
  if (alldefs.legend.type == legenddef.UNIFORM)
    { pix = (alldefs.legend.max - alldefs.legend.min) / w; }
  else
  { 
    if (alldefs.legend.min <= 0) alldefs.legend.min = 1;
    pix = Math.exp(Math.log(alldefs.legend.max/alldefs.legend.min) / w); 
  }

  double vpix = alldefs.legend.min;

  //drawMin
  s = format.doublewithdecimals(alldefs.legend.min, alldefs.legend.decimals);
  int ibegin = x0;
  int stringwidth = fm.stringWidth(s) + 1;
  if (ibegin + stringwidth < ilast)
  {
    g.drawString(s, ibegin,  y0);
 
    if (alldefs.legend.increment == 0) return;

    if (alldefs.legend.type == legenddef.UNIFORM)
      for (int i = 0; i < stringwidth; i++)
        { vpix += pix; }
    else
      for (int i = 0; i < stringwidth; i++)
        { vpix *= pix; }
  }
  ibegin += stringwidth;

  double v = 0;
  if (alldefs.legend.type == legenddef.UNIFORM)
  {
    if (alldefs.legend.min >= 0)
      while (v <= alldefs.legend.min)
        v += alldefs.legend.increment;
    else
      while (v - alldefs.legend.increment > alldefs.legend.min)
        v -= alldefs.legend.increment;
  }

    if (alldefs.legend.type == legenddef.UNIFORM)
    {
      while (vpix < v)
      {
        vpix += pix; 
        ibegin++;
      }
    }
    else
    {
      while (vpix < v)
      {
        vpix *= pix;
        ibegin++;
      }
    }
  s = format.doublewithdecimals(v, alldefs.legend.decimals);
  stringwidth = fm.stringWidth(s) + 1;

  while (v < alldefs.legend.max)
  {
    if (alldefs.legend.type == legenddef.UNIFORM)
    {
      while (vpix < v)
      {
        vpix += pix; 
        ibegin++;
      }
    }
    else
    {
      while (vpix < v)
      {
        vpix *= pix;
        ibegin++;
      }
    }

    if (ibegin + stringwidth < ilast)
    {
      g.drawString(s, ibegin,  y0);
      if (alldefs.legend.type == legenddef.UNIFORM)
        for (int i = 0; i < stringwidth; i++)
          { vpix += pix; }
      else
        for (int i = 0; i < stringwidth; i++)
          { vpix *= pix; }
    }
    ibegin += stringwidth;

    if (alldefs.legend.type == legenddef.UNIFORM)
      v += alldefs.legend.increment;
    else
      v *= 2;

    s = format.doublewithdecimals(v, alldefs.legend.decimals);
    stringwidth = fm.stringWidth(s) + 1;
  }
}

private Color[] setLegendGradiants(int w)
{
  Color[] gradiants = null;

  if (alldefs != null && alldefs.legend != null && w > 0)
  {
    int iar[] = new int[w]; 
    int iag[] = new int[w]; 
    int iab[] = new int[w]; 

    int size = alldefs.legend.colorsLegend.length - 1;
    int w2 = w / size;

    int idx0 = 0;
    int idx2 = w2 - 1;
    int pixels = w - w2 * size;
    if (pixels > 0)
    {
      idx2++;
      pixels--;
    }

    for (int c = 0; c < size; c++)
    {
      Color c0 = alldefs.legend.colorsLegend[c];
      int cr0 = c0.getRed();
      int cg0 = c0.getGreen();
      int cb0 = c0.getBlue();

      Color c2 = alldefs.legend.colorsLegend[c + 1];
      int cr2 = c2.getRed();
      int cg2 = c2.getGreen();
      int cb2 = c2.getBlue();

      assert w >= idx2;
      assert idx2 > idx0;

      int w3 = idx2 - idx0 + 1 - 2;
      int dr = (cr2 - cr0) / w3;
      int mr = (cr2 - cr0) % w3;

      int dg = (cg2 - cg0) / w3;
      int mg = (cg2 - cg0) % w3;

      int db = (cb2 - cb0) / w3;
      int mb = (cb2 - cb0) % w3;

      iar[idx0] = cr0;    
      iag[idx0] = cg0;    
      iab[idx0] = cb0;    

      for (int idx = idx0 + 1; idx < idx2; idx++)
      {
        cr0 += dr;
        if (mr > 0)
        {
          cr0++;
          mr--;
        }
        else if (mr < 0)
        {
          cr0--;
          mr++;
        }

        cg0 += dg;
        if (mg > 0)
        {
          cg0++;
          mg--;
        }
        else if (mg < 0)
        {
          cg0--;
          mg++;
        }

        cb0 += db;
        if (mb > 0)
        {
          cb0++;
          mb--;
        }
        else if (mb < 0)
        {
          cb0--;
          mb++;
        }

        iar[idx] = cr0;    
        iag[idx] = cg0;    
        iab[idx] = cb0;    
      }

      iar[idx2] = cr2;    
      iag[idx2] = cg2;    
      iab[idx2] = cb2;    

      idx0 = idx2 + 1;
      idx2 += w2;
      if (pixels > 0)
      {
        idx2++;
        pixels--;
      }
    }

    gradiants = new Color[w];
    for (int i = 0; i < w; i++)
    {
      Color c = new Color(iar[i], iag[i], iab[i]);
      gradiants[i] = c;
    }
  }

  return gradiants;
}

private void drawLegendGradiants(Graphics g, int x0, int y0, int w, int h,
                                 Color[] gradiants)
{
  if (gradiants != null && gradiants.length == w)
  {
    int y2 = y0 + h;
    int x2 = x0 + w;

    int idx = 0;
    for (int i = x0; i < x2; i++)
    {
      g.setColor(gradiants[idx++]);
      g.drawLine(i, y0, i, y2);
    }
  }
}

private Color[] colorsGradiants = null;
private void drawLegendGradiants(Graphics g, int x0, int y0, int w, int h)
{
  assert w > 0;
  assert alldefs.legend.colorsLegend.length >= 2;

  if (w <= 0
      || alldefs == null
      || alldefs.legend == null 
      || alldefs.legend.colorsLegend == null
      || alldefs.legend.colorsLegend.length < 2 
      || alldefs.legend.colorsLegend.length > w) 
    return; 

  int iar[] = new int[w]; 
  int iag[] = new int[w]; 
  int iab[] = new int[w]; 

  BufferedImage img = new BufferedImage(w, h, BufferedImage.TYPE_INT_ARGB);

  int size = alldefs.legend.colorsLegend.length - 1;
  int w2 = w / size;

  int idx0 = 0;
  int idx2 = w2 - 1;
  int pixels = w - w2 * size;
  if (pixels > 0)
  {
    idx2++;
    pixels--;
  }

  for (int c = 0; c < size; c++)
  {
    Color c0 = alldefs.legend.colorsLegend[c];
    int cr0 = c0.getRed();
    int cg0 = c0.getGreen();
    int cb0 = c0.getBlue();

    Color c2 = alldefs.legend.colorsLegend[c + 1];
    int cr2 = c2.getRed();
    int cg2 = c2.getGreen();
    int cb2 = c2.getBlue();

    assert w >= idx2;
    assert idx2 > idx0;

    int w3 = idx2 - idx0 + 1 - 2;
    int dr = (cr2 - cr0) / w3;
    int mr = (cr2 - cr0) % w3;

    int dg = (cg2 - cg0) / w3;
    int mg = (cg2 - cg0) % w3;

    int db = (cb2 - cb0) / w3;
    int mb = (cb2 - cb0) % w3;

    iar[idx0] = cr0;    
    iag[idx0] = cg0;    
    iab[idx0] = cb0;    

    for (int idx = idx0 + 1; idx < idx2; idx++)
    {
      cr0 += dr;
      if (mr > 0)
      {
        cr0++;
        mr--;
      }
      else if (mr < 0)
      {
        cr0--;
        mr++;
      }

      cg0 += dg;
      if (mg > 0)
      {
        cg0++;
        mg--;
      }
      else if (mg < 0)
      {
        cg0--;
        mg++;
      }

      cb0 += db;
      if (mb > 0)
      {
        cb0++;
        mb--;
      }
      else if (mb < 0)
      {
        cb0--;
        mb++;
      }

      iar[idx] = cr0;    
      iag[idx] = cg0;    
      iab[idx] = cb0;    
    }

    iar[idx2] = cr2;    
    iag[idx2] = cg2;    
    iab[idx2] = cb2;    

    idx0 = idx2 + 1;
    idx2 += w2;
    if (pixels > 0)
    {
      idx2++;
      pixels--;
    }
  }

  colorsGradiants = new Color[w];
  Graphics g2 = img.getGraphics();
  for (int i = 0; i < w; i++)
  {
    Color c = new Color(iar[i], iag[i], iab[i]);

    colorsGradiants[i] = c;

    g2.setColor(c);
    g2.drawLine(i, 0, i, h);
  }
  
  g.drawImage(img, x0, y0, this);

  img = null;
}

private Color getColor(double value, double[] scales, Color[] gradiants)
{
	assert scales != null;
	assert gradiants != null;
	assert scales.length == gradiants.length;
	int size = scales.length;
	if (size > 0 && value >= scales[0])
	{
		int i = 0;
		while (i < size)
		{
			if (value < scales[i])
			{
				return gradiants[i];
			}
			i++;
		}
		return gradiants[size - 1]; 
	}
	return null;
}

private double daSquares[] = null;
public void setSquares(double[] da)
{
  int size = da != null ? da.length : 0;
  if (size > 0)
  {
    daSquares = new double[size];
    for (int i = 0; i < size; i++)
      { daSquares[i] = da[i]; }
  }
}

private void drawSquares(Graphics g, int x0, int y0, int xpix, int ypix, 
                         double[] scales, Color[] gradiants)
{
  if (daSquares != null && scales != null && gradiants != null)
  {
    assert daSquares.length == alldefs.region.m * alldefs.region.n;
    assert scales.length == gradiants.length;

    int idx  = 0;
    int x = x0;
    for (int i = 0; i < alldefs.region.m; i++)
    {
      int y = y0;
      for (int j = 0; j < alldefs.region.n; j++) 
      {
        Color c = getColor(daSquares[idx++], scales, gradiants);
        if (c != null)
        {
          g.setColor(c);
          g.fillRect(x, y, xpix, ypix);
        }

        y -= ypix;
      }

      x += xpix;
    }
  } 
}

private double[] upperTriangles = null;
private double[] lowerTriangles = null;
public void setTriangles(double[] daU, double[] daL)
{
  int size = daU != null 
             && daL != null
             && daU.length == daL.length
             ? daU.length
             : 0;
  if (size > 0)
  {
    upperTriangles =  new double[size];
    lowerTriangles =  new double[size];
    for (int i = 0; i < size; i++)
    {
      upperTriangles[i] = daU[i];
      lowerTriangles[i] = daL[i];
    }
  }
}

public void drawTriangles(Graphics g, int x0, int y0, int xpix, int ypix,
                          double[] scales, Color[] gradiants)
{
  if (upperTriangles != null && lowerTriangles != null 
      && scales != null && gradiants != null)
  {
    int[] xt = new int[3];
    int[] yt = new int[3];

    int idx = 0;
    int size = upperTriangles.length;

    int x = x0;
    for (int i = 0; i < alldefs.region.m; i++)
    {
      int y = y0;
      for (int j = 0; j < alldefs.region.n; j++)
      {
        xt[1] = x;
        yt[1] = y + ypix;

        xt[2] = x + xpix;
        yt[2] = y;
        Color c = getColor(upperTriangles[idx], scales, gradiants);
        if (c != null)
        {
          xt[0] = x;
          yt[0] = y;

          g.setColor(c);
          g.fillPolygon(xt, yt, 3);
        }

        c = getColor(lowerTriangles[idx], scales, gradiants);
        if (c != null)
        {
          xt[0] = x + xpix;
          yt[0] = y + ypix;

          g.setColor(c);
          g.fillPolygon(xt, yt, 3);
        }
    
        y -= ypix;
        idx++;
      }

      x += xpix;
    }
  }
}

public int[] cloneArray(int[] ia)
{
  int[] ia2 = null;
  int size = ia != null ? ia.length : 0;
  if (size > 0)
  {
    ia2 = new int[ia.length];
    for (int i = 0; i < size; i++)
      { ia2[i] = ia[i]; }
  }

  return ia2;
}

/* SEGMENTED stucture
 * rgb_0, numsegments_0, x1_0, y1_0, ..., xnumsegments_0, ynumsegments_0, ...,
 * rgb_n, numsegments_n, x1_n, y1_n, ..., xnumsegments_n, ynumsegments_n,
 * where xi, yi are integer minutes
 */

private int[] segmentedOvals = null;
public void setOvals(int[] ia)
  { segmentedOvals = cloneArray(ia); }

private void drawOvals(Graphics g, int x0, int y0, int xpix, int ypix)
{
  double xpixm = (double)xpix / (double)alldefs.region.dx;
  double ypixm = (double)ypix / (double)alldefs.region.dy;

  int size = segmentedOvals.length;
  for (int i = 0; i < size; i += 6)
  {
    g.setColor(new Color(segmentedOvals[i]));

    //numsegments should be 1 for all segments 
    assert segmentedOvals[i + 1] == 1;

    int x1 = x0 + (int)(segmentedOvals[i + 2] * xpixm);
    int y1 = y0 - (int)(segmentedOvals[i + 3] * ypixm);
    int w  =      (int)(segmentedOvals[i + 4] * xpixm);
    int h  =      (int)(segmentedOvals[i + 5] * ypixm);
    
    //g.drawLine(x1 - x2 / 2, y1 - y2 / 2, x2, y2);
    g.drawOval(x1 - w / 2, y1 - h / 2, w, h);
  }
}

private int[] segmentedWorms = null;
public void setWorms(int[] ia)
  { segmentedWorms = cloneArray(ia); }

private int[] segmentedArrows = null;
public void setArrows(int[] ia)
  { segmentedArrows = cloneArray(ia); }

private void drawArrows(int[] ia,
                        Graphics g, int x0, int y0, int xpix, int ypix)
{
  double xpixm = (double)xpix / (double)alldefs.region.dx;
  double ypixm = (double)ypix / (double)alldefs.region.dy;

  double headsize = 9 * xpixm + 1;

  int size = ia.length;
  int i = 0;
  while (i < size)
  {
    g.setColor(new Color(ia[i++]));

    int numsegments = ia[i++];
    if (numsegments > 0)
    {
      int x1 = x0 + (int)(ia[i++] * xpixm);
      int y1 = y0 - (int)(ia[i++] * ypixm);
      for (int n = 0; n < numsegments - 1; n++) 
      {
        int x2 = x0 + (int)(ia[i++] * xpixm);
        int y2 = y0 - (int)(ia[i++] * ypixm);

        g.drawLine(x1, y1, x2, y2);

        x1 = x2;
        y1 = y2;
      }

      int x2 = x0 + (int)(ia[i++] * xpixm);
      int y2 = y0 - (int)(ia[i++] * ypixm);
      g.drawLine(x1, y1, x2, y2);

      double atan = Math.atan2((double)(y2  - y1), (double)(x2 - x1));
    
      double angle = atan + Math.PI / 4.0;
      int x = (int)(headsize * Math.cos(angle));
      int y = (int)(headsize * Math.sin(angle));
      g.drawLine(x2, y2, x2 - x, y2 - y);
              
      angle = atan - Math.PI / 4.0;
      x = (int)(headsize * Math.cos(angle));
      y = (int)(headsize * Math.sin(angle));
      g.drawLine(x2, y2, x2 - x, y2 - y);
    }
    else if (numsegments == 0)
    { 
      //don't draw just one point
      //i += 2; 
    }
    //else {} //empty null
  }
}

//format bar0stack0,               ,bar0stack_barStack-1,
//       bar_barTotal-1stack0, ... ,bar_barTotal-1stack_barStack-1,
double histograms[] = null;
public void setHistograms(double[] da, int stack)
{
  int size = histograms != null 
             && da != null
             && 0 <= stack && stack < alldefs.histogram.barStack
             ? da.length
             : 0;

  int idx = stack;
  for (int i = 0; i < da.length; i++)
  { 
    histograms[idx] = da[i]; 
    idx += alldefs.histogram.barStack;
  }
}

//assume histograms != null
public void drawHistograms(Graphics2D g, int x0, int y0, int w, int h)
{
  double ymax = alldefs.histogram.yaxisMax;
  double ymin = alldefs.histogram.yaxisMin;

  for (int i = 0; i < histograms.length; i+=alldefs.histogram.barStack)
  {
    double v0 = 0;
    double v2 = 0;
    for (int j = 0; j < alldefs.histogram.barStack; j++)
    { 
      double v = histograms[i + j]; 
      if (v > 0)
        v0 += v;
      else
        v2 += v;
    }

    if (v0 > ymax) ymax = v0;
    if (v2 < ymin) ymin = v2;
  }

  if (alldefs.histogram.yaxisIncrement > 0)
  {
    double y = 0;
    if (ymax >= 0)
    {
      while (ymax > y)
        { y += alldefs.histogram.yaxisIncrement; }
    }
    else
    {
      while (ymax < y - alldefs.histogram.yaxisIncrement)
        { y -= alldefs.histogram.yaxisIncrement; }
    }
    ymax = y;

    y = 0;
    if (ymin >= 0)
    {
      while (ymin > y)
        { y += alldefs.histogram.yaxisIncrement; }
    }
    else
    {
      while (ymin < y)
        { y -= alldefs.histogram.yaxisIncrement; }
    }
    ymin = y;
  }

  FontMetrics fm = getFontMetrics(getFont());
  int ascent = fm.getAscent(); 

  int yTop = 1        //space
             + ascent //font
             + 1;     //space

  int yBottom = 4        //tic and overlapping line
                + ascent //font
                + 1      //space
                + ascent //font
                + 1;     //space

  int xLeft = 1        //space
              + ascent //font
              + 1      //space
              + 0      //stringwidth placeholder
              + 4;     //tic and overlapping line

  int xRight = 4; //spacing only

  int sw1 = 
   fm.stringWidth(format.doublewithdecimals(ymin, alldefs.histogram.yaxisDecimals));
  int sw2 = 
   fm.stringWidth(format.doublewithdecimals(ymax, alldefs.histogram.yaxisDecimals));

  xLeft += sw1 > sw2 ? sw1 : sw2;

  double ypix = 0;
  if (ymax > ymin)
  {
    g.setColor(Color.black);

    if (yTop + yBottom < h)
    {
      ypix = Math.abs(ymax - ymin) / (double)(h - yTop - yBottom);

      g.drawLine(xLeft, h - yBottom, xLeft, yTop);
      g.drawLine(xLeft + 1, h - yBottom, xLeft + 1, yTop);
    }
    else
      { ypix = Math.abs(ymax - ymin) / (double)1; }

    if (alldefs.histogram.yaxisIncrement > 0 && ypix > 0)
    {
      int j0 = yTop;
      int jn = h - yBottom - 1;

      int factor = 0;
      {
        int total;
        int size = jn - j0 + 1 - ascent;
        do
        {
          total = 0;
          factor++;

          double y = ymax - factor * alldefs.histogram.yaxisIncrement;
          while (y > ymin)
          {
            total += ascent;
            y -= factor * alldefs.histogram.yaxisIncrement;
          }
        } while (total > size);
      }

      double y = ymax;
      while (j0 <= jn)
      {
        String s = format.doublewithdecimals(y, alldefs.histogram.yaxisDecimals);
        int sw = fm.stringWidth(s);

        g.drawString(s, xLeft - sw - 4, j0 + ascent / 2 + 1);
        g.drawLine(xLeft, j0, xLeft - 4, j0);
  
        y -= factor * alldefs.histogram.yaxisIncrement;
        j0 = yTop + (int)((ymax - y) / ypix);
      }
    }
    else
    {
      String s = 
        format.doublewithdecimals(ymax, alldefs.histogram.yaxisDecimals);
      int sw = fm.stringWidth(s);

      g.drawString(s, xLeft - sw - 4, yTop + ascent / 2 - 1);
      g.drawLine(xLeft, yTop, xLeft - 4, yTop);
    }

    if (alldefs.histogram.barTotal > 0)
    {
      int spacing = alldefs.histogram.barSpacing;
      int width = alldefs.histogram.barWidth;

      if (width <= 0)
      {
        width = (w - xLeft - xRight - alldefs.histogram.barTotal * spacing * 2) / alldefs.histogram.barTotal;
      }

      if (width <= 0)
        width = 1;

      int total = spacing * 2 + width;

      int xa = xLeft + 2;
      int xb = xa + alldefs.histogram.barTotal * total;
      int yb = h - yBottom;
      g.drawLine(xa - 2, yb, xb, yb);
      g.drawLine(xa - 2, yb - 1, xb, yb - 1);

      int yc = yTop + (int)(ymax / ypix);
      g.drawLine(xa - 2, yc, xb, yc);

      int idx = 0;
      for (int i = 0; i < alldefs.histogram.barTotal; i++)
      {
        int yT = yc;
        int yB = yc;
        for (int j = 0; j < alldefs.histogram.barStack; j++)
        {
          double v = histograms[idx++];
          int j2 = Math.abs((int)(v / ypix));
          if (j2 > 1)
          {
            g.setColor(alldefs.histogram.barColors[j]);

            if (v > 0)
            {
              yT -= j2;

              g.fillRect(xa + spacing, yT, width, j2);
  
              g.setColor(Color.black);
              g.drawRect(xa + spacing, yT, width, j2);
            }
            else
            {
              g.fillRect(xa + spacing, yB, width, j2);
  
              g.setColor(Color.black);
              g.drawRect(xa + spacing, yB, width, j2);

              yB += j2;
            }
          }
        }  

        if (alldefs.histogram.barLabels != null 
            && alldefs.histogram.barLabels.length 
               == alldefs.histogram.barTotal)
        {
          String s = alldefs.histogram.barLabels[i];
          if (s != null)
          {
            g.drawLine(xa + total / 2, yb, xa + total / 2, yb + 4);
       
            int sw0 = fm.stringWidth(s) / 2;
            g.drawString(s, xa + total / 2 - sw0, yb + 4 + 1 + ascent);
          }
        }
        xa += total;
      }
    }
  }

  g.setColor(Color.black);

  //draw Titles
  if (alldefs.histogram.title != null)
    g.drawString(alldefs.histogram.title, xLeft, yTop - 1);

  if (alldefs.histogram.barTitle != null)
    g.drawString(alldefs.histogram.barTitle,
                 w / 2,
                 h - 1);

  if (alldefs.histogram.yaxisTitle != null)
  {
    g.rotate(Math.PI / -2.0, 1 + ascent, h / 2);
    g.drawString(alldefs.histogram.yaxisTitle, 
                  1 + ascent, 
                  h / 2);
    g.rotate(Math.PI / 2.0, 1 + ascent, h / 2);
  }
}

Vector lineplots[] = null;
public void addLineplots(double da[])
{ 
  assert lineplots != null && da.length == lineplots.length;

  int size = da.length;
  for (int i = 0; i < size; i++)
    { lineplots[i].add(new Double(da[i])); }
}

public void drawLineplot(Graphics2D g, int x0, int y0, int w, int h)
{
  assert lineplots != null;

  int xmax = alldefs.lineplot.xaxisMax;
  double ymax = alldefs.lineplot.yaxisMax;
  double ymin = alldefs.lineplot.yaxisMin;

  for (int i = 0; i < lineplots.length; i++)
  {
    Vector vec = lineplots[i];

    int size = vec.size() - 1;
    if (size > xmax) 
      { xmax = size; }

    for (int j = 0; j <= size; j++)
    {
      Double yD = (Double)vec.get(j);
      double y = yD.doubleValue();

      if (ymax < y) ymax = y;
      if (ymin > y) ymin = y;
    }
  }

  //shift using increments
  if (alldefs.lineplot.xaxisIncrement > 0)
  {
    int x = 0;
    while (xmax > x)
      { x += alldefs.lineplot.xaxisIncrement; }
    xmax = x;
  }

  if (alldefs.lineplot.yaxisIncrement > 0)
  {
    double y = 0;
    if (ymax >= 0)
    {
      while (ymax > y)
        { y += alldefs.lineplot.yaxisIncrement; }
    }
    else
    {
      while (ymax < y - alldefs.lineplot.yaxisIncrement)
        { y -= alldefs.lineplot.yaxisIncrement; }
    }
    ymax = y;

    y = 0;
    if (ymin >= 0)
    {
      while (ymin > y)
        { y += alldefs.lineplot.yaxisIncrement; }
    }
    else
    {
      while (ymin <= y)
        { y -= alldefs.lineplot.yaxisIncrement; }
    }
    ymin = y;
  }

  FontMetrics fm = getFontMetrics(getFont());
  int ascent = fm.getAscent(); 

  int yTop = 1        //space
             + ascent //font
             + 1;     //space

  int yBottom = 4        //tic and overlapping line
                + ascent //font
                + 1      //space
                + ascent //font
                + 1;     //space

  int xLeft = 1        //space
              + ascent //font
              + 1      //space
              + 0      //stringwidth placeholder
              + 4;     //tic and overlapping line

  int sw1 = 
   fm.stringWidth(format.doublewithdecimals(ymin, alldefs.lineplot.yaxisDecimals));
  int sw2 = 
   fm.stringWidth(format.doublewithdecimals(ymax, alldefs.lineplot.yaxisDecimals));

  xLeft += sw1 > sw2 ? sw1 : sw2;

  int xRight = fm.stringWidth(Integer.toString(xmax))/2 + 1;

  g.setColor(Color.black);

  double ypix = 0;
  if (ymax > ymin)
  {
    if (yTop + yBottom < h)
    {
      ypix = Math.abs(ymax - ymin) / (double)(h - yTop - yBottom);

      g.drawLine(xLeft, h - yBottom, xLeft, yTop);
      g.drawLine(xLeft + 1, h - yBottom, xLeft + 1, yTop);
    }
    else
      { ypix = Math.abs(ymax - ymin) / (double)1; }

    if (alldefs.lineplot.yaxisIncrement > 0 && ypix > 0)
    {
      int j0 = yTop;
      int jn = h - yBottom - 1;

      int factor = 0;
      {
        int total;
        int size = jn - j0 + 1 - ascent;
        do
        {
          total = 0;
          factor++;

          double y = ymax - factor * alldefs.lineplot.yaxisIncrement;
          while (y > ymin)
          {
            total += ascent;
            y -= factor * alldefs.lineplot.yaxisIncrement;
          }
        } while (total > size);
      }

      double y = ymax;
      while (j0 <= jn)
      {
        String s = format.doublewithdecimals(y, alldefs.lineplot.yaxisDecimals);
        int sw = fm.stringWidth(s);

        g.drawString(s, xLeft - sw - 4, j0 + ascent / 2 + 1);
        g.drawLine(xLeft, j0, xLeft - 4, j0);
  
        y -= factor * alldefs.lineplot.yaxisIncrement;
        j0 = yTop + (int)((ymax - y) / ypix);
      }
    }
    else
    {
      String s = 
        format.doublewithdecimals(ymax, alldefs.lineplot.yaxisDecimals);
      int sw = fm.stringWidth(s);

      g.drawString(s, xLeft - sw - 4, yTop + ascent / 2 - 1);
      g.drawLine(xLeft, yTop, xLeft - 4, yTop);
    }

    if (xmax > 0)
    {
      int xpix = (w - xLeft - xRight) / xmax;
      if (xpix <= 0) xpix = 1;

      int xa = xLeft;
      int xb = xLeft + xmax * xpix;
      int yb = h - yBottom;
      g.drawLine(xa, yb, xb, yb);
      g.drawLine(xa, yb + 1, xb, yb + 1);

      if (alldefs.lineplot.xaxisIncrement > 0)
      {
        int increment = alldefs.lineplot.xaxisIncrement;
        {
          String sb = Integer.toString(xmax);
          int swb = fm.stringWidth(sb) / 2;

          String sa;
          int swa;
          int count = 0;
          do
          {
            count++;
            sa = Integer.toString(xmax - count * increment);
            swa = fm.stringWidth(sa) / 2;
          } while (count * increment * xpix - (swa + 4 + swb) < 0 && count < xmax);

          increment *= count;
        }

        int x = xb;
        String s = Integer.toString(xmax);
        int sw = fm.stringWidth(s) / 2;
        g.drawString(s, x - sw, yb + 4 + ascent);
        g.drawLine(x, yb, x, yb + 4);
        x -= xpix;

        int idx = xmax - increment;
        for (int i = xmax - 1; i > 0; i--)
        {
          if (i == idx)
          {
            s = Integer.toString(i);
            sw = fm.stringWidth(s) / 2;
            g.drawString(s, x - sw, yb + 4 + ascent);
            g.drawLine(x, yb, x, yb + 4);
            idx -= increment;
          }
          else
          {
            g.drawLine(x, yb, x, yb + 2);
          }

          x -= xpix;
        }
      }
      else
      {
        String s = Integer.toString(xmax);
        int sw = fm.stringWidth(s) / 2;
        g.drawLine(xb, yb, xb, yb + 4);
        g.drawString(s, xb - sw, yb + 4 + ascent);
      }

      for (int line = 0; line < lineplots.length; line++)
      {
        Vector vec = lineplots[line];

        //assume good
        g.setColor(alldefs.lineplot.linesColors[line]);

if (vec.size() > 0)
{
        Double d0 = (Double)vec.get(0);
        double value0 = d0.doubleValue();

        int j0;
        if (value0 >= ymax)
          { j0 = yTop - (int)((value0 - ymax) / ypix); }
        else
          { j0 = yTop + (int)((ymax - value0) / ypix); }

        int x_a = xLeft;
        int size = vec.size();
        for (int i = 1; i < size; i++)
        {
          Double d2 = (Double)vec.get(i);
          double value2 = d2.doubleValue();

          int j2;
          if (value2 >= ymax)
            { j2 = yTop - (int)((value2 - ymax) / ypix); }
          else
            { j2 = yTop + (int)((ymax - value2) / ypix); }
  
          int x_b = x_a + xpix;
          g.drawLine(x_a, j0, x_b, j2);
          x_a = x_b;
          j0 = j2;
        }
}
      }
    }
  }

  //g.setColor(defs.colorLine);
  g.setColor(Color.black);

  //draw Titles
  if (alldefs.lineplot.title != null)
    g.drawString(alldefs.lineplot.title, xLeft, yTop - 1);

  if (alldefs.lineplot.xaxisTitle != null)
    g.drawString(alldefs.lineplot.xaxisTitle,
                 w / 2,
                 h - 1);

  if (alldefs.lineplot.yaxisTitle != null)
  {
    g.rotate(Math.PI / -2.0, 1 + ascent, h / 2);
    g.drawString(alldefs.lineplot.yaxisTitle, 
                  1 + ascent, 
                  h / 2);
    g.rotate(Math.PI / 2.0, 1 + ascent, h / 2);
  }
}

public static void main(String[] sa)
{
  System.out.println("region.main");
  java.util.Random r = new java.util.Random();
  javax.swing.JFrame f = new javax.swing.JFrame("region");
  f.getContentPane().setLayout(new BorderLayout());
  f.setSize(500, 500);

  while (true)
  {
    f.getContentPane().removeAll();

    regiondef rdef = new regiondef();
    rdef.title = "region";
    rdef.m = r.nextInt(50) + 1;
    rdef.n = r.nextInt(50) + 1;
    rdef.dx = 60;
    rdef.dy = 60;

    int[] ia = new int[rdef.m * rdef.n];
    for (int i = 0; i < rdef.m * rdef.n; i++)
    {
      int j = 1;
      if (r.nextInt(10) > 5) j *= 2;
      if (r.nextInt(10) > 5) j *= 2;
      if (r.nextInt(10) > 5) j *= 2;
      if (r.nextInt(10) > 5) j *= -1;
      ia[i] = j;
    }
    rdef.iaAreas = ia;
    rdef.coastlinesFile = "yft16.cst";

    legenddef ldef = new legenddef();
    ldef.type = legenddef.EXPONENTIAL;

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

    region myregion = new region();
    myregion.addRegion(rdef);
    myregion.addLegend(ldef);
    myregion.addLineplot(pdef);

    f.getContentPane().add(myregion, "Center");
    f.setVisible(true);

    //for (int i = 0; i < 150; i++)
    while (true)
    {
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


      Vector<Integer> vec = new Vector<Integer>();
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
      myregion.addLineplots(daLineplots);
 
      myregion.drawAll();
      myregion.drawLayers();
      myregion.switchBuffers();
      myregion.repaint();

/*
      try 
        { Thread.sleep(1000); }
      catch (InterruptedException ie)
        { }
*/
    }

/*
    try 
      { Thread.sleep(4000); }
    catch (InterruptedException ie)
      { }
*/
  }
}
}//class region
