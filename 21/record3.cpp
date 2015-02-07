/*********************************************************************
* Record -- record handling module
*
* Functions:
*    next_record -- get the next record from the input
*********************************************************************/
#include <fstream.h>
#include <stdlib.h>

#include "ch_type.h"
#include "record3.h"
#include "trace.h"

//Define the record name list//
#define F(x) #x            //define x as a string//
const char *const RECORD_NAMES[] =
{
  RECORD_LIST
};
#undef F                  //remove old temp macro//

static Char_type Char_types;   //Character type information//

//Read the next record in an input stream//
RECORD_TYPE Record::next_record(cifstream& in_file, adstring& tag_record)
{
  char t[LINE_MAX];
  in_file.getline(t,LINE_MAX-1 );
  tag_record = adstring(t);
#ifdef __BCPLUSPLUS__
  if (in_file.good())
#else
//  #error Undifined for this compiler
  if  (in_file)
#endif
  {
    if (isdigit(tag_record[1]))
      return (F_RELEASE);
    else
      return (F_RECAPTURE);
  } 
  return (F_EOF);
}

