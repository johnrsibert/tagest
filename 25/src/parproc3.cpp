//$Id: parproc3.cpp 3205 2014-08-19 00:50:00Z eunjung $
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
int valid_rec = 0; //number of valid recapture from the two filtering (recap_ck3,cpp recpmang3.cpp)
int all_recaps = 0; // number of all recaptures from raw data
//dvector all_recaps1; // number of all recaptures from raw data

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
#define LINE_MAX 250		//Longest possible line
extern int invalid1_recaps;     //invalid tag recapture from recpmang3.cpp
extern int invalid2_recaps;     //invalid tag recapture from recpmang3.cpp
extern int invalid3_recaps;     //invalid tag recapture from recpmang3.cpp
extern indexed_regional_fishery_record global_ifr; //object ifr from MakePar.cpp

void par_eofile(List < Shift > &s_pos, fstream & psf);
void wrtag(const adstring & rootPath,
	   const adstring & tag_data_filename,
	   par_t < d3_array, dmatrix, dvector, double >&param,
	   recaptype_vector & recaps, const int maxr);
//void rec_sort(recaptype_vector& v, const int n);


adstring & adstring_utils_toupper(adstring & ads);

/**
Creates initial .par file.
Does a bunch of range chacksi, adds the fleet list, list of tag releases,
and writes a recapture file.

\param rootPath adstring containing path to data directory.
\param rootFilename adstring containing the file "root" name.
\param tagDataFilename adstring containing the file name of the tag data file.
\param fleetFilename adstring containing the name of the file containing a 
list of fleet names.
\param param reference to an instance of type par_t.
*/
void createParFile(const adstring & rootPath,
		   const adstring & rootFilename,
		   const adstring & tagDataFilename,
		   const adstring & fleetFilename,
		   par_t < d3_array, dmatrix, dvector, double >&param)


{
   //global_ifr.read(rootFilename,"./");
   // Build an array of fleets ///////////////////////////
   adstring fleet_names = rootPath + adstring(fleetFilename) + ".dat";
   ifstream fleets(fleet_names);
   if (!fleets)
   {
      cerr << "Cannot open " << fleet_names << " in par_process2\n";
      clogf << "Cannot open " << fleet_names << " in par_process2 " <<
	 endl;
      exit(1);
   }
   // Add fleets to param/
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
      param.fleet_names[j] = fleets_tmp(1, 4);
      adstring_utils_toupper(param.fleet_names[j]);
   }
   fleets.close();
   clogf << "Fleets: " << param.fleet_names << endl;

   /// Build link list of shifted positions ///////////////
   // Add corrected postions from *.pos if exist to s_pos
   List < Shift > s_pos;	//Link list of shifted positions
   adstring pos_file_name = adstring(rootFilename) + ".pos";
   fstream psf;
   psf.open((char *) pos_file_name, ios::in);
   if (!psf.bad())
   {
      psf >> s_pos;
   }
   psf.close();

   // Process release and recapture data /////////////////
#ifndef unix
   adstring tdname =
      rootPath + "..\\" + adstring(tagDataFilename) + adstring(".dat");
#else
   adstring tdname = rootPath + "../" + adstring(tagDataFilename) + ".dat";
