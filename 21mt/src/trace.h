#ifndef __TRACE__
#define __TRACE__
#include <fstream>
extern ofstream clogf;
#undef HERE
#define HERE clogf << "reached " << __LINE__ << " in " << __FILE__ << endl;

#undef TRACE
#define TRACE(object) clogf << "line " << __LINE__ << ", file " << __FILE__ << ", " << #object " = " << object << endl;

#undef TTRACE
#define TTRACE(o1,o2) clogf << "line " << __LINE__ << ", file " << __FILE__ << ", " << #o1 " = " << o1<< ", " << #o2 " = " << o2 << endl;

#undef ASSERT
#define ASSERT(object) if (!object) { cerr << "ASSERT: line = " << __LINE__ << " file = " << __FILE__ << " " << #object << " = " << object << " (false)\n"; exit(1); }
#endif
