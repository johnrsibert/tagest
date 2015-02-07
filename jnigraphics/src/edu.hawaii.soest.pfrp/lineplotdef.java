
import java.awt.Color;

public class lineplotdef
{
public String title = null;

public int   linesTotal = 0;
public Color linesColors[] = null;

public String yaxisTitle     = null;
public double yaxisMin       = 0;
public double yaxisMax       = 0;
public double yaxisIncrement = 0;
public int    yaxisDecimals  = 0;

public String xaxisTitle     = null;
//public int    xaxisMin       = 0;
public int    xaxisMax       = 0;
public int    xaxisIncrement = 0;

public lineplotdef() { /*DoNothing*/ }
public lineplotdef(lineplotdef def)
{
  if (def != null)
  {
    title = title != null ? new String(def.title) : null;

    linesTotal = def.linesTotal;

    if (linesTotal > 0)
    {
      linesColors = new Color[linesTotal];
      java.util.Random r = new java.util.Random();
      for (int i = 0; i < linesTotal; i++)
      { 
        linesColors[i] = 
          new Color(r.nextInt(255), r.nextInt(255), r.nextInt(255));
      }
      r = null;

      if (def.linesColors != null && linesTotal <= def.linesColors.length)
      {
        for (int i = 0; i < linesTotal; i++)
        { 
          Color c = def.linesColors[i];
          if (c != null)
            linesColors[i] = new Color(c.getRed(), c.getGreen(), c.getBlue());
        }
      }
    }

    yaxisTitle = def.yaxisTitle != null ? new String(def.yaxisTitle) : null;
    yaxisMin = def.yaxisMin;
    yaxisMax = def.yaxisMax;
    yaxisIncrement = def.yaxisIncrement;
    yaxisDecimals = def.yaxisDecimals;

    xaxisTitle = def.xaxisTitle != null ? new String(def.xaxisTitle) : null;
//xaxisMin = def.xaxisMin;
    xaxisMax = def.xaxisMax;
    xaxisIncrement = def.xaxisIncrement;
  }
}
public void setColor(Color c, int line)
{
  if (c != null && 0 <= line && line < linesTotal) 
  {
    if (linesColors == null)
    {
      java.util.Random r = new java.util.Random();
      linesColors = new Color[linesTotal];
      for (int i = 0; i < linesTotal; i++)
      {
        linesColors[i] = 
          new Color(r.nextInt(255), r.nextInt(255), r.nextInt(255));
      }
      r = null;
    }

    linesColors[line] = new Color(c.getRed(), c.getGreen(), c.getBlue());
  }
}
}
