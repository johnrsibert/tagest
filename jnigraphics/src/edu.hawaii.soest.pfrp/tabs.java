/* Started: 1999.06.19
 *
 * Note:
 *   - Might need to overwrite validate so that all components
 *     of tabs is correctly sized.
 */
import java.awt.*;
import java.awt.event.*;
import java.util.*;

public class tabs extends Container
{
public int id = -1;
public int[] cid = null;

public int numtabs = 0;
private String[] saTitles = null;
private clc holder = new clc();

public class buttons extends dbc 
{
  private int current = 1;
  private int[] iaPositions = null;
  public buttons()
  {
    saTitles = new String[numtabs];
    iaPositions = new int[numtabs];

    //for (int i = 1; i <= numtabs; i++)
    //  saTitles[i - 1] = "tab" + i;

    addMouseListener(new MouseAdapter()
                    {
                      public void mouseClicked(MouseEvent me)
                      {
                        //System.out.println("mouseClicked");
                        int x = me.getX();
                        for (int i = 1; i <= numtabs; i++)
                          if (iaPositions[i - 1] > x)
                          {
                            if (current == i) return;
                            current = i;
                            break;
                          }
                        drawButtons();
                        switchBuffers();
                        repaint();
                      } 
                      public void mousePressed(MouseEvent me)
                      {
                        //System.out.println("mousePressed");
                        int x = me.getX();
                        for (int i = 1; i <= numtabs; i++)
                          if (iaPositions[i - 1] > x)
                          {
                            if (current == i) return;
                            current = i;
                            break;
                          }
                        drawButtons();
                        switchBuffers();
                        repaint();
                      }
                    });

  }

  public Dimension getPreferredSize()
  {
     int ascent = 2   //for active rise 
                  + 1 //space
                  + 1 //line
                  + 1 //dot-line
                  + 2 //space
                  + getFontMetrics(getFont()).getAscent()
                  + 2; //space
     return new Dimension(ascent, ascent); 
  }

/*
  public void validate()
  {
    super.validate();
    drawButtons();
    switchBuffers();
    repaint();
    //System.out.println("tabs.buttons.validate done");
  }
*/
	public void paintButtons() {
	    	drawButtons();
		switchBuffers();
		repaint();	
	}   

  public synchronized void drawButtons()
  {
	allocateBufferedImages();

    Graphics g = getBufferGraphics();
    if (g == null) return;

    int h = getHeight();
    int w = getWidth();
    g.setColor(new Color(204, 204, 204));
    g.fillRect(0, 0, w, h);
    g.setColor(Color.white);
    g.drawLine(0, h - 1, w, h - 1);

    FontMetrics fm = g.getFontMetrics();
    int asc = fm.getAscent();

    int vert = 2     //space
               + asc //font
               + 2;  //space
    int x = 3;
    for (int i = 1; i <= numtabs; i++)
    {
      int sw = 2; //space
      assert saTitles != null;

      String s = saTitles[i - 1];
      if (s != null) 
        sw += fm.stringWidth(s)
              + 2; //space

      if (i == current)
      {
        g.setColor(Color.white);
        x -= 2;
        int x0 = x;
        g.drawLine(x, h - vert - 2, x, h + 1);
        x++;
        g.drawLine(x, h - vert - 1 - 2, x, h - vert - 1 - 2);
        x++;

        if (s != null) 
        {
          g.setColor(Color.black);
          g.drawString(s, 2 + x + 2, h - 2 - 2);
          g.setColor(Color.white);
        }

        g.drawLine(x, h - vert - 2 - 2, 2 + x + sw + 2, h - vert - 2 - 2);
        x += 2 + sw + 2;
        g.setColor(Color.darkGray);
        g.drawLine(x, h - vert - 1 - 2, x, h - vert - 1 - 2);
        x++;
        g.drawLine(x, h - vert - 2, x, h + 1);
        x++;
        g.setColor(new Color(204, 204, 204));
        g.drawLine(x0 + 1, h - 1, x - 2, h - 1);
      }
      else
      {
        g.setColor(Color.white);
        if (i - 1 != current)
        {
          g.drawLine(x, h - vert, x, h);
          x++;
        }
        g.drawLine(x, h - vert - 1, x, h - vert - 1);
        x++;

        if (s != null) 
        {
          g.setColor(Color.black);
          g.drawString(s, x + 2, h - 2 - 1);
          g.setColor(Color.white);
        }

        g.drawLine(x, h - vert - 2, x + sw, h - vert - 2);
        x += sw;
        g.setColor(Color.darkGray);
        g.drawLine(x, h - vert - 1, x, h - vert - 1);
        x++;
        g.drawLine(x, h - vert, x, h - 2);
        x += 2;
      }
      iaPositions[i - 1] = x;
    }
    g.dispose();
    holder.show(current);
  }
}//class buttons

void setTitles(String[] sa)
{
  if (sa == null) return;

  assert sa.length == saTitles.length; 

  for (int i = 0; i < sa.length; i++)
    { saTitles[i] = sa[i]; }
}

void setTitle(int i, String s)
{
  assert 0 <= i && i < saTitles.length; 

  saTitles[i] = s;
}

public class clc extends Container
{
  private CardLayout cl = new CardLayout();

