#include "jnigraphics3.h"

void jnigraphics3::addGridLayout(const int m, const int n, ivector& iv, const int id)
{
  iv.allocate(1, m * n);
  int* ia = addGridLayout(m, n, id);

  int mn = m * n;
  for (int i = 1; i <= mn; i++) 
    { iv(i) = ia[i - 1]; }
  if (ia) delete ia;
}

void jnigraphics3::addTabs(const int num, ivector& iv, const int id)
{
  iv.allocate(1, num);
  int* ia = addTabs(num, id);

  for (int i = 1; i <= num; i++) 
    { iv(i) = ia[i - 1]; }

  if (ia) delete ia;

}


int jnigraphics3::drawLineplot(const double d, const int id)
{
  dvector dv(1, 1);
  dv(1) = d;
  drawLineplots(dv, id);
  return 0;
}

int jnigraphics3::drawLineplots(const dvector& dv, const int id)
{
  jdoubleArray jda = dvector2jdoubleArray(dv);

  jmethodID mid = 
    jnienv->GetMethodID(jnigraphics3_cls, "getRegion", "(I)Lregion;"); 
  jobject obj = jnienv->CallObjectMethod(jnigraphics3_obj, mid, id);

  jclass region_cls = jnienv->FindClass("region");
  jmethodID mid2 = 
    jnienv->GetMethodID(region_cls, "addLineplots", "([D)V");
  jnienv->CallIntMethod(obj, mid2, jda);

  jnienv->DeleteLocalRef(jda);
  return 0;
}

int jnigraphics3::drawArrows(const dvector& dvX, const dvector& dvY, 
                             const double scaleX, const double scaleY, 
                             const int id)
{
  int size = dvX.indexmin() == dvY.indexmin()
             && dvX.indexmax() == dvY.indexmax()
             ? dvX.indexmax() - dvX.indexmin() + 1
             : 0;

  if (size > 0)
  {
    jmethodID mid = 
      jnienv->GetMethodID(jnigraphics3_cls, "getRegion", "(I)Lregion;"); 
    jobject region_obj = jnienv->CallObjectMethod(jnigraphics3_obj, mid, id);

    jclass region_cls = jnienv->FindClass("region");
    jfieldID defs_fid = jnienv->GetFieldID(region_cls, "alldefs", "Ldefs;"); 
    jobject defs_obj = jnienv->GetObjectField(region_obj, defs_fid);

    jclass defs_cls = jnienv->FindClass("defs");
    jfieldID regiondef_fid = 
      jnienv->GetFieldID(defs_cls, "region", "Lregiondef;"); 
    jobject regiondef_obj = jnienv->GetObjectField(defs_obj, regiondef_fid);

    jclass regiondef_cls = jnienv->FindClass("regiondef");

    jfieldID m_fid = jnienv->GetFieldID(regiondef_cls, "m", "I"); 
    jint m = jnienv->GetIntField(regiondef_obj, m_fid);

    jfieldID n_fid = jnienv->GetFieldID(regiondef_cls, "n", "I"); 
    jint n = jnienv->GetIntField(regiondef_obj, n_fid);

    jfieldID dx_fid = jnienv->GetFieldID(regiondef_cls, "dx", "I"); 
    jint dx = jnienv->GetIntField(regiondef_obj, dx_fid);

    jfieldID dy_fid = jnienv->GetFieldID(regiondef_cls, "dy", "I"); 
    jint dy = jnienv->GetIntField(regiondef_obj, dy_fid);

    jfieldID iaAreas_fid = jnienv->GetFieldID(regiondef_cls, "iaAreas", "[I"); 
    jobject iaAreas_obj = jnienv->GetObjectField(regiondef_obj, iaAreas_fid);
    //jsize iaAreas_size = jnienv->GetArrayLength((jarray)iaAreas_obj);
    jint* iaAreas = jnienv->GetIntArrayElements((jintArray)iaAreas_obj, 0);

    const int total = 1  //color 
                    + 1  //numsegments=1
                    + 2  //x0y0
                    + 2; //x1y1

    jintArray jia = jnienv->NewIntArray(size * total);
    jint* jip = jnienv->GetIntArrayElements(jia, 0);
   
    int idx = 0;
    for (int grid = dvX.indexmin(); grid <= dvX.indexmax(); grid++) 
    {
      jip[idx] = 0;

      double x = dvX(grid);
      double y = dvY(grid);
      if (x != 0 && y != 0)
      {
        jip[idx + 1] = 1;

      //determine tail location
        int x0 = m, x2 = 1,
            y0 = n, y2 = 1;
        int idx2 = 0;
        for (int i = 1; i <= m; i++) 
          for (int j = 1; j <= n; j++) 
          {
            if (iaAreas[idx2] == grid)
            {
              if (i < x0)
                x0 = i;

              if (x2 < i)
                x2 = i;

              if (j < y0)
                y0 = j;

              if (y2 < j)
                y2 = j;
            }

            idx2++;
          }
        
        int x1 = (int)((x2 - x0) * 0.5 * dx) + x0 * dx;
        int y1 = (int)((y2 - y0) * 0.5 * dy) + y0 * dy;
        jip[idx + 2] = x1;
        jip[idx + 3] = y1;

        jip[idx + 4] = x1 + (int)(x * scaleX);
        jip[idx + 5] = y1 + (int)(y * scaleY);

/*
        std::cout << "R: " << jip[idx + 2] << ' ' 
                           << jip[idx + 3] << ' ' 
                           << jip[idx + 4] << ' ' 
                           << jip[idx + 5] << std::endl;
*/
        idx += total;
      }
      else
      {
        jip[idx + 1] = 0;
        idx += 2;
      }
    }

    jnienv->ReleaseIntArrayElements(jia, jip, 0);

    jmethodID setArrows = 
      jnienv->GetMethodID(region_cls, "setArrows", "([I)V"); 
    jnienv->CallVoidMethod(region_obj, setArrows, jia);

    jnienv->DeleteLocalRef(jia);
  }

  return 0;
}

