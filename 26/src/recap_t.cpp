//$Id: recap_t.cpp 3100 2013-08-17 01:52:57Z jsibert $
#include "recap_t.h"
#include "trace.h"


recaptype::recaptype()
{
   strcpy(fleet,"xxxx");
   ifr_flindex = 0;
   par_flindex = 0;
   date.set(12,1940);
   cohort = 0;
   i = 0;
   j = 0;
   returns = 0;
   effort_flag = 0;
}

recaptype_vector* recaptype_vector::instance = 0;

recaptype_vector::recaptype_vector(int n1, int n2):totalRecords(0)
{
  v = (recaptype_vrep*) malloc( (size_t)sizeof(recaptype_vrep) );
  v->en1 = n1;
  v->en2 = n2;
  v->nsize = n2-n1+1;
  v->refcnt = 1;
  cohort_pooling_flag = 0;
  total_recaps = 0.0;
  current_position = 1;

  allocate ();
}

void recaptype_vector::allocate(void)
{
  v->p = (recaptype_*) malloc( (size_t)v->nsize*sizeof(recaptype_) );
  if (!v->p)
  {
    cerr << "New failed in vector allocation.\n";
    exit(1);
  }
}

recaptype_vector::~recaptype_vector()
{
  if (--v->refcnt==0)
  {
    free(v->p);
    free(v);
  }
}

recaptype_& recaptype_vector::operator[](const int i) const
{
#ifndef OPT_LIB
   if ( (i < v->en1) || (i > v->en2) )
   {
      cerr << "subscript out of range in recaptype_vector\n";
      cerr << "i="<< i << ", n1=" << v->en1 <<", n2="<<v->en2<<"\n";
      ad_exit(1);
   }
#endif
   return v->p[i-v->en1];
}

recaptype_vector& recaptype_vector::operator=(recaptype_vector& x)
{
  if ( (x.v->en1 != v->en1) || (x.v->en2 != v->en2) )
  {
    cerr << "Attempted assignment to vectors of unequal size.\n";
    exit(1);
  }
  x.v->refcnt++;
  v = x.v;
  return *this;
}

/** Count tag returns without reported fishing effort.
\return counts ivector[0,1]. 
counts[0] contians tag returns with no reported fishing effort.
counts[1] contians tag returns with reported fishing effort.
counts[2] contians total tag returns.
*/
ivector recaptype_vector::count_effortless_recaps(void)
{
  ivector counts(0,2);
  counts.initialize();
  //for (int k = indexmin(); k <= indexmax(); k++)
  for (int k = 1; k <= nrec; k++)
  {
    recaptype recaps = (*this)[k];
    int eflag = recaps.effort_flag;
    counts(eflag) += recaps.returns;
    counts(2) += recaps.returns;
  }
  return(counts);
}
 
void recaptype_vector::tabulate_effortless_recaps(ofstream& os,
                    const adstring_array& fleet_list)
{
  const int f1 = fleet_list.indexmin();
  const int nfleet = fleet_list.indexmax();
  const int first_year = (*this)[1].date.get_year();
  const int  last_year = (*this)[nrec].date.get_year();
  d4_array total(0,2,f1,nfleet,1,12,first_year,last_year);
  total.initialize();
  for (int k = 1; k <= nrec; k++)
  {
    recaptype recaps = (*this)[k];
    const int eflag = recaps.effort_flag;
    const int f = recaps.par_flindex;
    if (f > 0)
    {
       const int m = recaps.date.get_month_value();
       const int y = recaps.date.get_year();
       if (y < first_year)
       {
          clogf << "year is less than first year of tag recaptures" << endl;
          TTRACE(k,recaps)
          TTRACE(first_year, y)
       // ad_exit(1);
       }
       else if (y > last_year)
       {
          clogf << "year is greater than last year of tag recaptures" << endl;
          TTRACE(k,recaps)
          TTRACE(last_year, y)
       // exit(1);
       }
       else
       {
         total(eflag,f,m,y) += recaps.returns;
         total(2,f,m,y) += recaps.returns;
       } 
    }
  }

  int psave = os.precision();
  os.precision(0);

  int e = 0;
  os << "\nTotal recaptures by fleet:" << endl;
  os << "        fleet    W/O      W    sum" << endl;
  //    "  1      cnps     17      6     23"
 
  for (int f = f1; f <= nfleet; f++)
  {
     os << setw(3) << f << " " << setw(6) << fleet_list[f];
     for (int ee = 0; ee <= 2; ee++)
       os << " " << setw(6) << fixed << sum(total(ee,f));
     os << endl;
  }
  os << "\nTotal effortless recaptures by month:" << endl;
  os << setw(5) << "Year";
  for (int m = 1; m <= 12; m++)
     os << " " << setw(6) << m;
  os << endl;
  for (int y = first_year; y <= last_year; y++)
  {
    os << setw(5) << y;
    for (int m = 1; m <= 12; m++)
    {
      double msum = 0.0;
      for (int f = 1; f <= nfleet; f++)
      { 
         msum += total(e,f,m,y);
      }
      os << " " << setw(6) << msum;
    }
    os << endl;
  } 

  os << "\nTotal effortless recaptures by year, month and fleet:" << endl;
  os << " year mo";
  for (int f = 1; f <= nfleet; f++)
     os << " " << fleet_list[f];
  os << endl;
  for (int y = first_year; y <= last_year; y++)
  {
    for (int m = 1; m <= 12; m++)
    {
      os << setw(5) << y << setw(3) << m;
      for (int f = 1; f <= nfleet; f++)
         os <<  " " << setw(6) << total(e,f,m,y);
      os << endl;
    }
  }

  os << "\nProportion  effortless recaptures by month:" << endl;
  os << setw(5) << "Year";
  for (int m = 1; m <= 12; m++)
     os << " " << setw(10) << m;
  os << endl;
  for (int y = first_year; y <= last_year; y++)
  {
    os << setw(5) << y;
    for (int m = 1; m <= 12; m++)
    {
      double num = 0.0;
      double den = 0.0;
      for (int f = 1; f <= nfleet; f++)
      { 
         num += total(0,f,m,y);
         den += total(0,f,m,y)+total(1,f,m,y);
      }
      if (den > 0.0)
      {
        double prop = num/(den+1e-8);
        os << " " << setw(10) << setprecision(3) << prop;
      }
      else
      {
        os << " " << setw(10) << "NA";
      }
         
    }
    os << endl;
  } 


  os << endl;
  os.precision(psave);
}




