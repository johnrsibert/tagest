import java.awt.*;
import java.awt.event.*;
//import javax.swing.*;

public class myframe extends Frame //JFrame
{
public myframe(String s)
{
  super(s);
  //getContentPane().setLayout(new BorderLayout()); 
  setLayout(new BorderLayout()); 
  addWindowListener(new WindowAdapter()
                    {
                      public void windowClosing(WindowEvent e)
                      {
                        System.out.println("java "
                                           + getTitle()
                                           + " closed. ");
                        //System.exit(1);
                        dispose();
                      }//windowClosed(WindowEvent)
                    }
                   );                  
}

public void update(Graphics g)
  { paint(g); }

public void paint(Graphics g)
  { super.paint(g); }

//int i = 0;
public void validate()
{
  //System.out.println("validate");
  super.validate();
  //System.out.println(i);
  //i++;
}

public void invalidate()
{
  //System.out.println("invalidate");
  super.invalidate();
}

public static void main(String[] sa)
{
  myframe f = new myframe("myframe");
  f.setSize(500, 300);
  f.setVisible(true);
}
}
