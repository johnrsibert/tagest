/*
        A set of useful functions and macros for pthreads programs.
*/

#include <pthread.h>
#include <signal.h>
#include <semaphore.h>
#include <unistd.h>


#ifndef _POSIX_C_SOURCE 
#define _POSIX_C_SOURCE 199506L
#endif

/* Some useful constants */
#ifndef NULL
# define NULL           ((void*)0)
#endif
#ifndef TRUE
# define TRUE           1
#endif
#ifndef FALSE
# define FALSE          0
#endif
#define DEBUG(arg)      {if (getenv("DEBUG")) arg;}
/* This NULL_TID may not work on all implementations! */
#define NULL_TID        (pthread_t) 0L


/*
   ================================================================
                        General List
   ================================================================
*/

typedef struct _list_t {
        void            *frst;
        struct _list_t  *next;
} list_t;

extern          list_t  *cons(void *, void *);
extern          int     empty(list_t *);
extern          void    *first(list_t *);
extern          list_t  *rest(list_t *);

/*
   ================================================================
                        Recursive Readers / Writer Lock
   ================================================================
*/


#define THREAD_RRWLOCK_INITIALIZER \
 {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, \
    PTHREAD_COND_INITIALIZER, 0, 0, 0, NULL_TID}

typedef struct {
  pthread_mutex_t       lock;           /* lock for structure           */
  pthread_cond_t        readers;        /* waiting readers              */
  pthread_cond_t        writers;        /* waiting writers              */
  int                   nreaders;       /* 0:free,>0:readers  */
  int                   waiters;        /* number of waiting writers    */
  int			nwriters;	/* number of recursive write locks */
  pthread_t		owner;		/* Owner when writer */
} thread_rrwlock_t;

typedef int thread_rrwlock_attr_t;       /* Not implemented */

extern  int     thread_rrwlock_init(thread_rrwlock_t *,
                                        thread_rrwlock_attr_t *);
extern  int     thread_rrwlock_destroy(thread_rrwlock_t *);
extern  int     thread_rrw_rdlock(thread_rrwlock_t *);
extern  int     thread_rrw_tryrdlock(thread_rrwlock_t *);
extern  int     thread_rrw_wrlock(thread_rrwlock_t *);
extern  int     thread_rrw_trywrlock(thread_rrwlock_t *);
extern  int     thread_rrw_unlock(thread_rrwlock_t *);


/*
   ================================================================
                        Readers / Writer Lock
   ================================================================
*/

/*
   Definitions for rw locks adapted from Sun's SPILT package.
   RWLocks are  not part of the POSIX threads standard, but
   they are part of UNIX98.
*/

#define THREAD_RWLOCK_INITIALIZER \
 {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, \
    PTHREAD_COND_INITIALIZER, 0, 0}

typedef struct {
  pthread_mutex_t       lock;           /* lock for structure           */
  pthread_cond_t        readers;        /* waiting readers              */
  pthread_cond_t        writers;        /* waiting writers              */
  int                   state;          /* -1:writer,0:free,>0:readers  */
  int                   waiters;        /* number of waiting writers    */
} thread_rwlock_t;

typedef int thread_rwlock_attr_t;       /* Not implemented */

extern  int     thread_rwlock_init(thread_rwlock_t *,
                                        thread_rwlock_attr_t *);
extern  int     thread_rwlock_destroy(thread_rwlock_t *);
extern  int     thread_rw_rdlock(thread_rwlock_t *);
extern  int     thread_rw_tryrdlock(thread_rwlock_t *);
extern  int     thread_rw_wrlock(thread_rwlock_t *);
extern  int     thread_rw_trywrlock(thread_rwlock_t *);
extern  int     thread_rw_unlock(thread_rwlock_t *);



/*
   ================================================================
                        Fifo Mutex Lock
   ================================================================
*/

typedef struct _fifo_mutex_queue_t {
  pthread_t                     tid;
  sem_t                         semaphore;
  struct _fifo_mutex_queue_t    *next;
} fifo_mutex_queue_t;


