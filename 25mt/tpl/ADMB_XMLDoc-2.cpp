// $Id: ADMB_XMLDoc-2.cpp 2940 2012-02-04 01:06:20Z jsibert $
/** \file ADMB_XMLDoc.h
Source for class ADMB_XMLDoc. Based on the XMLDocument2 class by Johnoel Ancheta.
   Copyright&copy; 2012 University of Hawaii, Pelagic Fisheries Research Program
*/
#include "ADMB_XMLDoc.h"
#include <stdexcept>
#include "trace.h"

using namespace std;

/** Create new empty xml document.
*/
ADMB_XMLDoc::ADMB_XMLDoc()
{
   doc = 0;
   context = 0;
   xmlInitParser();
}

/** Create new xml document.
\param drn String containing the root name of the xml document tree.
\param prg String containint program name (not currently used).
\param id String containing indentifier for the model and and data values, e. g., 
some sort of fit code.
\param filen Name of the xml file produced.
*/
ADMB_XMLDoc::ADMB_XMLDoc(const adstring& drn, const adstring& prg, const adstring& id, const adstring filen)
{
   XMLFileName = filen;
   TRACE(XMLFileName)
   file.open(XMLFileName);
   if (!file)
   {
      cerr << "Error: unable to open file " << XMLFileName << endl;
      exit(1);
   }
   DocRootName = drn;
   TRACE(DocRootName)
   file << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" << endl;
   file << "<?xml-stylesheet type=\"text/xsl\" href=\"ADMB.xsl\"?>" << endl;

   TRACE(id)
   file << "<" << DocRootName << " id=" << "\"" << id << "\">" << endl;
 }

/** Terminates the document tree and closes the output file.
*/
ADMB_XMLDoc::~ADMB_XMLDoc()
{
   if (file)
   {
      file << "</" << DocRootName << ">" << endl;
      file.close();
   }
} 
 
string ADMB_XMLDoc::createValueTag(const double t)
{
   ostringstream ostr;
   ostr << "  <value>" << setprecision(16) << t << "</value>\n";
   return ostr.str();
}

string ADMB_XMLDoc::createValueTag(const adstring& t)
{
   ostringstream ostr;
   ostr << "  <value>" << t << "</value>\n";
   return ostr.str();
}

string ADMB_XMLDoc::createValueTag(const adstring_array& t)
{
   ostringstream ostr;
   ostr << "  <value>";
   for (int i = t.indexmin(); i <= t.indexmax(); i++)
     ostr << " " << t(i);
   //ostr << t;
   ostr << "</value>\n";
   return ostr.str();
}

string ADMB_XMLDoc::createValueTag(const dvector& t)
{
   ostringstream ostr;
   ostr << "  <value>";
   ostr << t;
   ostr << "</value>\n";
   return ostr.str();
}

string ADMB_XMLDoc::createValueTag(const ivector& t)
{
   ostringstream ostr;
   ostr << "  <value>";
   ostr << t;
   ostr << "</value>\n";
   return ostr.str();
}

string ADMB_XMLDoc::createValueTag(const dmatrix& t)
{
   ostringstream ostr;
   ostr.precision(16);
   ostr << "  <value>";
   ostr << t << endl;
   ostr << "</value>\n";
   return ostr.str();
}

string ADMB_XMLDoc::createValueTag(const imatrix& t)
{
   ostringstream ostr;
   ostr << "  <value>";
   ostr << t << endl;
   ostr << "</value>\n";
   return ostr.str();
}

string ADMB_XMLDoc::createBoundsTag(const double min, const double max)
{
   ostringstream ostr;
   ostr << "  <bounds><min>" << min << "</min><max>" << max << "</max></bounds>\n";
   return ostr.str();
}

string ADMB_XMLDoc::createBoundsTag(const dvector& min, const dvector& max)
{
   ostringstream ostr;
   ostr << "  <bounds><min>";
   for (int i = min.indexmin(); i <= min.indexmax(); i++)
     ostr << " " << min(i);
   ostr << "</min><max>";
   for (int i = max.indexmin(); i <= max.indexmax(); i++)
     ostr << " " << max(i);
   ostr << "</max></bounds>\n";
   return ostr.str();
}

