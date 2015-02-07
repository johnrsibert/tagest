#include "jnigraphics3.h"
#include <string>

JavaVM* jnigraphics3::jnivm;
JNIEnv* jnigraphics3::jnienv;

void jnigraphics3::test()
{
  jmethodID mid = jnienv->GetMethodID(jnigraphics3_cls, "addRegion", "(Lregiondef;I)V");
  jclass cls = jnienv->FindClass("regiondef");
}

int* jnigraphics3::addTabs(const int num, const int id)
{
  int* ia = new int[num];

  jmethodID mid = jnienv->GetMethodID(jnigraphics3_cls, "addTabs", "(II)[I");
  jintArray jia = (jintArray)jnienv->CallObjectMethod(jnigraphics3_obj, mid, num, id);
  jint* jip = jnienv->GetIntArrayElements(jia, 0);
  for (int i = 0; i < num; i++)
    { ia[i] = jip[i]; }
  jnienv->ReleaseIntArrayElements(jia, jip, 0);

  return ia;
}

void jnigraphics3::setTabTitle(const char* title, const int cid)
{
  if (title)
  {
    //jclass cls = jnienv->GetObjectClass(jnigraphics3_obj);
    jmethodID mid = 
      jnienv->GetMethodID(jnigraphics3_cls, 
                          "setTabTitle", 
                          "(Ljava/lang/String;I)V"); 
    jstring js = jnienv->NewStringUTF(title);
    jnienv->CallVoidMethod(jnigraphics3_obj, mid, js, cid);
    jnienv->DeleteLocalRef(js);
  }
}

int* jnigraphics3::addGridLayout(const int rows, const int cols, const int id)
{
  int size = rows * cols;
  int* ia = new int[size];

  jmethodID mid = jnienv->GetMethodID(jnigraphics3_cls, "addGridLayout", "(III)[I");
  jintArray jia = (jintArray)jnienv->CallObjectMethod(jnigraphics3_obj, mid, rows, cols, id);
  jint* jip = jnienv->GetIntArrayElements(jia, 0);
  for (int i = 0; i < size; i++)
    { ia[i] = jip[i]; }
  jnienv->ReleaseIntArrayElements(jia, jip, 0);

  return ia;
}

jobject jnigraphics3::newRegiondef(const regiondefs& region)
{
  jclass cls = jnienv->FindClass("regiondef");
  jmethodID mid = jnienv->GetMethodID(cls, "<init>", "()V");
 jobject obj = jnienv->NewObject(cls, mid);

  if (region.m > 0 && region.n > 0 && region.dx > 0 && region.dy > 0)
  {
    jfieldID  m = jnienv->GetFieldID(cls,  "m", "I");
    jnienv->SetIntField(obj, m, region.m);

    jfieldID  n = jnienv->GetFieldID(cls,  "n", "I");
    jnienv->SetIntField(obj, n, region.n);

    jfieldID dx = jnienv->GetFieldID(cls, "dx", "I");
    jnienv->SetIntField(obj, dx, region.dx);

    jfieldID dy = jnienv->GetFieldID(cls, "dy", "I");
    jnienv->SetIntField(obj, dy, region.dy);

    if (0 != region.coastlinesFile)
    {
      jfieldID coastlinesFile = 
        jnienv->GetFieldID(cls, "coastlinesFile", "Ljava/lang/String;");
      jstring js = jnienv->NewStringUTF(region.coastlinesFile);
      jnienv->SetObjectField(obj, coastlinesFile, js);

#ifndef unix
      //jnienv->ReleaseStringUTFChars(js, region.coastlinesFile);
#endif
    }

    if (!region.gridmap)
    {}
    else
    {
      jintArray jia = jnienv->NewIntArray(region.m * region.n);
      jint* jip = jnienv->GetIntArrayElements(jia, 0);

      int c = 0;
      for (int i = 1; i <= region.m; i++)
      {
        if (northwest_origin_inputs)
        {
          for (int j = region.n; j >= 1; j--)
          {
            jip[c] = region.gridmap(i, j);
            c++;
          }
        }
        else
        {
          for (int j = 1; j <= region.n; j++)
          {
            jip[c] = region.gridmap(i, j);
            c++;
          }
        }
      }

      jnienv->ReleaseIntArrayElements(jia, jip, 0);

      jfieldID iaAreas = jnienv->GetFieldID(cls, "iaAreas", "[I");
      jnienv->SetObjectField(obj, iaAreas, jia);

      jnienv->DeleteLocalRef(jia);
    }
  }

  if (region.title)
  {
    jfieldID title = jnienv->GetFieldID(cls, "title", "Ljava/lang/String;");
    jstring js = jnienv->NewStringUTF(region.title);
    jnienv->SetObjectField(obj, title, js);

#ifndef unix
    //jnienv->ReleaseStringUTFChars(js, region.title);
#endif
  }

 return obj;
}

