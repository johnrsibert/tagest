#include <fvar.hpp>
#include <iomanip>
#include "trace.h"

extern ofstream clogf;

void uvs_freq_comp(const dmatrix& unode, const dmatrix& vnode, const dmatrix& snode, const imatrix& gridmap, const int report)
{
  //HERE
  //TRACE(gridmap)
  //TRACE(snode)
   /***************************************************************************************
   *  Purpose:
   *  To calculate the frequencies of the u,v and snodes. NOTE: values greater or equal to the last bin
   *  would be allocated to the last bin. For instance  if the bin arrangements were as follows:
   *         bin       freq
   *           0        xx
   *       100        xx
   *       200        xx
   *  Than values greater than 200 would go to the last bin. This is not happens in the Excel's 
   *  <frequency> function. In that case values greater than 500 would be excluded. To vaoid this happening 
   *  make ensure to have enough bins so that largest value will not exceed the bin. .... msa 4/25/2002
    ***************************************************************************************/
   
   //TRACE(report)
   int b = 0;
  
  static int ubins = 21;;
  static dvector uu_bins;
  static ivector uu_count;
  
  if(! uu_bins)
  {
     uu_bins.allocate(1,ubins);
     uu_count.allocate(1,ubins);
     uu_count.initialize();
     uu_bins(1) = -3.01;
     for(  b = 2; b <= ubins; b++)
         uu_bins(b) = uu_bins(b -1 ) + 0.3;
     //TRACE(uu_bins)
   }
   
  static int vbins = 21;
  static dvector vv_bins;
  static ivector vv_count;
  
  if(! vv_bins)
  {
     vv_bins.allocate(1,vbins);
     vv_count.allocate(1,vbins);
     vv_count.initialize();
     vv_bins(1) = -3.01;
     for(  b = 2; b <= vbins; b++)
     vv_bins(b) = vv_bins(b -1 ) + 0.3;
     //TRACE(vv_bins)
   }
 
  static int sbins = 21;
  static dvector ss_bins;
  static ivector ss_count;
  
  if(! ss_bins)
  {
     ss_bins.allocate(1,sbins);
     ss_count.allocate(1,sbins);
     ss_count.initialize();
     ss_bins(1) = 200.0;
     for(  b = 2; b <= sbins; b++)
     ss_bins(b) = ss_bins(b -1 ) +400.0;
     //TRACE(ss_bins)
   }
   
       
   
  if (!report)
  {
    #define OP <=
    
    int i1 = unode.rowmin();
    int i2 = unode.rowmax();
    //TTRACE(i1,i2)
    for (int i = i1; i <= i2; i++)
    {
      int j1 = unode(i).indexmin();
      int j2 = unode(i).indexmax();
      //TTRACE(j1,j2)
      for (int j = j1; j <= j2; j++)
      {
       //TTRACE(i,j)
      
        if (gridmap(i,j) > 0 )
         { 
         for ( b = 1; b <  ubins; b++)
          {
            if (unode(i,j) OP uu_bins(b))
              break;
          }
          uu_count(b)++;
          //HERE  
        }
       

        if (gridmap(i,j) > 0)
         {
          for (b = 1; b < vbins; b++)
          {
            if (vnode(i,j) OP vv_bins(b))
              break;
          }
           vv_count(b)++;
         //HERE  
        }       
       
       
       
        if (gridmap (i,j) > 0)
        {
          
         for ( b = 1; b <  sbins; b++)
          {
             if (   snode(i,j) <= ss_bins(b)      )
                   break;
           }
          ss_count(b)++;
        }       
        

      } //for i
    }   //for j
 }  //if( report)
 
 else 
 {

    clogf << "\n Frequency distribution of the u" << endl;
    clogf << setw(10) << "bin" << setw(10) << "freq" << endl;
    for (b  = 1; b  <= ubins; b++) 
    {
      clogf << setw(10) << uu_bins(b) << setw(10) << uu_count(b) << endl;
    }
   TRACE(sum(uu_count))
       
    clogf << "\n Frequency distribution of the v" << endl;
    clogf << setw(10) << "bin" << setw(10) << "freq" << endl;
    for (b  = 1; b <= vbins; b++) 
    {
      clogf << setw(10) << vv_bins(b) << setw(10) << vv_count(b) << endl;
    }
    TRACE(sum(vv_count))
       
    clogf << "\n Frequency distribution of the sigma" << endl;
    clogf << setw(10) << "bin" << setw(10) << "freq" << endl;
     for (b = 1; b <= sbins; b++) 
    {
      clogf << setw(10) << ss_bins(b) << setw(10) << ss_count(b) << endl;
    }   
   TRACE(sum(ss_count))
  
 } //else

// HERE
  //clogf << "returning from nn_freq  with report = " << report << endl;
}
