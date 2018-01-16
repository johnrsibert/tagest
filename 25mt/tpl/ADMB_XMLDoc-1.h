/*
 * $Id: ADMB_XMLDoc-1.h 2931 2012-01-26 01:08:58Z jsibert $
 * Copyright&copy; 2012 University of Hawaii, Pelagic Fisheries Research Program
 */
#ifndef __ADMB_XMLDoc__
#define __ADMB_XMLDoc__

#include <fvar.hpp>
#include <admodel.h>

#include <iostream>
//#include <string>
//#include <stdexcept>
//#include <cstdlib>
#include <sstream>
#include <iomanip>

#include "trace.h"

using namespace std;

class ADMB_XMLDoc
{
private:
   //xmlDoc* doc;
   //xmlXPathContext* context;
   ofstream file;
   adstring filename;
   adstring doc_root_name;

public:
   ADMB_XMLDoc()
   {
      //doc = 0;
      //context = 0;
      //xmlInitParser();
   }

   ADMB_XMLDoc(const adstring & fname, const adstring& drn)
   {
      filename = fname;
      file.open(filename);
      if (!file)
      {
         cerr << "Error: unable to open file " << filename << endl;
         exit(1);
      }
      doc_root_name = drn;
      file << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" << endl;
      file << "<" << doc_root_name << ">" << endl;
    }

  ~ADMB_XMLDoc()
   {
      file << "</" << doc_root_name << ">" << endl;
      file.close();
   } 

   ADMB_XMLDoc(const ADMB_XMLDoc& t)
   {
       cerr << "Error: called undifined  ADMB_XMLDoc copy contsrutor." << endl;
       exit(1);
   }

   int read(const adstring& filename);

   int create_XMLelement(const adstring& name, const double t)
   {
      ostringstream ostr;
      ostr << "<value>" << setprecision(16) << t << "</value>";
      file << "<variable category=\"CONST\">"
           << "<name>" << name << "</name>"
           << ostr.str();
      file << "</variable>" << endl;

      return !(file);
    }

   int create_XMLelement(const param_init_bounded_number& t)
   {
      adstring name(t.get_name());
      ostringstream ostr;
      ostr << "<variable category=\"VAR\">"
           << "<name>" << name << "</name>"
           << "<value>" << setprecision(16) << t << "</value>\n";
      ostr << "  <bounds><min>" << t.get_minb() << "</min><max>" << t.get_maxb() << "</max></bounds>\n";
      ostr << "  <active>" << active(t) << "</active>\n";
      ostr << "  <phase>" << t.phase_start << "</phase>\n";
      ostr << "</variable>";
      file << ostr.str() << endl;
     
      return !(file);
    }

   int create_XMLelement(const adstring name,const param_init_bounded_number_vector& t)
   {
      //adstring name(t.get_name());
      const int i1 = t.indexmin();
      const int i2 = t.indexmax();
      ostringstream ostr;
      ostr << "<variable category=\"VAR\">" << "<name>" << name << "</name>\n";
      ostr << "  <range>" << i1 <<  " " << i2 << "</range>\n";
      ostr << "  <value>";
      for (int i = i1; i <= i2; i++)
        ostr << " " << setprecision(16) << t(i);
      ostr << "</value>\n";
      ostr << "  <bounds>\n";
      ostr << "    <min>";
      for (int i = i1; i <= i2; i++)
         ostr << " " << t(i).get_minb();
      ostr << "</min>\n";
      ostr << "    <max>";
      for (int i = i1; i <= i2; i++)
         ostr << " " << t(i).get_maxb();
      ostr << "</max>\n";
      ostr << "  </bounds>\n";
      ostr << "  <active>";
      for (int i = i1; i <= i2; i++)
        ostr << " " << active(t(i));
      ostr << "</active>\n";
      ostr << "  <phase>";
      for (int i = i1; i <= i2; i++)
         ostr << " " << t(i).phase_start;
      ostr << "</phase>\n";
      ostr << "</variable>";
      file << ostr.str() << endl;

      return !(file);
   }

   int create_XMLelement(const param_init_bounded_vector& t)
   {
      adstring name(t.get_name());
      const int i1 = t.indexmin();
      const int i2 = t.indexmax();
      ostringstream ostr;
      ostr << "<variable category=\"VAR\">" << "<name>" << name << "</name>\n";
      ostr << "  <range>" << i1 <<  " " << i2 << "</range>\n";
      ostr << "  <value>";
      for (int i = i1; i <= i2; i++)
        ostr << " " << setprecision(16) << t(i);
      ostr << "</value>\n";
      ostr << "  <bounds><min>" << t.get_minb() << "</min><max>" << t.get_maxb() << "</max></bounds>\n";
      ostr << "  <active>" << active(t) << "</active>\n";
      ostr << "  <phase>" << t.phase_start << "</phase>\n";
      ostr << "</variable>";
      file << ostr.str() << endl;

      return !(file);
   }

   int create_XMLelement(const param_init_bounded_matrix& t)
   {
      // this code is not correct for ragged arrays
      adstring name(t.get_name());
      const int i1 = t.colmin();
      const int i2 = t.colmax();
      const int j1 = t.rowmin();
      const int j2 = t.rowmax();
      ostringstream ostr;
      ostr << "<variable category=\"VAR\">" << "<name>" << name << "</name>\n";
      ostr << "  <range>" << j1 << " " << j2 << " " 
                          << i1 << " " << i2 << "</range>\n";
      ostr << "  <value>";
      for (int j = j1; j <= j2; j++)
      {
         for (int i = i1; i <= i2; i++)
         {
            ostr << " " << setprecision(16) << t(j,i);
         }
         if (j < j2)
           ostr << "\n";
      }
      ostr << "  </value>\n";
      ostr << "  <bounds><min>" << t.get_minb() << "</min><max>" << t.get_maxb() << "</max></bounds>\n";
      ostr << "  <active>" << active(t) << "</active>\n";
      ostr << "  <phase>" << t.phase_start << "</phase>\n";
      ostr << "</variable>";
      file << ostr.str() << endl;

      return !(file);
   }

};
#endif //#define __ADMB_XMLDoc__
