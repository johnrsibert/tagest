public class format
{
public static String doublewithdecimals(double d, int numDecimals)
{
  String s = String.valueOf(d);
  int len = s.length();
  int i = 0;
  while (i < len)
  {
    if (s.charAt(i) == '.') break;  
    i++;
  }

  //System.out.println("a: " + s);

  if (i < len) //found decimal
  {
    int decimals = len - i - 1;

    if (decimals > numDecimals)
    {
      int index = i + 1 + numDecimals;

      char[] ca = s.toCharArray();
     
      //check if need to round up
      int count = 0;
      while (index < len)
      {
        if (ca[index++] != '9')
          break;

        count++;
 
        if (count >= 4) 
          break;
      }

      index = i + 1 + numDecimals;
      if (count >= 4)
      {
        boolean done = false;
        while (index >= 0)
        {
          switch (ca[index])
          {
            case '.':
              break;
            case '0':
              ca[index] = '1';
              done = true;
              break;
            case '1':
              ca[index] = '2';
              done = true;
              break;
            case '2':
              ca[index] = '3';
              done = true;
              break;
            case '3':
              ca[index] = '4';
              done = true;
              break;
            case '4':
              ca[index] = '5';
              done = true;
              break;
            case '5':
              ca[index] = '6';
              done = true;
              break;
            case '6':
              ca[index] = '7';
              done = true;
              break;
            case '7':
              ca[index] = '8';
              done = true;
              break;
            case '8':
              ca[index] = '9';
              done = true;
              break;
            case '9':
              ca[index] = '0';
              if (index == 0)
              {
                s = "1" + String.valueOf(ca);
                ca = s.toCharArray();
                done = true;
              }
              break;
            default:
              System.out.println("Error: legend.getString found "
                                + ca[index]);
              System.exit(1);
              break;
          };
          if (done) break;
          index--;
        }//while
        if (numDecimals == 0)
          s = String.valueOf(ca).substring(0, i + numDecimals);
        else
          s = String.valueOf(ca).substring(0, i + 1 + numDecimals);
      }
      else
      {
        if (numDecimals == 0)
          s = s.substring(0, index - 1);
        else
          s = s.substring(0, index + numDecimals - 1);
      }
    }
    else if (decimals < numDecimals)
    {
      for (int c = decimals; c < numDecimals; c++) 
        s += "0";
    }
    else if (decimals == numDecimals)
    {
      //do nothing
    }
  }
  else //if (i >= len) //no decimal place
  {
    if (numDecimals > 0)
    {
      s += ".";
      for (int c = 0; c < numDecimals; c++)
        s += "0";
    }
    else if (numDecimals <= 0)
    {
      //do nothing
    }
  }

  //System.out.println("b: " + s);
  return s;
}
}
