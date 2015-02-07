#include <stdlib.h>
#include <string.h>

#include <math.h>
#ifdef __NDPX__
  extern "C" {
  #include <DOS.H>
  };
#endif

#ifdef __TURBOC__
  #include <alloc.h>
#endif


#ifdef __NDPX__
  #include <time.h>
#else
  #include <time.h>
#endif

#ifdef __i860
  #define DVECTOR register double*
#else
  #define DVECTOR dvector&
#endif

#ifdef __BORLANDC__
  #pragma hdrstop
#endif
