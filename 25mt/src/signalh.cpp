//$Id: signalh.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include <fvar.hpp>
#if defined(__cplusplus)
extern "C" {
#endif

void exit_handler(int k)
{
  exit(1);
}

#if defined(__cplusplus)
}
#endif

