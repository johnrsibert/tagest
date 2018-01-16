
/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * tpool.c -- 
 * 
 * Example caller for thread pooling library
 */

#include "tpool.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

char *s1[20]={  "STRING 0",
		"STRING 1",
                "STRING 2",
                "STRING 3",
                "STRING 4",
                "STRING 5",
                "STRING 6",
                "STRING 7",
                "STRING 8",
                "STRING 9",
		"STRING 10",
                "STRING 11",
                "STRING 12",
                "STRING 13",
                "STRING 14",
                "STRING 15",
                "STRING 16",
                "STRING 17",
                "STRING 18",
                "STRING 19"};

void r1(void * printstring)
{
   int i, x;

   printf("%s START\n", (char *)printstring);
   /*
   for (i = 0; i < 1000000; i++)  {
	x = x +i;
   }
   */
   sleep(5);

   printf("%s DONE\n", (char *)printstring);
}

main(void)
{
   //extern char *s1[];
   extern char *s1[20];

   pthread_t t1,t2;
   int i;  

   /*
   tpool_t test_pool;
   tpool_init(&test_pool, 10, 20, 0);
   */
  
   int nt = 20;
   tpool test_pool;
   test_pool.allocate( nt, 4, 0);

   //sleep(5);

   for ( i = 0; i < nt; i++) {
	   printf("tpool_add_work returned %d\n",
		test_pool.tpool_add_work(r1, s1[i]));

   }

   printf("main: all work queued\n"); 

   //tpool_destroy(test_pool, 1); 

   return 0;
   
}  
