#include "precomp.h"
#include "par_t.h"
#include <prnstrem.h>
  #ifdef unix
#include <strstream>
  #else
#include <strstrea.h>
#endif

#include "trace.h"
#pragma option -Vd
extern ofstream clogf;
// observed displacements from recapture file
void displacement_frequency(recaptype_vector& recaps, int nrecaps, par_t& param,
                            dmatrix& displacements, 
                            dmatrix& xdisp, dmatrix& ydisp, 
                            const dmatrix& distances, const dvector& bins)
{
  int b1 = bins.indexmin();
  int b2 = bins.indexmax();
  displacements.initialize();
  xdisp.initialize();
  ydisp.initialize();
  int kmax = displacements(0).indexmax();
  int nrelease = param.get_nrelease();
  int max_month = displacements.rowmax();
  int month = 0;
  for (int kc = 1; kc <= nrecaps; kc++)
  {
     int cohort = recaps[kc].cohort;
     int i = recaps[kc].i;
     int j = recaps[kc].j;
     for (int cc = 1; cc <= nrelease; cc++)
     {
       if (cohort == param.get_tr_cohort(cc))
       {
         break;
       }
     }
     int tal = recaps[kc].date - param.get_tr_date(cc);
     if (tal > max_month)
     {
       tal = max_month;
     }
     int irel = param.get_tr_i(cc);
     int jrel = param.get_tr_j(cc);
     int ii = i-irel;
     int jj = j-jrel;
     xdisp(tal,ii) += recaps[kc].returns;
     ydisp(tal,jj) += recaps[kc].returns;

     ii = abs(ii);
     jj = abs(jj);

     if (distances(ii,jj) >= bins(b2))
     {
       displacements(b2) += recaps[kc].returns; 
     }
     else
     {
       for (int b = b1; b < b2; b++)
       {
         if (distances(ii,jj) <= bins(b))
         {
            displacements(b) += recaps[kc].returns; 
            break;
         }
       }
     } 
  }
}

// diplacements of non-recaptured tags at liberty
void displacement_frequency(const int cohort, const dmatrix& tags, 
                        dvector& displacements, dvector& xdisp, dvector& ydisp, 
                        const dmatrix& distances, const dvector& bins, 
                        par_t& param)
{
  int irel = param.get_tr_i(cohort);
  int jrel = param.get_tr_j(cohort);

  int b1 = bins.indexmin();
  int b2 = bins.indexmax();

  int i1 = tags.rowmin();
  int i2 = tags.rowmax();
  for (int i = i1; i <= i2; i++)
  {
    int j1 = tags(i).indexmin();
    int j2 = tags(i).indexmax();
    for (int j = j1; j <= j2; j++)
    {
      int ii = i-irel;
      xdisp(ii) += tags(i,j);
      int jj = j-jrel;
      ydisp(jj) += tags(i,j);

      ii = abs(ii);
      jj = abs(jj);
 
      if (distances(ii,jj) >= bins(b2))
      {
        displacements(b2) += tags(i,j);
      }
      else
      {
        for (int b = b1; b < b2; b++)
        {
          if (distances(ii,jj) <= bins(b))
          {
             displacements(b) += tags(i,j);
             break;
          }
        }
      }
    }
  }
}

