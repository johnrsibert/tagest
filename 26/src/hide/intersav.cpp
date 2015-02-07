//$Id: intersav.cpp 2912 2012-01-09 22:48:14Z johnoel $
#include "trace.h"
#if defined(__MSVC32__) || defined (_MSC_VER)
  #define lseek _lseek
  #define  read _read
  #define write _write
  #define open _open 
  #include <sys\stat.h>
  #include <fcntl.h>
  #include <io.h>
#endif

#ifdef __SUN__
  #include <fcntl.h>
  #include <unistd.h>
  long tell(int f)
  {
    return lseek(f, 0L, SEEK_CUR);
  }
  long filelength(int handle);
#endif

#ifdef __GNUC__
 //#include <iostream>
  #include <fcntl.h>
  #include <sys/stat.h>
  #include <sys/types.h>
  #include <unistd.h>
  long tell(int f)
  {
    return lseek(f, 0L, SEEK_CUR);
  }
  long filelength(int handle);
#endif

#if defined(__BORLANDC__)
  #include <fcntl.h>
  #include <sys\stat.h>
  #include <io.h>
#endif

#ifdef __ZTC__
  #include <fcntl.h>
  #include <sys\stat.h>
  #include <io.h>
  #if (__ZTC__ <= 0x310)
    long tell(int f)
    {
      return lseek(f, 0L, SEEK_CUR);
    }
  #endif
#endif


#ifdef __NDPX__
  #include <stdio.h>
  //extern  int _pmode;		/* set to 0x8000 for binary */
  //#define O_TEXT 0x4000		/* input parameter for setmode */
  //#define O_BINARY 0x8000		/* input parameter for setmode */
#endif

#include "intersav.h"
#include <iostream>
using std::hex;
using std::dec;
//#define HERE clogf << "reached line " << dec << __LINE__ << " in " << __FILE__ << endl;

#ifdef __NDPX__
#define tell(P) lseek((P), 0L, SEEK_CUR)
// because their tell doesnt seem to work?
#endif

#ifdef __TURBOC__
  #include <iomanip.h>
//  #define SAVE_BUF_SIZE 0x1000  /* problems occur if you make it 0x1000L */
#endif

long save_buf_size = 0x40000L;
/*#ifdef DOS386
  #ifdef __NDPX__
    #define SAVE_BUF_SIZE 0x70000L
  #endif
  #ifdef __GNUC__
    #define SAVE_BUF_SIZE 0x100000L
  #endif
  #ifdef __ZTC__
    #define SAVE_BUF_SIZE 0x70000L
  #endif
  #ifdef __SUN__
    #define SAVE_BUF_SIZE 0x100000L
  #endif
#endif */

intersavetype * isp = NULL;

extern ofstream clogf;

long int number_of_elements(dmatrix& x);

void setsaveinterflag::savebuf(intersavetype * isp)
{
  //HERE
  unsigned int  unbw;
  int           inbw;

  /* save current position in buffer */
  //clogf << "saving isp->buf " << isp->buf
	//<< " at " << tell(isp->savefile) << endl;
  //HERE
#ifndef __NDPX__
  inbw =  write(isp->savefile, &(isp->buf), sizeof(double*));
#else
  inbw =  write(isp->savefile, (char*)&(isp->buf), sizeof(double*));
#endif
  if (inbw != sizeof(double*))
  {
    //HERE
    perror("Error occured saving current value of buffer pointer in savebuf");
    fprintf(stderr, "Attempted to write %lu bytes; actually wrote %i.\n",
      sizeof(double), inbw);
    exit(1);
  }
  isp->nb_write += inbw;

  /* write contents of buffer */
  clogf << "saving isp->head at " << tell(isp->savefile) << endl;
/*  if ( write(isp->savefile, isp->head, SAVE_BUF_SIZE*sizeof(double)) == -1) */

#ifndef __NDPX__
  inbw = write(isp->savefile, isp->head, save_buf_size*sizeof(double));
#else
  inbw = write(isp->savefile, (char*)isp->head, save_buf_size*sizeof(double));
#endif
  if (inbw < 0 )
  {
    unbw = 0xffff+inbw;
    unbw++;
  }
  else unbw = inbw;

  if (unbw != save_buf_size*sizeof(double) )
  {
    perror("Error occured saving buffer in savebuf");
    fprintf(stderr, "Attempted to write %lu bytes; actually wrote %u (%i).\n",
      save_buf_size*sizeof(double), unbw, inbw);
    exit(1);
  }
  else
  {
    clogf << "OK. Wrote " << unbw << " bytes" << endl;
  }

  isp->nb_write += unbw;
#ifdef __NDPX__
  if (lseek(isp->savefile, isp->nb_write, SEEK_SET) < 0)
  {
    perror("Error occured repositioning file pointer after write in savebuf()");
    exit(1);
  }
#endif
  //clogf << "after repositioning in savebuf(), file pointer is "
	//<< tell(isp->savefile) << endl;

  /* reset curret position to first position */

  //HERE
  isp->buf = isp->head;

  isp->segments++;

/*  fprintf(clogf,"Buffer saved in savebuf.\n"); */
}

