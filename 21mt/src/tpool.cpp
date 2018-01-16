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
 * Example thread pooling library
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

#include <iostream>
using std::cerr;
using std::endl;

#include <fstream>
extern ofstream clogf;

//#include <pthread.h>
#include "tpool.h"

//void *tpool_thread(void *);
tpool::tpool(void)
{

}

void tpool::allocate(const int _num_threads, const int _max_queue_size, 
                     const int _do_not_block_when_full) //tpool_init

{
  int i, rtn;

  /* initialize th fields */
  num_threads = _num_threads;
  max_queue_size = _max_queue_size;
  do_not_block_when_full = _do_not_block_when_full;
  threads = new pthread_t[num_threads];
  /*
  if ((threads = 
       (pthread_t *)malloc(sizeof(pthread_t)*num_worker_threads)) 
      == NULL)
    perror("malloc"), exit(1);
  */
  if (!threads)
  {
    cerr << "error allocating memory for thread pool at " 
         << __FILE__ << "(" << __LINE__ << ")" << endl;
    exit(1);
  }
  clogf << "allocated thread pool with " << num_threads << " threads" 
        << " and " << max_queue_size <<" item queue" << endl;

  cur_queue_size = 0;
  queue_head = NULL; 
  queue_tail = NULL;
  queue_closed = 0;  
  shutdown = 0; 
  if ((rtn = pthread_mutex_init(&(queue_lock), NULL)) != 0)
    fprintf(stderr,"pthread_mutex_init %s",strerror(rtn)), exit(1);
  if ((rtn = pthread_cond_init(&(queue_not_empty), NULL)) != 0)
    fprintf(stderr,"pthread_cond_init %s",strerror(rtn)), exit(1);
  if ((rtn = pthread_cond_init(&(queue_not_full), NULL)) != 0)
    fprintf(stderr,"pthread_cond_init %s",strerror(rtn)), exit(1);
  if ((rtn = pthread_cond_init(&(queue_empty), NULL)) != 0)
    fprintf(stderr,"pthread_cond_init %s",strerror(rtn)), exit(1);

  /* create threads */
  for (i = 0; i != num_threads; i++) {
    if ((rtn = pthread_create( &(threads[i]),
			      NULL,
			      s_tpool_thread,
			      (void *)this)) != 0)
      fprintf(stderr,"pthread_create %d",rtn), exit(1);
  }
}

int tpool::tpool_add_work(void (*routine)(void *), void *arg)
{
  int rtn;
  tpool_work_t *workp;

  if ((rtn = pthread_mutex_lock(&(queue_lock))) != 0)
    fprintf(stderr,"pthread_mutex_lock %d",rtn), exit(1);

  /* no space and this caller doesn't want to wait */
  if ((cur_queue_size == max_queue_size) &&
      do_not_block_when_full) {
    if ((rtn = pthread_mutex_unlock(&(queue_lock))) != 0)
      fprintf(stderr,"pthread_mutex_unlock %d",rtn), exit(1);

    return -1;
  }

  while( (cur_queue_size == max_queue_size) &&
	(!(shutdown || queue_closed))  ) {

    if ((rtn = pthread_cond_wait(&(queue_not_full),
				 &(queue_lock))) != 0)
      fprintf(stderr,"pthread_cond_wait %d",rtn), exit(1);

  }

  /* the pool is in the process of being destroyed */
  if (shutdown || queue_closed) {
    if ((rtn = pthread_mutex_unlock(&(queue_lock))) != 0)
      fprintf(stderr,"pthread_mutex_unlock %d",rtn), exit(1);
 
    return -1;
  }


  /* allocate work structure */
  //if ((workp = (tpool_work_t *)malloc(sizeof(tpool_work_t))) == NULL)
  //  perror("malloc"), exit(1);
  workp = new tpool_work_t;
  if (!workp)
  {
    cerr << "error allocating memory for work tsructure pool at " 
         << __FILE__ << "(" << __LINE__ << ")" << endl;
    exit(1);
  }

  workp->routine = routine;
  workp->arg = arg;
  workp->next = NULL;

  //printf("adder: adding an item %d\n", workp->routine);

  if (cur_queue_size == 0) {
    queue_tail = queue_head = workp;

     //printf("adder: queue == 0, waking all workers\n");

    if ((rtn = pthread_cond_broadcast(&(queue_not_empty))) != 0)
      fprintf(stderr,"pthread_cond_signal %d",rtn), exit(1);;
  } else {
    queue_tail->next = workp;
    queue_tail = workp;
  }

  cur_queue_size++; 
  if ((rtn = pthread_mutex_unlock(&(queue_lock))) != 0)
    fprintf(stderr,"pthread_mutex_unlock %d",rtn), exit(1);
  return 1;
}

tpool::~tpool()
{
  tpool_destroy(1);
}

