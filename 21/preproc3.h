#include <iostream.h>
#include <fstream.h>
#include <iomanip.h>
#include <stdlib.h>

#include "par_treg.h"
#include "ch_type.h"
#include "record3.h"
#include "shift.h"
#include "yr_mon2.h"
#include "recap_t.h"

class Process
{
public:
  virtual void take_record(RECORD_TYPE record) = 0;
  virtual ~Process () {};

  virtual void rel_par_work3(adstring& buffer, 
                     par_t& par_param,
                     tagreleasetype_& release_record, 
                     List<Shift>& ls, 
                     int& valid_rel){};
  virtual  void rec_par_work3(adstring& buffer, 
                            par_t& par_param, 
                       tagreleasetype_& release_record, 
//                       year_month_vector& last_recapture_date,
                       List<Shift>& ls, 
                       recaptype_vector& recaps){}; 
  int get_current(void) { return(-1); }
};

class Release_manager : public Process
{
public:
  Release_manager() { cur_release = 0; }  

  void take_record(RECORD_TYPE record)
    { if (record == F_RELEASE) ++cur_release; }

  void rel_par_work3(adstring& buffer, 
                     par_t& par_param,
                     tagreleasetype_& release_record, 
                     List<Shift>& ls, 
                     int& valid_rel);

  int get_current(void){ return(cur_release); }

private:
  int cur_release;
};

class Recapture_manager : public Process
{
public:
    Recapture_manager() { cur_recapture = 0; }  

    void take_record(RECORD_TYPE record)
      { if (record == F_RECAPTURE) ++cur_recapture; }

    void rec_par_work3(adstring& buffer, 
                       par_t& par_param, 
                       tagreleasetype_& release_record, 
//                       year_month_vector& last_recapture_date,
                       List<Shift>& ls, 
                       recaptype_vector& recaps); 
  
  int get_current(void){ return(cur_recapture); }

private:
  int cur_recapture;
};

static Release_manager release_func;
static Recapture_manager recapture_func;

static Process *process_list[] =
{
  &release_func,
  &recapture_func,
  NULL
};
