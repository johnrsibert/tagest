#include "precomp.h"
#include "intersav.h"

extern ofstream clogf;
extern "C" void dftridag(DVECTOR a, DVECTOR b, DVECTOR c,DVECTOR r, DVECTOR bet, DVECTOR gam, DVECTOR dfa,DVECTOR dfb,DVECTOR dfc,DVECTOR dfr, DVECTOR dfbet, DVECTOR dfgam, DVECTOR u, DVECTOR dfu, int n1, int n2, intersavetype *savefile, DVECTOR v, DVECTOR dfv, DVECTOR w, DVECTOR dfw);

extern setsaveinterflag interflag;

void coff_t::dfadi(coff_t& coff, dmatrix& u, dmatrix& dfu, 
                   intersavetype *savefile)
{
  #define ERROR 1e-7

  dmatrix& a_c = coff.a;
  dmatrix& b_c = coff.b;
  dmatrix& c_c = coff.c;
  dmatrix& d_c = coff.d;
  dmatrix& e_c = coff.e;
  dmatrix& f_c = coff.f;
  dmatrix& g_c = coff.g;
  dmatrix& xbet_c = coff.xbet;
  dmatrix& ybet_c = coff.ybet;
  dmatrix& xgam_c = coff.xgam;
  dmatrix& ygam_c = coff.ygam;
  dmatrix& psi_c  = coff.psi;

  dmatrix& dfa = a;
  dmatrix& dfb = b;
  dmatrix& dfc = c;
  dmatrix& dfd = d;
  dmatrix& dfe = e;
  dmatrix& dff = f;
  dmatrix& dfg = g;
  dmatrix& dfxbet = xbet;
  dmatrix& dfybet = ybet;
  dmatrix& dfxgam = xgam;
  dmatrix& dfygam = ygam;
  dmatrix& dfpsi  = psi; dfpsi.initialize();
 
  dvector& rr_c = coff.rr; //rr_c.initialize();
  dvector& uu_c = coff.uu; //uu_c.initialize();

  dvector& dfrr = rr; dfrr.initialize();
  dvector& dfuu = uu; dfuu.initialize();

  dvector& work_c = coff.work;// work_c.initialize(); 
  dvector& work1_c = coff.work1;// work1_c.initialize();

  dvector& dfwork = work; dfwork.initialize();
  dvector& dfwork1 = work1; dfwork1.initialize();

  int nits = k;
  double r  = 4 * k;
  int i, j;

  for (int kits=nits; kits>=1; kits--)
  {
      interflag.mgetp(psi_c, "psi",savefile);
      //clogf << "Read psi_c in dfadi" << endl;
   
      for (i=imax; i>=1; i--)
      {
          int jl = jlb(i);
          int ju = jub(i);
          for (j=jl;j<=ju;j++)
          {
	     if (i==ilb(j))
	       rr_c[j] = g_c[i][j] - (b_c[j][i]-r)*psi_c[i][j] 
                         - c_c[j][i]*psi_c[i+1][j];
	     else if (i==iub(j))
	       rr_c[j] = g_c[i][j] - a_c[j][i]*psi_c[i-1][j] 
                         - (b_c[j][i]-r)*psi_c[i][j];
	     else
	       rr_c[j]= g_c[i][j] - a_c[j][i]*psi_c[i-1][j] 
                        - (b_c[j][i]-r)*psi_c[i][j] - c_c[j][i]*psi_c[i+1][j];
          } // j loop

          for (j=jl; j<=ju; j++)
          {
//           u[i][j] = uu[j];
             dfuu[j] += dfu[i][j];

             dfu[i][j]=0;
          }

          interflag.vgetp(u[i], "u[i]",savefile);

          dfwork.initialize();
          dfwork1.initialize();
          dftridag(d_c[i], e_c[i], f_c[i], rr_c, ybet_c[i], ygam_c[i], 
                   dfd[i], dfe[i], dff[i], dfrr, dfybet[i], dfygam[i], 
                   uu_c, dfuu, jl, ju, savefile, work_c, dfwork, work1_c, dfwork1);

          for (j=ju; j>=jl; j--)
          {
	      if (i==ilb(j))
	      {
//                rr_c[j]=g[i][j] - (b[j][i]-r)*psi[i][j] - c[j][i]*psi[i+1][j];

	          dfg[i][j]+=dfrr[j];
	          dfb[j][i]-=psi_c[i][j]*dfrr[j];
	          dfpsi[i][j]-=(b_c[j][i]-r)*dfrr[j];
	          dfc[j][i]-=psi_c[i+1][j]*dfrr[j];
	          dfpsi[i+1][j]-=c_c[j][i]*dfrr[j];
	      }
 	      else 
              {
                  if (i==iub(j))
	          {
//                 rr[j]=g[i][j] - a[j][i]*psi[i-1][j] - (b[i][i]-r)*psi[i][j];

	            dfg[i][j]+=dfrr[j];
	            dfa[j][i]-=psi_c[i-1][j]*dfrr[j];
	            dfpsi[i-1][j]-=a_c[j][i]*dfrr[j];
	            dfb[j][i]-=psi_c[i][j]*dfrr[j];
	            dfpsi[i][j]-=(b_c[j][i]-r)*dfrr[j];
	          }
	          else
	          {
//            rr[j]=g[i][j] - a[j][i]*psi[i-1][j] - (b[j][i]-r)*psi[i][j]
//                          - c[j][i]*psi[i+1][j];

	            dfg[i][j]+=dfrr[j];
	            dfa[j][i]-=psi_c[i-1][j]*dfrr[j];
	            dfpsi[i-1][j]-=a_c[j][i]*dfrr[j];
	            dfb[j][i]-=psi_c[i][j]*dfrr[j];
	            dfpsi[i][j]-=(b_c[j][i]-r)*dfrr[j];
	            dfc[j][i]-=psi_c[i+1][j]*dfrr[j];
	            dfpsi[i+1][j]-=c_c[j][i]*dfrr[j];
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
                rr_c[i]=g_c[i][j] - (e_c[i][j]-r)*u[i][j] - f_c[i][j]*u[i][j+1];
            else if (j==jub(i))
                rr_c[i]=g_c[i][j] - d_c[i][j]*u[i][j-1] - (e_c[i][j]-r)*u[i][j];
            else
                rr_c[i]=g_c[i][j] - d_c[i][j]*u[i][j-1] - (e_c[i][j]-r)*u[i][j] 
                        - f_c[i][j]*u[i][j+1];
          } // i loop

          for (i=iu; i>=il; i-- )
          {
//           psi[i][j] = uu[i];

             dfuu[i]+=dfpsi[i][j];
             dfpsi[i][j]=0;
          }

          dfwork.initialize();
          dfwork1.initialize();
          dftridag(a_c[j], b_c[j], c_c[j], rr_c, xbet_c[j], xgam_c[j], 
                   dfa[j], dfb[j], dfc[j], dfrr, dfxbet[j], dfxgam[j], 
                   uu_c, dfuu, il, iu, savefile, work_c, dfwork, work1_c, dfwork1);

          for (i=iu; i>=il; i-- )
          {
              if (j==jlb(i))
              {
//            rr[i]=g[i][j] - (e[i][j]-r)*u[i][j] - f[i][j]*u[i][j+1];

                  dfg[i][j]+=dfrr[i];
                  dfe[i][j]-=u[i][j]*dfrr[i];
                  dfu[i][j]-=(e_c[i][j]-r)*dfrr[i];
                  dff[i][j]-=u[i][j+1]*dfrr[i];
                  dfu[i][j+1]-=f_c[i][j]*dfrr[i];
              }
              else
              {
                  if (j==jub(i))
                  {
//              rr[i]=g[i][j] - d[i][j]*u[i][j-1] - (e[i][j]-r)*u[i][j];

                      dfg[i][j]+=dfrr[i];
                      dfd[i][j]-=u[i][j-1]*dfrr[i];
                      dfu[i][j-1]-=d_c[i][j]*dfrr[i];
                      dfe[i][j]-=u[i][j]*dfrr[i];
                      dfu[i][j]-=(e_c[i][j]-r)*dfrr[i];
                  }
                  else
                  {
//              rr[i]=g[i][j] - d[i][j]*u[i][j-1] - (e[i][j]-r)*u[i][j]
//                            - f[i][j]*u[i][j+1];

                      dfg[i][j]+=dfrr[i];
                      dfd[i][j]-=u[i][j-1]*dfrr[i];
                      dfu[i][j-1]-=d_c[i][j]*dfrr[i];
                      dfe[i][j]-=u[i][j]*dfrr[i];
                      dfu[i][j]-=(e_c[i][j]-r)*dfrr[i];
                      dff[i][j]-=u[i][j+1]*dfrr[i];
                      dfu[i][j+1]-=f_c[i][j]*dfrr[i];
                 }
            }
            dfrr[i] = 0.0;
         }//End i loop
      }//End j loop
  }//End kits loop
}//End of dfadi
