//$Id: dbmatrix.h 2754 2011-01-02 20:57:07Z jsibert $
#ifndef __dbMatrix__
#define __dbMatrix__
#include <fvar.hpp>

class dbMatrix
{
public://Constructors and Destructors
  //////////////////////////////////////////////////////////////////
  //Output: Assign member variable set to empty 
  //Note: Use allocate(const ivector&, const int&) to assign class
  dbMatrix();

  //////////////////////////////////////////////////////////////////
  //Note: just a wrapper constructor for allocate
  dbMatrix(const ivector& _ivDiagLoc, const int _n);

  //////////////////////////////////////////////////////////////////
  //Note: Wrong should produce a shallow copy
  //Output: Creates a another allocated hard copy 
  dbMatrix(dbMatrix& _dbMatrix);

  ~dbMatrix(){ /*Do Nothing*/ };

public://Methods
  //////////////////////////////////////////////////////////////////
  //Input conditions: all elements of _ivDiagLoc must be unique,
  //                  and satisfy -_n < _ivDiagLoc[i] < _n for
  //                  _n > 0 and i = 1,...,_ivDiagLoc.size()  
  //Process: Build a ragged dmatrix using _ivDiagLoc 
  //         to determine the number of rows and 
  //         number of elements of each row 
  //Output: Assign member variables if all input conditions are true
  //        else all member variables are empty
  void allocate(const ivector& _ivDiagLoc, const int _n);

  //////////////////////////////////////////////////////////////////
  //Input conditions: 1 <= (i,j) <= n 
  //Process: Search ragged dmDiagVec_ for the above indexes return value
  //         if not in dmDiagVec_ returns 0.0
  //Output: if input conditions not valid, produce Error and returns 0.0
  //        else returns value from dmDiagVec_
  double& operator()(const int _i, const int _j);
 
  //////////////////////////////////////////////////////////////////
  //Input conditions: The size of _dv must be the same as n_ by the
  //                  rule for matrix multiplication (nxn)(nx1) 
  //                  The class will exit program if dbMatrix or _dv
  //                  is not allocated.
  //Process: Do the matrix multiplication matrix x vector = vector
  //         Ax = b
  //Output: Return b nx1 dvector
  dvector operator*(const dvector& _dv);

  //////////////////////////////////////////////////////////////////
  //Process: initalize diagonals to zeros
  dbMatrix initialize() 
  { 
    dmDiagVec_.initialize(); 
    return *this;
  }

  //////////////////////////////////////////////////////////////////
  //This doesn't work!!!!!!!!!!!!!!!!!!!!!!!!!!
  //Output: this points to _dbM 
  dbMatrix operator=(dbMatrix& _dbM);

  //////////////////////////////////////////////////////////////////
  //Process:Transpose the sparse Matrix by reversing ivDiagLoc_
  //Output: returns transposed dbMatrix *this::dmDiagVec is transposed
  dbMatrix& transpose();

  //////////////////////////////////////////////////////////////////
  //Another one that doesn't really work 
  ostream& printit(ostream& _os);

public://Access Functions
  int get_n() const { return n_; }
  int colsize() const { return n_; }
  int rowsize() const { return n_; }
  ivector get_ivDiagLoc() const { return ivDiagLoc_; }
  dmatrix get_dmDiagVec() { return dmDiagVec_; }

public://Test Methods
  void process();
  void print2();
  void print();
  void fill();
  void iden();
  
private://Members
  double dzero_;       //a dummy
  int n_;              //size of nxn matrix
  ivector ivDiagLoc_;  //locations of diagonals relative to main diag = 0
  dmatrix dmDiagVec_;  //contains the diagonals of sparse matrix
};//End dbMatrix

//Wrapper Functions
dbMatrix transpose(dbMatrix& _dbM);
ostream& operator<<(ostream& _os, dbMatrix& _dbM);
#endif
//End of File
