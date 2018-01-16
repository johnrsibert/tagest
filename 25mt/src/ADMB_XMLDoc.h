// $Id: ADMB_XMLDoc.h 2956 2012-03-11 01:34:21Z jsibert $
/** \file ADMB_XMLDoc.h
Definitaion of class ADMB_XMLDoc. Based on the XMLDocument2 class by Johnoel Ancheta.
   Copyright&copy; 2012 University of Hawaii, Pelagic Fisheries Research Program
*/

#ifndef __ADMB_XMLDoc__
#define __ADMB_XMLDoc__

#include <fvar.hpp>
#include <admodel.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

using namespace std;

class trv_ptr;
class ipar_t;

#undef __USE_OLD_VERSION__
#ifndef __USE_OLD_VERSION__

//LIBXML_TEST_VERSION;
/** Class for handling ADMB data types in XML. 
*/
class ADMB_XMLDoc
{
protected:
   xmlDocPtr doc;              ///< pointer to an instance of xmlDoc
   xmlXPathContext* context; ///< pointer to an in instance of a xmlContext
   xmlNodePtr RootNode;      ///< pointer to root node of document
  // = NULL, node = NULL, node1 = NULL;/* node pointers */
 
   string XMLFileName;       ///< file name associated with output file
   string DocRootName;       ///< root name of xml tree

   static const char * paramS;
   static const char * dataS;
   static const char * statS;

public:
   ADMB_XMLDoc();
   ADMB_XMLDoc(const adstring& drn, const adstring& prg, const adstring& id, const adstring file);
  ~ADMB_XMLDoc();
   ADMB_XMLDoc(const ADMB_XMLDoc& t); ///< Undefined copy construcor

   int write(void);

   adstring getFileName()const {return adstring(XMLFileName.c_str());};

    /** \name Element creation 
    @{ */
   /** 
    \public
    Create xml elements for different argument types recognized by ADMB.
   \param name adstring containing the human-readable name of the variable. 
    Should be the same as the variable name in the calling program.
    Not used for "named" ADMB types used in the PARAMETER_SECTION of a .tpl file.
   \param t Variable to be stored in the xml. Overloaded.
   \param title adstring containing nice descriptive name for the variable.
    Supports the goal of producing a "self-documenting" ascii data file.
   \return An integer indicating whether the write to file was successful.
    0 => successful write operation. 
   */
   int createXMLelement(const adstring& name, const double t, const adstring& title);
   int createXMLelement(const adstring& name, const int t, const adstring& title);
   int createXMLelement(const adstring& name, const adstring& t, const adstring& title);
   int createXMLelement(const adstring& name, const adstring_array& t, const adstring& title);
   int createXMLelement(const adstring& name, const ivector& t, const adstring& title);
   int createXMLelement(const adstring& name,const param_init_bounded_number_vector& t, const adstring& title);
   int createXMLelement(const adstring& name, const imatrix& t, const adstring& title);

   int createXMLelement(const param_init_bounded_number& t, const adstring& title);
   int createXMLelement(const param_init_bounded_vector& t, const adstring& title);
   int createXMLelement(const param_init_bounded_matrix& t, const adstring& title);

   int createXMLelement(const objective_function_value& t);

   int createXMLcomment(const adstring& t);
   ///@}

   /** Stub. Intended to update xml elemsnt value for any ADMB types.
   ADMB needs to be modifited for this to work. One possibility is to 
   change virtual void save_value(BOR_CONST ofstream& ofs,int prec)=0;
   into virtual void save_value(ostream& ofs,int prec)=0;
   \param t element of initial_params::varptr array.
   */ 
   int createXMLelement(const initial_params& t);


    /** \name Movemod-specific Element creation 
    @{ */
   /** 
    \public
    Handle xml elements for different argument types used in movemod.
   */
   int createXMLelement(const adstring& name, const  trv_ptr& t, const int nrelease,
                        const adstring& title);
   int createXMLelement(const adstring& name, const ipar_t& t, const adstring& title);
   ipar_t getIpar(const string& name);
   ///@}