void setsaveinterflag::getbuf(intersavetype * isp)
{
  double *      ispbuf;
  long int      seekl, newpos;//, headpos;
  int           inbr;
  unsigned      unbr;

  seekl = - ( (long int)save_buf_size*(long int)sizeof(double) +
	      (long int)sizeof(double*) );
/*  fprintf(clogf,"seekl = %lX\n", seekl); */
  //clogf << "getbuf: seekl = " << seekl << endl;
  if (isp->segments > 0)
  {
    if (isp->segments == 1)
    {
/*      fprintf(clogf,"moved file pointer from %lX ", tell(isp->savefile)); */
      //clogf << "moved file pointer from " << tell(isp->savefile);
      newpos = lseek(isp->savefile, 0L, SEEK_SET);
      if (newpos < 0L)
      {
	perror("Error occured positioning file pointer to file beginning in getbuf");
	exit(1);
      }
/*      fprintf(clogf," to %lX\n", tell(isp->savefile)); */
      //clogf <<" to " << tell(isp->savefile) << endl;
    }
    else
    {
      /* basckspace one buffer + one pointer */
/*      fprintf(clogf,"moved file pointer from %lX ", tell(isp->savefile)); */
      //clogf << "moved file pointer from " << tell(isp->savefile);
      newpos = lseek(isp->savefile, seekl, SEEK_CUR);
      if (newpos < 0)
      {
	perror("Error occured positioning file pointer in getbuf");
	exit(1);
      }
/*      fprintf(clogf," to %lX\n", tell(isp->savefile)); */
      //clogf <<" to " << tell(isp->savefile) << endl;
    }
    isp->segments--;
  }

  /* get previous value for current position */
/*  fprintf(clogf,"reading  ispbuf at %lX; ", tell(isp->savefile)); */
  //clogf << "reading  ispbuf at " << tell(isp->savefile) << endl;
#ifndef __NDPX__
  inbr = read(isp->savefile, &ispbuf, sizeof(double*));
#else
  inbr = read(isp->savefile, (char*)&ispbuf, sizeof(double*));
#endif
  if (inbr != sizeof(double*))
  {
    perror("Error occured reading value buffer pointer in getbuf");
    cerr << "Trying to read " << (sizeof(double*)) << " bytes; read "
	 << inbr << endl;

    exit(1);
  }
  //isp->nb_read += sizeof(double*);
  isp->nb_read += inbr;

#ifdef __NDPX__
  headpos = newpos + inbr;
  if (lseek(isp->savefile, headpos, SEEK_SET) < 0)
  {
    perror("Error occured repositioning file pointer before head read in getbuf()");
    exit(1);
  }
#endif

  /* read in buffer */
/*  fprintf(clogf,"reading isp->head at %i\n", tell(isp->savefile)); */
  //clogf << "reading isp->head at " << tell(isp->savefile) << endl;

#ifndef __NDPX__
  inbr = read(isp->savefile, isp->head, save_buf_size*sizeof(double));
#else
  //_pmode = O_BINARY;
  inbr = read(isp->savefile, (char*)isp->head, save_buf_size*sizeof(double));
#endif
  if (inbr < 0 )
  {
    unbr = 0xffff+inbr;
    unbr++;
  }
  else unbr = inbr;

  if (unbr != save_buf_size*sizeof(double) )
  {
    perror("Error occured reading buffer in getbuf");
    cerr << "Attempting to read " <<  save_buf_size*sizeof(double)
	 << " bytes; read " << unbr << endl;
    exit(1);
  }
  //clogf << "after reading isp->head, file pointer is " << tell(isp->savefile) << endl;
  isp->nb_read += unbr;

  /* restore current position */
  //HERE
  isp->buf = ispbuf;
  //clogf << "before post-read repositioning in getbuf(), file pointer is "
	//<< tell(isp->savefile) << endl;
#ifdef __NDPX__
  if (lseek(isp->savefile, newpos, SEEK_SET) < 0)
#else
  if (lseek(isp->savefile, seekl, SEEK_CUR) < 0)
#endif
  {
    perror("Error occured repositioning file pointer after read in getbuf()");
    exit(1);
  }
  //clogf << "after post-read repositioning in getbuf(), file pointer is "
	//<< tell(isp->savefile) << endl;

}