int tpool::tpool_destroy(const int finish)
{
  int          i,rtn;
  tpool_work_t *cur_nodep=NULL;
 

  if ((rtn = pthread_mutex_lock(&(queue_lock))) != 0)
    fprintf(stderr,"pthread_mutex_lock %d",rtn), exit(1);

  /* Is a shutdown already in progress? */
  if (queue_closed || shutdown) {
    if ((rtn = pthread_mutex_unlock(&(queue_lock))) != 0)
      fprintf(stderr,"pthread_mutex_unlock %d",rtn), exit(1);
    return 0;
  }

  queue_closed = 1;

  /* If the finish flag is set, wait for workers to 
     drain queue */ 
  if (finish == 1) {
    while (cur_queue_size != 0) {
      if ((rtn = pthread_cond_wait(&(queue_empty),
				   &(queue_lock))) != 0)
	fprintf(stderr,"pthread_cond_wait %d",rtn), exit(1);
    }
  }

  shutdown = 1;

  if ((rtn = pthread_mutex_unlock(&(queue_lock))) != 0)
    fprintf(stderr,"pthread_mutex_unlock %d",rtn), exit(1);

  if (!threads)
  {
    cerr << "error allocating memory for thread pool at " 
         << __FILE__ << "(" << __LINE__ << ")" << endl;
    exit(1);
  }


  /* Wake up any workers so they recheck shutdown flag */
  if ((rtn = pthread_cond_broadcast(&(queue_not_empty))) != 0)
    fprintf(stderr,"pthread_cond_broadcast %d",rtn), exit(1);
  if ((rtn = pthread_cond_broadcast(&(queue_not_full))) != 0)
    fprintf(stderr,"pthread_cond_broadcast %d",rtn), exit(1);


  /* Wait for workers to exit */
  for(i=0; i < num_threads; i++) {
    if ((rtn = pthread_join(threads[i],NULL)) != 0)
      fprintf(stderr,"pthread_join %d",rtn), exit(1);
  }

  /* Now free pool structures */
  //free(threads);
  if (threads)
  {
    delete[] threads;
    threads = NULL;
  }
  while(queue_head != NULL) 
  {
    cur_nodep = queue_head->next; 
    queue_head = queue_head->next;
    //free(cur_nodep);
    if (cur_nodep)
    {
      delete cur_nodep;
      cur_nodep=NULL;
    }
  }
}

void* tpool::s_tpool_thread(void *arg)
{
  tpool* pool = (tpool*)arg;
  pool -> tpool_thread(arg);
}


void* tpool::tpool_thread(void *arg)
{
  //tpool_t tpool = (tpool_t)arg; 
  int rtn;
  tpool_work_t	*my_workp = NULL;
	
  for(;;) {

    /* Check queue for work */ 
    if ((rtn = pthread_mutex_lock(&(queue_lock))) != 0)
      fprintf(stderr,"pthread_mutex_lock %d",rtn), exit(1);

    while ((cur_queue_size == 0) && (!shutdown)) {


      //printf("worker %d: I'm sleeping again\n", pthread_self());

      if ((rtn = pthread_cond_wait(&(queue_not_empty),
				   &(queue_lock))) != 0)
	fprintf(stderr,"pthread_cond_wait %d",rtn), exit(1);

    }
    //sleep(5); 
 
    //printf("worker %d: I'm awake\n", pthread_self());

    /* Has a shutdown started while i was sleeping? */
    if (shutdown == 1) {

      if ((rtn = pthread_mutex_unlock(&(queue_lock))) != 0)
	fprintf(stderr,"pthread_mutex_unlock %d",rtn), exit(1);
      pthread_exit(NULL);
    }


    /* Get to work, dequeue the next item */ 
    my_workp = queue_head;
    cur_queue_size--;
    if (cur_queue_size == 0)
      queue_head = queue_tail = NULL;
    else
      queue_head = my_workp->next;
 
    //printf("worker %d: dequeing item %d\n", pthread_self(), my_workp->next);

    /* Handle waiting add_work threads */
    if ((!do_not_block_when_full) &&
	(cur_queue_size ==  (max_queue_size - 1))) 

      if ((rtn = pthread_cond_broadcast(&(queue_not_full))) != 0)
	fprintf(stderr,"pthread_cond_broadcast %d",rtn), exit(1);

    /* Handle waiting destroyer threads */
    if (cur_queue_size == 0)

      if ((rtn = pthread_cond_signal(&(queue_empty))) != 0)
	fprintf(stderr,"pthread_cond_signal %d",rtn), exit(1);

    if ((rtn = pthread_mutex_unlock(&(queue_lock))) != 0)
      fprintf(stderr,"pthread_mutex_unlock %d",rtn), exit(1);
      
    /* Do this work item */
    (*(my_workp->routine))(my_workp->arg);
    //free(my_workp);
    delete my_workp;
  } 
  return(NULL);            
}