   double getDouble(const string& name);
   int getInt(const string& name);
   adstring getString(const string& name);
   adstring_array& getADStringArray(const string& name, const int lb, const int ub);
   void getADStringArray(const string& name, adstring_array* p);
   ivector getIvector(const string& name);
   ivector getIvector(const string& name, const int lb, const int ub); 
   dvector getDvector(const string& name);
   dvector getDvector(const string& name, const int lb, const int ub); 
   imatrix getImatrix(const string& name);
   dmatrix getDmatrix(const string& name);
   //dmatrix getDmatrix(const string& name, const int rowmin, const int rowmax,
   //                             const int colmin, const int colmax);

   bool set(const string& name, const double value);
   bool set(const string& name, const dvector& value);
   bool set(const string& name, const dmatrix& value);

   int read(const char* parfile);// throw (runtime_error);
   int write(const char* parfile);

protected:

   /** @name Node creation
    *  @{
    */
   /**  Create new xml tag for different types.
   Overloaded for different argument types. 
   Uses ostrstream class to process argument.
   \return std::string object using the ostrstream::str() function. 
   */
    xmlNodePtr createNameNode(const adstring& name, const char* category);

    int createTitleNode(xmlNodePtr parent, const adstring& title);

    int createValueNode(xmlNodePtr parent, const double t);
    int createValueNode(xmlNodePtr parent, const dvector&  t);
    int createValueNode(xmlNodePtr parent, const adstring&  t);
    //int createValueNode(xmlNodePtr parent, const param_init_bounded_number&  t);
    int createIndexNode(xmlNodePtr parent, const int lb, const int ub);

    int createBoundsNode(xmlNodePtr parent, const double min, const double max);
    int createBoundsNode(xmlNodePtr parent, const dvector& min, const dvector& max);
    int createActiveNode(xmlNodePtr parent, const int t);
    int createActiveNode(xmlNodePtr parent, const ivector& t);
    int createPhaseNode(xmlNodePtr parent, const int t);
    int createPhaseNode(xmlNodePtr parent, const ivector& t);


   /** @} */

   string* getContentStringArray(const string& xpath) const;
   string getContentString(const string& xpath) const;
   string getContentString(const string& name, const string& tag) const;
   int getIndexInt(const string& name);
   ivector getIndexIvector(const string& name);

   bool setContent(const string& name, const string& value);

};


//////////////////////////////////////////////////////////////////////////////
#else
/** Class for handling ADMB data types in XML. 
*/
class ADMB_XMLDoc
{
protected:
   xmlDoc* doc;              ///< pointer to an instance of xmlDoc
   xmlXPathContext* context; ///< pointer to an in instance of a xmlContext
 
   ofstream file;            ///< output file stream
   string XMLFileName;       ///< file name associated with output file
   string DocRootName;       ///< root name of xml tree

public:
   ADMB_XMLDoc();
   ADMB_XMLDoc(const adstring& drn, const adstring& prg, const adstring& id, const adstring file);
  ~ADMB_XMLDoc();
   ADMB_XMLDoc(const ADMB_XMLDoc& t); ///< Undefined copy construcor

   adstring getFileName()const {return adstring(XMLFileName.c_str());};

    /** \name Element creation 
    @{ */
   /** 
    \public
    Create xml elements for different argument types recognized by ADMB.
   \param name adstring containing the human-readable name of the variable. 
    Should be the same as the variable name in the calling program.
    Not used for "named" ADMB types used in the PARAMETER_SECTION of a .tpl file.
   \param t Variable to be stored in the xml. Overloaded.
   \param title adstring containing nice descriptive name for the variable.
    Supports the goal of producing a "self-documenting" ascii data file.
   \return An integer indicating whether the write to file was successful.
    0 => successful write operation. 
   */
   int createXMLelement(const adstring& name, const double t, const adstring& title);
   int createXMLelement(const adstring& name, const int t, const adstring& title);
   int createXMLelement(const adstring& name, const adstring& t, const adstring& title);
   int createXMLelement(const adstring& name, const adstring_array& t, const adstring& title);
   int createXMLelement(const adstring& name, const ivector& t, const adstring& title);
   int createXMLelement(const adstring& name,const param_init_bounded_number_vector& t, const adstring& title);
   int createXMLelement(const adstring& name, const imatrix& t, const adstring& title);

