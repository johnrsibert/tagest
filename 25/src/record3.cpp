//$Id: record3.cpp 2885 2011-11-23 18:28:11Z jsibert $
/*********************************************************************
* Record -- record handling module
*
* Functions:
*    next_record -- get the next record from the input
*********************************************************************/
#include <fstream>
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

#define LINE_MAX 250

static Char_type Char_types;   //Character type information//

//Read the next record in an input stream//
RECORD_TYPE Record::next_record(cifstream& in_file, adstring& tag_record)
{
  char t[LINE_MAX];
  in_file.getline(t,LINE_MAX-1 );
  tag_record = adstring(t);
  if (in_file.eof() == false && tag_record.size() > 0)
  {
    /*
    Simple test of first character fails if tag number starts with a digit, ie.
    if (isdigit(tag_record[1]))
    Checking first 2 characters correctly identifies partial date field in
    a release reccord and passes a tag number starting with a digit
    May be a problem only  for SSAP data.
    if (isdigit(tag_record[1]) && isdigit(tag_record[2]))
    works unless the the tag number begins with two digits.
    The following identified a date field in the relase record by the '/'
    separating year and month.
    */
    if ((tag_record[3] == '/') || (tag_record[5] == '/'))
      return (F_RELEASE);
    else
      return (F_RECAPTURE);
  } 
  return (F_EOF);
}