  public clc()
  {
    setLayout(cl);
  }
  
  public void show(int i)
  {
    cl.show(this, String.valueOf(i));

    Container con = containers[i - 1];
    con.getParent().invalidate();
    con.getParent().validate();
    Component[] ca = con.getComponents();
    int size = con.getComponentCount();
    //System.out.println(size);
    
    for (int j = 0; j < size; j++)
    {
      Component com = ca[j];
      if (com.isShowing())
        { com.repaint(); }
    }
  }
}//class clc 

public tabs()
{
  allocateTabs(0);
}

public tabs(int n)
{
  allocateTabs(n);
}
private Container[] containers = null;

public void addContainers(Container[] ca)
{
  assert ca != null;
  assert numtabs >= 0;

  containers = ca;
  
  if (numtabs == 0)
  {
    assert ca.length == 1;
    assert ca[0] != null;

    ca = new Container[1];
    holder.add(ca[0], "1");
  }
  else if (numtabs > 0)
  {
    assert ca.length >= numtabs;

    for (int i = 1; i <= numtabs; i++)
    {
      assert ca[i - 1] != null;
      holder.add(ca[i - 1], String.valueOf(i));
    }
  }
}

private buttons mybuttons = null;

private void allocateTabs(int n)
{
	setLayout(new BorderLayout());
	setNumTabs(n);
	if (numtabs > 0) { 
		mybuttons = new buttons();
		add(mybuttons, "North"); 
	}

	holder = new clc();
	add(holder, "Center");
}

private void setNumTabs(int n)
{
  assert n > 0;
  numtabs = n; 
}

public void paintTabs() {
	mybuttons.paintButtons();
}

public static void main(String[] sa)
{
  int count = 5;
  glc[] glc = new glc[count];
  for (int i = 1; i <= count; i++)
  {
    glc[i - 1] = new glc(1, 1);
    glc[i - 1].add(new javax.swing.JButton("Button" + i), String.valueOf(i));
  }

  tabs mytabs = new tabs(count);
  mytabs.addContainers(glc);
  mytabs.setTitle(2, "two");

  String s = mytabs.getClass().getName();
  javax.swing.JFrame f = new javax.swing.JFrame(s);
  f.getContentPane().setLayout(new BorderLayout());
  f.setSize(500, 300);
  f.getContentPane().add(mytabs, "Center");
  f.setVisible(true);

  for (int i = 0; i < 50; i ++)
  {
	mytabs.paintTabs();
    try
      { Thread.sleep(1000); } catch (InterruptedException ie) { }
  }
}
}