   int createXMLelement(const param_init_bounded_number& t, const adstring& title);
   int createXMLelement(const param_init_bounded_vector& t, const adstring& title);
   int createXMLelement(const param_init_bounded_matrix& t, const adstring& title);

   int createXMLelement(const objective_function_value& t);

   int createXMLcomment(const adstring& t);
   ///@}

   /** Stub. Intended to update xml elemsnt value for any ADMB types.
   ADMB needs to be modifited for this to work. One possibility is to 
   change virtual void save_value(BOR_CONST ofstream& ofs,int prec)=0;
   into virtual void save_value(ostream& ofs,int prec)=0;
   \param t element of initial_params::varptr array.
   */ 
   int createXMLelement(const initial_params& t);


    /** \name Movemod-specific Element creation 
    @{ */
   /** 
    \public
    Handle xml elements for different argument types used in movemod.
   */
   int createXMLelement(const adstring& name, const  trv_ptr& t, const int nrelease,
                        const adstring& title);
   int createXMLelement(const adstring& name, const ipar_t& t, const adstring& title);
   ipar_t getIpar(const string& name);
   ///@}

   double getDouble(const string& name);
   int getInt(const string& name);
   adstring getString(const string& name);
   adstring_array& getADStringArray(const string& name, const int lb, const int ub);
   void getADStringArray(const string& name, adstring_array* p);
   ivector getIvector(const string& name);
   ivector getIvector(const string& name, const int lb, const int ub); 
   dvector getDvector(const string& name);
   dvector getDvector(const string& name, const int lb, const int ub); 
   imatrix getImatrix(const string& name);
   dmatrix getDmatrix(const string& name);
   //dmatrix getDmatrix(const string& name, const int rowmin, const int rowmax,
   //                             const int colmin, const int colmax);

   bool set(const string& name, const double value);
   bool set(const string& name, const dvector& value);
   bool set(const string& name, const dmatrix& value);

   int read(const char* parfile);// throw (runtime_error);

protected:

   /** @name Tag creation
    *  @{
    */
   /**  Create new xml tag for different types.
   Overloaded for different argument types. 
   Uses ostrstream class to process argument.
   \return std::string object using the ostrstream::str() function. 
   */
   string createValueTag(const double t);
   string createValueTag(const dvector& t);
   string createValueTag(const ivector& t);
   string createValueTag(const dmatrix& t);
   string createValueTag(const imatrix& t);
   string createValueTag(const adstring& t);
   string createValueTag(const adstring_array& t);
   string createBoundsTag(const double min, const double max);
   string createBoundsTag(const dvector& min, const dvector& max);
   string createActiveTag(const int t);
   string createActiveTag(const ivector& t);
   string createPhaseTag(const int t);
   string createPhaseTag(const ivector& t);
   string createIndexTag(const int min, const int max);
   string createIndexTag(const int rlb, const int rub,
                         const int clb, const int cub);
   string createIndexTag(const int nrl, const int nru,
                         const ivector& ncl, const ivector& nch);
   string createTitleTag(const adstring& t);
   string createCommentTag(const adstring& t);
   /** @} */

   string* getContentStringArray(const string& xpath) const;
   string getContentString(const string& xpath) const;
   string getContentString(const string& name, const string& tag) const;
   int getIndexInt(const string& name);
   ivector getIndexIvector(const string& name);

   bool setContent(const string& name, const string& value);

};
#endif // __USE_OLD_VERSION__

#endif //#define __ADMB_XMLDoc__
