// $Id: ADMB_XMLDoc.cpp 2956 2012-03-11 01:34:21Z jsibert $
/** \file ADMB_XMLDoc.cpp for class ADMB_XMLDoc. Based on the XMLDocument2 class by Johnoel Ancheta.
   Copyright&copy; 2012 University of Hawaii, Pelagic Fisheries Research Program
*/
#include "ADMB_XMLDoc.h"
#include <stdexcept>
#include "trace.h"
#include "ipar.h"
#include "par_t.h"

using namespace std;
#undef __USE_OLD_VERSION__
#ifndef __USE_OLD_VERSION__
const char* ADMB_XMLDoc::dataS = (const char*)"CON";
const char* ADMB_XMLDoc::paramS = (const char*)"VAR";
const char* ADMB_XMLDoc::statS = (const char*)"STAT";

/** Create new empty xml document.
*/
ADMB_XMLDoc::ADMB_XMLDoc()
{
   doc = NULL;
   context = NULL;
   RootNode = NULL;
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
   doc = xmlNewDoc(BAD_CAST "1.0");
   //file << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" << endl;
   XMLFileName = (const char*)filen;
   TRACE(XMLFileName)
  
   DocRootName = (const char*)drn;
   TRACE(DocRootName)
   RootNode = xmlNewNode(NULL, BAD_CAST DocRootName.c_str());
   //file << "<?xml-stylesheet type=\"text/xsl\" href=\"ADMB.xsl\"?>" << endl;
   //xmlNewProp(ss, BAD_CAST "type",BAD_CAST "text/xsl");
   //xmlNewProp(ss, BAD_CAST "href",BAD_CAST "ADMB.xsl");
   xmlNodePtr ss = xmlNewPI(BAD_CAST "xml-stylesheet", 
                BAD_CAST "type=\"text/xsl\" href=\"ADMB.xsl\"");
  

   TRACE(id)
   xmlNewProp(RootNode, BAD_CAST "id", BAD_CAST (const char*)id);
   xmlDocSetRootElement(doc, RootNode);
   xmlAddPrevSibling(RootNode,ss);
   
   xmlNodePtr CNode = xmlNewComment (BAD_CAST "a simple comment in the previos node");
   xmlAddPrevSibling(RootNode,CNode);


   /*
   if (1)
   {
   xmlSaveFormatFileEnc(BAD_CAST XMLFileName.c_str(),doc,"UTF-8", 1);

    exit(1);
    }
   */
 }

/** Terminates the document tree
*/
ADMB_XMLDoc::~ADMB_XMLDoc()
{
    //Free the global variables that may
    //have been allocated by the parser.
    xmlCleanupParser();
} 
 
/* * Write the contents of doc to a file.
 * @param parfile character string containing filename to write the xml tree.
 */
int ADMB_XMLDoc::write(void)
{
   int nbyte = 0;
   nbyte = xmlSaveFormatFileEnc(BAD_CAST XMLFileName.c_str(),doc,
                     BAD_CAST "UTF-8", 1);
   return  nbyte;
}

int ADMB_XMLDoc::createXMLcomment(const adstring& t)
{
   xmlNodePtr node = xmlNewComment (BAD_CAST (const char*)t); 
   //xmlAddPrevSibling(RootNode,node);
   xmlAddChild(RootNode,node);
   return ((node == NULL));
}

int ADMB_XMLDoc::createXMLelement(const adstring& name, const double t, const adstring& title)
{
   // create name node for variable 
   xmlNodePtr node = createNameNode(name, ADMB_XMLDoc::dataS);
   
   // create title node and add to name node
   createTitleNode(node,title);

   // create value node and add to name node
   createValueNode(node,t);

   // add name node to root
   xmlNodePtr nnode = xmlAddChild(RootNode,node);
   return ((nnode == NULL));
 }

int ADMB_XMLDoc::createXMLelement(const param_init_bounded_number& t, const adstring& title)
{
   adstring name(t.get_name());
   TTRACE(name,title)
   xmlNodePtr node = createNameNode(name, ADMB_XMLDoc::paramS);
   createTitleNode(node,title);
   
   createValueNode(node,value(t));

   //file << createBoundsTag(t.get_minb(),t.get_maxb());
   createBoundsNode(node,t.get_minb(),t.get_maxb());

   //file << createActiveTag(active(t));
   createActiveNode(node, active(t));

   //file << createPhaseTag(t.phase_start);
   createPhaseNode(node,t.phase_start);

   xmlNodePtr nnode = xmlAddChild(RootNode,node);
   return ((nnode == NULL));
 }