int setsaveinterflag::iclose(intersavetype * isp)
{
  //HERE
  //char          eof = ' '; /* ^Z */
  //long          fl;
  //clogf << "iclose start" << endl;

  /* --------- NOT HERE -------------
  if (isp->nb_write > 0)
  {
    clogf << "Calling savebuf in iclose" << endl;
    savebuf(isp);
    clogf << "Buffer saved in iclose" << endl;
    clogf << "File position: " << tell(isp->savefile) << endl;
  }
  */

  //write(isp->savefile, &eof, 1);

  //fl = filelength(isp->savefile);

  //clogf << "  Size of intermediate file: " << fl << " bytes.\n";
  //clogf << "  Total bytes written = " << isp->nb_write << endl;
  //clogf << "  Total bytes read    = " << isp->nb_read << endl;

  //close(isp->savefile);
  //clogf << "Intermediate file " << isp->savefilename<< " closed.\n";
#ifdef __NDPX__
  if (lseek(isp->savefile, isp->nb_write, SEEK_SET) < 0)
  {
    perror("Error occured repositioning file pointer in iclose");
    exit(1);
  }
#endif
  //clogf << "after repositioning in iclose(), file pointer is "
	//<< tell(isp->savefile) << endl;


  //clogf << "iclose finished" << endl;
  return(0);
}

void setsaveinterflag::setsaveinter(int flag)
{
  extern int saveinterflag;
  saveinterflag = flag;

  if (flag == 1)
  {
    //Set search string to current directory
    adstring pathtosearch(1,40);
    pathtosearch = "movemod.cfg";
    //Check if config file exist
    if ( (access(pathtosearch,0))==-1 )
    {
      //No config file in current directory, so get path string
      pathtosearch = "";
      char* path = getenv("PATH");
      //Search entire path string until null char
      for (int i=0; path[i] != '\0'; i++ )
      {
	//Do subsearch for substring ending with a ;(DOS) or :(SUN)
      //#ifdef __SUN__
      #ifdef unix
	for (; (path[i] != ':') ; i++)
      #else
	for (; (path[i] != ';') ; i++)
      #endif
	{
	  //double check for end of path string
	  if (path[i] == '\0')
	  {
	    i--;
	    break;
	  }
	  //Store each substring char in the path to search string var
	  pathtosearch += path[i];
	}
	//Append appropriate config file name
	#ifdef unix
	  pathtosearch += "/movemod.cfg";
	#else
	  pathtosearch += "\\movemod.cfg";
	#endif
	//clogf << "*** searching path: " << pathtosearch << endl;
	//cout << "*** searching path: " << pathtosearch << endl;

	//Check if a config file exist in this directory
	if ((access(pathtosearch,0))==0)
	  break;
	pathtosearch = "";
      }//end for

    }//end if
    /*
    //Try to see if a config file was found by trying to open it
    cifstream params(pathtosearch);
    if (!params)
    {
      cerr << " Couldn't find a configure file" << endl;
      exit(1);
    }
    clogf << "Opened config file " << pathtosearch << endl;
    cout << "Opened config file " << pathtosearch << endl;
    //Get the size of the buffer from the contents of config file
    params >> save_buf_size;
    */


    cifstream cfg(pathtosearch);
    if (cfg)
    {
      clogf << "Opened config file " << pathtosearch << endl;
      cout << "Opened config file " << pathtosearch << endl;
    //Get the size of the buffer from the contents of config file
      cfg >> save_buf_size;

    }
    else
    {
      cout << " Couldn't find a configuration file; "
           << "buffer size set to default." << endl;
      clogf << " Couldn't find a configuration file; "
           << "buffer size set to default." << endl;
      save_buf_size = 1000000;
    }


    clogf << "Save buf size : " << hex << save_buf_size << dec << endl;
    if ( (isp = (intersavetype *)malloc( sizeof(intersavetype) ) )== NULL)
    {
      cerr << "Cannot allocate intersave structure.\n";
      exit(1);
    }
    printf("isp (pointer) = %p\n",isp);

  #if defined(_Windows) || defined(_WIN32)
    char* tmp_path = NULL;
  #else
    char* tmp_path = getenv("TMP");
  #endif
    if (tmp_path != NULL)
    {
      strcpy(isp->savefilename, tmp_path);
    #ifdef unix
      strcat(isp->savefilename, "/parsave.tmp");
    #else
      strcat(isp->savefilename, "\\parsave.tmp");
    #endif
    }
    else
    {
      strcpy(isp->savefilename, "parsave.tmp");
    }

    if ( (isp->buf = (double*)malloc(save_buf_size*sizeof(double)) )== NULL)
    {
      cerr << "Cannot allocate buf pointer." << " " << save_buf_size <<endl;
      exit(1);
    }
    printf("isp->buf (pointer) = %p; %ld bytes\n",
	   isp->buf, (long)(save_buf_size)*sizeof(double));

    isp->head    = isp->buf;
    isp->tail    = isp->buf + save_buf_size;

    isp->segments = 0;

    isp->nb_write = 0;
    isp->nb_read  = 0;

   clogf << "Intersave structure successfully initialized." << endl;
   clogf << "  buffer size = "  << hex << save_buf_size*sizeof(double)
	 << " bytes" << endl;
   clogf << "  buf = " << hex << isp->buf << endl;
   clogf << "  head = " << hex << isp->head << endl;
   clogf << "  tail = " << hex << isp->tail << dec << endl;
   clogf << "  intermediate data file name: " << isp->savefilename << endl;
  }

  else if (flag == 0)
  {
    if (isp != NULL)
    {
      isp->buf = isp->head;
      free(isp->buf);
      isp->buf = NULL;
    
      if (isp->savefilename)
      {
        // reopen file to get statistics and delete

      #if defined ( __SUN__) ||  defined ( __GNUC__)
        isp->savefile = open(isp->savefilename, O_RDONLY);
      #else
        isp->savefile = open(isp->savefilename, 
                               O_RDWR | O_CREAT | O_BINARY, S_IREAD | S_IWRITE);
      #endif
  
        long isp_length = filelength(isp->savefile);
        clogf << "Temporary file, " << isp->savefilename << ", lengh is "
              << isp_length << " bytes." << endl;
        int removed = remove(isp->savefilename);
        if (removed == 0)
          clogf << "File " << isp->savefilename << " was removed " << endl;
        else
          clogf << "File " << isp->savefilename << " was NOT removed " << endl;
      }  

      free(isp);
      isp = NULL;
    } 
  }

  else if (flag != MM_intersave::ADMB_mode)
  {
    fprintf(stderr,"Unknown flag (%i) in setintersaveflag.\n", flag);
    exit(1);
  }
  printf("returning from setintersaveflag.\n");
}


