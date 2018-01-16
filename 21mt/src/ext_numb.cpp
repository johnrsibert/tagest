#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma hdrstop
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#ifdef __TURBOC__
  #include <dir.h>
  #include <iostream>
#endif
#ifdef __ZTC__
  #include <dos.h>
  #include <iostreampp>
#endif
#ifdef __SUN__
  #include <dirent.h>
  #include <iostream>
  int findfirst(const char* mask, DIR** dirp, struct dirent** direntp);
  int findnext(const char* mask, DIR* dirp, struct dirent** direntp);
#endif
#ifdef __GNUC__
  #include <dirent.h>
  #include <iostream>
  int findfirst(const char* mask, DIR** dirp, struct dirent** direntp);
  int findnext(const char* mask, DIR* dirp, struct dirent** direntp);
#endif

#define HERE cout << "reached " << __LINE__ << " in " << __FILE__ << endl;


// returns the last used (ie highest) file extension number for a specified
// first letter in the file name extension
// valid for hex or decimal extension numbers

int last_ext_number(const char* root, const char letter)
{
  int file_num = 0;
  int test_num = 0;
  char mask[13];
  char name[16];
  int  number_pos;
 
#if defined(__SUN__) || defined(__GNUC__)
  sprintf(mask,"%s.%c\0",root, letter);
#else
  sprintf(mask,"%s.%c??\0",root, letter);
#endif
  int attrib = 0;
#ifdef __TURBOC__
  struct ffblk ffb;
  int find_result = findfirst(mask, &ffb, attrib);
#endif
#ifdef __ZTC__
  struct FIND* ffb;
  attrib = 0;
  ffb = findfirst(mask, attrib);
  int find_result = (ffb == NULL) ? -1 : 0;
#endif
#if defined(__SUN__) || defined(__GNUC__)
  DIR* dirp = NULL;
  struct dirent* ffb = NULL;
  int find_result = findfirst(mask, &dirp, &ffb);
#endif

  if (find_result < 0)
  {
    file_num = -1;  // return -1 if no numbered files found
  }
  else
  {
    do
    {
    #ifdef __TURBOC__
      strcpy(name, ffb.ff_name);
    #endif
    #ifdef __ZTC__
      strcpy(name, ffb->name);
    #endif
    #if defined(__SUN__) || defined(__GNUC__)
      strcpy(name, ffb->d_name);
    #endif
      number_pos = strlen(name) - 2;
      if ( isxdigit(name[number_pos]) &&
	      isxdigit(name[number_pos+1]) )
      {
      #if defined __TURBOC__ || __SUN__ || __GNUC__
	sscanf(&name[number_pos],"%2x",&test_num);
      #endif
      #ifdef __ZTC__
	sscanf(&name[number_pos],"%2x",&test_num);
      #endif
	if ((test_num <= 0xff) && (test_num > file_num) )
	  file_num = test_num;
      }
    #ifdef __TURBOC__
      find_result = findnext(&ffb);
    #endif
    #ifdef __ZTC__
      ffb = findnext();
      find_result = (ffb == NULL) ? -1 : 0;
    #endif
    #if defined(__SUN__) || defined(__GNUC__)
      find_result = findnext(mask, dirp, &ffb);
    #endif
      //cout << "findnext returned " << -find_result << endl;
    } while (find_result == 0);
  }
  return file_num;
}

void make_ext(const char c, const int n, char * ext)
{
  if ((n < 0) || (n > 0xff))
  {
    cerr << "** File extensions numbers must be in the range 0 .. 0xff\n";
    cerr << "   make_ext was passed " << std::hex << n << endl;
    exit(1);
  }
  sprintf(ext, ".%c%02x", c , n);
}

#if defined(__SUN__) || defined(__GNUC__)
int findfirst(const char* mask, DIR** dirp, struct dirent** _direntp)
{
  struct dirent* direntp = *_direntp;
  *dirp = opendir(".");
  if (!dirp)
  {
    perror("opendir() failed");
    //exit(1);
    return(-1);
  }
  do
  {
    direntp = readdir(*dirp);
    if (direntp && (strstr(direntp->d_name, mask)) )
    {
       *_direntp = direntp;
       return(0);
    }
  } while  (direntp);
  *_direntp = direntp;

  closedir(*dirp);
  return(-1);
  /*
  while  (  (direntp = readdir(dirp))  && 
           !(strstr(direntp->d_name, mask)) );

  if (direntp)
    return(0);
  else
  {
    closedir(dirp);
    return(-1);
  }
  */
}

int findnext(const char* mask, DIR* dirp, struct dirent** _direntp)
{
  struct dirent* direntp = *_direntp;
  do
  {
    direntp = readdir(dirp);
    if (direntp && (strstr(direntp->d_name, mask)) )
    {
      *_direntp = direntp;
       return(0);
    }
  } while  (direntp);

  *_direntp = direntp;
  closedir(dirp);
  return(-1);
}
#endif