dvector displacement_bins(par_t& param, dmatrix& distances)
{
  dvector gins ("{0.00000,60.00000,84.85281,120.00000,134.16408,169.70563,\
     180.00000,189.73666,216.33308,240.00000,247.38634,254.55844,\
     268.32816,300.00000,305.94117,323.10989,339.41125,349.85711,\
     360.00000,364.96575,379.47332,384.18745,402.49224,420.00000,\
     424.26407,432.66615,436.80659,456.94639,468.61498,480.00000,\
     483.73546,494.77268,509.11688,512.64022,516.13952,536.65631,\
     540.00000,543.32311,553.17267,566.03887,569.20998,590.93147,\
     593.96970,600.00000,602.99254,611.88234,617.73781,626.41839,\
     637.80875,646.21978,648.99923,660.00000,662.72166,670.82039,\
     678.82251,684.10526,699.71423,702.28199,720.00000,722.49567,\
     724.98276,729.93150,732.39334,742.15901,751.79785,758.94664,\
     763.67532,768.37491,780.00000,782.30429,789.17679,800.49984,\
     804.98447,807.21744,816.08823,833.54664,835.70330,840.00000,\
     842.14013,848.52814,852.76022,859.06926,865.33231,873.61319,\
     885.88938,891.96412,900.00000,901.99778,907.96476,913.89277,\
     915.86025,917.82351,931.45048,933.38095,937.22996,939.14855,\
     948.68330,960.00000,961.87317,967.47093,969.32967,976.72924,\
     984.07317,989.54535,993.17672,998.59902,1005.78328,1018.23376,\
    1020.00000,1021.76318,1025.28045,1027.03457,1032.27903,1035.76059,\
    1047.85495,1049.57134,1061.50836,1063.20271,1068.26963,1073.31263,\
    1080.00000,1081.66538,1086.64622,1094.89726,1101.45359,1103.08658,\
    1106.34533,1116.06451,1120.89250,1127.29765,1132.07774,1138.41996,\
    1140.00000,1141.57786,1146.29839,1152.56236,1154.12304,1158.79247,\
    1164.98927,1178.81296,1181.86294,1183.38498,1187.93939,1190.96599,\
    1195.49153,1200.00000,1300,1400,1500,1600,1700,1800,1900,2000}");
   /*1201.49906,1205.98507,1207.47671,1213.42490,\
    1214.90740,1223.76468,1231.09707,1235.47562,1236.93169,1248.51912,\
    1252.83678,1260.00000,1261.42776,1265.70139,1271.37721,1272.79221,\
    1275.61750,1282.65350,1284.05607,1288.25463,1292.43955,1295.22199,\
    1297.99846,1310.41978,1315.90273,1317.26990,1320.00000,1321.36293,\
    1325.44332,1328.15662,1332.21620,1341.64079,1348.33230,1353.66170,\
    1357.64502,1360.29409,1368.21051,1369.52547,1370.83916,1380.00000,\
    1381.30373,1385.20757,1391.68962,1395.56440,1399.42845,1400.71410,\
    1404.56399,1405.84494,1412.23228,1416.05085,1422.39235,1426.18372,\
    1431.22325,1440.00000,1441.24946,1442.49783,1444.99135,1449.96552,\
    1451.20639,1452.44621,1459.86301,1461.09548,1464.78667,1470.91808,\
    1475.80487,1481.89068,1484.31803,1485.53021,1490.36908,1500.00000,\
    1501.19952,1503.59569,1504.79234,1510.76140,1514.33154,1517.89328,\
    1519.07867,1527.35065,1529.70585,1533.23188,1536.74982,1537.92067,\
    1542.59522,1548.41855,1556.53461,1557.69060,1560.00000,1561.15342,\
    1564.60858,1570.35028,1574.92857,1578.35357,1584.04545,1585.18138,\
    1588.58428,1594.23963,1597.62323,1600.99969,1609.96894,1612.20346,\
    1614.43489,1615.54944,1620.00000,1621.11073,1624.43836,1629.96932,\
    1632.17646,1637.68129,1638.78003,1647.54363,1650.81798,1655.17371,\
    1659.51800,1663.85095,1667.09328,1668.17265,1671.40659,1673.55908,\
    1680.00000,1681.07109,1684.28026,1689.61534,1690.68034,1693.87131,\
    1697.05627,1698.11660,1699.17627,1705.52045,1706.57552,1707.62994,\
    1716.04196,1718.13853,1719.18585,1727.54161,1730.66461,1731.70436,\
    1740.00000,1741.03418,1744.13302,1747.22637,1749.28557,1752.36982,\
    1756.47374,1764.65294,1765.67268,1771.77877,1772.79440,1776.85115,\
    1780.89865,1781.90909,1783.92825,1789.97207,1797.99889,1800.00000,\
    1800.99972,1803.99557,1804.99307,1808.97761,1813.94597,1815.92951,\
    1821.86717,1824.82876,1827.78555,1828.77008,1831.72050,1835.64702,\
    1836.62734,1840.54340,1848.35062,1852.24188,1853.21343,1860.96749,\
    1862.90096,1863.86695,1866.76190,1868.68938,1874.45992,1878.29710,\
    1879.25517,1883.08258,1884.03822,1897.36660,1905.88562,1906.82983,\
    1909.65966,1913.42625,1914.36674,1917.18544,1920.93727,1932.14906,\
    1934.94186,1938.65933,1946.99769,1951.61472,1953.45847,1958.97933,\
    1961.73393,1965.40072,1968.14634,1980.90888,1986.35344,1987.25942,\
    1994.49242,1997.19804,1998.09910,2005.29300,2012.46118,2016.03571,\
    2016.92836,2030.27092,2036.46753,2038.23453,2040.00000,2043.52636,\
    2047.92578,2052.31577,2064.55806,2068.91276,2079.32681,2080.19230,\
    2082.78659,2089.68897,2099.14268,2100.00000,2113.66979,2121.32034,\
    2123.01672,2128.09774,2130.63371,2136.53926,2148.30165,2163.33077,\
    2164.16266,2167.48702,2174.12051,2184.03297,2197.18001,2206.17316,\
    2207.80434,2212.69067,2220.81066,2232.12903,2248.99978,2252.19893,\
    2258.58363,2268.12698,2291.02597,2292.59678,2297.30277,2305.12473,\
    2333.83804,2336.92105,2343.07490,2375.87878,2377.39353,2381.93199,\
    2418.67732,2421.65233,2460.73160,2462.19414,2503.51753,2545.58441}"); */
    
  TTRACE(gins.indexmin(),gins.indexmax())
  int d1 = gins.indexmin();
  int d2 = gins.indexmax();
  dvector bins(d1,d2);
  for (int d = d1; d < d2; d++)
  {
    bins(d) = gins(d)+0.5*(gins(d+1)-gins(d));
  }
  bins(d2) = gins(d2);
  int m = param.get_m();
  int n = param.get_n();
  const double dx = param.get_deltax_prn();
  const double dy = param.get_deltay(); 
  for (int i = 0; i <= m; i++)
  {
    double x = i*dx;
    for (int j = 0; j <= n; j++)
    {
      double y = j*dy;
      distances(i,j) = sqrt(x*x+y*y);
    }
  }
  return(bins);
}

