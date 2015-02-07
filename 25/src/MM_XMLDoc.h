// $Id:  $
/** \file MM_XMLDoc.h
Definitaion of class MM_XMLDoc. Based on the XMLDocument2 class by Johnoel Ancheta.
   Copyright&copy; 2012 University of Hawaii, Pelagic Fisheries Research Program

*/

#ifndef __MM_XMLDoc__
#define __MM_XMLDoc__

#include "ADMB_XMLDoc.h"
// specific to movemod
#include "ipar.h"
#include "par_t.h"

class MM_XMLDoc : public ADMB_XMLDoc
{
public:
   /*
   MM_XMLDoc();
  ~MM_XMLDoc();
   MM_XMLDoc(const MM_XMLDoc& t); ///< Undefined copy construcor
   */
   MM_XMLDoc(const adstring& drn, const adstring& prg, const adstring& id, const adstring file);
   int createXMLelement(const adstring& name, const ipar_t& t, const adstring& title);

   int createXMLelement(const adstring& name, const  trv_ptr& t, const int nrelease,
                        const adstring& title);

   int createXMLelement(const adstring& name, const double t, const adstring& title)
     { return ADMB_XMLDoc::createXMLelement(name, t, title); }
   int createXMLelement(const adstring& name, const int t, const adstring& title);
   int createXMLelement(const adstring& name, const adstring& t, const adstring& title);
   int createXMLelement(const adstring& name, const adstring_array& t, const adstring& title);
   int createXMLelement(const adstring& name, const ivector& t, const adstring& title);
   int createXMLelement(const adstring& name,const param_init_bounded_number_vector& t, const adstring& title);
   int createXMLelement(const adstring& name, const imatrix& t, const adstring& title);
   //int createXMLelement(const adstring& name, const ipar_t& t, const adstring& title);

   //int createXMLelement(const adstring& name, const  trv_ptr& t, const int nrelease,
   //                     const adstring& title);
   int createXMLelement(const param_init_bounded_number& t, const adstring& title);
   int createXMLelement(const param_init_bounded_vector& t, const adstring& title);
   int createXMLelement(const param_init_bounded_matrix& t, const adstring& title);

   int createXMLelement(const objective_function_value& t){return ADMB_XMLDoc::createXMLelement(t); }

   int createXMLcomment(const adstring& t);
    int createXMLelement(const initial_params& t);





};
#endif // __MM_XMLDoc__
