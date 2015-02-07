/***********************************************************************
* The class Char_type is used to tell the type of various charcters.
*
* The main member functions are:
*     is -- True if the character is the indicated type
*     type -- Return type of character
**********************************************************************/
#include <iostream.h>
#include "ch_type.h"

//Define the type information array//
Char_type::CHAR_TYPE Char_type::type_info[ARR_MAX];

//Fill in a range of types for the character type class//
void Char_type::fill_range(int start, int end, CHAR_TYPE type)
{
  int cur_ch;
  for (cur_ch = start; cur_ch <= end; ++cur_ch)
  {
    type_info[cur_ch] = type;
  }
}

//Intialize the char type table//
Char_type::Char_type()
{
  fill_range(0, ARR_MAX-1, C_WHITE);
  //fill_range('A', 'Z', C_ALPHA);
  //fill_range('a', 'z', C_ALPHA);
  //type_info['_'] = C_ALPHA;
  fill_range('0', '9', C_DIGIT);
  type_info['\n'] = C_NEWLINE;
  type_info['\t'] = C_TAB;
  type_info[' '] = C_SPACE;
}

int Char_type::is(int ch, CHAR_TYPE kind)
{
  if (ch == EOF)
    return (kind == C_EOF);
  return (type_info[ch] == kind);
}

Char_type::CHAR_TYPE Char_type::type(int ch)
{
  if (ch == EOF)
    return (C_EOF);
  return (type_info[ch]);
}