void write_displacement_files(const dmatrix& displacement, 
                         dmatrix& xdisp, dmatrix& ydisp, 
                         const dvector& bins,
                         const int fit_number, const char op)
{
  char fit_name[80];
  std::ostrstream ss(fit_name,80);
  ss << "fit" << setw(2) << setfill('0') << hex << fit_number << '_' << op << ends;
  
  adstring displacement_name = fit_name+adstring("displ.prn");
  TRACE(displacement_name)
  prnstream dfp(displacement_name);
  dfp << "displacement frequencies" << PENDL;
  dfp << "months at"  << "net displacement (Nmi) ->" << PENDL;
  dfp << "liberty";
  int d1 = bins.indexmin();
  int d2 = bins.indexmax();
  int d = 0;
  for (d = d1; d < d2; d+=2)
  {
    //dfp << bins(d)+0.5*(bins(d+1)-bins(d));
    dfp << bins(d+1);
  }
  dfp << PENDL;
  int max_month = displacement.rowmax();
  int month = 0;
  for (month = 0; month <= max_month; month++)
  {
    dfp << month;
    for (d = d1; d < d2; d+=2)
    {
      //dfp << displacement(month,d); 
      dfp << (displacement(month,d)+displacement(month,d+1)); 
    }
    dfp << PENDL;
  }
  dfp.close();

  
  displacement_name = fit_name+adstring("Xdispl.prn");
  TRACE(displacement_name)
  prnstream dfx(displacement_name);
  dfx << "E-W displacement frequencies" << PENDL;
  dfx << "months at"  << "net displacement (Nmi) ->" << PENDL;
  dfx << "liberty";
  d1 = xdisp(0).indexmin();
  d2 = xdisp(0).indexmax();
  for (d = d1; d <= d2; d++)
  {
    dfx << (d*60); //delta_x
  }
  dfx << PENDL;
  max_month = displacement.rowmax();
  for (month = 0; month <= max_month; month++)
  {
    dfx << month;
    for (d = d1; d <= d2; d++)
    {
      dfx << xdisp(month,d); 
    }
    dfx << PENDL;
  }
  dfx.close();

  displacement_name = fit_name+adstring("Ydispl.prn");
  TRACE(displacement_name)
  prnstream dfy(displacement_name);
  dfy << "N-S displacement frequencies" << PENDL;
  dfy << "months at"  << "net displacement (Nmi) ->" << PENDL;
  dfy << "liberty";
  d1 = ydisp(0).indexmin();
  d2 = ydisp(0).indexmax();
  for (d = d1; d <= d2; d++)
  {
    dfy << (d*60); //delta_y
  }
  dfy << PENDL;
  max_month = displacement.rowmax();
  for (month = 0; month <= max_month; month++)
  {
    dfy << month;
    for (d = d1; d <= d2; d++)
    {
      dfy << ydisp(month,d); 
    }
    dfy << PENDL;
  }
  dfy.close();
}
