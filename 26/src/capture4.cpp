//$Id: capture4.cpp 2878 2011-11-17 23:51:57Z jsibert $
#include "par_t.h"
#include "recap_t.h"
#include "trace.h"

extern ofstream clogf;

float poidev(float xm, int* idum);

void capcomp(year_month& date, int cc, par_t& param, dvar_matrix& tags,
             ofstream& capfile, dvariable& tcaps, dvar_vector& region_caps, 
             dvar_matrix& total_mort, dvar3_array& fish_mort, 
             ivector& effort_occured, dvar_vector& pred_tac, 
             float scale, int v, dvar_matrix& total_pred_caps)
{
  int seed = param.m_ipar[40];
  random_number_generator rng(seed);

  pred_tac.initialize(); // = 0.0;

  recaptype_ recapture_record;

  recapture_record.date = date;
  //int pool_tags = param.m_ipar[13];
  //if (pool_tags)
  if (param.m_ipar[13] == 1)
    recapture_record.cohort = 0;
  else
    recapture_record.cohort = param.get_tr_cohort(cc);


  int _m = param.get_m();
  dvar_matrix pred_caps(1,_m, param.get_jlb(), param.get_jub());

  total_pred_caps.initialize();
  //clogf << date << " tag density field " << tags << endl;

  int nf = param.get_nfleet();
  for (int f = 1; f <= nf; f++)
  {
    //if (effort_occured(f))
    {
      pred_caps.initialize();
      param.pred_recapture_comp(pred_caps, tags, total_mort, fish_mort, f,date);
//      clogf << "pred: " << param.tr[cc].cohort << " " << date << " " << f << " " << sum(pred_caps) << endl;
//      clogf << "tags: " << param.tr[cc].cohort << " " << date << " " << f << " " << sum(tags) << endl;
      //pred_tac += sum(pred_caps);
      pred_tac(f) = sum(pred_caps);

      total_pred_caps += pred_caps;

      imatrix& gridmap = param.get_gridmap();
      ivector& jlb = param.get_jlb();
      ivector& jub = param.get_jub();

      for (int i=1; i<=_m; i++)
      {
        recapture_record.i = i;
        int j1 = jlb(i);
        int j2 = jub(i);
        for (int j=j1; j<=j2; j++)
        {
          //float cx = (float(i) + .5 ) * scale;
         //float cy = (float(j) + .5 ) * scale;
         recapture_record.j = j;
         int grid = gridmap[i][j];

         strcpy(recapture_record.fleet, param.fleet_names(f));
         dvariable recaps = pred_caps[i][j];
         if (!(!region_caps))
           region_caps[grid] += recaps;
         tcaps += recaps;

         int icaps = 0;

         if (recaps < 1e-10)
           recaps = 0.0;

//double randpoisson(double lambda, const random_number_generator & rng);


         if (param.m_ipar[40])
           //icaps = (int)poidev(value(recaps), &seed);
           icaps = randpoisson(value(recaps),rng);
         else
         {
           icaps = int(value(recaps)+0.5);
         }

         if (icaps >= 1)
         {
           recapture_record.returns = icaps;
           recapture_record.write_on(capfile, param);
           capfile << endl;
           //capfile << recapture_record << endl;
           if (!capfile)
           {
             cerr << "Error writing recapture file." << endl;
             exit(1);
           }
         }

        } // effort_occured
      } // j loop
    } // i loop
  } // fleet loop
  param.m_ipar[40] = seed;
} // end of recapture
