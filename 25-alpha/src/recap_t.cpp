//$Id: recap_t.cpp 3206 2014-08-22 05:37:57Z jsibert $
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
ivector recaptype_vector::count_effortless_recaps(const int last_cohort,
        adstring_array& fleet_names)
{

   const int fleet0 = fleet_names.indexmin();
   const int nfleet = fleet_names.indexmax();
   imatrix fleet_recaps(fleet0, nfleet,0,1);
   fleet_recaps.initialize();
   imatrix cohort_recaps(0,last_cohort,0,1);
   cohort_recaps.initialize();
   i3_array general_recaps(0,1,0,last_cohort,fleet0,nfleet);
   general_recaps.initialize();
   int total_recaps = 0;

   for (int k = 1; k <= nrec; k++)
   {
      recaptype recaps = (*this)[k];
      const int cc = recaps.cohort;
      const int ff = recaps.par_flindex;
      const int eflag = recaps.effort_flag; // eflag is either 0 or 1
      const int returns = recaps.returns;

      total_recaps += returns;
      fleet_recaps(ff,eflag) += returns;
      cohort_recaps(cc,eflag) += returns;
      general_recaps(eflag,cc,ff) += returns;
   }

   clogf << "\nRecaptures by cohort and fleet:" << endl;

   for (int ef = 1; ef >=0; ef--)
   {
      if (ef == 0)
         clogf << "WITHOUT reported fishing effort" << endl;
      else
         clogf << "With reported fishing effort" << endl;
      clogf << "cohort";
      for (int ff = fleet0; ff <= nfleet; ff++)
         clogf << "  " << fleet_names(ff);
      clogf << "   Total" << endl;
   for (int cc = 0; cc <= last_cohort; cc++)
   {
      clogf << setw(6) << cc;
      for (int ff = 0; ff <= nfleet; ff++)
	 clogf << setw(6) << general_recaps(ef,cc,ff);
    
      clogf << setw(8) << sum(general_recaps(ef,cc));
      clogf << setw(8) << cohort_recaps(cc,ef);
      clogf << endl;
   }
   clogf << endl;
   }

   clogf << "\nRecaptures by fleet:" << endl;
   clogf << "      fleet  with E WITHOUT   total" << endl;
   for (int ff = 0; ff <= nfleet; ff++)
   {
      clogf << setw(8) << fleet_names(ff)
            << setw(8) << fleet_recaps(ff,1)
            << setw(8) << fleet_recaps(ff,0)
            << setw(8) << sum(fleet_recaps(ff)) << endl;
   }
   clogf << "      Total"
         << setw(8) << colsum(fleet_recaps,1) 
         << setw(8) << colsum(fleet_recaps,0)
         << setw(8) << sum(fleet_recaps) << endl;
   clogf << endl;
   TRACE(total_recaps)
   TRACE(sum(fleet_recaps))
   TRACE(sum(cohort_recaps))
   //TRACE(sum(general_recaps))

   //clogf << setw(6) << ssum << setw(6) << sum(effortless_recaps) <<  endl;
   //clogf << setw(6) << (sum(fleet_recaps)-colsum(fleet_recaps,0)) 
   //      << setw(6) << (sum(fleet_recaps)+sum(effortless_recaps)) << endl;

  ///////////////////////////////////////////////////

  ivector counts(0,2);
  counts.initialize();
  //for (int k = indexmin(); k <= indexmax(); k++)
  cout << "nrec: " << nrec << endl;
  for (int k = 1; k <= nrec; k++)
  {
    recaptype recaps = (*this)[k];
    const int eflag = recaps.effort_flag; // eflag is either 0 or 1
    counts(eflag) += recaps.returns;
    counts(2) += recaps.returns;
  }
  return(counts);
}

void recaptype_vector::compute_obs_history(imatrix& obs_hist, year_month& start_date)
{
   for (int k = 1; k <= nrec; k++)
   {
      recaptype recaps = (*this)[k];
      const int eflag = recaps.effort_flag; // eflag is either 0 or 1
      const int hist_index = recaps.date - start_date;
      if (eflag == 0)
        obs_hist(hist_index,2) += recaps.returns;
      else
        obs_hist(hist_index,1) += recaps.returns;
      obs_hist(hist_index,3) += recaps.returns;
   }
}

// recaps[k].ifr_flindex = irfr.flindex(fleet);
// recaps[k].par_flindex = adstring_utils_index(param.fleet_names,fleet);

