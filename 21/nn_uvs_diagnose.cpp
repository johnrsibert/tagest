#include "par_t_nn.h"
#include "trace.h"
#include <cifstrem.h>


//int a_move_corner(const int i, const int edge);                   //position.cpp
void make_ext(const char c, const int n, char * ext);           //ext_numb.cpp
int last_ext_number(const char* root, const char letter);     //ext_numb.cpp

extern ofstream clogf;

void par_t_nn::uvs_diagnose(dmatrix& unode, dmatrix& vnode, dmatrix& snode, 
            dmatrix& u, dmatrix& v, dmatrix& sigma, const char* _ext)
{

  char fullname[20];
  strcpy(fullname, pathname.getRoot());
  if (!_ext)
  {
    char ext[5];
    int ext_no = last_ext_number(pathname.getRoot(), 'p') + 1;
    m_ipar[8] = ext_no;
    make_ext('p', ext_no, ext);
    strcat(fullname, ext);
  }
  else
  {
    strcat(fullname, _ext);
  }
  //TRACE(fullname) 
 
  ofstream pf(fullname);
  if (!pf)
  {
    cerr << "Error: Cannot open file " << fullname 
         << " in par_t_nn::uvs_diagnose " << endl;
    exit(1);
  }

  for (int i=1; i <= m; i++)
  {
    int j1 = jlb(i);
    int jn = jub(i);
    for (int j = j1; j <= jn; j++)
    {
      if(gridmap(i,j) != 0)
      {
        pf  << unode(i,j) << "    " <<  vnode(i,j) << "   " << snode(i,j) << "   " 
            << u(i,j)         << "    " << v(i,j)          << "   " << sigma(i,j)  << endl;
      }
 
      if(gridmap(i,j) ==  0 )
      {
      //pf  << 0.0 << "   " << 0.0  << "   " << 0.0  << endl;
        pf  << "NA"  << "    " << "NA"  << "     " << "NA"  <<"     "
            <<  "NA"  << "   " << "NA"   << "    " << "NA"   <<  endl;  //for .R purposes!
      }     

    }//forj
  }//fori 
  //HERE
}  //end of par_t_nn::uvs_diagnose
