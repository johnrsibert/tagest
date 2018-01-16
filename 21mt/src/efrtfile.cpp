//#include "precomp2.h"
//#include <fvar.hpp>
#include "fish_rec.h"
#include <fstream>
#include "par_t.h"
#include "trace.h"

adstring effort_name(adstring& fleet, year_month& date, char* _directory=NULL);

int read_effort(par_t& param, d3_array& effort, year_month& date,
                ivector& effort_occured)
{
  int return_value = 0;

  int nfleet = param.get_nfleet();

  for (int fl = 1; fl <= nfleet; fl++)
  {
    /*
    adstring directory = "..\\";
    #if defined __GNUC__ || defined __SUN__
      directory = "../";
    #endif
    */
    
    adstring filename = effort_name(param.fleet_names(fl), date, 0);
    {
      //cifstream eff((char*)filename);
      effort_stream eff((char*)filename);
      if (!eff)
      {
        //cout << "No effort " << filename << " ";
        effort(fl).initialize();
        effort_occured(fl) = 0;
        return_value++;
        eff.clear();
      }
      else
      {
        //cout << "Effort " << filename << " ";
        eff >> effort(fl);
        if (!eff)
        {
          cerr << "Error reading effort " << filename << endl;
          exit(1);
        }
        effort_occured(fl) = 1;
      }//else !eff
      eff.close();
      //cout << "eff: " << date << " " << fl << " " << sum(effort(fl)) << endl;
    }
  }//for fl
  return return_value;
}

adstring effort_name(adstring& fleet, year_month& date, char* _directory)
{
/*
  adstring directory;
  if (_directory)
    directory = adstring(_directory);
  else
  {
    directory = "..\\";
    #if defined __GNUC__ || __SUN__
      directory = "../";
    #endif
  }
*/

  int month = (int)date.get_month().get_value();
  char month_str[3];
  sprintf(month_str,"%02d",month);
  int year = date.get_year();
  adstring file_name;
  if (year < 100)
  {
    char year_str[3];
    sprintf(year_str,"%02d",date.get_year());
    file_name = fleet + year_str + month_str;
  }
  else
  {
    char year_str[5];
    sprintf(year_str,"%4d",date.get_year());
    file_name = fleet + year_str + month_str;
  }

  //adstring file_name = directory+fleet+year_str+month_str+".eff";

#if defined __SUN__ || __GNUC__
  //file_name.to_lower();
  adstring_utils_tolower(file_name);
#endif

  return file_name;
}