void recaptype_vector::make_effort_proxy(d4_array& effort, year_month& start_date,
                       const adstring_array& fleet_list)
{
   const int hs1 = effort.hslicemin(); // minimum month index
   const int hs2 = effort.hslicemax(); // maximum month index
   effort.initialize();

   // s1 = fleet0; s2 = nfleet
   const int s1 = effort(hs1).slicemin();
   const int s2 = effort(hs1).slicemax();
   TTRACE(s1,s2)
   TTRACE(fleet_list.indexmin(), fleet_list.indexmax())
   dvector incidents(s1,s2);
   incidents.initialize();

   //d4_array returns;
   //HERE
   //returns.allocate(effort);
   //HERE
   //returns.initialize();
   //HERE
   // count cells in which recapture events occured
   for (int k = 1; k <= nrec; k++)
   {
      recaptype recaps = (*this)[k];
      const int m = recaps.date - start_date;
      if (m <= hs2)
      {
         const int f = recaps.par_flindex; // 0 for unspecified fleets
         const int i = recaps.i;
         const int j = recaps.j;

         incidents(f) ++;
         //effort(m,f,i,j) ++; // counts cells
         //returns(m,f,i,j) += recaps.returns;
         effort(m,f,i,j) += recaps.returns;
      }
   }
   TRACE(sum(effort))
   TTRACE(sum(incidents),incidents)

   dvector mean_effort(s1,s2);
   mean_effort.initialize();
   for (int f = s1; f <= s2; f++)
   {
      for (int m = hs1; m <= hs2; m++) // date loop
      {
         mean_effort(f) += sum(effort(m,f))/incidents(f);
      }
      TTRACE(f,mean_effort(f))
   }
   TTRACE(sum(mean_effort), mean_effort)

   for (int f = s1; f <= s2; f++)
   {
      for (int m = hs1; m <= hs2; m++) // date loop
      {
         effort(m,f) = sum(effort(m,f))/mean_effort(f);
      }
   }
}

/*
void set_effort_proxy(d3_array& effort, const d3_array& recaps)
{
   effort.initialize();

   const int f1 = recaps.slicemin();
   const int f2 = recaps.slicemax();
   TTRACE(f1,f2)
   // count cells in which recaptured occured
   for (int f = f1; f <= f2; f++)
   {
      int (i = recaps(f).rowmin(); i <= recaps(f).rowmax(); s++)
      {
         for (int j = recaps(f,i).indexmin(); j <= recaps(f,i).indexmin(); j++)
         {
            if (recaps(f,i,j) > 0)
               effort(f,i,j) ++;
         }
      }
   }
  // sum recaptures cells by date and fleet
  dvector den(f1,f2);
  den.initialize();
  for (int f = f1; f <= f2; f++) // fleet loop
  {
    den(f) += sum(effort(f));
  }

   for (int f = f1; f <= f2; f++)
   {
      if (den(f) > 0.0)
      {
         for (int i = effort(f).rowmin(); i <= effort(f).rowmax(); i++)
         {
           for (int j = effort(f,i).indexmin(); j <= effort(f,i).indexmax(); j++)
           {
             effort(f,i,j) /= den(h,s);
           }
        } //if (den(h,f) > 0.0)
     }
  }
}
*/

/*
dvector recaptype_vector::count_recap_strata(const adstring_array& fleet_list,
  const ivector& ilbv, const ivector& iubv,
  const imatrix& jlbm, const imatrix& jubm)
{
  const int f1 = 0; //fleet_list.indexmin();
  const int nfleet = fleet_list.indexmax();

  d3_array ObsRecaps(f1, nfleet, ilbv, iubv, jlbm, jubm);
  ObsRecaps.initialize();



  for (int k = 1; k <= nrec; k++)
  {
    recaptype recaps = (*this)[k];
    const int f = recaps.par_flindex;
    const int i = recaps.i;
    const int j = recaps.j;
    ObsRecaps(f,i,j) += recaps.returns;
  }

  ivector count(f1,nfleet);
  count.initialize();
  for (int f = f1; f <= nfleet; f++)
  {
     for (int i = ObsRecaps(f).rowmin(); i <= ObsRecaps(f).rowmax(); i++)
     {
        for (int j = ObsRecaps(f,i).indexmin(); j <= ObsRecaps(f,i).indexmax(); j++)
        {
           if (ObsRecaps(f,i,j) > 0)
              count(f) ++;
        }
      }
  }

  const double total = (double)sum(count);
  dvector ratio(f1,nfleet);
 
  clogf << "\nStrata fished by fleet"<< endl;
  clogf << "  f    fleet strata  ratio"<< endl;
            //1     pgps    105 0.09686
  for (int f = f1; f <= nfleet; f++)
  {
     ratio(f) = (double)count(f)/total;
     clogf << setw(3) << f;
     if (f < 1)
        clogf << setw(6) << "unk";
     else
        clogf << setw(6) << fleet_list(f);
     clogf << setw(7) << count(f) << setw(9) << fixed << ratio(f) << endl;
  }

  return(ratio);
}
*/
 
void recaptype_vector::tabulate_effortless_recaps(ofstream& os,
                    const adstring_array& fleet_list)
{
  const int f1 = fleet_list.indexmin();
  const int nfleet = fleet_list.indexmax();
  TTRACE(f1,nfleet)
  const int first_year = (*this)[1].date.get_year();
  const int  last_year = (*this)[nrec].date.get_year();
  d4_array total(0,2,f1,nfleet,1,12,first_year,last_year);
  total.initialize();
  for (int k = 1; k <= nrec; k++)
  {
    recaptype recaps = (*this)[k];
    const int eflag = recaps.effort_flag;
    const int f = recaps.par_flindex;
    //if (f > 0)
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




