//$Id: negtagpe.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "par_t.h"

//extern ofstream clogf;

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
int negative_tag_penalty(const MATRIX& tags, par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, DOUBLE& pen)
{
  int m = param.get_m();
  ivector& jlb = param.get_jlb();
  ivector& jub = param.get_jub();

  int negatives = 0;
  pen = 0.0;

  for (int i=1; i <= m; i++)
  {
      int lb = jlb(i);
      int ub = jub(i);
      for (int j=lb; j <= ub; j++)
          if (tags[i][j] < 0)
          {
	      pen += tags[i][j]*tags[i][j];
	      if (fabs(tags[i][j]) > 0.001)
	          negatives ++;
          }
  }//i loop

  //if (negatives > 0)
  //  clogf << "negative tags occured in " << negatives << " squares" << endl;

  return negatives;
}
template int negative_tag_penalty(const dmatrix& tags, par_t<d3_array,dmatrix,dvector,double>& param, double& pen);
template int negative_tag_penalty(const dvar_matrix& tags, par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& param, dvariable& pen);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void df_negative_tag_penalty(dmatrix& tags, dmatrix& dftags, par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, double& dfpen)
{
  int m = param.get_m();
  ivector& jlb = param.get_jlb();
  ivector& jub = param.get_jub();


  for (int i=m; i > 0; i--)
  {
      int lb = jlb(i);
      int ub = jub(i);
      for (int j=ub; j >= lb; j--)
      {
          if (tags[i][j] < 0)
          { 
              //clogf << "dftags: " << dftags(i,j) << endl;
              //pen += tags[i][j]*tags[i][j];
	      dftags(i,j) += 2*tags(i,j)*dfpen;
              //clogf << "dftags: " << dftags(i,j) << endl;
              //cout << dftags(i,j) << endl;
              //clogf << "tags: " << tags(i,j) << endl;
              //clogf << "dfpen: " << dfpen << endl; 
          } 
      }
  }

  //pen = 0.0;
  dfpen = 0.0;
}
template void df_negative_tag_penalty(dmatrix& tags, dmatrix& dftags, par_t<d3_array,dmatrix,dvector,double>& param, double& dfpen);
template void df_negative_tag_penalty(dmatrix& tags, dmatrix& dftags, par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& param, double& dfpen);
