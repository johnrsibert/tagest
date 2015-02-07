#include "precomp.h"
#include "intersav.h"
#ifdef SIMULATE
  #undef SIMULATE
#endif
#undef HERE
#define HERE
//#define TRACE(object)
#define TRACE(object) clogf << "line " << __LINE__ << ", file "\
      << __FILE__ << ", " << #object " = " << object << endl;
//#define TTRACE(o1,o2)

extern setsaveinterflag interflag;

extern "C" void tridag(DVECTOR a, DVECTOR b, DVECTOR c, DVECTOR r, DVECTOR u, DVECTOR bet, DVECTOR gam, int nl, int nu, intersavetype * savefile, DVECTOR v);

extern int saveinterflag;
extern ofstream clogf;
void grid_report(dmatrix& t, ofstream& c, const char* name);

/* for debugging derivative calculations */
extern double fsave,dfstep,dsave,deriv;
//extern iswitch;
extern int cr;

void coff_t::adi(dmatrix& u, intersavetype * savefile)
/* change u releases */
{

  #define ERROR 1e-7
  int i,j;

  rr.initialize();
  uu.initialize();
  work.initialize();
//    psi.initialize();

  int nits = k;
  double r  = 4 * k;


  for (int kits = 1; kits <= nits; kits++)
  {
     for (j = 1; j <= jmax; j++)
     {
         int il = ilb(j);
         int iu = iub(j);
         for (i = il; i <= iu; i++)
         {
	     if (j == jlb(i))
	         rr[i]  = g[i][j] + (r-e[i][j])*u[i][j] - f[i][j]*u[i][j+1];
	     else if (j == jub(i))
	         rr[i] = g[i][j] - d[i][j]*u[i][j-1] + (r-e[i][j])*u[i][j];
	     else
	         rr[i] = g[i][j] - d[i][j]*u[i][j-1] + (r-e[i][j])*u[i][j] 
                         - f[i][j]*u[i][j+1];
         }

         tridag(a[j],b[j],c[j],rr,uu,xbet[j],xgam[j],il, iu,savefile,work);

         for (i=il; i<=iu; i++)
         {
	         psi[i][j] = uu[i];
         }
     }//j loop


  #ifdef SIMULATE
    grid_report(psi, clogf, "psi (first half)");
  #endif
    
     for (i=1;i<=imax;i++)
     {
         int jl = jlb(i);
         int ju = jub(i);
         for (j=jl;j<=ju;j++)
         {
	     if (i==ilb(j))
	         rr[j] = g[i][j] + (r-b[j][i])*psi[i][j] - c[j][i]*psi[i+1][j];
	     else if (i==iub(j))
	         rr[j] = g[i][j] - a[j][i]*psi[i-1][j] + (r-b[j][i])*psi[i][j];
	     else
	         rr[j] = g[i][j] - a[j][i]*psi[i-1][j] + (r-b[j][i])*psi[i][j] 
                         - c[j][i]*psi[i+1][j];
         }

         tridag(d[i],e[i],f[i],rr,uu,ybet[i],ygam[i],jl,ju,savefile,work);
         if (saveinterflag > 0)
	         interflag.vsavep(u[i], "u[i]",savefile);
         for (j=jl; j<=ju; j++)
         {
            u[i][j]=uu[j];
         }
         
     }//i loop 
     
  #ifdef SIMULATE
    grid_report(u, clogf, "u (second half)");
  #endif

    if (saveinterflag > 0)
    { 
       interflag.msavep(psi, "psi", savefile );
    }
  } /* end of kits loop */
} /* end of adi */

#ifdef SIMULATE
//#undef TTRACE(o1,o2)
//#define TTRACE(o1,o2) clogf << "line " << __LINE__ << ", file "
//      << __FILE__ << ", " << #o1 " = " << o1<< ", " << #o2 " = " << o2 << endl;
void grid_report(dmatrix& t, ofstream& c, const char* name)
{
  double sum = 0;
  int negatives = 0;
  double tmin = 0;
  double tmax = 0;

  int i1=t.rowmin();
  int i2=t.rowmax();
  for (int i=i1; i<=i2; i++)
  {
    int j1 = t(i).indexmin();
    int j2 = t(i).indexmax();
    for (int j=j1; j<=j2; j++)
    {
      sum += t[i][j];
      if (t[i][j] > tmax)
	tmax = t[i][j];
      if (t[i][j] < 0)
      {
	 negatives ++;
	 if (t[i][j] < tmin)
	   tmin = t[i][j];
      }
    }
  }

  c << "sum of " << name <<" cells = " << sum
    << "; the maximum was " << tmax << endl;
  if (negatives > 0)
  {
    c << "  there were " << negatives << " negatives" << endl;
    c << "  the minimum was " << tmin << endl;
  }
  HERE
}
#endif
