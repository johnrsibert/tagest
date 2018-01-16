/*
 * $Id: XMLDocument2.h 2624 2009-10-29 02:19:27Z jsibert $
 * Copyright&copy;2005 University of Hawaii, Pelagic Fisheries Research Program
 */
#ifndef __XMLDocument2_h__
#define __XMLDocument2_h__

#include <fvar.hpp>

#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include <sstream>
#include <iomanip>

#include "trace.h"

/// @cond TellsDoxygenToSkipThese
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::atoi;
//using std::atof;
using std::strtod;
using std::runtime_error;
using std::istringstream;
using std::setprecision;
/// @endcond

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

class XMLDocument2 {
public:
/**
 \author Johnoel Ancheta
 */
XMLDocument2() { 
   doc = 0;
   context = 0;
   xmlInitParser();
}

/**
 *
 */
XMLDocument2(const XMLDocument2& p) { 
   cerr << "Error: " << __FILE__ << ':' << __LINE__ << '\n'; 
}

/**
 *
 */
virtual ~XMLDocument2() {
   if (doc != 0) {
      xmlFreeDoc(doc);
      doc = 0;
   }

   if (context != 0) {
      xmlXPathFreeContext(context);
      context = 0;
   }

   xmlCleanupParser();
}

public:

/**
 * import a parfile into document
 * @param parfile filename
 */
int read(const char* parfile) throw (runtime_error) {
   if (parfile == 0) {
      return -1;
   }

   doc = xmlParseFile(parfile);
   if (doc == NULL) {
      throw runtime_error("Error: \n");
   }

   context = xmlXPathNewContext(doc);
   if (context == 0) {
      throw runtime_error("Error: \n");
   }

   return 1;
}

/**
 * @param xpath absolute element xpath
 * @param value set xpath with the values in this dvector
 */
bool set(const string& xpath, const dvector& value)
{

   std::ostringstream ostr;
   ostr << endl;
   for (int i = value.indexmin(); i <= value.indexmax(); i++)
        {
      ostr << " " << setprecision(16) << value(i);
      //if ((i>1) && (i%4 == 1))
      if (i%4 == 0)
         ostr << endl;
   }
   ostr << endl;
   return set(xpath,ostr.str());

}

/**
 * @param xpath absolute element xpath
 * @param value set xpath with this value
 */
bool set(const string& xpath, const string& value) {
   string xpathfull = "/par" + xpath;
   TRACE(xpathfull)
   xmlXPathObject* xpathObj = xmlXPathEvalExpression((xmlChar*)xpathfull.c_str(), context);
   if (xpathObj == 0) {
      return false;
   }

   if (xpathObj->type == XPATH_NODESET) {
      xmlNodeSet* xmlnodeset = xpathObj->nodesetval;
      const int max = xmlnodeset->nodeNr;
      for (int i = 0; i < max; i++) {
         xmlNode* xmlnode = xmlnodeset->nodeTab[i];
         if (xmlnode != 0 && xmlnode->children) {
            if (xmlnode->children->content != 0) {
               delete xmlnode->children->content;
               xmlnode->children->content = 0;
            }
            xmlnode->children->content = xmlCharStrdup(value.c_str());
         }
      }
   }

   return true;
}

/**
 * @param xpath absolute element xpath
 * @param attribute name of attribute
 * @param value set xpath with this value
 */
bool set(const string& xpath, const string& attribute, const int value) {
   std::ostringstream ostr;
   ostr << setprecision(6) << value;
   return set(xpath, attribute, ostr.str());
}

/**
 * @param xpath absolute element xpath
 * @param value set xpath with this value
 */
bool set(const string& name, const double value) 
{
   TTRACE(name,value)
   std::ostringstream ostr;
   ostr << setprecision(16) << value;
   return setContent(name, ostr.str());
}

/**
 * @param xpath absolute element xpath
 * @param value set xpath with this value
 */
bool setContent(const string& name, const string& value) 
{
   TTRACE(name,value)
   string xpathfull = "/FIT/" + name + "/value";
   TRACE(xpathfull)
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
         TTRACE(i,(const xmlChar*)xmlnode->name)
         TTRACE(i,(char*)xmlnode->children->content)
         if (xmlnode != 0 && xmlnode->children)
         {
           xmlnode->children->content = xmlCharStrdup(value.c_str());  
         }
         TTRACE(i,(char*)xmlnode->children->content)
         TTRACE(i,(const xmlChar*)xmlnode->name)
       }
   }

   xmlXPathFreeObject(xpathObj);

   return true;
}
/**
 * @param xpath absolute element xpath
 * @param attribute name of attribute
 * @param value set xpath with this value
 */
bool set(const string& xpath, const string& attribute, const string& value) {
   string xpathfull = "/par" + xpath;
   xmlXPathObject* xpathObj = xmlXPathEvalExpression((xmlChar*)xpathfull.c_str(), context);
   if (xpathObj == 0) {
      return false;
   }

   if (xpathObj->type == XPATH_NODESET) {
      xmlNodeSet* xmlnodeset = xpathObj->nodesetval;
      const int max = xmlnodeset->nodeNr;
      for (int i = 0; i < max; i++) {
         xmlNode* xmlnode = xmlnodeset->nodeTab[i];
         if (xmlnode != 0) {
            for (xmlAttr *attributenode = xmlnode->properties; attributenode != 0; attributenode = attributenode->next) {
               if (attribute.compare(string((char*)attributenode->name)) == 0) {
                  attributenode->children->content = xmlCharStrdup(value.c_str());
               }
            }
         }
      }
   }

   xmlXPathFreeObject(xpathObj);

   return true;
}


