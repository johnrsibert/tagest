// $Id: ADMB_XMLDoc.h 2939 2012-02-02 00:12:05Z jsibert $
/** \file ADMB_XMLDoc.h
Definitaion of class ADMB_XMLDoc. Based on the XMLDocument2 class by Johnoel Ancheta.
   Copyright&copy; 2012 University of Hawaii, Pelagic Fisheries Research Program
*/

#ifndef __ADMB_XMLDoc__
#define __ADMB_XMLDoc__

#include <fvar.hpp>
#include <admodel.h>

// specific to movemod
#include "ipar.h"
#include "par_t.h"

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>


using namespace std;
/** Class for handling ADMB data types in XML. 
*/
class ADMB_XMLDoc
{
private:
   xmlDoc* doc;              ///< pointer to an instance of xmlDoc
   xmlXPathContext* context; ///< pointer to an in instance of a xmlContext

   ofstream file;            ///< output file stream
   adstring XMLFileName;     ///< file name associated with output file
   adstring DocRootName;     ///< root name of xml tree

public:
   ADMB_XMLDoc();
   ADMB_XMLDoc(const adstring& drn, const adstring& prg, const adstring& id, const adstring file);
  ~ADMB_XMLDoc();
   ADMB_XMLDoc(const ADMB_XMLDoc& t); ///< Undefined copy construcor

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
   int createXMLelement(const adstring& name, const ipar_t& t, const adstring& title);

   int createXMLelement(const adstring& name, const  trv_ptr& t, const int nrelease,
                        const adstring& title);
   int createXMLelement(const param_init_bounded_number& t, const adstring& title);
   int createXMLelement(const param_init_bounded_vector& t, const adstring& title);
   int createXMLelement(const param_init_bounded_matrix& t, const adstring& title);
   int createXMLcomment(const adstring& t);
   ///@}


   double getDouble(const string& name);
   dvector getDvector(const string& name);
   dvector getDvector(const string& name, const int lb, const int ub); 
   dmatrix getDmatrix(const string& name);
   dmatrix getDmatrix(const string& name, const int rowmin, const int rowmax,
                                const int colmin, const int colmax);

   bool set(const string& name, const double value);
   bool set(const string& name, const dvector& value);
   bool set(const string& name, const dmatrix& value);

   int read(const char* parfile);// throw (runtime_error);
   int write(const char* parfile);


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
   string createRangeTag(const int min, const int max);
   string createRangeTag(const int rlb, const int rub,
                         const int clb, const int cub);
   string createTitleTag(const adstring& t);
   string createCommentTag(const adstring& t);
   /** @} */

   string getContentString(const string& xpath, const string& tag) const;

   bool setContent(const string& name, const string& value);

};
#endif //#define __ADMB_XMLDoc__