jobject jnigraphics3::newLegenddef(const legenddefs& legend)
{
  jclass cls = jnienv->FindClass("legenddef");
  jmethodID mid = jnienv->GetMethodID(cls, "<init>", "()V");
 jobject obj = jnienv->NewObject(cls, mid);

  if (legend.min < legend.max)
  {
    if (legend.type == legenddefs::UNIFORM
        || (legend.type == legenddefs::EXPONENTIAL 
            && legend.min > 0 
            && legend.max > 0))
    {
      jfieldID min = jnienv->GetFieldID(cls,  "min", "D");
      jnienv->SetDoubleField(obj, min, legend.min);

      jfieldID max = jnienv->GetFieldID(cls,  "max", "D");
      jnienv->SetDoubleField(obj, max, legend.max);

      if (legend.increment > 0)
      {
        jfieldID increment = jnienv->GetFieldID(cls,  "increment", "D");
        jnienv->SetDoubleField(obj, increment, legend.increment);
      }

      if (legend.decimals > 0)
      {
        jfieldID decimals = jnienv->GetFieldID(cls,  "decimals", "I");
        jnienv->SetIntField(obj, decimals, (jint)legend.decimals);
      }

      jfieldID type = jnienv->GetFieldID(cls,  "type", "I");
      jnienv->SetIntField(obj, type, legend.type);
    }
  }

 return obj;
}

