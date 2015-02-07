
import java.awt.Color;

public class legenddef
{
public final static int NONE = -1;
public final static int UNIFORM = 0;
public final static int EXPONENTIAL = 1;

public double min         = 1;
public double max         = 100;
public double increment   = 5;
public int    decimals    = 0;
public int    type        = legenddef.NONE;

Color colorsLegend[] = 
  { Color.blue, Color.cyan, Color.green, Color.yellow, Color.red };

public legenddef() { /*DoNothing*/ } 
public legenddef(legenddef def) 
{
  if (def != null)
  {
    min = def.min;
    max = def.max;
    increment = def.increment;
    decimals = def.decimals;
    type = def.type;

    if (def.colorsLegend != null && def.colorsLegend.length > 0)
    {
      int size = def.colorsLegend.length;
      colorsLegend = new Color[size];
      for (int i = 0; i < size; i++)
      { 
        Color c = def.colorsLegend[i]; 
        colorsLegend[i] = new Color(c.getRed(), c.getGreen(), c.getBlue());
      }
    }
  }
}
}