int jnigraphics3::drawArrows(const dmatrix& dmX, const dmatrix& dmY, 
                             const double scaleX, const double scaleY, 
                             const int id)
{
  int size = 0;

  int i0 = dmX.rowmin();
  int in = dmX.rowmax();
  for (int i = i0; i <= in; i++)
  {
    int j0 = dmX(i).indexmin();
    int jn = dmX(i).indexmax();
  
    size += jn - j0 + 1;
  }

  if (size > 0)
  {
    jmethodID mid = 
      jnienv->GetMethodID(jnigraphics3_cls, "getRegion", "(I)Lregion;"); 
    jobject region_obj = jnienv->CallObjectMethod(jnigraphics3_obj, mid, id);

    jclass region_cls = jnienv->FindClass("region");
    jfieldID defs_fid = jnienv->GetFieldID(region_cls, "alldefs", "Ldefs;"); 
    jobject defs_obj = jnienv->GetObjectField(region_obj, defs_fid);

    jclass defs_cls = jnienv->FindClass("defs");
    jfieldID regiondef_fid = 
      jnienv->GetFieldID(defs_cls, "region", "Lregiondef;"); 
    jobject regiondef_obj = jnienv->GetObjectField(defs_obj, regiondef_fid);

    jclass regiondef_cls = jnienv->FindClass("regiondef");
    jfieldID dx_fid = jnienv->GetFieldID(regiondef_cls, "dx", "I"); 
    jint dx = jnienv->GetIntField(regiondef_obj, dx_fid);

    jfieldID dy_fid = jnienv->GetFieldID(regiondef_cls, "dy", "I"); 
    jint dy = jnienv->GetIntField(regiondef_obj, dy_fid);

    jfieldID n_fid = jnienv->GetFieldID(regiondef_cls, "n", "I"); 
    jint n = jnienv->GetIntField(regiondef_obj, n_fid);

    const int total = 1  //color 
                    + 1  //numsegments=1
                    + 2  //x0y0
                    + 2; //x1y1

    jintArray jia = jnienv->NewIntArray(size * total);
    jint* jip = jnienv->GetIntArrayElements(jia, 0);
    
    int idx = 0;
    int i0 = dmX.rowmin();
    int in = dmX.rowmax();
    for (int i = i0; i <= in; i++)
    {
      int x0 = (i - 1) * dx + dx / 2; 

      int k = dmX(i).indexmax();
      int j0 = northwest_origin_inputs ? n - dmX(i).indexmax() + 1 : dmX(i).indexmin();
      int jn = northwest_origin_inputs ? n - dmX(i).indexmin() + 1 : dmX(i).indexmax();
      for (int j = j0; j <= jn; j++)
      {
        int y0 = (j - 1) * dy + dy / 2; 

        jip[idx    ] = 0;

        double x = northwest_origin_inputs ? dmX(i, k) * scaleX : dmX(i, j) * scaleX;
        double y = northwest_origin_inputs ? dmY(i, k) * scaleY : dmY(i, j) * scaleY;
	k--;

        if (x != 0 && y != 0)
        {
          int x2 = (int)(x / 2);
          int y2 = (int)(y / 2);

          jip[idx + 1] = 1;

          jip[idx + 2] = x0 - x2;
          jip[idx + 3] = y0 - y2;

          jip[idx + 4] = x0 + x2;
          jip[idx + 5] = y0 + y2;

/*
          std::cout << "R: " << jip[idx + 2] << ' ' 
                             << jip[idx + 3] << ' ' 
                             << jip[idx + 4] << ' ' 
                             << jip[idx + 5] << std::endl;
*/

          idx += total;
        }
        else
        {
          jip[idx + 1] = 0;
          idx += 2;
        }
      }
    }

    jnienv->ReleaseIntArrayElements(jia, jip, 0);

    jmethodID setArrows = 
      jnienv->GetMethodID(region_cls, "setArrows", "([I)V"); 
    jnienv->CallVoidMethod(region_obj, setArrows, jia);

    jnienv->DeleteLocalRef(jia);
  }

  return 0;
}

