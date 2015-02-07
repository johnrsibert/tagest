import java.awt.*;

public class statusbar extends dbc
{
private String s = null;
public statusbar() {}

public Dimension getPreferredSize()
{
	int h = 4 + getFontMetrics(getFont()).getAscent() + 2;
	return new Dimension(0, h);
}

public void setString(String s)
	{ this.s = new String(s); }

public void drawStatusBar()
{
	allocateBufferedImages();
	//printBuffers();
	//System.out.println(isValid());

	Graphics g = getBufferGraphics();
	if (g == null) { 
		return;
	}

	int h = getHeight();
	int w = getWidth();
	g.setColor(new Color(204, 204, 204));
	g.fillRect(0, 0, w, h);
	g.setColor(Color.white);
	g.drawLine(0, 0, w, 0);
	g.setColor(Color.black);
	if (s != null) 
		{ g.drawString(s, 2, h - 2); }
	g.dispose();
}

public void drawStatusBar(String s)
{
	setString(s);
	drawStatusBar();
}

public static void main(String[] sa)
{
  statusbar mystatusbar = new statusbar();

  myframe f = new myframe(mystatusbar.getName());
  f.setLayout(new BorderLayout());
  f.add(mystatusbar, "South");
  f.setSize(200, 300);
  f.setVisible(true);
 
  for (int i = 0; i < 50; i ++)
  {
    mystatusbar.setString(String.valueOf(i) + "<-----"); 
    mystatusbar.drawStatusBar();
    mystatusbar.switchBuffers();
    mystatusbar.repaint();
  
    try
      { Thread.sleep(1000); } catch (InterruptedException ie) { }
  }
}
}
