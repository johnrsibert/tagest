
import java.awt.Color;

public class histogramdef
{
public String title = null;

public String yaxisTitle     = null;
public double yaxisMin       = 0;
public double yaxisMax       = 0;
public double yaxisIncrement = 0;
public int    yaxisDecimals  = 0;

public String barTitle    = null;
public int    barTotal    = 0;
public int    barStack    = 1; //default
public String barLabels[] = null;
public Color  barColors[] = null;
public int    barSpacing  = 0;
public int    barWidth    = 0;

public histogramdef() { /*DoNothing*/ }
public histogramdef(histogramdef def)
{
  if (def != null)
  {
    title = def.title != null ? new String(def.title) : null;

    yaxisTitle = def.yaxisTitle != null ? new String(def.yaxisTitle) : null;
    yaxisMin = def.yaxisMin;
    yaxisMax = def.yaxisMax;
    yaxisIncrement = def.yaxisIncrement;
    yaxisDecimals = def.yaxisDecimals;

    barTitle = def.barTitle != null ? new String(def.barTitle) : null;
    barTotal = def.barTotal;
    barStack = def.barStack >= 1 ? def.barStack : 1;

    barColors = null;
    barColors = new Color[barStack];
    if (def.barColors != null)
    {

      if (barColors.length == def.barColors.length)
      {
        for (int i = 0; i < barStack; i++)
        {   
          Color c = def.barColors[i]; 
          barColors[i] = new Color(c.getRed(), c.getGreen(), c.getBlue());
        }
      }
    }

    {
      java.util.Random r = new java.util.Random();
      for (int i = 0; i < barStack; i++)
      {
        if (barColors[i] == null)
          barColors[i] = 
            new Color(r.nextInt(255), r.nextInt(255), r.nextInt(255));
      }
      r = null;
    }

    barSpacing = def.barSpacing;
    barWidth   = def.barWidth;

    if (def.barLabels != null && def.barLabels.length == barTotal)
    {
      if (barLabels == null)
        { barLabels = new String[barTotal]; }

      for (int i = 0; i < barTotal; i++)
      {
        String s = def.barLabels[i];
        if (s != null)
          barLabels[i] = new String(s);
      }
    }
  }
}

public void setColor(Color c, int stack)
{
  if (c != null && 0 <= stack && stack < barStack) 
  {
    if (barColors == null)
    {
      java.util.Random r = new java.util.Random();
      barColors = new Color[barStack];
      for (int i = 0; i < barStack; i++)
      {
        barColors[i] = 
          new Color(r.nextInt(255), r.nextInt(255), r.nextInt(255));
      }
      r = null;
    }

    barColors[stack] = new Color(c.getRed(), c.getGreen(), c.getBlue());
  }
}

public void setLabel(String label, int bar)
{
  if (label != null && 0 <= bar && bar < barTotal)
  {
    if (barLabels == null)
      { barLabels = new String[barTotal]; }

    barLabels[bar] = new String(label);
  }
}
}
