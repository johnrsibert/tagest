#ifndef __TRACE__
#define __TRACE__

#undef HERE
#define HERE cout << "reached " << __LINE__ << " in " << __FILE__ << endl;

#undef HALT
#define HALT cout <<"\nBailing out in file"<<__FILE__<<" at line " <<__LINE__<< endl; exit(1);

#undef TRACE
#define TRACE(object) cout << "line " << __LINE__ << ", file " << __FILE__ << ", " << #object " = " << object << endl;

#undef TTRACE
#define TTRACE(o1,o2) cout << "line " << __LINE__ << ", file " << __FILE__ << ", " << #o1 " = " << o1<< ", " << #o2 " = " << o2 << endl;

#undef ASSERT
#define ASSERT(object) if (!object) { cerr << "ASSERT: line = " << __LINE__ << " file = " << __FILE__ << " " << #object << " = " << object << " (false)\n"; exit(1); }
#endif
