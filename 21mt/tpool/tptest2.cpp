#include <fvar.hpp>
#include "tpool.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <semaphore.h>
#include "trace.h"

sem_t barrier;
typedef struct farg
{
   int ident;
   dvector* x;
   int i1;
   int i2;
   double* y;
} farg_t;

void r1(void * t)
{
  farg_t* rarg = (farg_t*)t;
  dvector* x = (dvector*)rarg->x; 
  int i1 = rarg->i1;
  int i2 = rarg->i2;
  int printstring = rarg->ident;
  //cout << "r1:   " << i1 << "  "  << i2 << endl;
  //cout << "      " << x->indexmin() << "  " << x->indexmax() << endl;

  dvector subx = (*x)(i1,i2);
  double s = 0;
  for (int i=1; i<= 6000; i++)
    s = norm2(subx);
    //s = norm2((*x)(i1,i2));

  //cerr << printstring << "a: " << s << endl;
  double* yptr = rarg->y;
  *yptr = s;
  //cerr << printstring << "b: " << s << endl;
  int err = sem_post(&barrier);
  if (err)
  {
     cerr << "Error in sem_post barrier; " << strerror(err) << endl;
     exit(1);
  }
  //cout << x << endl;
}

int main(int argc, const char* argv[] )
{
   pthread_t t1,t2;
   int i;  

   int nthread = 0;
   int ntask = 0;
   if ( argc == 2 ) 
   {
     nthread = atoi( argv[1] );
     //ntask = atoi( argv[2] );
   }
   else
   {
      cerr << "Usage: tptest2 nthread" << endl;
      exit(1);
   }
   ntask = nthread;

   cout << "number of pthreads to allocate: " << nthread << endl;;
   cout << "number of tasks to run: " << ntask << endl;

   dvector y(0,ntask-1);
   int m = 1000000;
   dvector vect(1,m);
   vect.fill_seqadd(2.0,0.0);
   /*
   dmatrix mat(0,(ntask-1),1,m);
   for ( i = 0; i < ntask; i++) 
   {
     mat(i).fill_seqadd((double)(i+1),0.0);
     //cout << sum(mat(i)) << endl;
   }
   */

   farg_t* targs = new farg_t[nthread];
   tpool test_pool;
   test_pool.allocate(nthread, ntask, 0);

    int err= SEM_INIT(&barrier, 0, 0);
    if (err)
    {
      cerr << "Error initializing barrier; " << strerror(err) << endl;
      exit(1);
    }

   int i2 = 0;
   int i1 = 0;
   for ( i = 0; i < ntask; i++) 
   {
      i1 = i2 + 1;
      i2 = i1 + m/ntask -1;
      if (i2 > m)
        i2 = m;
      //cout << "main: " << i1 << "  "  << i2 << endl;
      targs[i].i1 = i1;
      targs[i].i2 = i2;
      targs[i].x =&vect;
      targs[i].ident = i;
      targs[i].y = &y(i);
   }

   for ( i = 0; i < ntask; i++) 
   {
     //cout << i << "  " << targs[i].ident << "  " << targs[i].y 
     //          << "  " << (unsigned)targs[i].x <<  "  " << (unsigned)&targs[i] << endl;
     err = test_pool.tpool_add_work(r1, (void*)&targs[i]);
   //printf("main: tpool_add_work returned %d\n",err);

   }
   cout << "main: all work queued; waiting for results "; 

   thread_single_barrier(&barrier, ntask);
   //test_pool.tpool_destroy(1); 
   double sum = 0.0;
   for ( i = 0; i < ntask; i++) 
   {
     cout <<".";
      //cout << i << ": " << sum;
      sum += y(i); //targs[i].y;
      //cout << "  " << sum << endl;
   }
   cout << " and the answer is " << setprecision(10) << sum << endl;

   delete[] targs;
   return 0;
   
}  
