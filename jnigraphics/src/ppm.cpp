#include <iostream>
#include <fstream>
#include <jni.h>

extern "C"
JNIEXPORT void JNICALL Java_ppm_toFile
  (JNIEnv *env, jobject jobj, jstring js, jintArray jia, jint w, jint h)
{
  const char* ca = env->GetStringUTFChars(js, 0);
  std::ofstream ofs(ca, std::ios::out|std::ios::binary);  
  if (!ofs)
  {
    std::cerr << "Error: unable to open " << ca << " for output\n";
    return;
  }
  env->ReleaseStringUTFChars(js, ca);

  ofs << "P6\n" 
      << w      << ' ' << h << '\n' 
      << 255    << '\n';

  jint* jip = env->GetIntArrayElements(jia, 0);

  int size = w * h;
  for (int i = 0; i < size; i++)
  {
    //black rgb = 0xff000000;
    //blue  rgb = 0x00ff0000; 16711680
    //green rgb = 0x0000ff00; 65280
    //red   rgb = 0x000000ff; 255

    unsigned int rgb = (jip[i] & 0x00ffffff);
    unsigned int r   = (rgb & 0x00ff0000);
    unsigned int  g  = (rgb & 0x0000ff00);
    unsigned int   b = (rgb & 0x000000ff);
    rgb = (b << 16) | g | (r >> 16);

    ofs.write((char*)&rgb, 3);
  }

  env->ReleaseIntArrayElements(jia, jip, JNI_ABORT);

  ofs.close();
}
