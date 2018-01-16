#ifndef __thread_pool_h__
#define __thread_pool_h__

/*******************************************************
 *  C++ version of tpool example from the book
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad Nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
*/

//#include <iostream>

//#include <stdlib.h>
//#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
//#include <string.h>

#include <pthread.h>

typedef void* (*worker_fn_t)(void*);
typedef void (*key_destructor_fn_t)(int*);

typedef struct tpool_work
{
  worker_fn_t		routine;
  void*			arg;
  int*			thread_id;
  struct tpool_work* 	next;
} tpool_work_t;

class thread_pool
{
private:
  sigset_t            mask;
  int                 num_threads;
  int                 max_queue_size;
  int                 do_not_block_when_full;
  // pool state
  pthread_t           *threads;
  int                 cur_queue_size;
  tpool_work_t        *queue_head;
  tpool_work_t        *queue_tail;
  tpool_work_t        *start_args;
  int                 queue_closed;
  int                 shutdown;
  int                 join_flag;
  int                 finished_flag;

  // pool synchronization
  pthread_mutex_t     queue_lock;
  pthread_cond_t      queue_not_empty;
  pthread_cond_t      queue_not_full;
  pthread_cond_t      queue_empty;
  pthread_cond_t      finished_condition;
  pthread_mutex_t     finished_lock;

  // thread work manager
  void *tpool_thread(void *);
  static void* s_tpool_thread(void *);

  // thread identification
  pthread_key_t		thread_id_key;
  void			thread_id_destroy(void* t);
  static void		s_thread_id_destroy(void* t);
  int*			thread_id;

public:
  thread_pool(void);
  ~thread_pool(void);

  void allocate(const int num_threads, const int max_queue_size,
                const int do_not_block_when_full=0); // a.k.a. void tpool_init(

  int tpool_add_work( worker_fn_t routine, void* arg, void* client=NULL);

  int join(const int join_code = 0);
//  pthread_key_t* get_thread_id_key(void) { return(&thread_id_key); }
  int get_thread_id(void);
  void set_finished(void);

};

#endif //ifndef __thread_pool_h__

