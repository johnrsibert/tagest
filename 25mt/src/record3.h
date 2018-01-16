//$Id: record3.h 2754 2011-01-02 20:57:07Z jsibert $
#ifndef __RECORD_H__
#define __RECORD_H__
#include <fvar.hpp>
#include <cifstrem.h>
#include <adstring.hpp>
#include <strstream>

//A list of Records//
#define RECORD_LIST F(F_NEWLINE),  F(F_RELEASE), F(F_RECAPTURE), F(F_EOF)

//Define enumerated list of records//
#define F(x) x           //Define F() as the name//
enum RECORD_TYPE
{
  RECORD_LIST
};
#undef F                //Remove old temp macro//

//A list of the names of the records//
extern const char *const RECORD_NAMES[];

//const int LINE_MAX = 250;    //Longest possible line//
//#define LINE_MAX 250    //Longest possible line//

/*****************************************************************
* Input_file -- data from the input file
*
* The current two characters are stored in cur_char and next_char
*
* The line is buffered

class Input_file : public ifstream
{
  public:
    int cur_char;            //Current character (can be EOF)//
    int next_char;           //Next character (can be EOF)//

    //Initialize input file and read first 2 chars//
   #ifndef __SC__
    Input_file(const char *name) : ifstream(name)
   #else
    Input_file(const char *name) : ios(&buffer), ifstream(name)
   #endif 
    {
      if (bad())
        return;
      cur_char = get();
      next_char = get();
      char_ptr = line;
    }
    //Write line to screen//
    void flush_line()
    {
      *char_ptr = (char)NULL;
      //cout << line;
      //cout.flush();
      char_ptr = line;
    }
    //Advance one char//
    void read_char(void)
    {
      *char_ptr = cur_char;
      ++char_ptr;
      cur_char = next_char;
      next_char = get();
    }
    void copy(strstream& to)
    {
      *char_ptr = NULL;
      to << line << ends;
    }
    void print()
    {
      cout << line << endl;
    }
  //private:
    char line[LINE_MAX];    //Current line//
    char *char_ptr;         //Current character on line//
};

*****************************************************************/
#ifndef TRUE
  #define TRUE 1
  #define FALSE 0
#endif

/*********************************************************************
* Record class
*
* Reads next record in the input stream and returns its type
*********************************************************************/
class Record
{
  public:
    Record()
    {  need_to_read_one = 0; }
    //Returns the next record in the input file//
    RECORD_TYPE next_record(cifstream& in_file, adstring& tag_record);
 
  private:
    //True if we need to read a character//
    int need_to_read_one;
};

#endif
