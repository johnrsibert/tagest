//$Id: dfadi.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "coff_t.h"
#include "intersav.h"
#include "trace.h"

extern ofstream clogf;
void dftridag(dvector& a, dvector& b, dvector& c,dvector& r, dvector& bet, dvector& gam, dvector& dfa,dvector& dfb,dvector& dfc,dvector& dfr, dvector& dfbet, dvector& dfgam, dvector& u, dvector& dfu, int n1, int n2, dvector& v, dvector& dfv, dvector& w, dvector& dfw);

extern setsaveinterflag interflag;
extern int saveinterflag;

void bogus_dfadi(void)
{

  verify_identifier_string((char*)"adi6");
  void * pcoff = restore_pointer_value();

  verify_identifier_string((char*)"adi5");
  //int test = restore_int_value();
  //TRACE(test)
 
  coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>* coff = 
    (coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>*)pcoff;

  dvar_matrix_position upos = restore_dvar_matrix_position();
  dmatrix dfu = restore_dvar_matrix_derivatives(upos);
  dmatrix u(upos);
  //int test2 = restore_int_value();
  //TRACE(test2)

  verify_identifier_string((char*)"adi4");
  dvar_matrix_position ybetpos = restore_dvar_matrix_position();
  dmatrix ybet = restore_dvar_matrix_value(ybetpos);
  dmatrix dfybet = restore_dvar_matrix_derivatives(ybetpos);
  verify_identifier_string((char*)"ybet");

  const dvar_matrix_position xbetpos = restore_dvar_matrix_position();
  dmatrix xbet = restore_dvar_matrix_value(xbetpos);
  dmatrix dfxbet = restore_dvar_matrix_derivatives(xbetpos);
  verify_identifier_string((char*)"xbet");

  dvar_matrix_position ygampos = restore_dvar_matrix_position();
  dmatrix ygam = restore_dvar_matrix_value(ygampos);
  dmatrix dfygam = restore_dvar_matrix_derivatives(ygampos);
  const dvar_matrix_position xgampos = restore_dvar_matrix_position();
  dmatrix xgam = restore_dvar_matrix_value(xgampos);
  dmatrix dfxgam = restore_dvar_matrix_derivatives(xgampos);
  verify_identifier_string((char*)"adi3");
  dvar_matrix_position gpos = restore_dvar_matrix_position();
  dmatrix g = restore_dvar_matrix_value(gpos);
  dmatrix dfg = restore_dvar_matrix_derivatives(gpos);
  dvar_matrix_position fpos = restore_dvar_matrix_position();
  dmatrix f = restore_dvar_matrix_value(fpos);
  dmatrix dff = restore_dvar_matrix_derivatives(fpos);
  dvar_matrix_position epos = restore_dvar_matrix_position();
  dmatrix e = restore_dvar_matrix_value(epos);
  dmatrix dfe = restore_dvar_matrix_derivatives(epos);
  dvar_matrix_position dpos = restore_dvar_matrix_position();
  dmatrix d = restore_dvar_matrix_value(dpos);
  dmatrix dfd = restore_dvar_matrix_derivatives(dpos);
  verify_identifier_string((char*)"adi2");
  dvar_matrix_position cpos = restore_dvar_matrix_position();
  dmatrix c = restore_dvar_matrix_value(cpos);
  dmatrix dfc = restore_dvar_matrix_derivatives(cpos);
  const dvar_matrix_position bpos = restore_dvar_matrix_position();
  dmatrix b = restore_dvar_matrix_value(bpos);
  dmatrix dfb = restore_dvar_matrix_derivatives(bpos);
  dvar_matrix_position apos = restore_dvar_matrix_position();
  dmatrix a = restore_dvar_matrix_value(apos);
  dmatrix dfa = restore_dvar_matrix_derivatives(apos);
  const dvar_matrix_position& psipos = restore_dvar_matrix_position();
  verify_identifier_string((char*)"adi1");

  dmatrix psi(psipos);
  dmatrix dfpsi(psipos); dfpsi.initialize();


  dvector rr = coff->vGet(coff->_rr);
  dvector uu = coff->vGet(coff->_uu);

  dvector dfrr(rr.indexmin(),rr.indexmax()); dfrr.initialize();
  dvector dfuu(uu.indexmin(),uu.indexmax()); dfuu.initialize();

  dvector work = coff->vGet(coff->_work);
  dvector work1 = coff->vGet(coff->_work1);

  dvector dfwork(work.indexmin(),work.indexmax()); dfwork.initialize();
  dvector dfwork1(work1.indexmin(),work1.indexmax()); dfwork1.initialize();

  coff->dfadi(a, dfa, b, dfb, c, dfc, d, dfd, e, dfe, f, dff, g, dfg,
        xbet, dfxbet, xgam, dfxgam, ybet, dfybet, ygam, dfygam,
        psi, dfpsi, rr, dfrr, uu, dfuu, work, dfwork, work1, dfwork1,
        u, dfu, NULL);


  dfa.save_dmatrix_derivatives(apos);
  dfb.save_dmatrix_derivatives(bpos);
  dfc.save_dmatrix_derivatives(cpos);
  dfd.save_dmatrix_derivatives(dpos);
  dfe.save_dmatrix_derivatives(epos);
  dff.save_dmatrix_derivatives(fpos);
  dfg.save_dmatrix_derivatives(gpos);
  dfxbet.save_dmatrix_derivatives(xbetpos);
  dfxgam.save_dmatrix_derivatives(xgampos);
  dfybet.save_dmatrix_derivatives(ybetpos);
  dfygam.save_dmatrix_derivatives(ygampos);

  dfu.save_dmatrix_derivatives(upos);
}

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::
dfadi(coff_t& coff, dmatrix& u, dmatrix& dfu, intersavetype *savefile)
{
  dmatrix a = vGet(coff._a);
  dmatrix b = vGet(coff._b);
  dmatrix c = vGet(coff._c);
  dmatrix d = vGet(coff._d);
  dmatrix e = vGet(coff._e);
  dmatrix f = vGet(coff._f);
  dmatrix g = vGet(coff._g);
  dmatrix xbet = vGet(coff._xbet);
  dmatrix ybet = vGet(coff._ybet);
  dmatrix xgam = vGet(coff._xgam);
  dmatrix ygam = vGet(coff._ygam);
  dmatrix psi  = vGet(coff._psi);

  dmatrix dfa = vGet(_a);
  dmatrix dfb = vGet(_b);
  dmatrix dfc = vGet(_c);
  dmatrix dfd = vGet(_d);
  dmatrix dfe = vGet(_e);
  dmatrix dff = vGet(_f);
  dmatrix dfg = vGet(_g);
  dmatrix dfxbet = vGet(_xbet);
  dmatrix dfybet = vGet(_ybet);
  dmatrix dfxgam = vGet(_xgam);
  dmatrix dfygam = vGet(_ygam);

  dmatrix dfpsi  = vGet(_psi); dfpsi.initialize();
 
  dvector rr = vGet(coff._rr); //rr_c.initialize();
  dvector uu = vGet(coff._uu); //uu_c.initialize();

  dvector dfrr = vGet(_rr); dfrr.initialize();
  dvector dfuu = vGet(_uu); dfuu.initialize();

  dvector work = vGet(coff._work);// work_c.initialize(); 
  dvector work1 = vGet(coff._work1);// work1_c.initialize();

  dvector dfwork = vGet(_work); dfwork.initialize();
  dvector dfwork1 = vGet(_work1); dfwork1.initialize();

  dfadi(a, dfa, b, dfb, c, dfc, d, dfd, e, dfe, f, dff, g, dfg,
        xbet, dfxbet, xgam, dfxgam, ybet, dfybet, ygam, dfygam,
        psi, dfpsi, rr, dfrr, uu, dfuu, work, dfwork, work1, dfwork1,
        u, dfu, savefile);

}//End of dfadi

