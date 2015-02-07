//$Id: make_d3a.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include <fvar.hpp>

dvar3_array make_dvar3_array(int sl,int sh, int nrl, int nrh,
                       const ivector& ncl, const ivector& nch)
{
  imatrix nclm(sl,sh,nrl,nrh);
  imatrix nchm(sl,sh,nrl,nrh);
  for (int s = sl; s <= sh; s++)
  {
    nclm(s) = ncl;
    nchm(s) = nch;
  }

  dvar3_array t(sl, sh, nrl, nrh, nclm, nchm);

  return t;
}


d3_array make_d3_array(int sl,int sh, int nrl, int nrh,
                       const ivector& ncl, const ivector& nch)
{
  imatrix nclm(sl,sh,nrl,nrh);
  imatrix nchm(sl,sh,nrl,nrh);
  for (int s = sl; s <= sh; s++)
  {
    nclm(s) = ncl;
    nchm(s) = nch;
  }

  d3_array t(sl, sh, nrl, nrh, nclm, nchm);

  return t;
}


