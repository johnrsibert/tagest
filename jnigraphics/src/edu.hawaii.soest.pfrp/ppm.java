class ppm implements Runnable
{
private static boolean loadLibrary = false;
static
{
  try
  { 
    Runtime.getRuntime().loadLibrary("ppm"); 
    loadLibrary = true;
  }
  catch (UnsatisfiedLinkError e)
  {
    e.printStackTrace();
    loadLibrary = false;
    System.out.println("Warning: unable to load ppm");
    //System.exit(1);
  }
}

public ppm() {}
public ppm(String s, int w, int h) 
  { set(s, w, h); }

public void set(String s, int w, int h) 
{ 
  file = s;
  img = new java.awt.image.BufferedImage(w, h, java.awt.image.BufferedImage.TYPE_INT_ARGB);
}

public String file = null;
public java.awt.image.BufferedImage img = null;

native private void toFile(String s, int[] ia, int w, int h);
public void toFile(String s, java.awt.image.BufferedImage img)
{
  if (loadLibrary == false)
  {
    int w = img.getWidth();
    int h = img.getHeight();
    int[] ia = img.getRGB(0, 0, w, h, null, 0, w);
    toFile(s, ia, w, h);
  }
}

public void run()
  { toFile(file, img); }
}
