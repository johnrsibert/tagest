//$Id: parproc3.cpp 2848 2011-08-25 17:54:33Z jsibert $
/** \file parproc3.cpp
Functions to build initial .par file
*/
#include <stdio.h>
#include <fvar.hpp>
#include <adstring.hpp>

#include "preproc3.h"
#include "shift.h"
#include "yr_mon2.h"
#include "trace.h"
#include "record3.h"

//extern ofstream clogf;

// maximum releases to be inserted to p
//#define MAX_PAR 2000
/*
#ifndef LINE_MAX
#error "LINE_MAX not defined"
#endif
*/

/**
\def  LINE_MAX
Sets the maximum number of bytes in an input record
*/
#define LINE_MAX 250    //Longest possible line

void par_eofile(List<Shift>& s_pos, fstream& psf);
void wrtag(const adstring& rootPath,
           const adstring& tag_data_filename,
           par_t<d3_array,dmatrix,dvector,double>& param,
           recaptype_vector& recaps,
           const int maxr);
//void rec_sort(recaptype_vector& v, const int n);

adstring& adstring_utils_toupper(adstring& ads);

/**
Creates initial .par file.
Does a bunch of range chacksi, adds the fleet list, list of tag releases,
and writes a recapture file.

\param rootPath adstring containing path to data directory.
\param rootFilename adstring containing the file "root" name.
\param tagDataFilename adstring containing the file name of the tag data file.
\param fleetFilename adstrubg containing the name of the file containing a 
list of fleet names.
\param param reference to an instance of type par_t.
*/
void createParFile(const adstring& rootPath,
                   const adstring& rootFilename,
                   const adstring& tagDataFilename,
                   const adstring& fleetFilename,
                   par_t<d3_array,dmatrix,dvector,double>& param)
{
  // Build an array of fleets ///////////////////////////
  adstring fleet_names = rootPath + adstring(fleetFilename) + ".dat";
  ifstream fleets(fleet_names);
  if (!fleets)
  {
    cerr << "Cannot open " << fleet_names << " in par_process2\n";
    clogf << "Cannot open " << fleet_names << " in par_process2 " << endl;
    exit(1);
  }

  // Add fleets to param
  adstring fleets_tmp;
  int nfleet = param.get_nfleet();
  for (int j = 1; j <= nfleet; j++)
  {
    fleets >> fleets_tmp;
    if (!fleets)
    {
      cerr << "Error reading fleet code " << j << " from file "
           << fleet_names << " in par_process2\n";
      clogf << "Error reading fleet code " << j << " from file "
           << fleet_names << " in par_process2" << endl;
      exit(1);
    }
    param.fleet_names[j] = fleets_tmp(1,4);
    adstring_utils_toupper(param.fleet_names[j]);
  }
  fleets.close();
  clogf << "Fleets: " << param.fleet_names << endl;

  /// Build link list of shifted positions ///////////////
  // Add corrected postions from *.pos if exist to s_pos
  List<Shift> s_pos;   //Link list of shifted positions
  adstring pos_file_name = adstring(rootFilename) + ".pos";
  fstream psf;
  psf.open((char*)pos_file_name, ios::in);
  if (!psf.bad())
  {
    psf >> s_pos;
  }
  psf.close();

  // Process release and recapture data /////////////////
#ifndef unix
  adstring tdname = rootPath + "..\\" + adstring(tagDataFilename) + adstring(".dat");
#else
  adstring tdname = rootPath + "../" + adstring(tagDataFilename) + ".dat";
#endif

  //Input_file in_file(tdname);
  cifstream in_file(tdname);
  in_file.set_ignore_eof();
  if (in_file.bad())
  //if (!in_file)
  {
    cerr << "Error: Could not open file " << tdname << " for reading." << endl;
    clog << "Error: Could not open file " << tdname << " for reading." << endl;
    return;
  }
  else
  {
    cout  << "Opening " << tdname << " for reading." << endl;
    clogf << "Opening " << tdname << " for reading." << endl;
  }
  adstring tag_record(LINE_MAX);

#ifdef DOS386
  const int MAXRETURNS = 50000;     // increase for more tag returns
#else
  const int MAXRETURNS = 2500;     // less than 1 segment
#endif

  const int nrec = MAXRETURNS;// used as check in readrecaps
  recaptype_vector recaps(1, MAXRETURNS);

  int valid_rel = 0,
      nrelease = 0,
      k1 = 0,
      k2 = 0;
  tagreleasetype_ release_record;
  Record record;
  RECORD_TYPE cur_record;
  class Process **cur_process;

  while (1)
  {
    cur_record = record.next_record(in_file, tag_record);
    for (cur_process = process_list; *cur_process != NULL; ++cur_process)
    {
      (*cur_process)->take_record(cur_record);
    }

    int ncaps = 0;
    switch (cur_record)
    {
      case F_RECAPTURE:
        if (valid_rel)
        {
          for  (cur_process = process_list;
               *cur_process != NULL;
              ++cur_process)
          {
            (*cur_process)->rec_par_work3(tag_record, param, release_record,
                                          s_pos, recaps);
            ncaps = ((Release_manager*)(*cur_process))->get_current(); 
          }
        }
        //in_file.flush_line();
        break;
      case F_RELEASE:
        nrelease++;
        // Catches error though it might be better to reallocate
        // the block.
        if (nrelease > param.get_nrelease() - 2)
        {
          clog << "Error: .prn file nrelease is set too small.\n"
                  "       readjust to be greater than " << nrelease
               << endl;
          cerr << "Error: .prn file nrelease is set too small.\n"
                  "       readjust to be greater than " << nrelease
               << "-->Process will not be continued." << endl;
          return;
        }

        for (cur_process = process_list; *cur_process != NULL; ++cur_process)
        {
          (*cur_process)->rel_par_work3(tag_record, param, release_record,
                                        s_pos, valid_rel);
        }
        //in_file.flush_line();
        break;
      case F_EOF:
        param.set_nrelease(nrelease);

        // Update new corrected positions
        psf.open((char*)pos_file_name, ios::out);
        if (psf.bad())
        {
          cerr << "\nError: could not open file " << pos_file_name
               << " for output!" << endl;
          clog << "\nError: could not open file " << pos_file_name
               << " for output!" << endl;
        }
        else
        {
          par_eofile(s_pos, psf);
          psf.close();
          cout << "Pos file " << pos_file_name << " written." << endl;
        }

        recaptype_sort(recaps, MAXRETURNS);

        k1=1;
        while ( (k1 < MAXRETURNS) && (recaps[k1].cohort <= 0) )
        { 
          recaps[k1].write_on(clogf,param); clogf << "\n"; 
          k1++;
        }

        k2 = k1+1;
        while (k2 < MAXRETURNS)
        {
          while (recaps[k1] == recaps[k2])
          {
            recaps[k1] += recaps[k2];
            k2 ++;
          }
          k1 ++;
          recaps[k1] = recaps[k2];
          k2++;
          recaps[k1].write_on(clogf,param); clogf << "\n";
        }

        // Write tag file
        recaps.set_nrec(k1);
        wrtag(rootPath, tagDataFilename, param, recaps, k1);//nn);
        return;

      default:
        break;
    }//End Switch
  }//Outer while
}//createParFile


