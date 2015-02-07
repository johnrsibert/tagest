//$Id: ch_type.h 2754 2011-01-02 20:57:07Z jsibert $
#ifndef __chtype_h__
#define __chtype_h__

const int ARR_MAX = 128;

class Char_type
{
  public:
    Char_type();   //Initialize the data//

    enum CHAR_TYPE
    {
      C_EOF,      //End of file character//
      C_WHITE,    //Whitespace or control character//
      C_SPACE,    //Space character//
      C_TAB,      //Space character//
      C_NEWLINE,  //A newline character//
      C_DIGIT     //A number//
      //C_ALPHA,    //A letter (includes _)//
    };  

    //Returns true if character is a given type//
    int is(int ch, CHAR_TYPE kind);
    CHAR_TYPE type(int ch);
  private:
    static enum CHAR_TYPE type_info[ARR_MAX];
    void fill_range(int start, int end, CHAR_TYPE type);
};

#endif
