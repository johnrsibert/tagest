// $Id: MM_XMLDoc.cpp 2947 2012-02-08 01:44:43Z jsibert $
/** \file MM_XMLDoc.cpp for class ADMB_XMLDoc. Based on the XMLDocument2 class by Johnoel Ancheta.
   Copyright&copy; 2012 University of Hawaii, Pelagic Fisheries Research Program
*/
#include "MM_XMLDoc.h"
#include "trace.h"
#include "ipar.h"
#include "par_t.h"

using namespace std;

//MM_XMLDoc::MM_XMLDoc(const adstring& drn, const adstring& prg, 
//                     const adstring& id, const adstring _file):
//                     ADMB_XMLDoc(drn, prg, id, _file) {;}

 
int MM_XMLDoc::createXMLelement(const adstring& name, const ipar_t& t, const adstring& title)
{
   file << "<" << name << " category=\"FLAG\">" << endl;
   file << createTitleTag(title);
   const int i1 = t.indexmin();
   const int i2 = t.indexmax();
   for (int i = i1; i <= i2; i++)
   {
      file << "<flag>\n";
      file << "  <name>" << t.get_name(i) << "</name>";
      file << "  <index>" << i << "</index>";
      file << createValueTag(t(i));
      file << "</flag>\n";
   }
   file << "</" << name << ">" << endl;

   return !(file);
}

int MM_XMLDoc::createXMLelement(const adstring& name, const  trv_ptr& tr, const int nrelease,
                                  const adstring& title)
{
   file << "<releases>\n";
   file << createTitleTag(title);
   for (int i = 1; i <= nrelease; i++)
   {
      file << "  <cohort>\n";
      file << "    <number>" << tr(i).cohort << "</number>";
      file << "<date>" << tr(i).date << "</date>";
      file << "<longitude>" << tr(i).i << "</longitude>";
      file << "<latitude>" << tr(i).j << "</latitude>";
      file << "<tags>" << tr(i).tags << "</tags>\n";
      file << "  </cohort>\n";
   }
   file << "</releases>\n";
   
   
   return !(file);
}
