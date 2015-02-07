#include <iostream>
#include <fstream>
#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL Java_region_drawCst(JNIEnv *env, jobject jobj,
                                           jstring jsCstFile,
                                           jobject joGraphics2D,
                                           jobject jocolorCoastline,
                                           jobject jocolorLand,
                                           jint x0, jint y0,
                                           jdouble xpix, jdouble ypix
)
{ 
  const char* cp = env->GetStringUTFChars(jsCstFile, 0);
  std::ifstream ifs(cp, std::ios::in|std::ios::binary);
  if (!ifs)
  {
    std::cerr << "Error: Unable to open " << cp << "\n";
    return;
  }
  env->ReleaseStringUTFChars(jsCstFile, cp);

  jclass Graphics2D = env->GetObjectClass(joGraphics2D);
  jmethodID setColor = env->GetMethodID(Graphics2D, "setColor", "(Ljava/awt/Color;)V");
  jmethodID drawPolygon = env->GetMethodID(Graphics2D, "drawPolygon", "([I[II)V");
  jmethodID fillPolygon = env->GetMethodID(Graphics2D, "fillPolygon", "([I[II)V");

  //check version
  short int version = 0;
  ifs.read((char*)&version, sizeof(short int));
  if (version != 3)
  {
    std::cerr << "Warning: wrong version \"" << version << "\" expecting v3 for cstfile " << cp << '\n';
    return;
  }

  const int sizeofint = sizeof(int);
  while (!ifs.eof())
  {
    int num = 0;
    ifs.read((char*)&num, sizeofint);
   
    if (num > 0)
    {
      jintArray jxia = env->NewIntArray(num);
      jintArray jyia = env->NewIntArray(num);
  
      jint* jxip = env->GetIntArrayElements(jxia, 0);
      jint* jyip = env->GetIntArrayElements(jyia, 0);

      for (int i = 0; i < num; i++)
      {
	int x = 0, y = 0;
        ifs.read((char*)&x, sizeofint);
        ifs.read((char*)&y, sizeofint);

        jxip[i] = (int)(x0 + x * xpix);
        jyip[i] = (int)(y0 - y * ypix);
      }

      env->ReleaseIntArrayElements(jxia, jxip, 0);
      env->ReleaseIntArrayElements(jyia, jyip, 0);

      env->CallVoidMethod(joGraphics2D, setColor, jocolorCoastline);
      env->CallVoidMethod(joGraphics2D, drawPolygon, jxia, jyia, num);

      env->CallVoidMethod(joGraphics2D, setColor, jocolorLand);
      env->CallVoidMethod(joGraphics2D, fillPolygon, jxia, jyia, num);
    }
  }

  ifs.close();
}

#ifdef __cplusplus
}
#endif