void makename(const char *name, char *padded)
{
  int   i;
  strncpy(padded, name, 8);
  i = strlen(name);
  while (i < 8)
  {
    padded[i] = ' ';
    i++;
  }
  padded[8]='\0';
}

void setsaveinterflag::msavep(dmatrix& x, const char *name, intersavetype * isp)
{
  char        padded[9];
  double *    ispbuf;

  //clogf << "Saving in " << name <<" in msavep " << endl;

  int offset = 1 + number_of_elements(x); //1 + (imax-i1+1)*(jmax-j1+1) ;
  if (isp->buf + offset > isp->tail)
  {
    clogf << "Writing " << name <<" in msavep " << endl;
    clogf << "  isp->buf = " << isp->buf << endl;
    clogf << "  offset = " << offset << endl;
    clogf << "  isp->tail = " << isp->tail << endl;
    //HERE
    savebuf(isp);
  }

  makename(name, padded);

  strncpy((char*)isp->buf++,padded, 8);

  //HERE
  ispbuf = isp->buf;
  int i1 = x.rowmin();
  int imax = x.rowmax();

  for (int i=i1; i<=imax; i++)
  {
     int j1 = x(i).indexmin();
     int jmax = x(i).indexmax();
     for (int j=j1; j<=jmax; j++)
     {
       *(ispbuf++) = x[i][j];
     }
  }

  //HERE
  isp->buf = ispbuf;

//  clogf <<  "msavep '" << padded <<"'" << endl;

} /* end of msavep */