jobject jnigraphics3::newLineplotdef(const lineplotdefs& lineplot)
{
  jclass cls = jnienv->FindClass("lineplotdef");

  jmethodID mid = jnienv->GetMethodID(cls, "<init>", "()V");

  jobject obj = jnienv->NewObject(cls, mid);

  if (lineplot.title)
  {
    jfieldID title = 
      jnienv->GetFieldID(cls, "title", "Ljava/lang/String;");
    jstring js = jnienv->NewStringUTF(lineplot.title);
    jnienv->SetObjectField(obj, title, js);

#ifndef unix
    //jnienv->ReleaseStringUTFChars(js, lineplot.title);
#endif
  }

  if (lineplot.linesTotal > 0)
  {
    jfieldID linesTotal = jnienv->GetFieldID(cls,  "linesTotal", "I");
    jnienv->SetIntField(obj, linesTotal, lineplot.linesTotal);
  }

  if (lineplot.yaxisTitle)
  {
    jfieldID yaxisTitle = 
      jnienv->GetFieldID(cls, "yaxisTitle", "Ljava/lang/String;");
    jstring js = jnienv->NewStringUTF(lineplot.yaxisTitle);
    jnienv->SetObjectField(obj, yaxisTitle, js);

#ifndef unix
    //jnienv->ReleaseStringUTFChars(js, lineplot.yaxisTitle);
#endif
  }

  if (lineplot.yaxisMin > 0)
  {
    jfieldID yaxisMin = jnienv->GetFieldID(cls,  "yaxisMin", "D");
    jnienv->SetDoubleField(obj, yaxisMin, lineplot.yaxisMin);
  }

  if (lineplot.yaxisMax > 0)
  {
    jfieldID yaxisMax = jnienv->GetFieldID(cls,  "yaxisMax", "D");
    jnienv->SetDoubleField(obj, yaxisMax, lineplot.yaxisMax);
  }

  if (lineplot.yaxisIncrement > 0)
  {
    jfieldID yaxisIncrement = jnienv->GetFieldID(cls,  "yaxisIncrement", "D");
    jnienv->SetDoubleField(obj, yaxisIncrement, lineplot.yaxisIncrement);
  }

  if (lineplot.yaxisDecimals > 0)
  {
    jfieldID yaxisDecimals = jnienv->GetFieldID(cls,  "yaxisDecimals", "I");
    jnienv->SetIntField(obj, yaxisDecimals, lineplot.yaxisDecimals);
  }

  if (lineplot.xaxisTitle)
  {
    jfieldID xaxisTitle = 
      jnienv->GetFieldID(cls, "xaxisTitle", "Ljava/lang/String;");
    jstring js = jnienv->NewStringUTF(lineplot.xaxisTitle);
    jnienv->SetObjectField(obj, xaxisTitle, js);

#ifndef unix
    //jnienv->ReleaseStringUTFChars(js, lineplot.xaxisTitle);
#endif
  }

  if (lineplot.xaxisMax > 0)
  {
    jfieldID xaxisMax = jnienv->GetFieldID(cls,  "xaxisMax", "I");
    jnienv->SetIntField(obj, xaxisMax, lineplot.xaxisMax);
  }

  if (lineplot.xaxisIncrement > 0)
  {
    jfieldID xaxisIncrement = jnienv->GetFieldID(cls,  "xaxisIncrement", "I");
    jnienv->SetIntField(obj, xaxisIncrement, lineplot.xaxisIncrement);
  }

  if (!lineplot.linesColors)
    {}
  else
  {
    int i0 = lineplot.linesColors.indexmin();
    int i2 = lineplot.linesColors.indexmax();
    if (i0 == 1 && lineplot.linesTotal <= i2)
    {
      jmethodID mid = 
        jnienv->GetMethodID(cls, "setColor", "(Ljava/awt/Color;I)V");

      jclass cls2 = jnienv->FindClass("java/awt/Color");
      jmethodID mid2 = jnienv->GetMethodID(cls2, "<init>", "(I)V");

      for (int i = i0; i <= i2; i++)
      { 
        jobject obj2 = jnienv->NewObject(cls2, mid2, lineplot.linesColors(i));
        jnienv->CallVoidMethod(obj, mid, obj2, i - 1); 
      }
    }
  }

 return obj;
}

