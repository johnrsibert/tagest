#include "coff_t.h"
#include "par_t.h"
#include "modlarea.h"

coff_t::coff_t(Model_area& param, int ipar6)
 : a(1,param.n,param.ilb,param.iub), b(1,param.n,param.ilb,param.iub),
 raw_b(1,param.n,param.ilb,param.iub), c(1,param.n,param.ilb,param.iub),
 xbet(1,param.n,param.ilb,param.iub), xgam(1,param.n,param.ilb,param.iub),
 d(1,param.m,param.jlb,param.jub), e(1,param.m,param.jlb,param.jub), 
 f(1,param.m,param.jlb,param.jub), ybet(1,param.m,param.jlb,param.jub),
 ygam(1,param.m,param.jlb,param.jub), g(1,param.m,param.jlb,param.jub),
 imax(param.m), jmax(param.n), psi(1, param.m, param.jlb, param.jub),
 ilb(param.ilb), iub(param.iub), jlb(param.jlb), jub(param.jub)
{
  if (ipar6 > 1)
      k = ipar6;  // time steps per month
  else
      k = 1;

  int JJ;
  if (imax > jmax) 
      JJ = imax;
  else 
      JJ = jmax;

  rr.allocate(1,JJ);
  uu.allocate(1,JJ);
  work.allocate(1,JJ);
  work1.allocate(1,JJ);
}

coff_t::~coff_t()
{
  //cout << "~coff_t" << endl;
}

void coff_t::clean()
{
    a.initialize();
    b.initialize();
    raw_b.initialize();
    c.initialize();
    d.initialize();
    e.initialize();
    f.initialize();
    g.initialize();

    xbet.initialize();
    ybet.initialize();
    xgam.initialize();
    ygam.initialize();

    rr.initialize();
    uu.initialize();
    work.initialize();
    work1.initialize();
    psi.initialize();
}

void coff_t::write(ostream& s, int i, int j)
{
   s << "coff["<<i<<"]["<<j<<"]";
   s << " .a = " << a[j][i];
   s << " .b = " << b[j][i];
   s << " .raw_b = " << raw_b[j][i];
   s << " .c = " << c[j][i];
   s << endl;
   s << "coff["<<i<<"]["<<j<<"]";
   s << " .d = " << d[i][j];
   s << " .e = " << e[i][j];
   s << " .f = " << f[i][j];
   s << " .g = " << g[i][j];
   s << endl;
   s << "coff["<<i<<"]["<<j<<"]";
   s << " .xgam = " << xgam[j][i];
   s << " .xbet = " << xbet[j][i];
   s << " .ygam = " << ygam[i][j];
   s << " .ybet = " << ybet[i][j];
   s << endl;
}