void setsaveinterflag::mgetp(dmatrix& x, const char *name, intersavetype * isp)
{
  char        test[9], padded[9];
  double *    ispbuf;
  //clogf << "Restoring in " << name <<" in mgetp " << endl;

  int offset = 1 + number_of_elements(x); //(imax-i1+1)*(jmax-j1+1);
  //HERE

  if ( isp->buf == isp->head )
  {
    getbuf(isp);
  }

  //HERE
  isp->buf -= offset;

  /* use copy of ispbuf to preserve value for next access */

  //HERE
  ispbuf = isp->buf;

  makename(name, padded);
  strncpy(test, (char*)ispbuf++, 8);
  if (strncmp(test,padded,8) != 0)
  {
    //fprintf(stderr,"\nError in mgetp for matrix %s.\n", name);
    //fprintf(stderr,"Looking for %.8s; found %.8s.\n",padded,test);
    cerr << "\nError in mgetp for matrix " << name << endl;
    cerr << "Looking for '" << padded << "'; found '" << test << "'." << endl;
    lseek(isp->savefile,80L,SEEK_CUR);
    char t[161];
    read(isp->savefile, t, 160);
    cerr << "Previous 80 bytes:" << endl;
    for (int i = 0; i<80; i++)
      cerr << t[i];
    cerr << endl;
    cerr << "Following 80 bytes:" << endl;
    for (int i = 80; i<160; i++)
      cerr << t[i];
    cerr << endl;
    exit(1);
  }

  int i1 = x.rowmin();
  int imax = x.rowmax();
  for (int i=i1; i<=imax; i++)
  {
    int j1 = x(i).indexmin();
    int jmax = x(i).indexmax();
    for (int j=j1; j<=jmax; j++)
    {
      x[i][j] = *(ispbuf++);
    }
  }

//  clogf <<  "mgetp '" << padded <<"'" << endl;

} /* end of mgetp */

void setsaveinterflag::vsavep(dvector& x, const char *name, intersavetype * isp)
{
  char          padded[9];
  double *      ispbuf;

  //clogf << "Saving in " << name <<" in vsavep " << endl;
  int j1 = x.indexmin();
  int jmax = x.indexmax();
  int offset = 1 + (jmax-j1+1);
  if (isp->buf + offset > isp->tail)
  {
    clogf << "Writing in " << name <<" in vsavep " << endl;
    clogf << "  isp->buf = " << isp->buf << endl;
    clogf << "  offset = " << offset << endl;
    clogf << "  isp->tail = " << isp->tail << endl;
    //HERE
    savebuf(isp);
  }

  makename(name, padded);
  strncpy((char*)isp->buf++,padded, 8);

  //HERE
  ispbuf = isp->buf;

  for (int j=j1; j<=jmax; j++)
  {
    *(ispbuf++) =x[j];
  }

  //HERE
  isp->buf = ispbuf;

//  clogf <<  "vsavep '" << padded <<"'" << endl;
} /* end of vsavep */

void setsaveinterflag::vgetp(dvector& x, const char *name, intersavetype *isp)
{
  char        test[9], padded[9];
  double      * ispbuf;
  //clogf << "Restoring in " << name <<" in vgetp " << endl;

  int j1 = x.indexmin();
  int jmax = x.indexmax();
  int offset = 1 + (jmax-j1+1);

  if (isp->buf == isp->head)
  {
    getbuf(isp);
  }

  //HERE
  isp->buf -= offset;

  /* use copy of pointer for speed and to preserve isp->buf for next use */
  ispbuf = isp->buf;

  makename(name, padded);

  strncpy(test, (char*)ispbuf++, 8);
  if (strncmp(test,padded,8) != 0)
  {
    fprintf(stderr,"\nError in vgetp for vector %s.\n", name);
    fprintf(stderr,"Looking for %.8s; found %.8s.\n",padded,test);
    exit(1);
  }

  for (int j=j1; j <= jmax; j++)
  {
    x[j] = *(ispbuf++);
  }

//  clogf <<  "vgetp '" << padded <<"'" << endl;
} /* end of vgetp */

void setsaveinterflag::isp_reset()
{
  extern int saveinterflag;

  if (saveinterflag == 1)
  {
    //clogf << "isp_reset" << endl;
    isp->buf  = isp->head;
    isp->tail = isp->buf + save_buf_size;

    isp->segments = 0;

    isp->nb_write = 0;
    isp->nb_read  = 0;
    //clogf << "moved file pointer from " << tell(isp->savefile);
      //lseek(isp->savefile,0L,SEEK_CUR) << endl;
    lseek(isp->savefile,0L,SEEK_SET);
    //clogf <<" to " << tell(isp->savefile) << endl;
      //lseek(isp->savefile,0L,SEEK_CUR) << endl;

  }
}

long int number_of_elements(dmatrix& x)
{
  long int n = 0;
  int i1 = x.rowmin();
  int i2 = x.rowmax();
  for (int i = i1; i <= i2; i++)
  {
    n += x(i).size();
  }
  return(n);

}

#ifndef _WIN32
  long filelength(int filedes)
  {
    long length = lseek(filedes, 0L, SEEK_END);
    return(length);
  }
#endif