jobject jnigraphics3::newHistogramdef(const histogramdefs& histogram)
{
  jclass cls = jnienv->FindClass("histogramdef");
  jmethodID mid = jnienv->GetMethodID(cls, "<init>", "()V");
 jobject obj = jnienv->NewObject(cls, mid);
  if (histogram.title)
  {
    jfieldID title = 
      jnienv->GetFieldID(cls, "title", "Ljava/lang/String;");
    jstring js = jnienv->NewStringUTF(histogram.title);
    jnienv->SetObjectField(obj, title, js);

#ifndef unix
    //jnienv->ReleaseStringUTFChars(js, histogram.title);
#endif
  }

  if (histogram.yaxisTitle)
  {
    jfieldID yaxisTitle = 
      jnienv->GetFieldID(cls, "yaxisTitle", "Ljava/lang/String;");
    jstring js = jnienv->NewStringUTF(histogram.yaxisTitle);
    jnienv->SetObjectField(obj, yaxisTitle, js);

#ifndef unix
    //jnienv->ReleaseStringUTFChars(js, histogram.yaxisTitle);
#endif
  }

  if (histogram.yaxisMin < 0)
  {
    jfieldID yaxisMin = jnienv->GetFieldID(cls,  "yaxisMin", "D");
    jnienv->SetDoubleField(obj, yaxisMin, histogram.yaxisMin);
  }

  if (histogram.yaxisMax > 0)
  {
    jfieldID yaxisMax = jnienv->GetFieldID(cls,  "yaxisMax", "D");
    jnienv->SetDoubleField(obj, yaxisMax, histogram.yaxisMax);
  }

  if (histogram.yaxisIncrement > 0)
  {
    jfieldID yaxisIncrement = jnienv->GetFieldID(cls,  "yaxisIncrement", "D");
    jnienv->SetDoubleField(obj, yaxisIncrement, histogram.yaxisIncrement);
  }

  if (histogram.yaxisDecimals > 0)
  {
    jfieldID yaxisDecimals = jnienv->GetFieldID(cls,  "yaxisDecimals", "I");
    jnienv->SetIntField(obj, yaxisDecimals, histogram.yaxisDecimals);
  }

  if (histogram.barTitle != 0)
  {
    jfieldID barTitle = 
      jnienv->GetFieldID(cls, "barTitle", "Ljava/lang/String;");
    jstring js = jnienv->NewStringUTF(histogram.barTitle);
    jnienv->SetObjectField(obj, barTitle, js);

#ifndef unix
    //jnienv->ReleaseStringUTFChars(js, histogram.barTitle);
#endif
  }

  if (histogram.barTotal > 0)
  {
    jfieldID barTotal = jnienv->GetFieldID(cls,  "barTotal", "I");
    jnienv->SetIntField(obj, barTotal, histogram.barTotal);
  }

  if (histogram.barStack > 0)
  {
    jfieldID barStack = jnienv->GetFieldID(cls,  "barStack", "I");
    jnienv->SetIntField(obj, barStack, histogram.barStack);
  }

  if (histogram.barSpacing > 0)
  {
    jfieldID barSpacing = jnienv->GetFieldID(cls,  "barSpacing", "I");
    jnienv->SetIntField(obj, barSpacing, histogram.barSpacing);
  }

  if (histogram.barWidth > 0)
  {
    jfieldID barWidth = jnienv->GetFieldID(cls,  "barWidth", "I");
    jnienv->SetIntField(obj, barWidth, histogram.barWidth);
  }

  if (!histogram.barColors)
    {}
  else
  {
    int i0 = histogram.barColors.indexmin();
    int i2 = histogram.barColors.indexmax();
    if (i0 == 1 && histogram.barStack <= i2)
    {
      jmethodID mid = 
        jnienv->GetMethodID(cls, "setColor", "(Ljava/awt/Color;I)V");

      jclass cls2 = jnienv->FindClass("java/awt/Color");
      jmethodID mid2 = jnienv->GetMethodID(cls2, "<init>", "(I)V");

      for (int i = i0; i <= i2; i++)
      { 
        jobject obj2 = jnienv->NewObject(cls2, mid2, histogram.barColors(i));
        jnienv->CallVoidMethod(obj, mid, obj2, i - 1); 
      }
    }
  }

  if (histogram.barLabels != 0)
  {
    jmethodID mid = 
      jnienv->GetMethodID(cls, "setLabel", "(Ljava/lang/String;I)V");

    for (int i = 0; i < histogram.barTotal; i++)
    {
      if (histogram.barLabels != 0)
      {
        jstring js = jnienv->NewStringUTF(histogram.barLabels[i]);
        jnienv->CallVoidMethod(obj, mid, js, i); 
      }
    }
  }

 return obj;
}

int jnigraphics3::add(const int id)
{
  jmethodID mid = jnienv->GetMethodID(jnigraphics3_cls, "add", "(I)Lregion;");
  jnienv->CallVoidMethod(jnigraphics3_obj, mid, id);

  return check_exceptions();
}

int jnigraphics3::addRegion(const regiondefs& region, 
                            const int id)
{
  jobject obj = newRegiondef(region);

  jmethodID mid = 
    jnienv->GetMethodID(jnigraphics3_cls, "addRegion", "(Lregiondef;I)V");
  jnienv->CallVoidMethod(jnigraphics3_obj, mid, obj, id);

  return check_exceptions();
}

int jnigraphics3::addLegend(const legenddefs& legend,
                            const int id)
{
  jobject obj = newLegenddef(legend);

  jmethodID mid = 
    jnienv->GetMethodID(jnigraphics3_cls, "addLegend", "(Llegenddef;I)V");
  jnienv->CallVoidMethod(jnigraphics3_obj, mid, obj, id);

  return check_exceptions();
}

int jnigraphics3::addLineplot(const lineplotdefs& lineplot,
                              const int id)
{
  jobject obj = newLineplotdef(lineplot);

  jmethodID mid = 
    jnienv->GetMethodID(jnigraphics3_cls, "addLineplot", "(Llineplotdef;I)V");
  jnienv->CallVoidMethod(jnigraphics3_obj, mid, obj, id);

  return check_exceptions();
}