xmlNodePtr ADMB_XMLDoc::createNameNode(const adstring& name, const char* category)
{
   xmlNodePtr node = xmlNewNode(NULL, BAD_CAST (const char*)name);
   if (category)
     xmlNewProp(node, BAD_CAST "category", BAD_CAST category);
   return node;
}


int ADMB_XMLDoc::createTitleNode(xmlNodePtr parent, const adstring& title)
{
   xmlNodePtr tNode = xmlNewNode(NULL, BAD_CAST "title");
   xmlNodeAddContent(tNode,BAD_CAST (const char*)title);
   xmlAddChild(parent,tNode);

   return ((tNode==NULL));
}

int ADMB_XMLDoc::createValueNode(xmlNodePtr parent, const adstring& t)
{
   xmlNodePtr tNode = xmlNewChild(parent,NULL, BAD_CAST "value", 
                                  BAD_CAST (const char*)t);
   return ((tNode==NULL));
}

int ADMB_XMLDoc::createValueNode(xmlNodePtr parent, const double t)
{
   ostringstream ostr;
   ostr << setprecision(16) << t;
   // create value node and add to name node
   xmlNodePtr vNode = xmlNewNode(NULL, BAD_CAST "value");
   xmlNodeAddContent(vNode, BAD_CAST (const char*)ostr.str().c_str());
   xmlAddChild(parent,vNode);

   return ((vNode==NULL));
}

/*
int ADMB_XMLDoc::createValueNode(xmlNodePtr parent, const param_init_bounded_number&  t)
{
   ostringstream ostr;
   ostr << setprecision(16) << value(t);
   // create value node and add to name node
   xmlNodePtr vNode = xmlNewNode(NULL, BAD_CAST "value");
   xmlNodeAddContent(vNode, BAD_CAST (const char*)ostr.str().c_str());
   xmlAddChild(parent,vNode);

   return ((vNode==NULL));
}
*/

int ADMB_XMLDoc::createValueNode(xmlNodePtr parent, const dvector& t)
{
   ostringstream ostr;
   ostr << setprecision(16) << t;
   xmlNodePtr vNode = xmlNewChild(parent,NULL, BAD_CAST "value", 
                                  BAD_CAST (const char*)ostr.str().c_str());

   return ((vNode==NULL));
}

int ADMB_XMLDoc::createBoundsNode(xmlNodePtr parent,  const double min, const double max)
{
   /*
   xmlNodePtr mNode = xmlNewNode(NULL, BAD_CAST "min");
   createValueNode(mNode, min);
   xmlAddChild(parent,mNode);

   mNode = xmlNewNode(NULL, BAD_CAST "max");
   createValueNode(mNode, max);
   xmlAddChild(parent,mNode);

   return ((mNode==NULL));
   */
   xmlNodePtr bNode = xmlNewNode(NULL, BAD_CAST "bounds");
   xmlNodePtr mNode = xmlNewNode(NULL, BAD_CAST "min");
   ostringstream ostr;
   ostr << min;
   xmlNodeAddContent(mNode, BAD_CAST (const char*)ostr.str().c_str());
   xmlAddChild(bNode,mNode);

   mNode = xmlNewNode(NULL, BAD_CAST "max");
   //ostringstream ostrN;
   ostr.seekp(ios_base::beg);
   //ostrN << max;
   ostr << max;
   xmlNodeAddContent(mNode, BAD_CAST (const char*)ostr.str().c_str());
   xmlAddChild(bNode,mNode);
   
   xmlAddChild(parent,bNode);

   return ((bNode==NULL));

}

int ADMB_XMLDoc::createBoundsNode(xmlNodePtr parent,  const dvector& min, const dvector& max)
{
   xmlNodePtr node = createNameNode("bounds",NULL);
   ostringstream ostr;
   ostr << min << ends;
   xmlNewChild(node, NULL, BAD_CAST "min", BAD_CAST ostr.str().c_str());
   
   ostr.seekp(ios_base::beg);
   ostr << max << ends;
   xmlNewChild(node, NULL, BAD_CAST "max", BAD_CAST ostr.str().c_str());

   
   xmlNodePtr tNode = xmlAddChild(parent,node);

   return ((tNode==NULL));

}

