//$Id: halfcomp.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include <fvar.hpp>
//#include "trace.h"


void assignSeapodym(dmatrix& density, const dmatrix& seapodym)
{
  int i1 = density.rowmin();
  int i2 = density.rowmax();
  for(int i = i1; i <= i2; i++)
    {
      int j1 = density(i).indexmin();
      int j2 = density(i).indexmax();
      for (int j = j1; j <= j2; j++)
	{
	  density(i, j) = seapodym(i, j);
	}
    }
}
 
 
void startingDensity(dmatrix& density, const dmatrix& initial)
{
  int i1 = density.rowmin();
  int i2 = density.rowmax();
  for (int i = i1; i <= i2; i++)
    {
      int j1 = density(i).indexmin();
      int j2 = density(i).indexmax();
      for (int j = j1; j <= j2; j++)
        {
          density(i, j) = initial(i, j);
        }
    }
}


void initialDensity(const dmatrix& density, const imatrix map, dvector& initial_density)
{
  int i1 = density.rowmin();
  int i2 = density.rowmax();
  for (int i = i1; i <= i2; i++)
    {
      int j1 = density(i).indexmin();
      int j2 = density(i).indexmax();
      for (int j = j1; j <= j2; j++)
        {
          int k = map(i, j);
          initial_density(k) += density(i, j);
        }
    }
}


void densitycomp(const dmatrix& density, const imatrix map,
		 const double curr_time, dmatrix& zone_density)
{
  int i1 = density.rowmin();
  int i2 = density.rowmax();
  for (int i = i1; i <= i2; i++)
    {
      int j1 = density(i).indexmin();
      int j2 = density(i).indexmax();
      for (int j = j1; j <= j2; j++)
	{
	  int k = map(i, j);
	  zone_density(k, int(curr_time)) += density(i, j);
	}
    }
}
 
void remove_tags(dmatrix& density, const imatrix map, const int region_drop)
{
  int i1 = density.rowmin();
  int i2 = density.rowmax();
  for (int i = i1; i <= i2; i++)
    {
      int j1 = density(i).indexmin();
      int j2 = density(i).indexmax();
      for (int j = j1; j <= j2; j++)
        {
          int k = map(i, j);
          if ( k == region_drop )
	    {
	    density(i, j) = 0.0;
	    }
        }
    }
}

void initial_prev_zs(const dvector& sum0, dvector& prev_sum)
{
  int k1 = sum0.indexmin();
  int k2 = sum0.indexmax();
    for (int k = k1; k <= k2; k++)
      {
	prev_sum(k) = sum0(k);
      }
}

void halfcomp(const dmatrix& density, dmatrix& prev_density, 
              const double curr_time, double& prev_time,
              const double half, const dmatrix& ini_density, 
              dmatrix& half_life)
{
  int i1 = density.rowmin();
  int i2 = density.rowmax();
  for (int i = i1; i <= i2; i++)
  {
    int j1 = density(i).indexmin();
    int j2 = density(i).indexmax();
    for (int j = j1; j <= j2; j++)
    {
      if ( (half_life(i,j) < 0.0) && ((density(i,j)/ini_density(i,j)) <= half) )
      {
        double b = (density(i,j)-prev_density(i,j))/(curr_time-prev_time);
        half_life(i,j) = (b*prev_time + half - prev_density(i,j))/b;
      }
    }
  }
  prev_density = density;
  //prev_time = curr_time;
}        

void update_average(const dmatrix& current, dmatrix& average, 
                    const double& curr_time, double& prev_time)
{
  double w = curr_time;
  double w1 = prev_time; 
  int i1 = average.rowmin();
  int i2 = average.rowmax();
  for (int i = i1; i <= i2; i++)
  {
    int j1 = average(i).indexmin();
    int j2 = average(i).indexmax();
    for (int j = j1; j <= j2; j++)
    {
        average(i,j) = (w1*average(i,j)+current(i,j))/w;
    }
  }
  //prev_time = curr_time;
} 


void halfcomp(const dmatrix& density, const imatrix map, dvector& sum0, 
              dvector& prev_sum, dvector& cur_sum, 
              const double cur_time, double& prev_time,
              const double half, dvector& half_life)
{
  cur_sum.initialize();
  int i1 = density.rowmin();
  int i2 = density.rowmax();
  for (int i = i1; i <= i2; i++)
  {
    int j1 = density(i).indexmin();
    int j2 = density(i).indexmax();
    for (int j = j1; j <= j2; j++)
    {
      int k = map(i,j);
      cur_sum(k) += density(i,j);
    }
  } 

  if (sum(sum0) <= 0.0)
  {
    sum0 = cur_sum;
  }
  else
  {
    int k1 = sum0.indexmin();
    int k2 = sum0.indexmax();
    for (int k = k1; k <= k2; k++)
    {
      if ( (half_life(k) < 0.0) && 
           (sum0(k) > 0.0) && 
           (cur_sum(k)/sum0(k) <= half) )
      {  
        double yp = prev_sum(k)/sum0(k);
        double yc = cur_sum(k)/sum0(k);
        double b = (yp-yc)/(prev_time-cur_time);
        half_life(k) = (b*prev_time-yp+half)/b;
        /*
        if (k == 25)
        {
          TTRACE(yp,yc)
          TTRACE(prev_time,cur_time)
          TTRACE(b,half_life(k))
        }
        */
      } 
    }
  }

  //prev_time = cur_time;
  prev_sum = cur_sum;
}

void update_average(const dvector& current, dvector& average, 
                    const double& curr_time, double& prev_time)
{
  double w = curr_time;
  double w1 = prev_time; 
  int i1 = average.indexmin();
  int i2 = average.indexmax();
  for (int i = i1; i <= i2; i++)
  {
    //if (current(i) > 0.0)
      average(i) = (w1*average(i)+current(i))/w;
  }
  //prev_time = curr_time;
} 
