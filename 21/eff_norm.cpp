#include "precomp.h"
#include "fish_rec.h"

extern ofstream clogf;

// t.set_name(121, "1 => normalize fishing effort to fleet means");
// t.set_name(122, "1 => catchabilities have been adjusted to normaized effort");

void par_t::set_fishing_effort_normalization(indexed_regional_fishery_record& irfr)
{
  if (m_ipar(121) == 1)
  {
     irfr.normalize();
     if (m_ipar(122) == 0)
     {
       int nfleet = get_nfleet();
       for (int f = 1; f <= nfleet; f++)
       {
          q(f) *= irfr.get_mean_effort(f);
       }
       m_ipar(122) = 1;
       cout << "Catchabilities adjusted for normalized effort." << endl;
       clogf << "Catchabilities adjusted for normalized effort." << endl;
     }
  }
  
  else if (m_ipar(121) == 0)
  {
     if (m_ipar(122) == 1)
     {
       int nfleet = get_nfleet();
       for (int f = 1; f <= nfleet; f++)
       {
          q(f) /= irfr.get_mean_effort(f);
       }
       m_ipar(122) = 0;
     }
  }
  
  else
  {
     cerr << "Illegal switch (" << m_ipar(121) 
          << ") passed to set_fishing_effort_normailzation" << endl;
     clogf << "Illegal switch (" << m_ipar(121) 
           << ") passed to set_fishing_effort_normailzation" << endl;
     exit(1);
  }

}