template void coff_t<d3_array,dmatrix,dvector,double>::
dfadi(coff_t& coff, dmatrix& u, dmatrix& dfu, intersavetype *savefile);

template<> void coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::
dfadi(coff_t& coff, dmatrix& u, dmatrix& dfu, intersavetype *savefile) {}

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::
dfadi(dmatrix &a_c, dmatrix &dfa, dmatrix &b_c, dmatrix &dfb, dmatrix &c_c, dmatrix &dfc, 
      dmatrix &d_c, dmatrix &dfd, dmatrix &e_c, dmatrix &dfe, dmatrix &f_c, dmatrix &dff, 
      dmatrix &g_c, dmatrix &dfg, dmatrix &xbet_c, dmatrix &dfxbet, dmatrix &xgam_c, dmatrix &dfxgam, 

      dmatrix &ybet_c, dmatrix &dfybet, dmatrix &ygam_c, dmatrix &dfygam, 
      dmatrix &psi_c, dmatrix& dfpsi, 
      dvector &rr_c, dvector &dfrr, dvector &uu_c, dvector &dfuu, dvector &work_c, 
      dvector &dfwork, dvector &work1_c, dvector &dfwork1, dmatrix& u, dmatrix& dfu,
      intersavetype * savefile)
{
  int nits = k;
  double r  = 4.0 * (double)k;
  int i, j;

  dmatrix_position upos(u);
  dmatrix_position psipos(psi_c);

  for (int kits=nits; kits>=1; kits--)
  {
      vGetValue(psi_c, "psi",savefile,&psipos);
   
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

          dfwork.initialize();
          dfwork1.initialize();
          dftridag(d_c[i], e_c[i], f_c[i], rr_c, ybet_c[i], ygam_c[i], 
                   dfd[i], dfe[i], dff[i], dfrr, dfybet[i], dfygam[i], 
                   uu_c, dfuu, jl, ju,  work_c, dfwork, work1_c, dfwork1);

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

      vGetValue(u, "u",savefile,&upos);
    
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
          } // j loop

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
                   uu_c, dfuu, il, iu,  work_c, dfwork, work1_c, dfwork1);

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
template void coff_t<d3_array,dmatrix,dvector,double>::
dfadi(dmatrix &a, dmatrix &dfa, dmatrix &b, dmatrix &dfb, dmatrix &c, 
      dmatrix &dfc, dmatrix &d, dmatrix &dfd, dmatrix &e, dmatrix &dfe, 
      dmatrix &f, dmatrix &dff, dmatrix &g, dmatrix &dfg, dmatrix &xbet, 
      dmatrix &dfxbet, dmatrix &xgam, dmatrix &dfxgam, dmatrix &ybet, 
      dmatrix &dfybet, dmatrix &ygam, dmatrix &dfygam, dmatrix &psi, 
      dmatrix& dfpsi, dvector &rr, dvector &dfrr, dvector &uu, dvector &dfuu, 
      dvector &work, dvector &dfwork, dvector &work1, dvector &dfwork1, 
      dmatrix& u, dmatrix& dfu, intersavetype * savefile);
template void coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::
dfadi(dmatrix &a, dmatrix &dfa, dmatrix &b, dmatrix &dfb, dmatrix &c, 
      dmatrix &dfc, dmatrix &d, dmatrix &dfd, dmatrix &e, dmatrix &dfe, 
      dmatrix &f, dmatrix &dff, dmatrix &g, dmatrix &dfg, dmatrix &xbet, 
      dmatrix &dfxbet, dmatrix &xgam, dmatrix &dfxgam, dmatrix &ybet, 
      dmatrix &dfybet, dmatrix &ygam, dmatrix &dfygam, dmatrix &psi, 
      dmatrix& dfpsi, dvector &rr, dvector &dfrr, dvector &uu, dvector &dfuu, 
      dvector &work, dvector &dfwork, dvector &work1, dvector &dfwork1, 
      dmatrix& u, dmatrix& dfu, intersavetype * savefile);