string ADMB_XMLDoc::createPhaseTag(const int t)
{
   ostringstream ostr;
   ostr << "  <phase>" << t<< "</phase>\n";
   return ostr.str();
}

string ADMB_XMLDoc::createPhaseTag(const ivector& t)
{
   ostringstream ostr;
   ostr << "  <phase>";
   ostr << t;
   ostr << "</phase>\n";
   return ostr.str();
}

string ADMB_XMLDoc::createActiveTag(const int t)
{
   ostringstream ostr;
   ostr << "  <active>" << t<< "</active>\n";
   return ostr.str();
}

string ADMB_XMLDoc::createActiveTag(const ivector& t)
{
   ostringstream ostr;
   ostr << "  <active>" << t << "</active>\n";
   return ostr.str();
}

string ADMB_XMLDoc::createRangeTag(const int lb, const int ub)
{
   ostringstream ostr;
   ostr << "  <range>" << lb << " " << ub << "</range>\n";
   return ostr.str();
}

string ADMB_XMLDoc::createRangeTag(const int rlb, const int rub,
                                   const int clb, const int cub)
{
   ostringstream ostr;
   ostr << "  <range>" << rlb << " " << rub << " " << clb << " " << cub << "</range>\n";
   return ostr.str();
}

string ADMB_XMLDoc::createTitleTag(const adstring& t)
{
   ostringstream ostr;
   ostr << "  <title>" << t << "</title>\n";
   return ostr.str();
}

string ADMB_XMLDoc::createCommentTag(const adstring& t)
{
   ostringstream ostr;
   ostr << "\n<!-- " << t << " -->\n";
   return ostr.str();
}

int ADMB_XMLDoc::createXMLelement(const adstring& name, const ipar_t& t, const adstring& title)
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

int ADMB_XMLDoc::createXMLelement(const adstring& name, const double t, const adstring& title)
{
   file << "<" << name << " category=\"CON\">" << endl;
   file << createTitleTag(title);
   file << createValueTag(t);
   file << "</" << name << ">" << endl;

   return !(file);
 }

int ADMB_XMLDoc::createXMLelement(const adstring& name, const int t, const adstring& title)
{
   file << "<" << name << " category=\"CON\">" << endl;
   file << createTitleTag(title);
   file << createValueTag(t);
   file << "</" << name << ">" << endl;

   return !(file);
}

int ADMB_XMLDoc::createXMLelement(const adstring& name, const adstring& t, const adstring& title)
{
   file << "<" << name << " category=\"CON\">" << endl;
   file << createTitleTag(title);
   file << createValueTag(t);
   file << "</" << name << ">" << endl;

   return !(file);
}

int ADMB_XMLDoc::createXMLelement(const adstring& name, const adstring_array& t, const adstring& title)
{
   file << "<" << name << " category=\"CON\">" << endl;
   file << createTitleTag(title);
   file << createValueTag(t);
   file << "</" << name << ">" << endl;

   return !(file);
}

int ADMB_XMLDoc::createXMLelement(const param_init_bounded_number& t, const adstring& title)
{
   adstring name(t.get_name());
   file << "<" << name << " category=\"VAR\">" << endl;
   file << createTitleTag(title);
   file << createValueTag(value(t));
   file << createBoundsTag(t.get_minb(),t.get_maxb());
   file << createActiveTag(active(t));
   file << createPhaseTag(t.phase_start);
   file << "</" << name << ">" << endl;
   return !(file);

 }

int ADMB_XMLDoc::createXMLelement(const adstring& name,const param_init_bounded_number_vector& t, const adstring& title)
{
   const int i1 = t.indexmin();
   const int i2 = t.indexmax();
   dvector minb(i1,i2);
   dvector maxb(i1,i2);
   ivector phase(i1,i2);
   ivector activef(i1,i2);
   dvector vt(i1,i2);
   for (int i = i1; i <= i2; i++)
   {
      minb(i) = t(i).get_minb();
      maxb(i) = t(i).get_maxb();
      phase(i) = t(i).phase_start;
      activef(i) = active(t(i));
      vt(i) = value(t(i));
   } 
   file << "<" << name << " category=\"VAR\">\n";
   file << createTitleTag(title);
   file << createValueTag(vt);
   file << createRangeTag(i1,i2);
   file << createBoundsTag(minb,maxb);
   file << createPhaseTag(phase);
   file << createActiveTag(activef);
   file << "</" << name << ">" << endl;

   return !(file);
}

