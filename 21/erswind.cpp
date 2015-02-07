#include <iostream.h>
#include <adstring.hpp>
#include <cstdlib>
#include "ers12.h"
#include "modlarea.h"
#include "trace.h"

void linear_input_scale(dmatrix& t, const double  min_t, const double max);
void linear_input_scale(dmatrix& t);
void write_ma_matrix(const dmatrix& t, const char* filename);
void resolution_double(dmatrix& x, dmatrix& xx);
double min (const dvector& v);
double max(const dvector& v);
extern ofstream clogf("ers.log");

int main()
{

 //char ersarea[15] =   "test.inf";  //information  file to create object
 //char modelarea[15] =  "test_subset.inf";  //information file to create object

 char ersarea[15] =   "ers.inf";            //information  file to create object
 char modelarea[15] =  "ers_subset.inf";     //information file to create object

 clogf <<"ersarea    : " << ersarea << endl;
 clogf <<"modelarea: " << modelarea << endl;

 ers12  ers(ersarea);           //ers12 objects
 Model_area subset(modelarea);  //model area object

 const int m = subset.get_m();
 const int n = subset.get_n();
 
 clogf << m <<" "<< n << endl;  
 
/* Need to write here cerr statement to to check if the subset is out of the range of ES12 data set */

 const ivector ilb = subset.get_ilb();
 const ivector iub = subset.get_iub();

 dmatrix wind_curl(1,m,1,n);  wind_curl.initialize();
 
 //const char filename[15] = "smallmald.wne";   
// ofstream ofs(filename);

 cout <<"Reading and subsetting data..\n" << endl;
 
  const char filename[20] = "..\\binwscurl.wsc";
  uostream uo(filename);
  uo <<  (int)90 << (int)1; //start year and month of the windstress time series
  
  int num_files = 132;
  ivector fp(1,num_files);            //file pointer vector = number of months that will be read
  
  int yr, mo;
  double _min, _max;
  int nrecords = num_files * m * n;
  dvector temp_wind(1, nrecords);
  int nrec =0;
 
 cout <<"\Finding <min> and <max> of the subsetting region..." << endl;
 for(yr = 1990; yr <= 2000; yr++)
  {
    for( mo = 1;  mo <= 12; mo++)
    {
         if(ers.readata(yr,mo) == true)
         {
           cout <<"." << flush;
           for(int j = n; j  >= 1; j--)
              {
                double dlat = subset.index_to_lat(j);
                int i1 = ilb(j);
                int i2 = iub(j);
                for(int i = i1;  i <= i2;  i++)
                 { 
                   nrec ++;
                   double dlong = subset.index_to_long(i);
                   temp_wind(nrec) =  ers.wnd(dlong, dlat);       //read the data into model_area

                 } //i
             }  //j
         }
         
    } 
  } 
  
  _min = min(temp_wind);
  _max = max(temp_wind);
  
  cout << "\n\nNow reading and writing data to outputfile " << filename << endl;
  
  num_files = 0; 
  for( yr = 1990; yr <= 2000; yr++)
  {
    for(mo = 1;  mo <= 12; mo++)
    {
         //if(mo == 5){ mo = 10; } //for the ne season
         if (ers.readata(yr,mo) == true)  //read the data from the file
         {
             cout <<"." << flush;
             num_files ++;
             TTRACE(yr,mo)

             for(int j = n; j  >= 1; j--)
              {
                double dlat = subset.index_to_lat(j);
                int i1 = ilb(j);
                int i2 = iub(j);
                for(int i = i1;  i <= i2;  i++)
                 {
                   double dlong = subset.index_to_long(i);
                   //wind_curl( i,j ) +=  ers.wnd(dlong, dlat);  //read the data into model_area
                   wind_curl( i,j ) =  ers.wnd(dlong, dlat);       //read the data into model_area

                 } //i
             }  //j

         } //if (ers.readata(yr,mo) == true)
          
         
         linear_input_scale(wind_curl,_min,_max);
         fp(num_files) = uo.tellp();  //get the position file pointer corresponding for each month block
         uo << wind_curl;  //write the binary output
         
         
         //write_ma_matrix(wind_curl, filename);
         //ofs.close();

    } //mo
  } //yr

cout <<"\n\nPositions in " << filename << " file for random data accessing "<< endl;
cout << fp << endl;
clogf << fp << endl;

 //wind_curl /= double(num_files);   //get the average of matrix

 //increase the resolution
 //int i2 = wind_curl.rowmax(); int rr = i2 * 2;
 //int j2 = wind_curl.colmax(); int cc = j2 * 2;
 
 //dmatrix wwind_curl(1,rr,1,cc);
 //resolution_double(wind_curl,wwind_curl);

 //write_ma_matrix(wind_curl, filename);
 //ofs.close();


 //ofs.close();
 uo.close();
 cout <<"\nData saved in '" <<filename <<"'" << endl; 
 cout <<"\nPau!..\n" << endl;
 return (0);

}

void resolution_double(dmatrix& x, dmatrix& xx)
{
  int i1 = x.rowmin();
  int i2 = x.rowmax();
  int j1 = x.colmin();
  int j2 = x.colmax();

  int bb = 0; int aa = 0;
  
  for(int j = j1; j <= j2; j++)
  {
    for(int jj = 1; jj <= 2; jj++)
    {
       aa++;
       bb = 0;
       for(int i = i1; i <= i2; i++)
       {
         for(int ii = 1; ii <= 2; ii++)
         {
            bb++;
            xx(bb,aa) = x(i,j);
         }
       }
      
    }
  }

// linear_input_scale(xx);
} //end of resolution_double