int jnigraphics3::drawOvals(const dvector& dvX, const dvector& dvY, 
                            const double scaleX, const double scaleY, 
                            const int id)
{
  int size = dvX.indexmin() == dvY.indexmin()
             && dvX.indexmax() == dvY.indexmax()
             ? dvX.indexmax() - dvX.indexmin() + 1
             : 0;

  if (size > 0)
  {
    jmethodID mid = 
      jnienv->GetMethodID(jnigraphics3_cls, "getRegion", "(I)Lregion;"); 
    jobject region_obj = jnienv->CallObjectMethod(jnigraphics3_obj, mid, id);

    jclass region_cls = jnienv->FindClass("region");
    jfieldID defs_fid = jnienv->GetFieldID(region_cls, "alldefs", "Ldefs;"); 
    jobject defs_obj = jnienv->GetObjectField(region_obj, defs_fid);

    jclass defs_cls = jnienv->FindClass("defs");
    jfieldID regiondef_fid = 
      jnienv->GetFieldID(defs_cls, "region", "Lregiondef;"); 
    jobject regiondef_obj = jnienv->GetObjectField(defs_obj, regiondef_fid);

    jclass regiondef_cls = jnienv->FindClass("regiondef");

    jfieldID m_fid = jnienv->GetFieldID(regiondef_cls, "m", "I"); 
    jint m = jnienv->GetIntField(regiondef_obj, m_fid);

    jfieldID n_fid = jnienv->GetFieldID(regiondef_cls, "n", "I"); 
    jint n = jnienv->GetIntField(regiondef_obj, n_fid);

    jfieldID dx_fid = jnienv->GetFieldID(regiondef_cls, "dx", "I"); 
    jint dx = jnienv->GetIntField(regiondef_obj, dx_fid);

    jfieldID dy_fid = jnienv->GetFieldID(regiondef_cls, "dy", "I"); 
    jint dy = jnienv->GetIntField(regiondef_obj, dy_fid);

    jfieldID iaAreas_fid = jnienv->GetFieldID(regiondef_cls, "iaAreas", "[I"); 
    jobject iaAreas_obj = jnienv->GetObjectField(regiondef_obj, iaAreas_fid);
    //jsize iaAreas_size = jnienv->GetArrayLength((jarray)iaAreas_obj);
    jint* iaAreas = jnienv->GetIntArrayElements((jintArray)iaAreas_obj, 0);

    const int total = 1  //color 
                    + 1  //numsegments=1
                    + 2  //x0y0
                    + 2; //x1y1

    jintArray jia = jnienv->NewIntArray(size * total);
    jint* jip = jnienv->GetIntArrayElements(jia, 0);
   
    int idx = 0;
    for (int grid = dvX.indexmin(); grid <= dvX.indexmax(); grid++) 
    {
      jip[idx] = 0;

      double x = dvX(grid);
      double y = dvY(grid);
      if (x != 0 && y != 0)
      {
        jip[idx + 1] = 1;

      //determine tail location
        int x0 = m, x2 = 1,
            y0 = n, y2 = 1;
        int idx2 = 0;
        for (int i = 1; i <= m; i++) 
          for (int j = 1; j <= n; j++) 
          {
            if (iaAreas[idx2] == grid)
            {
              if (i < x0)
                x0 = i;

              if (x2 < i)
                x2 = i;

              if (j < y0)
                y0 = j;

              if (y2 < j)
                y2 = j;
            }

            idx2++;
          }
        
        int x1 = (int)((x2 - x0) * 0.5 * dx) + x0 * dx;
        int y1 = (int)((y2 - y0) * 0.5 * dy) + y0 * dy;
        jip[idx + 2] = x1;
        jip[idx + 3] = y1;

        jip[idx + 4] = (int)(x * scaleX);
        jip[idx + 5] = (int)(y * scaleY);
        idx += total;
      }
      else
      {
        jip[idx + 1] = 0;
        idx += 2;
      }
    }

    jnienv->ReleaseIntArrayElements(jia, jip, 0);

    jmethodID setOvals = jnienv->GetMethodID(region_cls, "setOvals", "([I)V"); 
    jnienv->CallVoidMethod(region_obj, setOvals, jia);

    jnienv->DeleteLocalRef(jia);
  }

  return 0;
}