int ADMB_XMLDoc::createXMLelement(const adstring& name,const ivector& t, const adstring& title)
{
   file << "<" << name << " category=\"CON\">\n";
   file << createTitleTag(title);
   file << createValueTag(t);
   file << createRangeTag(t.indexmin(),t.indexmax());
   file << "</" << name << ">" << endl;

   return !(file);
}

int ADMB_XMLDoc::createXMLelement(const param_init_bounded_vector& t, const adstring& title)
{
   adstring name(t.get_name());
   const int i1 = t.indexmin();
   const int i2 = t.indexmax();
   file << "<" << name << " category=\"VAR\">\n";
   file << createTitleTag(title);
   file << createValueTag(value(t));
   file << createRangeTag(i1,i2);
   file << createBoundsTag(t.get_minb(),t.get_maxb());
   file << createActiveTag(active(t));
   file << createPhaseTag(t.phase_start);
   file << "</" << name << ">" << endl;

   return !(file);
}

int ADMB_XMLDoc::createXMLelement(const param_init_bounded_matrix& t, const adstring& title)
{
   // this code is not correct for ragged arrays
   adstring name(t.get_name());
   const int i1 = t.colmin();
   const int i2 = t.colmax();
   const int j1 = t.rowmin();
   const int j2 = t.rowmax();
   file << "<" << name << " category=\"VAR\">\n";
   file << createTitleTag(title);
   file << createValueTag(value(t));
   file << createRangeTag(j1,j2,i1,i2);
   file << createBoundsTag(t.get_minb(),t.get_maxb());
   file << createActiveTag(active(t));
   file << createPhaseTag(t.phase_start);
   file << "</" << name << ">\n";

   return !(file);
}

int ADMB_XMLDoc::createXMLelement(const adstring& name, const imatrix& t, const adstring& title)
{
   // this code is not correct for ragged arrays
   const int i1 = t.colmin();
   const int i2 = t.colmax();
   const int j1 = t.rowmin();
   const int j2 = t.rowmax();
   file << "<" << name << " category=\"CON\">\n";
   file << createTitleTag(title);
   file << createValueTag(t);
   file << createRangeTag(j1,j2,i1,i2);
   file << "</" << name << ">\n";

   return !(file);
}

int ADMB_XMLDoc::createXMLcomment(const adstring& t)
{
   file << createCommentTag(t);

   return !(file);
}