string itoa(const int i) const {
   string s;
   return s;
}

/**
 * @param xpath absolute element xpath
 * @param array index
 */
int getInteger(const string& xpath, const int idx) const throw (runtime_error) {
   int i = 0;
   string value = get(xpath, idx);
   if (!value.empty()) {
      i = atoi(value.c_str());
   } else {
      throw runtime_error("Error: XPath=\"" + xpath + "\"  with index=\"" + string(itoa(idx)) + "\" does not exist.\n");
   }
   return i;
}

/**
 * @param xpath absolute element xpath
 * @param array index
 */
double getDouble(const string& name, const int idx) const throw (runtime_error) {
   string xpath = string("/variable/name/")+name;
   TRACE(xpath)
   double d = 0;
   string value = get(xpath, idx);
   if (!value.empty()) {
      //d = atof(value.c_str());
      d = strtod(value.c_str(),0);
   } else {
      throw runtime_error("Error: XPath=\"" + xpath + "\"  with index=\"" + string(itoa(idx)) + "\" does not exist.\n");
   }
   return d;
}

/**
 * @param xpath absolute element xpath
 * @param lb minimum valid index of dvector
 * @param ub maximum valid index of dvector
 * @return dvector
 */
dvector getDvector(const string& xpath, const int lb, const int ub) const throw (runtime_error)
{
   dvector d(lb,ub);
   for (int i = lb; i <= ub; i++)
   {
      d(i) = getDouble(xpath, i-lb);
   }
   return d;
}   

/**
 * @param xpath absolute element xpath
 * @param array index
 */
string get(const string& xpath, const int idx) const {
   string value;

   istringstream is(get(xpath));
   for (int count = 0; is && count <= idx; count++) {
      value.clear();
      is >> value;
   }

   return value;
}

/**
 * @param xpath absolute element xpath
 */
string get(const string& xpath) const {
   string value;
   string xpathfull = "/FIT" + xpath;
   TRACE(xpathfull)
   xmlXPathObject* xpathObj = xmlXPathEvalExpression((xmlChar*)xpathfull.c_str(), context);
   TRACE(xpathObj)
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
            TTRACE(i,(const xmlChar*)xmlnode->name)
            TTRACE(i,(char*)xmlnode->children->content)
               value += (char*)xmlnode->children->content;

            //value = string((char*)xmlnode->children);
            //TTRACE(i,(char*)xmlnode->children
            //value = string((char*)xmlnode->children->content);
         }
      }
   }

   return value;
}

/**
 * @param xpath absolute element xpath
 * @param attribute name of attribute
 */
string get(const string& xpath, const string& attribute) const {
   string value;

   string xpathfull = "/par" + xpath;
   xmlXPathObject* xpathObj = xmlXPathEvalExpression((xmlChar*)xpathfull.c_str(), context);
   if (xpathObj == 0) {
      return value;
   }

   if (xpathObj->type == XPATH_NODESET) {
      xmlNodeSet* xmlnodeset = xpathObj->nodesetval;
      const int max = xmlnodeset->nodeNr;
      for (int i = 0; i < max; i++) {
         xmlNode* xmlnode = xmlnodeset->nodeTab[i];
         if (xmlnode != 0) {
            for (xmlAttr *attributenode = xmlnode->properties; attributenode != 0; attributenode = attributenode->next) {
               if (attribute.compare(string((char*)attributenode->name)) == 0 && attributenode->children != 0) {
                  value += string((char*)attributenode->children->content);
               }
            }
         }
      }
   }

   xmlXPathFreeObject(xpathObj);

   return value;
}

/**
 * @param xpath absolute element xpath
 */
double getDouble(const string& name) const throw (runtime_error) {
   string xpath = "/"+name+"/value";
   TRACE(xpath)
   double d = 0;
   string s = get(xpath);
   if (!s.empty()) {
      //d = atof(s.c_str());
      d = strtod(s.c_str(),0);
   } else {
      throw runtime_error("Error: XPath=\"" + xpath + "\" does not exist.\n");
   }
   return d;
}

/**
 * @param xpath absolute element xpath
 */
double getDouble(const string& xpath, const string& attribute) const throw (runtime_error) {
   double d = 0;
   string s = get(xpath, attribute);
   if (!s.empty()) {
      //d = atof(s.c_str());
      d = strtod(s.c_str(),0);
   } else {
      throw runtime_error("Error: XPath=\"" + xpath + "\"  with attribute=\"" + attribute + "\" does not exist.\n");
   }
   return d;
}

/**
 * @param xpath absolute element xpath
 */
int getInteger(const string& xpath) const throw (runtime_error) {
   int integer = 0;
   string s = get(xpath);
   if (!s.empty()) {
      integer = atoi(s.c_str());
   } else {
      throw runtime_error("Error: XPath=\"" + xpath + "\" does not exist.\n");
   }
   return integer;
}

/**
 * @param xpath absolute element xpath
 */
int getInteger(const string& xpath, const string& attribute) const throw (runtime_error) {
   int integer = 0;
   string s = get(xpath, attribute);
   if (!s.empty()) {
      integer = atoi(s.c_str());
   } else {
      throw runtime_error("Error: XPath=\"" + xpath + "\"  with attribute=\"" + attribute + "\" does not exist.\n");
   }
   return integer;
}

/**
 * write the contents of doc to parfile
 * @param xpath absolute element xpath
 */
int write(const char* parfile) 
{
   const int nbyte = xmlSaveFile(parfile, doc);
   return  nbyte;
}

private:

xmlDoc* doc;
xmlXPathContext* context;
};
#endif