int jnigraphics3::drawOvals(const dmatrix& dmX, const dmatrix& dmY, 
                            const double scaleX, const double scaleY, 
                            const int id)
{
  int size = 0;

  int i0 = dmX.rowmin();
  int in = dmX.rowmax();
  for (int i = i0; i <= in; i++)
  {
    int j0 = dmX(i).indexmin();
    int jn = dmX(i).indexmax();
  
    size += jn - j0 + 1;
  }

  if (size > 0)
  {
    jmethodID mid = 
      jnienv->GetMethodID(jnigraphics3_cls, "getRegion", "(I)Lregion;"); 
    jobject region_obj = jnienv->CallObjectMethod(jnigraphics3_obj, mid, id);

    jclass region_cls = jnienv->FindClass("region");
    jfieldID defs_fid = jnienv->GetFieldID(region_cls, "alldefs", "Ldefs;"); 
    jobject defs_obj = jnienv->GetObjectField(region_obj, defs_fid);

    jclass defs_cls = jnienv->FindClass("defs");
    jfieldID regiondef_fid = 
      jnienv->GetFieldID(defs_cls, "region", "Lregiondef;"); 
    jobject regiondef_obj = jnienv->GetObjectField(defs_obj, regiondef_fid);

    jclass regiondef_cls = jnienv->FindClass("regiondef");
    jfieldID dx_fid = jnienv->GetFieldID(regiondef_cls, "dx", "I"); 
    jint dx = jnienv->GetIntField(regiondef_obj, dx_fid);

    jfieldID dy_fid = jnienv->GetFieldID(regiondef_cls, "dy", "I"); 
    jint dy = jnienv->GetIntField(regiondef_obj, dy_fid);

    const int total = 1  //color 
                    + 1  //numsegments=1
                    + 2  //x0y0
                    + 2; //x1y1

    jintArray jia = jnienv->NewIntArray(size * total);
    jint* jip = jnienv->GetIntArrayElements(jia, 0);
    
    int idx = 0;
    int i0 = dmX.rowmin();
    int in = dmX.rowmax();
    for (int i = i0; i <= in; i++)
    {
      int x0 = (i - 1) * dx + dx / 2; 

      int j0 = dmX(i).indexmin();
      int jn = dmX(i).indexmax();
      for (int j = j0; j <= jn; j++)
      {
        int y0 = (j - 1) * dy + dy / 2; 

        jip[idx    ] = 0; 
        jip[idx + 1] = 1;

        jip[idx + 2] = x0;
        jip[idx + 3] = y0;

        jip[idx + 4] = 2 * (int)(dmX(i, j) * scaleX); 
        jip[idx + 5] = 2 * (int)(dmY(i, j) * scaleY);

        idx += total;
      }
    }

    jnienv->ReleaseIntArrayElements(jia, jip, 0);

    jmethodID setOvals = jnienv->GetMethodID(region_cls, "setOvals", "([I)V"); 
    jnienv->CallVoidMethod(region_obj, setOvals, jia);

    jnienv->DeleteLocalRef(jia);
  }

  return 0;
}