int ADMB_XMLDoc::createXMLelement(const adstring& name, const  trv_ptr& tr, const int nrelease,
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

/**
   \param name Variable name of devarialbe to fetch
   \return Value of name in xml node 
*/
double ADMB_XMLDoc::getDouble(const string& name)
{
   double d = 0.0;
   string s = getContentString(name,"value");
   if (!s.empty()) {
      d = strtod(s.c_str(),0);
   }
   else {
      throw runtime_error("Error: XPath=\"" + name + "\" does not exist.\n");
   }

   return d;
}

dvector ADMB_XMLDoc::getDvector(const string& name)
{
   string s = getContentString(name,"range");
   istringstream istr(s);
   int lb, ub;
   istr >> lb >> ub;
   dvector d = getDvector(name, lb, ub);

   return d;
}

dvector ADMB_XMLDoc::getDvector(const string& name, const int lb, const int ub)
{
   string s = getContentString(name,"value");
   istringstream istr(s);
   dvector d(lb,ub);
   /*
   for (int i = lb; i <= ub; i++)
   {
       istr >> d(i);
   }
   */
   istr >> d;
   return d;
}

dmatrix ADMB_XMLDoc::getDmatrix(const string& name)
{
   string s = getContentString(name,"range");
   istringstream istr(s);
   int rowmin, rowmax, colmin, colmax;
   istr >> rowmin >> rowmax >> colmin >> colmax;
   dmatrix m = getDmatrix(name, rowmin, rowmax, colmin, colmax);

   return m;
}

dmatrix ADMB_XMLDoc::getDmatrix(const string& name, const int rowmin, const int rowmax,
                                const int colmin, const int colmax)
{
   string s = getContentString(name,"value");
   istringstream istr(s);
   dmatrix m(rowmin, rowmax, colmin, colmax);
   istr >> m;
   return m;
}



/**
 * @param xpath absolute element xpath
 */
string ADMB_XMLDoc::getContentString(const string& name, const string& tag) const 
{
   string value;
   //string xpathfull = "/FIT/" + name + "/value";
   string xpathfull = "/FIT/" + name + "/" + tag;
   xmlXPathObject* xpathObj = xmlXPathEvalExpression((xmlChar*)xpathfull.c_str(), context);
   if (xpathObj == 0) {
      return value;
   }

   if (xpathObj->type == XPATH_NODESET) {
      xmlNodeSet* xmlnodeset = xpathObj->nodesetval;
      const int max = xmlnodeset->nodeNr;
      for (int i = 0; i < max; i++) {
         xmlNode* xmlnode = xmlnodeset->nodeTab[i];
         if (xmlnode != 0 && xmlnode->children)
         {
               value += (char*)xmlnode->children->content;
         }
      }
   }

   return value;
}

/**
 * @param xpath absolute element xpath
 * @param value set xpath with this value
 */
bool ADMB_XMLDoc::setContent(const string& name, const string& value) 
{
   string xpathfull = "/FIT/" + name + "/value";
   xmlXPathObject* xpathObj = xmlXPathEvalExpression((xmlChar*)xpathfull.c_str(), context);
   if (xpathObj == 0) {
      return false;
   }

   if (xpathObj->type == XPATH_NODESET) 
   {
      xmlNodeSet* xmlnodeset = xpathObj->nodesetval;
      const int max = xmlnodeset->nodeNr;
      for (int i = 0; i < max; i++) 
      {
         xmlNode* xmlnode = xmlnodeset->nodeTab[i];
         if (xmlnode != 0 && xmlnode->children)
         {
           xmlnode->children->content = xmlCharStrdup(value.c_str());  
         }
       }
   }

   xmlXPathFreeObject(xpathObj);

   return true;

}

/**
 * @param name absolute element xpath
 * @param value double contining the value to set name
 */
bool ADMB_XMLDoc::set(const string& name, const double value) 
{
   ostringstream ostr;
   ostr << setprecision(16) << value;
   return setContent(name, ostr.str());
}

/**
 * @param name absolute element xpath
 * @param value dvector contining the value to set name
 */
bool ADMB_XMLDoc::set(const string& name, const dvector& value) 
{
   ostringstream ostr;
   ostr << setprecision(16) << value;
   return setContent(name, ostr.str());
}


/**
 * @param name absolute element xpath
 * @param value dmatrix contining the value to set name
 */
bool ADMB_XMLDoc::set(const string& name, const dmatrix& value) 
{
   ostringstream ostr;
   ostr << setprecision(16) << value;
   return setContent(name, ostr.str());
}


/**
 * import a parfile into document
 * @param parfile XMLFileName
 */
int ADMB_XMLDoc::read(const char* parfile)
{
   if (parfile == 0) {
      return -1;
   }

   doc = xmlParseFile(parfile);
   if (doc == NULL) {
      throw runtime_error("Error: \n");
   }
   XMLFileName = adstring((char*)(doc->URL));
   DocRootName = adstring((char*)(doc->last->name));
   TTRACE(XMLFileName,DocRootName)

   context = xmlXPathNewContext(doc);
   if (context == 0) {
      throw runtime_error("Error: \n");
   }

   return 1;
}

/**
 * Write the contents of doc to a file.
 * @param parfile character string containing filename to write the xml tree.
 */
int ADMB_XMLDoc::write(const char* parfile) 
{
   const int nbyte = xmlSaveFile(parfile, doc);
   return  nbyte;
}


