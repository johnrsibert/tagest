/* dbc.java DoubleBufferedComponent
 * Started: 1999.06.03
 *
 * LightweightComponent whose job is to ensure the image buffers
 * imgPrimary and imgBuffer are correctly sized and painted. 
 *
 * Usage:
 *   dbc()
 *   * getBufferGraphics() do not allow direct access of imgBuffer
 *   * switchBuffers() set imgBuffer to imgPrimary and vice versa 
 *   * repaint() forces java vm to paint component
 * Problems:
 *   - if component hasn't been resized getBufferGraphics return null
 *     + use a function like isDrawable that uses stuff in main to 
 *       wait for componentResized to finish
       = removed componentResized and moved stuff to validate
 *   - watch out for the case where width or height is 0
       = added 1 to createImg 
 */
import java.awt.*;
import java.awt.image.*;
import javax.swing.JComponent;

public class dbc extends JComponent
{
public int id = -1;

//private Image imgPrimary = null;
//private Image imgBuffer  = null;
private BufferedImage imgPrimary = null;
private BufferedImage imgBuffer  = null;

public dbc() 
{ /*DoNothing*/ 
    //imgBuffer = new BufferedImage(200, 200, BufferedImage.TYPE_INT_RGB);
    //imgPrimary = new BufferedImage(200, 200, BufferedImage.TYPE_INT_RGB);
}

/*
public void invalidate()
  { super.invalidate(); }

public void validate()
{
	super.validate();
	allocateBufferedImages();
}
*/

public void printBuffers() 
{
int h = getHeight();
int w = getWidth();
System.out.println(h);
System.out.println(w);

if (imgBuffer != null) {
	System.out.println("imgBuffer Height " + imgBuffer.getHeight());
	System.out.println("imgBuffer Width " + imgBuffer.getWidth());
}

if (imgPrimary != null) {
	System.out.println("imgPrimary Height " + imgPrimary.getHeight());
	System.out.println("imgPrimary Width " + imgPrimary.getWidth());
}
}

public void allocateBufferedImages()
{
	if (!isShowing()) {
		if (imgBuffer != null) { 
			imgBuffer.flush();
			imgBuffer = null;
		}
		if (imgPrimary != null) {
			imgPrimary.flush();
			imgPrimary = null;
		}

		return;
	}

	int h = getHeight();
	int w = getWidth();
	if (h > 0 && w > 0) {
		if (imgBuffer != null) { 
			int heightImgBuffer = imgBuffer.getHeight();
			int widthImgBuffer = imgBuffer.getWidth();
			if (h != heightImgBuffer || w != widthImgBuffer) {
				imgBuffer.flush();
				imgBuffer = null;
			}
		}

		if (imgBuffer == null) {
			imgBuffer = new BufferedImage(w, h, BufferedImage.TYPE_INT_RGB);

			Graphics g = imgBuffer.getGraphics();
			g.setColor(new Color(204, 204, 204));
			g.fillRect(0, 0, w, h);
			g.dispose();
		}

		if (imgPrimary != null) {
			int heightImgPrimary = imgPrimary.getHeight();
			int widthImgPrimary = imgPrimary.getWidth();
			if (h != heightImgPrimary || w != widthImgPrimary) {
				imgPrimary.flush();
				imgPrimary = null;
			}
		}

		if (imgPrimary == null) {
			imgPrimary = new BufferedImage(w, h, BufferedImage.TYPE_INT_RGB);

			Graphics g = imgPrimary.getGraphics();
			g.setColor(new Color(204, 204, 204));
			g.fillRect(0, 0, w, h);
			g.dispose();
		}
	}
}

public void clear()
{
  if (imgBuffer != null)
  {
    Graphics g = imgBuffer.getGraphics();
    assert g != null;

    g.setColor(new Color(204, 204, 204));
    g.fillRect(0, 0, imgBuffer.getWidth(this), imgBuffer.getHeight(this));
    
    g.dispose();
  }
}

public void paint(BufferedImage img)
{
  if (imgBuffer != null)
  {
    Graphics g = imgBuffer.getGraphics();
    assert g != null;

    //g.setColor(Color.red);//new Color(204, 204, 204));
    //g.fillRect(0, 0, imgBuffer.getWidth(this) - 5, imgBuffer.getHeight(this) - 5);
    //g.clearRect(0, 0, imgBuffer.getWidth(this) - 5, imgBuffer.getHeight(this) - 5);
    g.drawImage(img, 0, 0, this); 
    
    g.dispose();
  }

  img.flush();
}

public void update(Graphics g)
  { paint(g); }

public void paint(Graphics g)
{
  if (imgPrimary != null)
    g.drawImage(imgPrimary, 0, 0, this); 
}

public Graphics getBufferGraphics()
  { return imgBuffer != null ? imgBuffer.getGraphics() : null; }

public void switchBuffers()
{
  BufferedImage tmp  = imgBuffer;
  imgBuffer  = imgPrimary;
  imgPrimary = tmp;
}

public static void main(String[] sa)
{
  dbc mydbc = new dbc();

  javax.swing.JFrame f = new javax.swing.JFrame("dbc");
  f.getContentPane().setLayout(new BorderLayout());
  f.setSize(100, 100);
  f.getContentPane().add(mydbc, "Center");
  f.setVisible(true);

  boolean red = false;
  while (true)
  {
    Graphics g = mydbc.getBufferGraphics();
    assert g != null;

    if (red)
    {
      g.setColor(Color.red);
      g.drawLine(100,0,0,100);
    }
    else
    {
      g.setColor(Color.blue);
      g.drawLine(0,0,100,100);
    }
    red = !red;
    g.dispose();

    mydbc.switchBuffers();
    mydbc.repaint();

    try 
    {
      Thread.sleep(1000);
    }
    catch (InterruptedException ie)
    {
    }
  }
}
}//class dbc
/*
import com.sun.image.codec.jpeg.*;
public ppm cppm = new ppm();
public void saveppm(String s)
  { saveppm(s, getWidth(), getHeight()); }
public void saveppm(String s, int w, int h)
  { cppm.set(s, w, h); }

protected String sjpeg = null;
public void savejpeg(String s)
{ 
  ASSERT.NOTNULL(s);
  sjpeg = s; 
  savejpeg();
}

protected void savejpeg()
{
  if (imgBuffer == null || sjpeg == null) return;

  try
  { 
    //System.out.print(getSize());
    BufferedImage bimg = (BufferedImage)imgBuffer;

    JPEGEncodeParam param = JPEGCodec.getDefaultJPEGEncodeParam(bimg);
    param.setQuality((float)1, true);

    java.io.FileOutputStream fos = new java.io.FileOutputStream(sjpeg);
    JPEGImageEncoder encoder = JPEGCodec.createJPEGEncoder(fos, param);
    encoder.encode(bimg);
    fos.close(); 

    sjpeg = null;
    return;
  }
  catch (java.io.FileNotFoundException e) { }
  catch (java.io.IOException e) { e.printStackTrace(); }
}
*/