int jnigraphics3::drawTriangles(const dmatrix& dmU, 
                                const dmatrix& dmL, 
                                const int id)
{
  jmethodID mid = 
    jnienv->GetMethodID(jnigraphics3_cls, "getRegion", "(I)Lregion;"); 
  jobject region_obj = jnienv->CallObjectMethod(jnigraphics3_obj, mid, id);

  jclass region_cls = jnienv->FindClass("region");
  jfieldID defs_fid = jnienv->GetFieldID(region_cls, "alldefs", "Ldefs;"); 
  jobject defs_obj = jnienv->GetObjectField(region_obj, defs_fid);

  jclass defs_cls = jnienv->FindClass("defs");
  jfieldID regiondef_fid = 
    jnienv->GetFieldID(defs_cls, "region", "Lregiondef;"); 
  jobject regiondef_obj = jnienv->GetObjectField(defs_obj, regiondef_fid);

  jclass regiondef_cls = jnienv->FindClass("regiondef");
  jfieldID m_fid = jnienv->GetFieldID(regiondef_cls, "m", "I"); 
  jint m = jnienv->GetIntField(regiondef_obj, m_fid);

  jfieldID n_fid = jnienv->GetFieldID(regiondef_cls, "n", "I"); 
  jint n = jnienv->GetIntField(regiondef_obj, n_fid);

  jfieldID legenddef_fid = 
    jnienv->GetFieldID(defs_cls, "legend", "Llegenddef;"); 
  jobject legenddef_obj = jnienv->GetObjectField(defs_obj, legenddef_fid);

  jclass legenddef_cls = jnienv->FindClass("legenddef");
  jfieldID min_fid = jnienv->GetFieldID(legenddef_cls, "min", "D"); 
  jdouble min = jnienv->GetDoubleField(legenddef_obj, min_fid);

  jdoubleArray jdaU = dmatrix2jdoubleArray(dmU, m, n, min);
  jdoubleArray jdaL = dmatrix2jdoubleArray(dmL, m, n, min);

  jmethodID drawTriangles = 
    jnienv->GetMethodID(region_cls, "setTriangles", "([D[D)V"); 
  jnienv->CallVoidMethod(region_obj, drawTriangles, jdaU, jdaL);

  jnienv->DeleteLocalRef(jdaU);
  jnienv->DeleteLocalRef(jdaL);

  return 0;
}