#define THREAD_FIFO_MUTEX_INITIALIZER \
 {PTHREAD_MUTEX_INITIALIZER, NULL, FALSE, NULL_TID}


typedef struct  {
  pthread_mutex_t       lock;           /* lock for structure           */
  fifo_mutex_queue_t    *waitersQueue;  /* queue of waiting writers     */
  fifo_mutex_queue_t    *waitersQueueLast; /* last of waiting writers   */
  int                   owned;          /* TRUE | FALSE | -1:awaiting   */
  pthread_t             owner;          /* tid of owner                 */
} thread_fifo_mutex_t;

extern  int     thread_fifo_mutex_init(thread_fifo_mutex_t *,
                                            pthread_mutexattr_t *);
extern  int     thread_fifo_mutex_destroy(thread_fifo_mutex_t *);
extern  int     thread_fifo_mutex_lock(thread_fifo_mutex_t *);
extern  int     thread_fifo_mutex_trylock(thread_fifo_mutex_t *);
extern  int     thread_fifo_mutex_unlock(thread_fifo_mutex_t *);


/*
   ================================================================
                        Debug Mutex Lock
   ================================================================
*/


#define PTHREAD_DMUTEX_INITIALIZER \
 {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, \
    NULL, NULL, 0, 0, FALSE, NULL_TID}

typedef struct _sleep_queue_t {
  pthread_t                     tid;
  struct _sleep_queue_t         *next;
} sleep_queue_t;


typedef struct _dmutex {
  pthread_mutex_t       lock;           /* lock for structure   */
  pthread_cond_t        cv;             /* waiting list control */
  char                  *name;          /* The declared name    */
  sleep_queue_t         *sleepers;      /* List of sleepers     */
  int                   n_locked;       /* Number of times locked */
  int                   n_failed;       /* Times woken w/o locking */
  int                   owned;          /* TRUE | FALSE         */
  pthread_t             owner;          /* NULL_TID or TID      */
} pthread_dmutex_t;

typedef struct _dmutex_queue_t {
  struct _dmutex                *mutex;
  struct _dmutex_queue_t        *next;
} dmutex_queue_t;

extern  int     pthread_dmutex_init(pthread_dmutex_t *, pthread_mutexattr_t *);
extern  int     pthread_dmutex_destroy(pthread_dmutex_t *);
extern  int     pthread_dmutex_lock(pthread_dmutex_t *);
extern  int     pthread_dmutex_trylock(pthread_dmutex_t *);
extern  int     pthread_dmutex_unlock(pthread_dmutex_t *);

#define pthread_dmutex_init(m, a) pthread_dmutex_init1(m, a, #m)

/*
   ================================================================
                        Recursive Mutex Lock
   ================================================================
*/

#define THREAD_RECURSIVE_MUTEX_INITIALIZER \
 {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, 0, FALSE, NULL_TID}


typedef struct  {
  pthread_mutex_t       lock;           /* lock for structure           */
  pthread_cond_t        cv;             /* waiting list control         */
  int                   count;          /* times locked recursively     */
  int                   owned;          /* TRUE | FALSE         */
  pthread_t             owner;          /* NULL_TID or TID      */
} thread_recursive_mutex_t;

extern  int     thread_recursive_mutex_init(thread_recursive_mutex_t *,
                                                 pthread_mutexattr_t *);
extern  int     thread_recursive_mutex_destroy(thread_recursive_mutex_t *);
extern  int     thread_recursive_mutex_lock(thread_recursive_mutex_t *);
extern  int     thread_recursive_mutex_trylock(thread_recursive_mutex_t *);
extern  int     thread_recursive_mutex_unlock(thread_recursive_mutex_t *);

/*
   ================================================================
                                Barrier
   ================================================================
*/



