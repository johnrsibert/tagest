#include <sys/resource.h>
#include <iostream.h>

int surpress_core_dump(void)
{
  int result = 0;
  rlimit rlp;
  getrlimit(RLIMIT_CORE, &rlp);
  rlp.rlim_cur = 0;
  result = setrlimit(RLIMIT_CORE, &rlp);
  if ((result = -1) && (rlp.rlim_cur != 0 ))
  {
    cerr << "error attmpting to surpress core dump with setrlimit(RLIMIT_CORE, &rlp);" << endl;
  }
  getrlimit(RLIMIT_CORE, &rlp);
  cout << "core dump surpressed with setrlimit(RLIMIT_CORE, &rlp) to " 
       << rlp.rlim_cur << endl;
  return (result);

}
/*
     Upon successful completion, the function getrlimit() returns
     a  value  of 0; otherwise, it returns a value of -1 and sets
     errno to indicate an error.

ERRORS
     Under the following conditions,  the  functions  getrlimit()
     and setrlimit() fail and set errno to:

     EFAULT rlp points to an illegal address.

     EINVAL An  invalid  resource  was   specified;   or   in   a
            setrlimit()  call,  the  new rlim_cur exceeds the new
            rlim_max.

     EPERM  The limit specified to setrlimit() would have  raised
            the  maximum  limit  value, and the effective user of
            the calling process is not super-user.

*/
