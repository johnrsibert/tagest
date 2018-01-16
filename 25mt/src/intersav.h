//$Id: intersav.h 2754 2011-01-02 20:57:07Z jsibert $
#ifndef INTERSAVE_H
#define INTERSAVE_H

#include <fvar.hpp>
#include <adstring.hpp>
#include <cifstrem.h>

namespace MM_intersave
{
  enum intersave_modes
  {
    NONE = 0,
    MM_mode,
    ADMB_mode
  };
}

typedef struct
{
  double * buf;               /* current position in buffer */
  int      savefile;          /* file handle */
  double * head;              /* first position in buffer */
  double * tail;              /* end of buffer */
  char     savefilename[30];  /* name of file */
  int      segments;          /* number of writes to file */
  unsigned long nb_write;          /* number of bytes written */
  unsigned long nb_read;           /* number of bytes read */
} intersavetype;

class setsaveinterflag
{
public:
//All defined in intersav.cpp
void setsaveinter(int);		//called from tagmove, tagest main functions
void getbuf(intersavetype * );	//called from intersav
void savebuf(intersavetype * );	//called from intersav
int iclose(intersavetype *);	//called from fgcomp
void msavep(dmatrix& , const char *, intersavetype * );//called from adi
void msavep(dvar_matrix& , const char *, intersavetype * ) {}
void mgetp(dmatrix& , const char *, intersavetype * ); //called from dfadi
void vsavep(dvector& , const char *, intersavetype *); //called from adi
void vgetp(dvector& , const char *, intersavetype *);  //called from dfadi
void vsavep(dvar_vector& , const char *, intersavetype *) {/*NOT called*/};
void vgetp(dvar_vector& , const char *, intersavetype *) {/*NOT called*/};
void isp_reset();	//called from fgcomp
};

#endif //#ifndef INTERSAVE_H

