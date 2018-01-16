#include <fvar.hpp>
#include <cifstrem.h>
#include "trace.h"

void read_ma_matrix(dmatrix& t, const char* filename)
{
  cifstream inf(filename);
  if ( !inf )
  {
    cerr << "Cannot input open file: " << filename << endl;
    exit(1);
  } 
   int m1 = t.rowmin();
   int m2 = t.rowmax(); 
 
  int n1 = t.colmin();
  int n2 = t.colmax();
  
  for (int j = n2; j >= n1; j--)
  {
    for (int i = m1; i <= m2; i++)
      inf >> t(i,j);
  }//for j
  
  if ( !inf )
  {
    cerr << "Error reading file: " << filename << endl;
    exit(1);
  }
} //end of void read_ma_matrix(dmatrix& t, const char* filename)



//overloaded for an imatrix
void read_ma_matrix(imatrix& t, const char* filename)
{
  cifstream inf(filename);
  if ( !inf )
  {
    cerr << "Cannot input open file: " << filename << endl;
    exit(1);
  }

  int m1 = t.rowmin();
  int m2 = t.rowmax(); 
 
  int n1 = t.colmin();
  int n2 = t.colmax();

  for (int j = n2; j >= n1; j--)
  {
    for (int i = m1; i <= m2; i++)
      inf >> t(i,j);
  }//for j
  if ( !inf )
  {
    cerr << "Error reading file: " << filename << endl;
    exit(1);
  }
} //end of void read_ma_matrix(imatrix& t, const char* filename)




//////////////////////////////////////

void write_ma_matrix(const dmatrix& t, const char* filename)
{
  ofstream of(filename);
  if (!of)
  {
    cerr << "Cannot open output file: " << filename << endl;
    exit(1);
  }
  of << "# " << filename << " in standard model_area format" << endl;

  int m1 = t.rowmin();
  int m2 = t.rowmax(); 
 
  int n1 = t.colmin();
  int n2 = t.colmax();
  
  for (int j = n2; j >= n1; j--)
  {
    for (int i = m1; i <= m2; i++)
    {
      of << " " << t(i,j);
    }
    of << endl;
  }//for j
  if ( !of )
  {
    cerr << "Error writing file: " << filename << endl;
    exit(1);
  }
} //end of void read_ma_matrix(dmatrix& t, const char* filename)



//overloaded for an imatrix
void write_ma_matrix(const imatrix& t, const char* filename)
{
  ofstream of(filename);
  if (!of)
  {
    cerr << "Cannot open output file: " << filename << endl;
    exit(1);
  }
  of << "# " << filename << " in standard model_area format" << endl;
  
  int m1 = t.rowmin();
  int m2 = t.rowmax(); 
 
  int n1 = t.colmin();
  int n2 = t.colmax();
  
  for (int j = n2; j >= n1; j--)
  {
    for (int i = m1; i <= m2; i++)
    {
      of << " " << t(i,j);
    }
    of << endl;
  }//for j
  if ( !of )
  {
    cerr << "Error writing file: " << filename << endl;
    exit(1);
  }
} //end of void read_ma_matrix(dmatrix& t, const char* filename)
