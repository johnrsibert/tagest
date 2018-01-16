#include <iostream>
#include <jni.h>
#include <adstring.hpp>

extern JNIEnv* jnienv;
extern jobject jniobj;

int tagmove(JNIEnv* env, jobject jobj, adstring& path, adstring& file);

#ifdef __cplusplus
extern "C" 
#endif

JNIEXPORT jint JNICALL Java_jnigraphics2_executenative
  (JNIEnv* env, jobject obj)
{
  jnienv = env;
  jniobj = obj;

  tagmove(env, obj, "", "");
  return (jint)1;
}