int ADMB_XMLDoc::createActiveNode(xmlNodePtr parent,  const int t)
{
   xmlNodePtr node = xmlNewNode(NULL, BAD_CAST "active");
   ostringstream ostr;
   ostr << t;
   xmlNodeAddContent(node, BAD_CAST (const char*)ostr.str().c_str());

   xmlAddChild(parent,node);
   
   return ((node==NULL));
}

int ADMB_XMLDoc::createActiveNode(xmlNodePtr parent,  const ivector& t)
{
   ostringstream ostr;
   ostr << t;
   xmlNodePtr node = xmlNewChild(parent, NULL, BAD_CAST "active",
                                 BAD_CAST (const char*)ostr.str().c_str());

   return ((node==NULL));
}


int ADMB_XMLDoc::createPhaseNode(xmlNodePtr parent,  const int t)
{
   xmlNodePtr node = xmlNewNode(NULL, BAD_CAST "phase");
   ostringstream ostr;
   ostr << t;
   xmlNodeAddContent(node, BAD_CAST (const char*)ostr.str().c_str());

   xmlAddChild(parent,node);
   
   return ((node==NULL));
}


int ADMB_XMLDoc::createPhaseNode(xmlNodePtr parent,  const ivector& t)
{
   ostringstream ostr;
   ostr << t;
   
   xmlNodePtr node = xmlNewChild(parent,NULL, BAD_CAST "phase",
                         BAD_CAST (const char*)ostr.str().c_str());
   
   return ((node==NULL));
}


int ADMB_XMLDoc::createXMLelement(const objective_function_value& t)
{
   adstring name(t.get_name());
   xmlNodePtr node = createNameNode(name, ADMB_XMLDoc::statS);
   createTitleNode(node, adstring("Fit Statistics"));
   
   ostringstream ostr;
   ostr << initial_params::nvarcalc() << ends;
   xmlNewChild(node, NULL, BAD_CAST "n", BAD_CAST ostr.str().c_str());

   createValueNode(node,value(t));

   ostr.seekp(ios_base::beg);
   ostr << t.fun_without_pen << ends;
   xmlNewChild(node, NULL, BAD_CAST "fun_without_pen", BAD_CAST ostr.str().c_str());

   ostr.seekp(ios_base::beg);
   ostr << setprecision(16) << t.gmax << ends;
   xmlNewChild(node, NULL, BAD_CAST "gmax", BAD_CAST ostr.str().c_str());
   
   xmlNodePtr tNode =  xmlAddChild(RootNode,node);
   return ((tNode == NULL));
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

   xmlNodePtr node = createNameNode(name, ADMB_XMLDoc::paramS);
   createTitleNode(node,title);
   createValueNode(node,vt);
   createIndexNode(node,i1,i2);
   createBoundsNode(node,minb,maxb);
   createActiveNode(node,activef);
   createPhaseNode(node,phase);

   xmlNodePtr tnode = xmlAddChild(RootNode,node);
   return ((tnode == NULL));
}

int ADMB_XMLDoc::createXMLelement(const param_init_bounded_vector& t, const adstring& title)
{
   adstring name(t.get_name());
   const int i1 = t.indexmin();
   const int i2 = t.indexmax();

   xmlNodePtr node = createNameNode(name, ADMB_XMLDoc::paramS);
   createTitleNode(node,title);
   createValueNode(node,value(t));
   createIndexNode(node,i1,i2);
   createBoundsNode(node,t.get_minb(),t.get_maxb());
   createActiveNode(node,active(t));
   createPhaseNode(node,t.phase_start);

   xmlNodePtr tnode = xmlAddChild(RootNode,node);
   return ((tnode == NULL));
}

int ADMB_XMLDoc::createIndexNode(xmlNodePtr parent, const int lb, const int ub)
{
   xmlNodePtr node = createNameNode("index",NULL);
   ostringstream ostr;
   ostr << lb << ends;
   xmlNewChild(node, NULL, BAD_CAST "lb", BAD_CAST ostr.str().c_str());
   ostr.seekp(ios_base::beg);
   ostr << ub << ends;
   xmlNewChild(node, NULL, BAD_CAST "ub", BAD_CAST ostr.str().c_str());
   xmlNodePtr tnode = xmlAddChild(parent,node);
   return ((tnode == NULL));
}

