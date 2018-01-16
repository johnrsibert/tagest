#include "precomp2.h"
#include "coff_t.h"
#if defined (__REGIONAL__)
  #include "par_treg.h"
#elif defined (__NEURAL_NET__)
  #include "par_t_nn.h"
#elif defined (__HABITAT__)
  #include "par_thab.h"
#else
  #include "par_treg.h"
//  #error Error: Undefined par_t include file
#endif

