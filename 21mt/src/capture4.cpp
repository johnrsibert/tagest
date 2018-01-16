#include "par_t.h"
#include "recap_t.h"
#include "precomp2.h"
#include "trace.h"

#if defined __BCPLUSPLUS__ || __NDPX__
  #include <iomanip>
#endif
#ifdef __ZTC__
  #include <iomanippp>
#endif

extern ofstream clogf;

float poidev(float xm, int* idum);
//int a_move_corner(const int i, const int edge);
//void pred_recapture_comp(par_t& param, dmatrix& pred_recaptures,
//                          dmatrix& pred_tags, dmatrix& z, d3_array& fmort, int fleet);

void capcomp(year_month& date, int cc, par_t& param, dmatrix& tags,
             ofstream& capfile, double& tcaps, dvector& region_caps,
             dmatrix& total_mort, d3_array& fish_mort, ivector& effort_occured,
             dvector& pred_tac, float scale, int v, dmatrix& total_pred_caps)
{
  int seed = param.m_ipar[40];
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
  int _n = param.get_n();
  //float rx = (float(param.get_tr_i(cc) - .5 )) * scale;
  //float ry = (float(_n-param.get_tr_j(cc) + 1) - .5 ) * scale;
  dmatrix pred_caps(1,_m, param.get_jlb(), param.get_jub());

  total_pred_caps.initialize();
  //clogf << date << " tag density field " << tags << endl;

  int nf = param.get_nfleet();
  for (int f = 1; f <= nf; f++)
  {
    if (effort_occured(f))
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
         double recaps = pred_caps[i][j];
         if (!(!region_caps))
           region_caps[grid] += recaps;
         tcaps += recaps;

          #if defined(__ATLANTIC__)
           double icaps = 0.0;
         #else
           int icaps = 0;
         #endif

         if (recaps < 1e-10)
          recaps = 0.0;

         if (param.m_ipar[40])
          icaps = (int)poidev(recaps, &seed);
         else
         {
           icaps = int(recaps+0.5);
           #if defined(__ATLANTIC__)
             icaps = recaps;
           #endif
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