int jnigraphics3::addHistogram(const histogramdefs& histogram,
                               const int id)
{
  jobject obj = newHistogramdef(histogram);

  jmethodID mid = 
    jnienv->GetMethodID(jnigraphics3_cls, "addHistogram", "(Lhistogramdef;I)V");
  jnienv->CallVoidMethod(jnigraphics3_obj, mid, obj, id);

  return check_exceptions();
}

int jnigraphics3::addStatusBar()
{
  jmethodID mid = jnienv->GetMethodID(jnigraphics3_cls, "addStatusBar", "()V"); 
  jnienv->CallVoidMethod(jnigraphics3_obj, mid);

  return check_exceptions();
}

int jnigraphics3::layoutAll(const int w, const int h)
{
  jmethodID mid = jnienv->GetMethodID(jnigraphics3_cls, "layoutAll", "(II)V"); 
  jnienv->CallVoidMethod(jnigraphics3_obj, mid, w, h);

  return check_exceptions();
}

int jnigraphics3::drawWorms(const worms& ws, const int id)
{
  int totalworms = ws.wormstotal();
  int size = 0;
  for (int i = 0; i < totalworms; i++)
  {
    int total = ws.wormsize(i);
    if (total > 1)
    { 
      size += 1 + 1 + 2 * total; //rgb + size + numxys
    }
  }

  if (size > 0)
  {
    jintArray jia = jnienv->NewIntArray(size);
    jint* jip = jnienv->GetIntArrayElements(jia, 0);

    int c = 0;
    jca::list<int>::node* n = ws.getHead()->next;
    while (n)
    { 
      int rgb = n->t;
      n = n->next;

      int total = n->t;
      n = n->next;

      if (total > 1)
      {
        jip[c++] = rgb;
        jip[c++] = total - 1;
        for (int j = 0; j < total; j++)
        {
          jip[c++] = n->t;
          n = n->next;
          jip[c++] = n->t;
          n = n->next;
        }
      }
      else if (total == 1) //skip single points
      {
        n = n->next;
        n = n->next;
      }
    }

    jnienv->ReleaseIntArrayElements(jia, jip, 0);

    jmethodID mid = 
      jnienv->GetMethodID(jnigraphics3_cls, "getRegion", "(I)Lregion;"); 
    jobject obj = jnienv->CallObjectMethod(jnigraphics3_obj, mid, id);
    jclass cls = jnienv->FindClass("region");
    jmethodID setWorms = jnienv->GetMethodID(cls, "setWorms", "([I)V"); 
    jnienv->CallVoidMethod(obj, setWorms, jia);
    jnienv->DeleteLocalRef(jia);
  }

  return check_exceptions();
}

int jnigraphics3::drawStatusBar(const char* statusbar)
{
  jstring js = jnienv->NewStringUTF((char*)statusbar);
  drawStatusBar(js);

#ifndef unix
  //jnienv->ReleaseStringUTFChars(js, (char*)statusbar);
#endif

  return check_exceptions();
}

int jnigraphics3::drawStatusBar(const jstring js)
{
  jmethodID mid = jnienv->GetMethodID(jnigraphics3_cls, "drawStatusBar", "(Ljava/lang/String;)V"); 
  jnienv->CallVoidMethod(jnigraphics3_obj, mid, js);

  return check_exceptions();
}

int jnigraphics3::save_png(const char* filename, const int w, const int h, const int id)
{
  if (filename)
  {
    jstring js = jnienv->NewStringUTF((char*)filename);
    jmethodID mid = jnienv->GetMethodID(jnigraphics3_cls, 
                                        "save_png", 
                                        "(Ljava/lang/String;III)I"); 
    jnienv->CallIntMethod(jnigraphics3_obj, mid, js, w, h, id);
  }

  return check_exceptions();
}