int ADMB_XMLDoc::createXMLelement(const adstring& name, const adstring& t, const adstring& title)
{
   xmlNodePtr node = createNameNode(name, ADMB_XMLDoc::dataS);
   createTitleNode(node,title);
   createValueNode(node,t);
   xmlNodePtr tnode = xmlAddChild(RootNode,node);
   return ((tnode == NULL));
}

int ADMB_XMLDoc::createXMLelement(const adstring& name, const int t, const adstring& title)
{
   xmlNodePtr node = createNameNode(name, ADMB_XMLDoc::dataS);
   createTitleNode(node,title);
   createValueNode(node,t);
   xmlNodePtr tnode = xmlAddChild(RootNode,node);
   return ((tnode == NULL));
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
   XMLFileName = string((char*)(doc->URL));
   DocRootName = string((char*)(doc->last->name));
   TTRACE(XMLFileName,DocRootName)

   context = xmlXPathNewContext(doc);
   if (context == 0) {
      throw runtime_error("Error: \n");
   }

   return 1;
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

int ADMB_XMLDoc::getInt(const string& name)
{
   int d = 0;
   string s = getContentString(name,"value");
   if (!s.empty()) {
      d = atoi(s.c_str());
   }
   else {
      throw runtime_error("Error: XPath=\"" + name + "\" does not exist.\n");
   }

   return d;
}

adstring ADMB_XMLDoc::getString(const string& name)
{
   adstring d;
   string s = getContentString(name,"value");
   if (!s.empty()) {
      d = adstring(s.c_str());
   }
   else {
      throw runtime_error("Error: XPath=\"" + name + "\" does not exist.\n");
   }

   return d;
}

/*
<fleet_names category="CON">
  <title>Names of fleets to use in the model</title>
  <value> fjpl jppl pgpl sbpl jpps</value>
</fleet_names>

#include <stdio.h>
#include <string.h>

int main ()
{
  char str[] ="- This, a sample string.";
  char * pch;
  printf ("Splitting string \"%s\" into tokens:\n",str);
  pch = strtok (str," ,.-");
  while (pch != NULL)
  {
    printf ("%s\n",pch);
    pch = strtok (NULL, " ,.-");
  }
  return 0;
}
*/

/** Extract adstring_array from xml document.
This function causes a segment violation on return. 
The problem seems to be passing the array.
\param name Name of the variable.
\parm lb Upper bound of array.
\parm ub Lower bound of array.
\return Reference to adstring_array&
*/
adstring_array& ADMB_XMLDoc::getADStringArray(const string& name, const int lb, const int ub)
{
   string s = getContentString(name,"value");
   TRACE(s)
   adstring_array a(lb,ub);
   TTRACE(a.indexmin(),a.indexmax())

   istringstream istr(s);
   const int tmp_size = 250;
   char * tmp = new char[tmp_size];
   for (int i = lb; i <= ub; i++)
   {
     //istr >> adstring(a(i));
     istr >> tmp;
     TTRACE(i,tmp)
     a(i) = adstring(tmp); //adstring((const char*)pch);
     
     //TTRACE(i,a(i))
     TTRACE(i,a(i))
   }
   delete [] tmp;
   TRACE(a)

   return a;
}

void ADMB_XMLDoc::getADStringArray(const string& name, adstring_array* p)
{
   TRACE(name)
   string s = getContentString(name,"value");
   TRACE(s)
   char * pch = NULL;
   pch = strtok(s.c_str()," ");

   const int lb = p->indexmin();
   const int ub = p->indexmax();
   for (int i = lb; ((i <= ub)&&(pch != NULL)); i++)
   {
      clogf << i << "  \"" <<  pch << "\"" << endl;
      adstring tmp(pch);
      (*p)(i) = tmp; //adstring((const char*)pch);
      pch = strtok (NULL, " ");
      TTRACE(i,(*p)(i))
   }
   TRACE(*p)
}

ivector ADMB_XMLDoc::getIvector(const string& name)
{
   string xpath = "/"+DocRootName + "/" + name + "/index/";
   string s = getContentString(xpath+"lb");
   int lb = atoi(s.c_str());
   s = getContentString(xpath+"ub");
   int ub = atoi(s.c_str());
   ivector d = getIvector(name, lb, ub);

   return d;
}

ivector ADMB_XMLDoc::getIvector(const string& name, const int lb, const int ub)
{
   string s = getContentString(name,"value");
   istringstream istr(s);
   ivector d(lb,ub);
   istr >> d;
   return d;
}

dvector ADMB_XMLDoc::getDvector(const string& name)
{
   string xpath = "/"+DocRootName + "/" + name + "/index/";
   string s = getContentString(xpath+"lb");
   int lb = atoi(s.c_str());
   s = getContentString(xpath+"ub");
   int ub = atoi(s.c_str());
   dvector d = getDvector(name, lb, ub);

   return d;
}

dvector ADMB_XMLDoc::getDvector(const string& name, const int lb, const int ub)
{
   string s = getContentString(name,"value");
   istringstream istr(s);
   dvector d(lb,ub);
   istr >> d;
   return d;
}
/*
<m_ipar category="FLAG">
  <title>The ipars</title>
<flag>
  <name>Number of ipars</name>  <index>0</index>  <value>100</value>
</flag>
*/
ipar_t ADMB_XMLDoc::getIpar(const string& name)
{
   string xpath = "/"+DocRootName + "/" + name + "/flag/"; 
   //TRACE(xpath)
   string* iparname = getContentStringArray(xpath+"name");
   string* index = getContentStringArray(xpath+"index");
   string* value = getContentStringArray(xpath+"value");
   ipar_t v(100);
   //TTRACE(v.indexmin(),v.indexmax())
   for (int i = v.indexmin(); i <= v.indexmax(); i++)
   {
      //TTRACE(index[i],iparname[i])
      int k = atoi(index[i].c_str());
      v(k) = atoi(value[i].c_str());
      v.set_name(k,strdup(iparname[i].c_str()));
      //TTRACE(v(k),v.get_name(k))
   }

   int i = 11;
   TTRACE(v(i),v.get_name(i))
   delete [] iparname;
   delete [] index;
   delete [] value;
   TTRACE(v(i),v.get_name(i))
   return v;
}     

imatrix ADMB_XMLDoc::getImatrix(const string& name)
{
   string xpath = "/"+DocRootName + "/" + name + "/index/"; 
   string s = getContentString(xpath+"nrl");
   int nrl = atoi(s.c_str());
   s = getContentString(xpath+"nru");
   int nru = atoi(s.c_str());

   // you must call istringstream::clear() 
   // in order to reuse the istringstream instance

   istringstream istr;

   s = getContentString(xpath+"ncl");
   istr.str(s);
   ivector ncl(nrl,nru);
   istr >> ncl;

   s = getContentString(xpath+"nch");
   istr.clear();
   istr.str(s);
   ivector nch(nrl,nru);
   istr >> nch;

   imatrix m(nrl, nru, ncl, nch);

   s =  getContentString(name,"value");
   istr.clear();
   istr.str(s);
   istr >> m;

   return m;
}

dmatrix ADMB_XMLDoc::getDmatrix(const string& name)
{
   string xpath = "/"+DocRootName + "/" + name + "/index/"; 
   string s = getContentString(xpath+"nrl");
   int nrl = atoi(s.c_str());
   s = getContentString(xpath+"nru");
   int nru = atoi(s.c_str());

   // you must call istringstream::clear() 
   // in order to reuse the istringstream instance

   istringstream istr;

   s = getContentString(xpath+"ncl");
   istr.str(s);
   ivector ncl(nrl,nru);
   istr >> ncl;

   s = getContentString(xpath+"nch");
   istr.clear();
   istr.str(s);
   ivector nch(nrl,nru);
   istr >> nch;

   dmatrix m(nrl, nru, ncl, nch);

   s =  getContentString(name,"value");
   istr.clear();
   istr.str(s);
   istr >> m;

   return m;
}

/**
 * @param xname absolute element xpath
 */
string ADMB_XMLDoc::getContentString(const string& xpath) const 
{
   string value;
   string xpathfull = xpath;
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
   \param name Element name
   \param tag A specific tag, i. e., "value"
 */
string ADMB_XMLDoc::getContentString(const string& name, const string& tag) const 
{
   string value;
   string xpathfull = "/"+DocRootName + "/" + name + "/" + tag;
   value = getContentString(xpathfull);

   /*
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
   */
   return value;
}

/**
 * @param xname absolute element xpath
 */
string* ADMB_XMLDoc::getContentStringArray(const string& xpath) const 
{
   string* value = NULL;
   string xpathfull = xpath;
   xmlXPathObject* xpathObj = xmlXPathEvalExpression((xmlChar*)xpathfull.c_str(), context);
   if (xpathObj == 0) {
      return value;
   }

   if (xpathObj->type == XPATH_NODESET) 
   {
      xmlNodeSet* xmlnodeset = xpathObj->nodesetval;
      const int max = xmlnodeset->nodeNr;
      value = new string[max];
      for (int i = 0; i < max; i++) 
      {
         xmlNode* xmlnode = xmlnodeset->nodeTab[i];
         if (xmlnode != 0 && xmlnode->children)
         {
             value[i] = (char*)xmlnode->children->content;
         }
      }
   }

   return value;
}




//////////////////////////////////////////////////////////////////////////
#else
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
   XMLFileName = (const char*)filen;
   TRACE(XMLFileName)
   file.open(XMLFileName.c_str());
   if (!file)
   {
      cerr << "Error: unable to open file " << XMLFileName << endl;
      exit(1);
   }
   DocRootName = (const char*)drn;
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

string ADMB_XMLDoc::createIndexTag(const int lb, const int ub)
{
   ostringstream ostr;
   ostr << "  <index>" << "<lb>" << lb << "</lb><ub>" << " " << ub << "</ub></index>\n";
   return ostr.str();
}

string ADMB_XMLDoc::createIndexTag(const int nrl, const int nru,
                                   const int ncl, const int nch)
{
   ivector lb(nrl,nru);
   ivector ub(nrl,nru);
   for (int i = nrl; i <= nru; i++)
   {
       lb(i) = ncl;
       ub(i) = nch;
   }
   return createIndexTag(nrl,nru,lb,ub);
}

string ADMB_XMLDoc::createIndexTag(const int nrl, const int nru,
                                   const ivector& ncl, const ivector& nch)
{
   ostringstream ostr;
   ostr << "  <index>" << "<nrl>" << nrl << "</nrl><nru>" << nru << "</nru>\n";
   ostr << "     <ncl>" << ncl << "</ncl>\n";
   ostr << "     <nch>" << nch << "</nch>\n";
   ostr << "  </index>\n";
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

int ADMB_XMLDoc::createXMLelement(const initial_params& t)
{
    clogf << t.label() << endl;
    if (active(t))
    {
        t.save_value(clogf,15);
        clogf << endl;
     }

    //TTRACE(t.get_minb(),t.get_maxb())
    //TRACE(active(t))
    //TRACE(t.phase_start)
    //TRACE(t.get_initial_value())
    //TRACE(value(t))
    //ivirtual void save_value(BOR_CONST ofstream& ofs,int prec)=0;
    //TTRACE(t.indexmin, t.indexmax)

    return 1;
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
   file << createIndexTag(i1,i2);
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
   file << createIndexTag(t.indexmin(),t.indexmax());
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
   file << createIndexTag(i1,i2);
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
   file << createIndexTag(j1,j2,i1,i2);
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
   file << createIndexTag(j1,j2,i1,i2);
   file << "</" << name << ">\n";

   return !(file);
}

int ADMB_XMLDoc::createXMLcomment(const adstring& t)
{
   file << createCommentTag(t);

   return !(file);
}

int ADMB_XMLDoc::createXMLelement(const objective_function_value& t)
{
   file << "<" << name << " category=\"STAT\">\n";
   file << createTitleTag("Fit Statistics");
   file << "  <n>" << initial_params::nvarcalc() << "</n>\n";
   file << createValueTag(value(t));
   file << "  <fun_without_pen>" << t.fun_without_pen << "</fun_without_pen>\n";
   file << "  <gmax>" << t.gmax << "</gmax>\n";
   file << "</" << name << ">\n";
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

int ADMB_XMLDoc::getInt(const string& name)
{
   int d = 0;
   string s = getContentString(name,"value");
   if (!s.empty()) {
      d = atoi(s.c_str());
   }
   else {
      throw runtime_error("Error: XPath=\"" + name + "\" does not exist.\n");
   }

   return d;
}

adstring ADMB_XMLDoc::getString(const string& name)
{
   adstring d;
   string s = getContentString(name,"value");
   if (!s.empty()) {
      d = adstring(s.c_str());
   }
   else {
      throw runtime_error("Error: XPath=\"" + name + "\" does not exist.\n");
   }

   return d;
}

/*
<fleet_names category="CON">
  <title>Names of fleets to use in the model</title>
  <value> fjpl jppl pgpl sbpl jpps</value>
</fleet_names>

#include <stdio.h>
#include <string.h>

int main ()
{
  char str[] ="- This, a sample string.";
  char * pch;
  printf ("Splitting string \"%s\" into tokens:\n",str);
  pch = strtok (str," ,.-");
  while (pch != NULL)
  {
    printf ("%s\n",pch);
    pch = strtok (NULL, " ,.-");
  }
  return 0;
}
*/

/** Extract adstring_array from xml document.
This function causes a segment violation on return. 
The problem seems to be passing the array.
\param name Name of the variable.
\parm lb Upper bound of array.
\parm ub Lower bound of array.
\return Reference to adstring_array&
*/
adstring_array& ADMB_XMLDoc::getADStringArray(const string& name, const int lb, const int ub)
{
   //TRACE(name)
   string s = getContentString(name,"value");
   TRACE(s)
   //char * pch = NULL;
   //pch = strtok(s.c_str()," ");
   /*
   while (pch != NULL)
   {
      clogf << "\"" <<  pch << "\"" << endl;
      pch = strtok (NULL, " ,.-");
   }
   */   
   //TTRACE(lb,ub)
   adstring_array a(lb,ub);
   TTRACE(a.indexmin(),a.indexmax())

   istringstream istr(s);
   const int tmp_size = 250;
   char * tmp = new char[tmp_size];
   //for (int i = lb; ((i <= ub)&&(pch != NULL)); i++)
   for (int i = lb; i <= ub; i++)
   {
     //istr >> adstring(a(i));
     istr >> tmp;
     TTRACE(i,tmp)
     // clogf << i << "  \"" <<  pch << "\"" << endl;
     // adstring tmp(pch);
      a(i) = adstring(tmp); //adstring((const char*)pch);
     // pch = strtok (NULL, " ");
     
     //TTRACE(i,a(i))
     TTRACE(i,a(i))
   }
   delete [] tmp;
   TRACE(a)

   return a;
}

void ADMB_XMLDoc::getADStringArray(const string& name, adstring_array* p)
{
   TRACE(name)
   string s = getContentString(name,"value");
   TRACE(s)
   char * pch = NULL;
   pch = strtok(s.c_str()," ");

   const int lb = p->indexmin();
   const int ub = p->indexmax();
   for (int i = lb; ((i <= ub)&&(pch != NULL)); i++)
   {
      clogf << i << "  \"" <<  pch << "\"" << endl;
      adstring tmp(pch);
      (*p)(i) = tmp; //adstring((const char*)pch);
      pch = strtok (NULL, " ");
      TTRACE(i,(*p)(i))
   }
   TRACE(*p)
}

ivector ADMB_XMLDoc::getIvector(const string& name)
{
   string xpath = "/"+DocRootName + "/" + name + "/index/";
   string s = getContentString(xpath+"lb");
   int lb = atoi(s.c_str());
   s = getContentString(xpath+"ub");
   int ub = atoi(s.c_str());
   ivector d = getIvector(name, lb, ub);

   return d;
}

ivector ADMB_XMLDoc::getIvector(const string& name, const int lb, const int ub)
{
   string s = getContentString(name,"value");
   istringstream istr(s);
   ivector d(lb,ub);
   istr >> d;
   return d;
}

dvector ADMB_XMLDoc::getDvector(const string& name)
{
   string xpath = "/"+DocRootName + "/" + name + "/index/";
   string s = getContentString(xpath+"lb");
   int lb = atoi(s.c_str());
   s = getContentString(xpath+"ub");
   int ub = atoi(s.c_str());
   dvector d = getDvector(name, lb, ub);

   return d;
}

dvector ADMB_XMLDoc::getDvector(const string& name, const int lb, const int ub)
{
   string s = getContentString(name,"value");
   istringstream istr(s);
   dvector d(lb,ub);
   istr >> d;
   return d;
}
/*
<m_ipar category="FLAG">
  <title>The ipars</title>
<flag>
  <name>Number of ipars</name>  <index>0</index>  <value>100</value>
</flag>
*/
ipar_t ADMB_XMLDoc::getIpar(const string& name)
{
   string xpath = "/"+DocRootName + "/" + name + "/flag/"; 
   //TRACE(xpath)
   string* iparname = getContentStringArray(xpath+"name");
   string* index = getContentStringArray(xpath+"index");
   string* value = getContentStringArray(xpath+"value");
   ipar_t v(100);
   //TTRACE(v.indexmin(),v.indexmax())
   for (int i = v.indexmin(); i <= v.indexmax(); i++)
   {
      //TTRACE(index[i],iparname[i])
      int k = atoi(index[i].c_str());
      v(k) = atoi(value[i].c_str());
      v.set_name(k,strdup(iparname[i].c_str()));
      //TTRACE(v(k),v.get_name(k))
   }

   int i = 11;
   TTRACE(v(i),v.get_name(i))
   delete [] iparname;
   delete [] index;
   delete [] value;
   TTRACE(v(i),v.get_name(i))
   return v;
}     

imatrix ADMB_XMLDoc::getImatrix(const string& name)
{
   string xpath = "/"+DocRootName + "/" + name + "/index/"; 
   string s = getContentString(xpath+"nrl");
   int nrl = atoi(s.c_str());
   s = getContentString(xpath+"nru");
   int nru = atoi(s.c_str());

   // you must call istringstream::clear() 
   // in order to reuse the istringstream instance

   istringstream istr;

   s = getContentString(xpath+"ncl");
   istr.str(s);
   ivector ncl(nrl,nru);
   istr >> ncl;

   s = getContentString(xpath+"nch");
   istr.clear();
   istr.str(s);
   ivector nch(nrl,nru);
   istr >> nch;

   imatrix m(nrl, nru, ncl, nch);

   s =  getContentString(name,"value");
   istr.clear();
   istr.str(s);
   istr >> m;

   return m;
}

dmatrix ADMB_XMLDoc::getDmatrix(const string& name)
{
   string xpath = "/"+DocRootName + "/" + name + "/index/"; 
   string s = getContentString(xpath+"nrl");
   int nrl = atoi(s.c_str());
   s = getContentString(xpath+"nru");
   int nru = atoi(s.c_str());

   // you must call istringstream::clear() 
   // in order to reuse the istringstream instance

   istringstream istr;

   s = getContentString(xpath+"ncl");
   istr.str(s);
   ivector ncl(nrl,nru);
   istr >> ncl;

   s = getContentString(xpath+"nch");
   istr.clear();
   istr.str(s);
   ivector nch(nrl,nru);
   istr >> nch;

   dmatrix m(nrl, nru, ncl, nch);

   s =  getContentString(name,"value");
   istr.clear();
   istr.str(s);
   istr >> m;

   return m;
}
/*
dmatrix ADMB_XMLDoc::getDmatrix(const string& name, const int rowmin, const int rowmax,
                                const int colmin, const int colmax)
{
   string s = getContentString(name,"value");
   istringstream istr(s);
   dmatrix m(rowmin, rowmax, colmin, colmax);
   istr >> m;
   return m;
}
*/

/**
 * @param xname absolute element xpath
 */
string ADMB_XMLDoc::getContentString(const string& xpath) const 
{
   string value;
   string xpathfull = xpath;
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
 * @param xname absolute element xpath
 */
string* ADMB_XMLDoc::getContentStringArray(const string& xpath) const 
{
   string* value = NULL;
   string xpathfull = xpath;
   xmlXPathObject* xpathObj = xmlXPathEvalExpression((xmlChar*)xpathfull.c_str(), context);
   if (xpathObj == 0) {
      return value;
   }

   if (xpathObj->type == XPATH_NODESET) 
   {
      xmlNodeSet* xmlnodeset = xpathObj->nodesetval;
      const int max = xmlnodeset->nodeNr;
      value = new string[max];
      for (int i = 0; i < max; i++) 
      {
         xmlNode* xmlnode = xmlnodeset->nodeTab[i];
         if (xmlnode != 0 && xmlnode->children)
         {
             value[i] = (char*)xmlnode->children->content;
         }
      }
   }

   return value;
}

/**
   \param name Element name
   \param tag A specific tag, i. e., "value"
 */
string ADMB_XMLDoc::getContentString(const string& name, const string& tag) const 
{
   string value;
   string xpathfull = "/"+DocRootName + "/" + name + "/" + tag;
   value = getContentString(xpathfull);

   /*
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
   */
   return value;
}


/**
 * @param name element name
 * @param value set name with this value
 */
bool ADMB_XMLDoc::setContent(const string& name, const string& value) 
{
   string xpathfull = "/" +DocRootName + "/" + name + "/value";
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
   XMLFileName = string((char*)(doc->URL));
   DocRootName = string((char*)(doc->last->name));
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
#endif // __USE_OLD_VERSION__



