//$Id: wormwalk.cpp 2754 2011-01-02 20:57:07Z jsibert $
/*
#if defined(__BORLANDC__)
#include <condefs.h>
USEUNIT("worms.cpp");
USEUNIT("walkproc3.cpp");
USEUNIT("tag_pack3.cpp");
USEUNIT("walk2.cpp");
USELIB("jnigraphics2.lib");
USELIB("orl32.lib");
USELIB("ads32.lib");
USELIB("movemod.lib");
#endif // defined(__BORLANDC__)
*/
#include <iostream>
#include <jni.h>
#include <adstring.hpp>
#undef TRACE
#define TRACE(object) cout << "line " << __LINE__ << ", file " << __FILE__ << ", " << #object " = " << object << endl;

extern JNIEnv* jnienv;
extern jobject jniobj;

void wormwalk(void);

#ifdef __cplusplus
extern "C"
#endif

JNIEXPORT jint JNICALL Java_jnigraphics2_executenative
  (JNIEnv* env, jobject obj)
{
  TRACE(env)
  jnienv = env;
  jniobj = obj;
  wormwalk();
  return (jint)1;
}

#define DllEntryPoint