int jnigraphics3::drawArrows(const int* ia, const int size, const int row,
                             const int xmins, const int ymins, const int id)
{
  if (ia && size > 0)
  {
    jmethodID mid = 
      jnienv->GetMethodID(jnigraphics3_cls, "getRegion", "(I)Lregion;"); 
    jobject obj = jnienv->CallObjectMethod(jnigraphics3_obj, mid, id);

    jintArray jia = jnienv->NewIntArray(size);
    jint* jip = jnienv->GetIntArrayElements(jia, 0);

    int idx = 0;
    for (int i = 0; i < size; i++)
      { jip[idx++] = ia[i]; }

    jnienv->ReleaseIntArrayElements(jia, jip, 0);

    jclass region_cls = jnienv->FindClass("region");
    jmethodID mid2 = 
      jnienv->GetMethodID(region_cls, "drawArrows2", "([IIII)V");

    jnienv->CallVoidMethod(obj, mid2, jia, row, xmins, ymins);
  }
  
  return 0;
}

int jnigraphics3::drawCst(const char* filename, 
                          const int xmins, const int ymins, const int id)
{
  jmethodID mid = 
    jnienv->GetMethodID(jnigraphics3_cls, "getRegion", "(I)Lregion;"); 
  jobject obj = jnienv->CallObjectMethod(jnigraphics3_obj, mid, id);

  jclass region_cls = jnienv->FindClass("region");
  jmethodID mid2 = 
    jnienv->GetMethodID(region_cls, "drawCst2", "(Ljava/lang/String;II)V");

  jstring js = jnienv->NewStringUTF(filename);
  jnienv->CallVoidMethod(obj, mid2, js, xmins, ymins);

  return 0;
}

int jnigraphics3::paintAll(const int milliseconds)
{
  jmethodID mid = jnienv->GetMethodID(jnigraphics3_cls, "paintAll", "(I)I"); 
  int r = jnienv->CallIntMethod(jnigraphics3_obj, mid, milliseconds);

  check_exceptions();

  return r;
}

void jnigraphics3::invoke(const char* classpath)
{
	std::string java_class_path = "-Djava.class.path="

#ifdef __CYGWIN__
        + std::string(classpath) + std::string("\\jnigraphics3.jar;")
        + std::string(classpath) + std::string("\\jai_codec.jar;")
        + std::string(classpath) + std::string("\\jai_core.jar");
#else
        + std::string(classpath) + std::string("/jnigraphics3.jar:")
        + std::string(classpath) + std::string("/jai_codec.jar:")
        + std::string(classpath) + std::string("/jai_core.jar");
#endif
	jvmoptions[0].optionString = (char*)java_class_path.c_str();

	std::string java_library_path = std::string("-Djava.library.path=") + std::string(classpath);
	jvmoptions[1].optionString = (char*)java_library_path.c_str();

	jvmoptions[2].optionString = "-ea";
	jvmoptions[3].optionString = "-Xmx138m";


	JavaVMInitArgs jvmargs;
	jvmargs.version = JNI_VERSION_1_4;
	jvmargs.nOptions = 4;
	jvmargs.options = jvmoptions;

	if (JNI_CreateJavaVM(&jnivm, (void**)&jnienv, &jvmargs) != 0) {
		std::cerr << "Error: unable to create jvm using classpath " << classpath
			  << "\n\t 1. check classpath\n"
			  <<   "\t 2. path to jvm.[so/dll] is in the ORIGINAL installation directory\n"
			  <<   "\t 3. set PATH to point to jvm.[so/dll]\n";
		exit(1);
	}

	jnigraphics3_cls = jnienv->FindClass("jnigraphics3");
	if (!jnigraphics3_cls) {
		std::cerr << "Error: unable to find jnigraphics3.class in classpath \"" << classpath << "\"\n";
		check_exceptions();
		std::cerr << jvmoptions[0].optionString << std::endl;
		std::cerr << jvmoptions[1].optionString << std::endl;
		exit(1);
	}

	jmethodID cmid = jnienv->GetMethodID(jnigraphics3_cls, "<init>", "()V");
	jnigraphics3_obj = jnienv->NewObject(jnigraphics3_cls, cmid);
}

void jnigraphics3::unvoke()
{
  if (jnivm)
  {
    jnivm->DestroyJavaVM();
    jnivm = 0;
  }
}

bool jnigraphics3::check_exceptions()
{
  if (jnienv->ExceptionOccurred())
  {
    jnienv->ExceptionDescribe();
    jnienv->ExceptionClear();

    return true;
  }

  return false;
}
