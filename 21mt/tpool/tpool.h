/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * tpool.h --
 *
 * Structures for thread pool
 *
 * Implemented as C++ class by John Sibert
 */

#ifndef __TOOOL_H__
#define __TOOOL_H__
#include <pthread.h>
#include <semaphore.h>

typedef struct tpool_work {
	void               (*routine)(void *);
	void                *arg;
	struct tpool_work   *next;
} tpool_work_t;

class tpool
{
  private:
	/* pool characteristics */
	int                 num_threads;
        int                 max_queue_size;
        int                 do_not_block_when_full;
        /* pool state */
	pthread_t           *threads;
        int                 cur_queue_size;
	tpool_work_t        *queue_head;
	tpool_work_t        *queue_tail;
	int                 queue_closed;
        int                 shutdown;
	/* pool synchronization */
        pthread_mutex_t     queue_lock;
        pthread_cond_t      queue_not_empty;
        pthread_cond_t      queue_not_full;
	pthread_cond_t      queue_empty;

  public:
    tpool(void);
   ~tpool(void);
    void allocate(const int num_threads, const int max_queue_size, 
                  const int do_not_block_when_full); //tpool_init

     int tpool_add_work( void (*routine)(void *), void *arg);

     int tpool_destroy(const int finish);
     static void* s_tpool_thread(void *arg);
     void* tpool_thread(void *arg);

}; //class tpool

// from thread_extensions.c
int SEM_INIT(sem_t *sem, int pshared, unsigned int value);
void SEM_WAIT(sem_t *arg);
void thread_single_barrier(sem_t *barrier, int count);

#endif