int jnigraphics3::drawSquares(const dmatrix& dm, 
                              const int id)
{
  jmethodID mid = 
    jnienv->GetMethodID(jnigraphics3_cls, "getRegion", "(I)Lregion;"); 
  jobject region_obj = jnienv->CallObjectMethod(jnigraphics3_obj, mid, id);

  jclass region_cls = jnienv->FindClass("region");
  jfieldID defs_fid = jnienv->GetFieldID(region_cls, "alldefs", "Ldefs;"); 
  jobject defs_obj = jnienv->GetObjectField(region_obj, defs_fid);

  jclass defs_cls = jnienv->FindClass("defs");
  jfieldID regiondef_fid = 
    jnienv->GetFieldID(defs_cls, "region", "Lregiondef;"); 
  jobject regiondef_obj = jnienv->GetObjectField(defs_obj, regiondef_fid);

  jclass regiondef_cls = jnienv->FindClass("regiondef");
  jfieldID m_fid = jnienv->GetFieldID(regiondef_cls, "m", "I"); 
  jint m = jnienv->GetIntField(regiondef_obj, m_fid);

  jfieldID n_fid = jnienv->GetFieldID(regiondef_cls, "n", "I"); 
  jint n = jnienv->GetIntField(regiondef_obj, n_fid);

  jfieldID legenddef_fid = 
    jnienv->GetFieldID(defs_cls, "legend", "Llegenddef;"); 
  jobject legenddef_obj = jnienv->GetObjectField(defs_obj, legenddef_fid);

  jclass legenddef_cls = jnienv->FindClass("legenddef");
  jfieldID min_fid = jnienv->GetFieldID(legenddef_cls, "min", "D"); 
  jdouble min = jnienv->GetDoubleField(legenddef_obj, min_fid);

  jdoubleArray jda = dmatrix2jdoubleArray(dm, m, n, min);

  jmethodID drawSquares = 
    jnienv->GetMethodID(region_cls, "setSquares", "([D)V"); 
  jnienv->CallVoidMethod(region_obj, drawSquares, jda);

  jnienv->DeleteLocalRef(jda);

  return 0;
}

jdoubleArray jnigraphics3::dvector2jdoubleArray(const dvector& dv) const
{
  jdoubleArray jda;
  int i0 = dv.indexmin();
  int in = dv.indexmax();

  int size = in - i0 + 1;
  jda = jnienv->NewDoubleArray(size);
  jdouble* jdp = jnienv->GetDoubleArrayElements(jda, 0);

  int idx = 0;
  for (int i = i0; i <= in; i++)
    { jdp[idx++] = dv(i); }

  jnienv->ReleaseDoubleArrayElements(jda, jdp, 0);

  return jda;
}

jintArray jnigraphics3::ivector2jintArray(const ivector& iv) const
{
  jintArray jia;
  int i0 = iv.indexmin();
  int in = iv.indexmax();

  int size = in - i0 + 1;
  jia = jnienv->NewIntArray(size);
  jint* jip = jnienv->GetIntArrayElements(jia, 0);

  int idx = 0;
  for (int i = i0; i <= in; i++)
    { jip[idx++] = iv(i); }

  jnienv->ReleaseIntArrayElements(jia, jip, 0);

  return jia;
}

jdoubleArray jnigraphics3::dmatrix2jdoubleArray(const dmatrix& dm, const int m, const int n, const double min) const
{
  jdoubleArray jda;

  int size = m * n;
  if (size > 0)
  {
    jda = jnienv->NewDoubleArray(size);
    jdouble* jdp = jnienv->GetDoubleArrayElements(jda, 0);

    int idx = 0;
    int imin = dm.rowmin();
    int imax = dm.rowmax();
    for (int i = 1; i <= m; i++)
    {
      int jmin = dm(i).indexmin();
      int jmax = dm(i).indexmax();
      if (northwest_origin_inputs)
      {
        for (int j = n; j >= 1; j--)
        {
          jdp[idx++] = imin <= i && i <= imax && jmin <= j && j <= jmax ? dm(i, j) : min;
        }
      }
      else
      {
        for (int j = 1; j <= n; j++)
        {
          jdp[idx++] = imin <= i && i <= imax && jmin <= j && j <= jmax ? dm(i, j) : min;
        }
      }
    }
    jnienv->ReleaseDoubleArrayElements(jda, jdp, 0);
  }

  return jda;
}

int jnigraphics3::drawHistogram(const dvector& dv, const int stack, const int id)
{
  jdoubleArray jda = dvector2jdoubleArray(dv);

  jmethodID mid = 
    jnienv->GetMethodID(jnigraphics3_cls, "getRegion", "(I)Lregion;"); 
  jobject obj = jnienv->CallObjectMethod(jnigraphics3_obj, mid, id);

  jclass region_cls = jnienv->FindClass("region");
  jmethodID mid2 = 
    jnienv->GetMethodID(region_cls, "setHistograms", "([DI)V");
  jnienv->CallIntMethod(obj, mid2, jda, stack);

  jnienv->DeleteLocalRef(jda);

  return 0;
}
