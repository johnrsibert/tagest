 #include <fvar.hpp>

 double min (const dmatrix& m);
 double max(const dmatrix& m);
 
 double min (const dvector& v);
 double max(const dvector& v);

/******************************************************
   purpose: linear scaling  input nodes of the neural_net   
  The matrix t is replaced with the scaled values                
*******************************************************/

void linear_input_scale(dmatrix& t)
{
  double  min_t  = min( t );
  double  max_t = max( t );
    
  double t_c =  - (1.0 + ( (2.0 * min_t ) / ( max_t - min_t) )  );     //intercept for linear scaling
  double t_m =  2.0 / (max_t  - min_t );                               //slope for linear scaling
   
  int m2 = t.rowmax();  
  int n2 = t.colmax();
  
  int m1 = t.rowmin();
  int n1 = t.colmin();
  
  for(int  j = n1; j <= n2;  j++)
  {
     for(int i = m1; i <= m2 ;  i++)
     {
        t( i,j )  =  t_m * t( i,j ) + t_c;   //scales from -1 to + 1.0
     }
   }
     
 } // end linear_input_scale(dmatrix& t)



/******************************************************
   OVERLOADED version of above which takes <min> and <max> as
   arguments instead of computing them internally.           
*******************************************************/

void linear_input_scale(dmatrix& t, const double min_t, const double max_t)
{
  //double  min_t  = min( t );
  //double  max_t = max( t );
    
  double t_c =  - (1.0 + ( (2.0 * min_t ) / ( max_t - min_t) )  );     //intercept for linear scaling
  double t_m =  2.0 / (max_t  - min_t );                               //slope for linear scaling
   
  int m2 = t.rowmax();
  int n2 = t.colmax();
  
  int m1 = t.rowmin();
  int n1 = t.colmin();
  
  for(int  j = n1; j <= n2;  j++)
  {
     for(int i = m1; i <= m2 ;  i++)
     {
        t( i,j )  =  t_m * t( i,j ) + t_c;   //scales from -1 to + 1.0
     }
   }
     
 } // end linear_input_scale(dmatrix& t)



/******************************************************
   purpose: linear scaling  input nodes of the neural_net   
  The vector  is replaced with the scaled values                
*******************************************************/
void linear_input_scale(dvector& v)
{
   int i1 = v.indexmin();
   int i2 = v.indexmax();
      
   double min_v = min( v);
   double max_v = max( v);
   
   double v_c;  //intercept
   double v_m; //slope
   
   v_c = - (1.0 + ( (2.0 * min_v ) / ( max_v - min_v) )  );     //intercept for linear scaling
   v_m =  2.0 / (max_v  - min_v );                                         //slope for linear scaling

  
   for(int i = i1; i <= i2; i++)
   {
     v( i ) =  v_m * v( i ) + v_c;   //scales from -1 to + 1.0
   }
    
} // end  linear_input_scale(dvector& v)