#endif

   //Input_file in_file(tdname);
   cifstream in_file(tdname);
   in_file.set_ignore_eof();
   //if (in_file.bad())
   if (!in_file)
   {
      cerr << "Error: Could not open file " << tdname << " for reading." <<
	 endl;
      clog << "Error: Could not open file " << tdname << " for reading." <<
	 endl;
      ad_exit(1);
   } else
   {
      cout << "Opening " << tdname << " for reading." << endl;
      clogf << "Opening " << tdname << " for reading." << endl;
   }
   adstring tag_record(LINE_MAX);

   const int MAXRETURNS = 50000;	// increase for more tag returns

   const int nrec = MAXRETURNS;	// used as check in readrecaps
   recaptype_vector recaps(1, MAXRETURNS);
   
   int valid_rel = 0;
   int nrelease = 0;
   int k1 = 0;
   int k2 = 0;
   int ncaps = 0;
   int n_valid_rel = 0;
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

      switch (cur_record)
      {
      case F_RECAPTURE:
      all_recaps++;
	 if (valid_rel)
	 {
	    for (cur_process = process_list;
		 *cur_process != NULL; ++cur_process)
	    {
               
	       int state = (*cur_process)->rec_par_work3(tag_record, param,
					     release_record, s_pos,
					     recaps);
	       ncaps = ((Release_manager *) (*cur_process))->get_current();
	    }
            valid_rec = (all_recaps) - (invalid1_recaps+invalid2_recaps+invalid3_recaps); //the number of valid recapture from the filtering.
            TTRACE(valid_rec,all_recaps)
	 }
	 //in_file.flush_line();
	 break;
       case F_RELEASE:
         if (nrelease >0)
         {
            TTRACE(valid_rec,all_recaps)
            TTRACE(invalid1_recaps,invalid2_recaps)
            TRACE(invalid3_recaps)
         }
	 nrelease++;
	 // Catches error though it might be better to reallocate
	 // the block.
	 if (nrelease > param.get_nrelease() - 2)
	 {
	    clog << "Error: .prn file nrelease is set too small.\n"
	       "       readjust to be greater than " << nrelease << endl;
	    cerr << "Error: .prn file nrelease is set too small.\n"
	       "       readjust to be greater than " << nrelease
	       << "-->Process will not be continued." << endl;
	    return;
	 }
         if (valid_rel == 1)
         {
         TRACE(valid_rel)
            //n_valid_rel++;
            n_valid_rel=nrelease-1;
            if (n_valid_rel > 0)
            {
              if (all_recaps > 0)
              {
                 TRACE(nrelease)
                 TRACE(n_valid_rel)
                 //TRACE(param.tr[n_valid_rel].tags)
                 //for pttp-skj: 1 degree accuracy 58.7%; 0.5 degree accuracy 43.5%
                 param.tr[n_valid_rel].tags=param.tr[n_valid_rel].tags*0.587;
                 TRACE(param.tr[n_valid_rel].tags)
                 param.tr[n_valid_rel].tags=(param.tr[n_valid_rel].tags*valid_rec)/all_recaps; // new number of release R'(new release)=[R(total release)*r'(valid_rec)]/r(all_recaps)
                 //TRACE(param.tr[n_valid_rel].tags)
                 all_recaps = 0;     //when it comes to new release reset all_recaps, and invalid1,2
                 invalid1_recaps = 0;
                 invalid2_recaps = 0;
                 invalid3_recaps = 0;
              }
              else
              {
                     
                 param.tr[n_valid_rel].tags = 0;
                 TRACE(param.tr[n_valid_rel].tags)
                 all_recaps = 0;     //reset all_recaps, invalid123
                 invalid1_recaps = 0;
                 invalid2_recaps = 0;
                 invalid3_recaps = 0;
              }
            }
         }
         else   // reset all when it's not valid_rel 
         {
             all_recaps = 0;     
             valid_rec = 0;     
             invalid1_recaps = 0;
             invalid2_recaps = 0;
             invalid3_recaps = 0;
         }
	 for (cur_process = process_list; *cur_process != NULL;
	      ++cur_process)
	 {
      	       (*cur_process)->rel_par_work3(tag_record, param,
					  release_record, s_pos,
	    				  valid_rel);
	 }
	 //in_file.flush_line();
	 break;

      case F_EOF:

	 param.set_nrelease(nrelease);

	 // Update new corrected positionsgr
	 psf.open((char *) pos_file_name, ios::out);
	 if (psf.bad())
	 {
	    cerr << "\nError: could not open file " << pos_file_name
	       << " for output!" << endl;
	    clog << "\nError: could not open file " << pos_file_name
	       << " for output!" << endl;
	 } else
	 {
	    par_eofile(s_pos, psf);
	    psf.close();
	    cout << "Pos file " << pos_file_name << " written." << endl;
	 }

	 tagreleasetype_sort(param.tr, nrelease);
	 // renumber the release cohorts after sorting
	 //for (int k = 1; k <= nrelease; k++)
	 //{
	 //   param.tr[k].cohort = k;
	 //}

	 recaptype_sort(recaps, MAXRETURNS);

	 k1 = 1;
	 while ((k1 < MAXRETURNS) && (recaps[k1].cohort <= 0))
	 {
	    recaps[k1].write_on(clogf, param);
	    clogf << "\n";
	    k1++;
	 }

	 k2 = k1 + 1;
	 while (k2 < MAXRETURNS)
	 {
	    while (k2 < MAXRETURNS && recaps[k1] == recaps[k2])
	    {
	       recaps[k1] += recaps[k2];
	       k2++;
	    }
            k1++;
            recaps[k1] = recaps[k2];
            k2++;
/*
	    recaps[k1] = recaps[k2];
*/
//	    k1 = k2;

	    recaps[k1].write_on(clogf, param);
	    clogf << "\n";
	 }
  
	 // Write tag file
	 cout << "HERE:  " << endl;
	 recaps.set_nrec(k1);
	 wrtag(rootPath, tagDataFilename, param, recaps, k1);	//nn);
	 return;

      default:
	 break;
      }				//End Switch
   }				//Outer while
}				//createParFile
