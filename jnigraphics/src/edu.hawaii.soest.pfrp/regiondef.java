import java.awt.Color;

public class regiondef
{
public int    m  =  0,  
              n  =  0;
public int    dx = 60, 
              dy = 60; //defaults to 1 degree squares

//titleFont
public String title = null;

public Color  coastlinesBoundaryColor = new Color(24, 180, 24);
public Color  coastlinesFillColor = new Color(0, 255, 0, 80);
public String coastlinesFile = null;

public Color colorLine = Color.blue;
public int[] iaAreas  = null;

public regiondef() { /*DoNothing*/ } 
public regiondef(regiondef def) 
{
  if (def != null)
  {
    m = def.m;
    n = def.n;
    dx = def.dx;
    dy = def.dy;

    title = def.title != null 
            ? new String(def.title) 
            : null;

    coastlinesBoundaryColor = null;
    if (def.coastlinesBoundaryColor != null)
    { 
      Color c = def.coastlinesBoundaryColor;
      coastlinesBoundaryColor = 
        new Color(c.getRed(), c.getGreen(), c.getBlue());
    }

    coastlinesFillColor = null;
    if (def.coastlinesFillColor != null)
    { 
      Color c = def.coastlinesFillColor;
      coastlinesFillColor = 
        new Color(c.getRed(), c.getGreen(), c.getBlue(), c.getAlpha());
    }

    coastlinesFile = null;
    if (def.coastlinesFile != null)
      { coastlinesFile = new String(def.coastlinesFile); }

    colorLine = null;
    if (def.colorLine != null)
    { 
      Color c = def.colorLine; 
      colorLine =
        new Color(c.getRed(), c.getGreen(), c.getBlue());
    }

    if (def.iaAreas != null && def.iaAreas.length > 0)
    {
      int size = def.iaAreas.length;
      iaAreas = new int[size];
      for (int i = 0; i < size; i++)
        { iaAreas[i] = def.iaAreas[i]; }
    }
  }
}
}
