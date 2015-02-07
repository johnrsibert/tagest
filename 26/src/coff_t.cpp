//$Id: coff_t.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "coff_t.h"
#include "par_t.h"
#include "modlarea.h"
#include "trace.h"


 
coff_t::coff_t(Model_area& param, int ipar6)
	:_a(1,param.n,param.ilb,param.iub),
	 _b(1,param.n,param.ilb,param.iub),
	 _raw_b(1,param.n,param.ilb,param.iub),
	 _c(1,param.n,param.ilb,param.iub),
	 _d(1,param.m,param.jlb,param.jub),
	 _e(1,param.m,param.jlb,param.jub), 
	 _f(1,param.m,param.jlb,param.jub),
	 _g(1,param.m,param.jlb,param.jub),
	 _ybet(1,param.m,param.jlb,param.jub),
	 _ygam(1,param.m,param.jlb,param.jub),
	 _xbet(1,param.n,param.ilb,param.iub),
	 _xgam(1,param.n,param.ilb,param.iub),
	 _psi(1, param.m, param.jlb, param.jub),
	 imax(param.m),
	 jmax(param.n),
	 ilb(param.ilb),
	 iub(param.iub),
	 jlb(param.jlb),
	 jub(param.jub)
{
	k = ipar6 > 1 ?  ipar6 : 1;// time steps per month
	const int JJ = imax > jmax ? imax : jmax;
	_rr.allocate(1,JJ);
	_uu.allocate(1,JJ);
	_work.allocate(1,JJ);
	_work1.allocate(1,JJ);
}

 
void coff_t::clean()
{
	_a.initialize();
	_b.initialize();
	_raw_b.initialize();
	_c.initialize();
	_d.initialize();
	_e.initialize();
	_f.initialize();
	_g.initialize();
	_xbet.initialize();
	_ybet.initialize();
	_xgam.initialize();
	_ygam.initialize();
	_rr.initialize();
	_uu.initialize();
	_work.initialize();
	_work1.initialize();
	_psi.initialize();
}
 
void coff_t::write(ostream& s, int i, int j)
{
   s << "coff["<<i<<"]["<<j<<"]";
   s << " .a = " << _a[j][i];
   s << " .b = " << _b[j][i];
   s << " .raw_b = " << _raw_b[j][i];
   s << " .c = " << _c[j][i];
   s << endl;
   s << "coff["<<i<<"]["<<j<<"]";
   s << " .d = " << _d[i][j];
   s << " .e = " << _e[i][j];
   s << " .f = " << _f[i][j];
   s << " .g = " << _g[i][j];
   s << endl;
   s << "coff["<<i<<"]["<<j<<"]";
   s << " .xgam = " << _xgam[j][i];
   s << " .xbet = " << _xbet[j][i];
   s << " .ygam = " << _ygam[i][j];
   s << " .ybet = " << _ybet[i][j];
   s << endl;
}
