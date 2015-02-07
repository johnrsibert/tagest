//$Id: dfadi2.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "coff_t.h"
#include "intersav.h"

extern ofstream clogf;
template <typename VECTOR>
void tridag(const VECTOR& a, const VECTOR& b, const VECTOR& c, const VECTOR& r, VECTOR& u, const VECTOR& bet, const VECTOR& gam, const int nl, const int nu, VECTOR& v);
void dftridag(dvector& a, dvector& b, dvector& c,dvector& r, dvector& bet, dvector& gam, dvector& dfa,dvector& dfb,dvector& dfc,dvector& dfr, dvector& dfbet, dvector& dfgam, dvector& u, dvector& dfu, int n1, int n2, dvector& v, dvector& dfv, dvector& w, dvector& dfw);

extern setsaveinterflag interflag;

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::dfadi(coff_t& coff, dmatrix& u, dmatrix& dfu)
{
  #define ERROR 1e-7

  dmatrix& a = coff.get_a();
  dmatrix& b = coff.get_b();
  dmatrix& c = coff.get_c();
  dmatrix& d = coff.get_d();
  dmatrix& e = coff.get_e();
  dmatrix& f = coff.get_f();
  dmatrix& g = coff.get_g();
  dmatrix& xbet = coff.get_xbet();
  dmatrix& ybet = coff.get_ybet();
  dmatrix& xgam = coff.get_xgam();
  dmatrix& ygam = coff.get_ygam();
  dmatrix& psi  = coff.get_psi();

  dmatrix& dfa = get_a();
  dmatrix& dfb = get_b();
  dmatrix& dfc = get_c();
  dmatrix& dfd = get_d();
  dmatrix& dfe = get_e();
  dmatrix& dff = get_f();
  dmatrix& dfg = get_g();
  dmatrix& dfxbet = get_xbet();
  dmatrix& dfybet = get_ybet();
  dmatrix& dfxgam = get_xgam();
  dmatrix& dfygam = get_ygam();
  dmatrix& dfpsi  = get_psi(); dfpsi.initialize();
 
  dvector& rr = coff.get_rr(); //rr.initialize();
  dvector& uu = coff.get_uu(); //uu.initialize();

  dvector& dfrr = get_rr(); dfrr.initialize();
  dvector& dfuu = get_uu(); dfuu.initialize();

  dvector& work = coff.get_work();// work.initialize(); 
  dvector& work1 = coff.get_work1();// work1.initialize();

  dvector& dfwork = get_work(); dfwork.initialize();
  dvector& dfwork1 = get_work1(); dfwork1.initialize();
	dfadi(	a, dfa,
		b, dfb,
		c, dfc,
		d, dfd,
		e, dfe,
		f, dff,
		g, dfg,
		xbet, dfxbet,
		xgam, dfxgam,
		ybet, dfybet,
		ygam, dfygam,
		psi, dfpsi,
		rr, dfrr,
		uu, dfuu,
		work, dfwork,
		work1, dfwork1,
		u, dfu);
}
template void coff_t<d3_array,dmatrix,dvector,double>::dfadi(coff_t& coff, dmatrix& u, dmatrix& dfu);
template<> void coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::dfadi(coff_t& coff, dmatrix& u, dmatrix& dfu) {}

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::dfadi(dmatrix &a, dmatrix &dfa, dmatrix &b, dmatrix &dfb, dmatrix &c, dmatrix &dfc, dmatrix &d, dmatrix &dfd, dmatrix &e, dmatrix &dfe, dmatrix &f, dmatrix &dff, dmatrix &g, dmatrix &dfg, dmatrix &xbet, dmatrix &dfxbet, dmatrix &xgam, dmatrix &dfxgam, dmatrix &ybet, dmatrix &dfybet, dmatrix &ygam, dmatrix &dfygam, dmatrix &psi, dmatrix& dfpsi, dvector &rr, dvector &dfrr, dvector &uu, dvector &dfuu, dvector &work, dvector &dfwork, dvector &work1, dvector &dfwork1, dmatrix& u, dmatrix& dfu)
{
  int nits = k;
  double r  = 4 * k;
  int i, j;

  dmatrix u2;
  u2.allocate(u);
  u2 = u;
  for (int kits=nits; kits>=1; kits--)
  {
u = u2;
for (int x = 1; x < kits; x++) {
     for (j = 1; j <= jmax; j++) {
         int il = ilb(j);
         int iu = iub(j);
         for (i = il; i <= iu; i++) {
             if (j == jlb(i))
                 rr[i]  = g[i][j] + (r-e[i][j])*u[i][j] - f[i][j]*u[i][j+1];
             else if (j == jub(i))
                 rr[i] = g[i][j] - d[i][j]*u[i][j-1] + (r-e[i][j])*u[i][j];
             else
                 rr[i] = g[i][j] - d[i][j]*u[i][j-1] + (r-e[i][j])*u[i][j] - f[i][j]*u[i][j+1];
         }
         tridag<VECTOR>(a[j],b[j],c[j],rr,uu,xbet[j],xgam[j],il, iu,work);
         for (i=il; i<=iu; i++) {
                 psi[i][j] = uu[i];
         }
     }

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
                 rr[j] = g[i][j] - a[j][i]*psi[i-1][j] + (r-b[j][i])*psi[i][j] - c[j][i]*psi[i+1][j];
         }
                                                                                                                                                                                                    
         tridag<VECTOR>(d[i],e[i],f[i],rr,uu,ybet[i],ygam[i],jl,ju,work);
         for (j=jl; j<=ju; j++)
         {
            u[i][j]=uu[j];
         }
     }
}
     for (j = 1; j <= jmax; j++) {
         int il = ilb(j);
         int iu = iub(j);
         for (i = il; i <= iu; i++) {
             if (j == jlb(i))
                 rr[i]  = g[i][j] + (r-e[i][j])*u[i][j] - f[i][j]*u[i][j+1];
             else if (j == jub(i))
                 rr[i] = g[i][j] - d[i][j]*u[i][j-1] + (r-e[i][j])*u[i][j];
             else
                 rr[i] = g[i][j] - d[i][j]*u[i][j-1] + (r-e[i][j])*u[i][j] - f[i][j]*u[i][j+1];
         }
         tridag<VECTOR>(a[j],b[j],c[j],rr,uu,xbet[j],xgam[j],il, iu,work);
         for (i=il; i<=iu; i++) {
                 psi[i][j] = uu[i];
         }
     }
   
      for (i=imax; i>=1; i--)
      {
          int jl = jlb(i);
          int ju = jub(i);
          for (j=jl;j<=ju;j++)
          {
	     if (i==ilb(j))
	       rr[j] = g[i][j] - (b[j][i]-r)*psi[i][j] 
                         - c[j][i]*psi[i+1][j];
	     else if (i==iub(j))
	       rr[j] = g[i][j] - a[j][i]*psi[i-1][j] 
                         - (b[j][i]-r)*psi[i][j];
	     else
	       rr[j]= g[i][j] - a[j][i]*psi[i-1][j] 
                        - (b[j][i]-r)*psi[i][j] - c[j][i]*psi[i+1][j];
          } // j loop

          for (j=jl; j<=ju; j++)
          {
//           u[i][j] = uu[j];
             dfuu[j] += dfu[i][j];

             dfu[i][j]=0;
          }


          dfwork.initialize();
          dfwork1.initialize();
          dftridag(d[i], e[i], f[i], rr, ybet[i], ygam[i], 
                   dfd[i], dfe[i], dff[i], dfrr, dfybet[i], dfygam[i], 
                   uu, dfuu, jl, ju, work, dfwork, work1, dfwork1);

          for (j=ju; j>=jl; j--)
          {
	      if (i==ilb(j))
	      {
//                rr[j]=g[i][j] - (b[j][i]-r)*psi[i][j] - c[j][i]*psi[i+1][j];

	          dfg[i][j]+=dfrr[j];
	          dfb[j][i]-=psi[i][j]*dfrr[j];
	          dfpsi[i][j]-=(b[j][i]-r)*dfrr[j];
	          dfc[j][i]-=psi[i+1][j]*dfrr[j];
	          dfpsi[i+1][j]-=c[j][i]*dfrr[j];
	      }
 	      else 
              {
                  if (i==iub(j))
	          {
//                 rr[j]=g[i][j] - a[j][i]*psi[i-1][j] - (b[i][i]-r)*psi[i][j];

	            dfg[i][j]+=dfrr[j];
	            dfa[j][i]-=psi[i-1][j]*dfrr[j];
	            dfpsi[i-1][j]-=a[j][i]*dfrr[j];
	            dfb[j][i]-=psi[i][j]*dfrr[j];
	            dfpsi[i][j]-=(b[j][i]-r)*dfrr[j];
	          }
	          else
	          {
//            rr[j]=g[i][j] - a[j][i]*psi[i-1][j] - (b[j][i]-r)*psi[i][j]
//                          - c[j][i]*psi[i+1][j];

	            dfg[i][j]+=dfrr[j];
	            dfa[j][i]-=psi[i-1][j]*dfrr[j];
	            dfpsi[i-1][j]-=a[j][i]*dfrr[j];
	            dfb[j][i]-=psi[i][j]*dfrr[j];
	            dfpsi[i][j]-=(b[j][i]-r)*dfrr[j];
	            dfc[j][i]-=psi[i+1][j]*dfrr[j];
	            dfpsi[i+1][j]-=c[j][i]*dfrr[j];
                  }
	          }
	        dfrr[j] = 0.0;
          }//j loop
      }//i loop
    

      for (j=jmax; j>=1; j--)
      {
          int il = ilb(j);
          int iu = iub(j);

          for (i=il; i<=iu; i++)
          {
            if (j==jlb(i))
                rr[i]=g[i][j] - (e[i][j]-r)*u[i][j] - f[i][j]*u[i][j+1];
            else if (j==jub(i))
                rr[i]=g[i][j] - d[i][j]*u[i][j-1] - (e[i][j]-r)*u[i][j];
            else
                rr[i]=g[i][j] - d[i][j]*u[i][j-1] - (e[i][j]-r)*u[i][j] 
                        - f[i][j]*u[i][j+1];
          } // i loop

          for (i=iu; i>=il; i-- )
          {
//           psi[i][j] = uu[i];

             dfuu[i]+=dfpsi[i][j];
             dfpsi[i][j]=0;
          }

          dfwork.initialize();
          dfwork1.initialize();
          dftridag(a[j], b[j], c[j], rr, xbet[j], xgam[j], 
                   dfa[j], dfb[j], dfc[j], dfrr, dfxbet[j], dfxgam[j], 
                   uu, dfuu, il, iu, work, dfwork, work1, dfwork1);

          for (i=iu; i>=il; i-- )
          {
              if (j==jlb(i))
              {
//            rr[i]=g[i][j] - (e[i][j]-r)*u[i][j] - f[i][j]*u[i][j+1];

                  dfg[i][j]+=dfrr[i];
                  dfe[i][j]-=u[i][j]*dfrr[i];
                  dfu[i][j]-=(e[i][j]-r)*dfrr[i];
                  dff[i][j]-=u[i][j+1]*dfrr[i];
                  dfu[i][j+1]-=f[i][j]*dfrr[i];
              }
              else
              {
                  if (j==jub(i))
                  {
//              rr[i]=g[i][j] - d[i][j]*u[i][j-1] - (e[i][j]-r)*u[i][j];

                      dfg[i][j]+=dfrr[i];
                      dfd[i][j]-=u[i][j-1]*dfrr[i];
                      dfu[i][j-1]-=d[i][j]*dfrr[i];
                      dfe[i][j]-=u[i][j]*dfrr[i];
                      dfu[i][j]-=(e[i][j]-r)*dfrr[i];
                  }
                  else
                  {
//              rr[i]=g[i][j] - d[i][j]*u[i][j-1] - (e[i][j]-r)*u[i][j]
//                            - f[i][j]*u[i][j+1];

                      dfg[i][j]+=dfrr[i];
                      dfd[i][j]-=u[i][j-1]*dfrr[i];
                      dfu[i][j-1]-=d[i][j]*dfrr[i];
                      dfe[i][j]-=u[i][j]*dfrr[i];
                      dfu[i][j]-=(e[i][j]-r)*dfrr[i];
                      dff[i][j]-=u[i][j+1]*dfrr[i];
                      dfu[i][j+1]-=f[i][j]*dfrr[i];
                 }
            }
            dfrr[i] = 0.0;
         }//End i loop
      }//End j loop
  }//End kits loop
}
template void coff_t<d3_array,dmatrix,dvector,double>::dfadi(dmatrix &a, dmatrix &dfa, dmatrix &b, dmatrix &dfb, dmatrix &c, dmatrix &dfc, dmatrix &d, dmatrix &dfd, dmatrix &e, dmatrix &dfe, dmatrix &f, dmatrix &dff, dmatrix &g, dmatrix &dfg, dmatrix &xbet, dmatrix &dfxbet, dmatrix &xgam, dmatrix &dfxgam, dmatrix &ybet, dmatrix &dfybet, dmatrix &ygam, dmatrix &dfygam, dmatrix &psi, dmatrix& dfpsi, dvector &rr, dvector &dfrr, dvector &uu, dvector &dfuu, dvector &work, dvector &dfwork, dvector &work1, dvector &dfwork1, dmatrix& u, dmatrix& dfu);
template<> void coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::dfadi(dmatrix &a, dmatrix &dfa, dmatrix &b, dmatrix &dfb, dmatrix &c, dmatrix &dfc, dmatrix &d, dmatrix &dfd, dmatrix &e, dmatrix &dfe, dmatrix &f, dmatrix &dff, dmatrix &g, dmatrix &dfg, dmatrix &xbet, dmatrix &dfxbet, dmatrix &xgam, dmatrix &dfxgam, dmatrix &ybet, dmatrix &dfybet, dmatrix &ygam, dmatrix &dfygam, dmatrix &psi, dmatrix& dfpsi, dvector &rr, dvector &dfrr, dvector &uu, dvector &dfuu, dvector &work, dvector &dfwork, dvector &work1, dvector &dfwork1, dmatrix& u, dmatrix& dfu) {}