typedef struct {
  pthread_mutex_t       lock;           /* lock for structure           */
  pthread_cond_t        cv;             /* waiting list control         */
  int                   count;          /* Number of threads to wait for*/
  int                   n_sleepers;     /* Number of threads to waiting */
  int                   releasing;      /* Still waking up sleepers     */
} thread_barrier_t;

extern  int     thread_barrier_init(thread_barrier_t *, int);
extern  int     thread_barrier_destroy(thread_barrier_t *);
extern  int     thread_barrier_wait(thread_barrier_t *);
extern  int     thread_barrier_trywait(thread_barrier_t *);


/*
   ================================================================
                                Single Barrier
   ================================================================
*/



typedef struct  {
  pthread_mutex_t       lock;           /* lock for structure           */
  pthread_cond_t        cv;             /* waiting list control         */
  int                   count;          /* Number of threads to post    */
  int                   n_posters;      /* Number of threads who posted */
  int                   n_waiters;      /* Number of threads waiting    */
  int                   releasing;      /* Still waking up sleepers     */
} thread_single_barrier_t;

extern  int     thread_single_barrier_init(thread_single_barrier_t *, int);
extern  int     thread_single_barrier_destroy(thread_single_barrier_t *);
extern  int     thread_single_barrier_wait(thread_single_barrier_t *);
extern  int     thread_single_barrier_post(thread_single_barrier_t *);

/*
   ================================================================
                        Timed Mutex Lock
   ================================================================
*/

#define THREAD_TIMED_MUTEX_INITIALIZER  \
 {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, FALSE, NULL_TID, 0}


typedef struct _timed_mutex {
  pthread_mutex_t       lock;           /* lock for structure           */
  pthread_cond_t        cv;             /* waiting list control         */
  int                   owned;          /* TRUE | FALSE                 */
  pthread_t             owner;          /* NULL_TID or TID              */
  const struct timespec *abstime;       /* absolute time before timeout */
} thread_timed_mutex_t;

extern  int     thread_timed_mutex_init(thread_timed_mutex_t *,
                                                 pthread_mutexattr_t *);
extern  int     thread_timed_mutex_destroy(thread_timed_mutex_t *);
extern  int     thread_timed_mutex_lock(thread_timed_mutex_t *,
                                            const struct timespec *);
extern  int     thread_timed_mutex_trylock(thread_timed_mutex_t *);
extern  int     thread_timed_mutex_unlock(thread_timed_mutex_t *);

/*
   ================================================================
                        Assorted Useful Functions
   ================================================================
*/

extern  char    *thread_name(pthread_t);


int PTHREAD_CREATE(pthread_t *new_thread_ID,
                   const pthread_attr_t *attr,
                   void * (*start_func)(void *), void *arg);

int PTHREAD_ATTR_INIT(pthread_attr_t *a);

int PTHREAD_CONDATTR_INIT(pthread_condattr_t *a);

int PTHREAD_MUTEXATTR_INIT(pthread_mutexattr_t *a);

int SEM_INIT(sem_t *sem, int pshared, unsigned int value);

int SEM_WAIT(sem_t *arg);

void delay(int sleep_ms, int cpu_us);

void *killer(void *arg);

/*-----------------------------------------------------------------------------
 *
 * Library Name: UTIL
 * Module Name:  iobench
 *
 * Designer:    R. C. Sullivan 
 * Programmer:  R. C. Sullivan
 * Date:        Sep 22, 1995
 *
 * History Of Changes:
 *      Name         Date            Description
 *      ----         ----            -----------
 *      RCS     Jan 17, 1996     Inital release
 *
 * Purpose:
 *   To report resource usage statistics that will be correct for
 * programs using threads on a Solaris system.
 *
 * Notes:
 *
 *-----------------------------------------------------------------------------
 */

extern struct prusage prusagebuf_start, prusagebuf_end;
extern int procfd;
extern double real_time, user_time, system_time, trap_time, wait_time;
extern unsigned long minor_pfs, major_pfs, input_blocks, output_blocks, iochars;

void iobench_start();
void iobench_end();
void iobench_report();
