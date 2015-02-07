#include <iostream>
#include <jni.h>
#include <adstring.hpp>

extern JNIEnv* jnienv;
extern jobject jniobj;

int halflife(void);

#ifdef __cplusplus
extern "C" 
#endif

JNIEXPORT jint JNICALL Java_jnigraphics2_executenative
  (JNIEnv* env, jobject obj)
{
  jnienv = env;
  jniobj = obj;

  halflife();
  return (jint)1;
}
