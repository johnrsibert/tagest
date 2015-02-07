#include <fvar.hpp>
#include <cifstrem.h>
#include "modlarea.h"
#include "values.h"

#include "trace.h"

void linear_input_scale(dmatrix& t, const double  min_t, const double max);
double min_data(dvector& dd);

extern ofstream clogf("sepodym.log");

int main()
{
 /* Purpose: To subset and create input data for neural_net model
 The program first creates an object of the SEPODYM data area
 followed by the area to subset. It allows to scale the data and
 writes in binary format */


 char model_area[15] = "grid10.inf";
 Model_area modelarea(model_area);
 const int m = modelarea.get_m();
 const int n = modelarea.get_n();
 const int nslice = 132;


 clogf << m <<" "<< n << endl;
 const ivector ilb = modelarea.get_ilb();
 const ivector iub = modelarea.get_iub();

 const ivector jlb = modelarea.get_jlb();
 const ivector jub = modelarea.get_jub();

 char fullname[12] ={ "sst.txt"};
 cifstream  infile(fullname);
 if( !infile)
 {
   cerr <<"File "<<fullname << " not found " << endl;
   exit(1);
 }
 cout <<"\nReading data from <" << fullname  <<"> .." <<  endl;

 d3_array sepodym_data(1,nslice,1,m,1,n);  sepodym_data.initialize();

  for(int slice = 1; slice <= nslice; slice++)
  {
    cout <<"." << flush; //this is for moving dots on the screen
    for(int j = n; j >= 1;  j--)
    {
       for(int i = 1; i <= m; i++)
       {
          infile >> sepodym_data(slice,i,j);
       } //for j
    }  //for i
  } //for k

 cout <<"\nReading complete... " << endl;


 dmatrix temp_sep_dat(1,m,1,n);
 dmatrix movemod_data(1,m,1,n);

 const char filename[20] = "..\\grid10.sst";
  uostream uo(filename);
  uo <<  (int)77 << (int)1; //start year and month of the windstress time series

  ivector fp(1,nslice);            //file pointer vector = number of months that will be read

  double _min, _max;
  int nrecords = nslice  * m * n;
  dvector temp_dat(1, nrecords);
  int nrec =0;

 cout <<"\Finding <min> and <max> of the data set..." << endl;

  for(int slice= 1;  slice <= nslice; slice++)
  {
   for(int j  = 1; j <= n ; j++)
    {
    for(int i = 1; i <= m;  i++)
      {
        nrec ++;
        temp_dat(nrec) =  sepodym_data(slice,i,j);       //read the data into model_area
      } //col
    } //row
  } //slice

  _min = min_data(temp_dat);
  _max = max(temp_dat);

  cout << "\nNow reading and writing data to outputfile " << filename << endl;
 TTRACE(_min,_max)


//write the data to file
for(int slice = 1; slice <= nslice; slice++)
  {
      cout <<"." << flush;
      temp_sep_dat.initialize();

      temp_sep_dat  =  sepodym_data(slice);       //read the data into model_area

      //linear_input_scale(temp_sep_dat,_min, _max);
      fp(slice) = uo.tellp();  //get the position file pointer corresponding for each month block
      uo << temp_sep_dat;  //write the binary output

  } //slice


cout <<"\n\nPositions in " << filename << " file for random data accessing "<< endl;
cout << fp << endl;
clogf << fp << endl;

 uo.close();
 cout <<"\nData saved in '" <<filename <<"'" << endl;
 cout <<"\nPau!..\n" << endl;


 return (0);

}


double min_data(dvector& dd)
{
   int i1 = dd.indexmin();
   int i2 = dd.indexmax();

   double t =  MAXDOUBLE;
   TRACE(t);
   double MinData = 0.0;

   for(int  j = i1; j <= i2; j++)
   {
     // do{
            double tt = dd(j);
             if (tt <  t)
                 t = tt;
        //   }
   //       while( dd(j) > 0 );
   }
   MinData = t;
   return MinData;


}
