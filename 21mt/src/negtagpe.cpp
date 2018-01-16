#include "precomp.h"

extern ofstream clogf;

void negative_tag_penalty(dmatrix& tags, par_t& param, double& pen, int& negatives)
{
  int m = param.get_m();
  ivector& jlb = param.get_jlb();
  ivector& jub = param.get_jub();

  negatives = 0;
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

}//End of negative_tag_penalty

void df_negative_tag_penalty(dmatrix& tags, dmatrix& dftags, par_t& param, double& dfpen)
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
}//End of negative_tag_penalty
