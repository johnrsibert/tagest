/*                thread_extensions.c                */

/*
  Lots of useful functions for all sorts of neat things.

   I changed the names to avoid all conflicts / confusion
   with Solaris threads.
*/

/*
cc  -G -o libthread_extensions.so thread_extensions.c -g -lpthread
        -lthread -lposix4

*/




#include <stdio.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/procfs.h>
#include "thread_extensions.h"


#define _POSIX_C_SOURCE 199506L


pthread_key_t   sleep_queue_key, FIFO_MUTEX_QUEUE_KEY;

void dmutex_key_initializer();

#ifdef __sun
 /*  This runs before main() and declares the TSD key. */
#pragma init(dmutex_key_initializer)
#pragma init(fifo_key_initializer)
#endif

#if defined(__alpha) && (__osf__)
void __init_dmutex_key()
{
  dmutex_key_initializer();
  fifo_key_initializer();
}
#endif




/*
   ================================================================
                        General List
   ================================================================
*/

list_t *cons(void *obj, void *lst) {
  list_t                *l = (list_t *) malloc(sizeof(list_t));

  l->frst = obj;
  l->next = lst;
  return(l);
}

int empty(list_t *l) {return(l == NULL);}
void *first(list_t *l) {return(l->frst);}
list_t *rest(list_t *l) {return(l->next);}


/*
   ================================================================
                        Stringify (for printing) TIDs
   ================================================================
*/


typedef struct _pt
{pthread_t      tid;
 char           *name;
 struct _pt     *next;
} thread_name_t;

#if (!defined(__sun) && !defined(__sgi))

char *thread_name(pthread_t tid)
{char s[100];
 thread_name_t *n;
 static int tid_count = 1000;
 static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
 static thread_name_t *thread_names = NULL;

 if (pthread_equal(NULL_TID, tid)) tid = pthread_self();
 pthread_mutex_lock(&lock);

   for (n = thread_names; n != NULL; n=n->next)
     {if (pthread_equal(n->tid, tid))
        goto L;
    }
 n = (thread_name_t *) malloc(sizeof(thread_name_t));
 n->tid = tid;
#if defined(__alpha) && defined(__osf__)
 sprintf(s, "T@%d", pthread_getsequence_np(tid));
#else
 sprintf(s, "T@%d", tid_count);
 tid_count++;
#endif
 n->name = (char *) malloc(strlen(s)+1);
 strcpy(n->name, s);
 n->next = thread_names;
 thread_names = n;

L:pthread_mutex_unlock(&lock);
 return(n->name);
}

#else
/* For known implementations, use the underlying TID numbering */

char *thread_name(pthread_t tid)
{char s[100];
 thread_name_t *n;

 static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
 static thread_name_t *thread_names = NULL;

 if (pthread_equal(NULL_TID, tid)) tid = pthread_self();
 pthread_mutex_lock(&lock);

   for (n = thread_names; n != NULL; n=n->next)
     {if (pthread_equal(n->tid, tid))
        goto L;
    }
 n = (thread_name_t *) malloc(sizeof(thread_name_t));
 n->tid = tid;
 sprintf(s, "T@%d", (int) tid);
 n->name = (char *) malloc(strlen(s)+1);
 strcpy(n->name, s);
 n->next = thread_names;
 thread_names = n;

L:pthread_mutex_unlock(&lock);
 return(n->name);
}
#endif


/*
   ================================================================
                        Readers / Writer Lock
   ================================================================
*/

int thread_rwlock_init(thread_rwlock_t *rwp,
                           thread_rwlock_attr_t *attr)
{int                    ptype;
 pthread_mutexattr_t    mutex_attr;
 pthread_condattr_t     cond_attr;

 pthread_mutexattr_init(&mutex_attr);
 pthread_condattr_init(&cond_attr);

    /* Not Implemented ...   
    ptype = thread_rwlock_attr_getshared(&attr);
    pthread_mutexattr_setpshared(&mutex_attr, ptype);
    pthread_condattr_setpshared(&cond_attr, ptype);
    */

 pthread_mutex_init(&rwp->lock, &mutex_attr);
 pthread_cond_init(&rwp->readers, &cond_attr);
 pthread_cond_init(&rwp->writers, &cond_attr);

 rwp->state     = 0;
 rwp->waiters   = 0;

 return(0);
}


int thread_rwlock_destroy(thread_rwlock_t *rwp)
{
  pthread_mutex_destroy(&rwp->lock);
  pthread_cond_destroy(&rwp->readers);
  pthread_cond_destroy(&rwp->writers);

  return(0);
}


int thread_rw_rdlock(thread_rwlock_t *rwp) 
{pthread_mutex_t        *lkp    = &rwp->lock;

 pthread_mutex_lock(lkp);
 pthread_cleanup_push(pthread_mutex_unlock, lkp);
 /* active or queued writers            */
 while ((rwp->state < 0) || rwp->waiters)
   pthread_cond_wait(&rwp->readers, lkp);

 rwp->state++;
 pthread_cleanup_pop(1);

 return(0);
}


int thread_rw_tryrdlock(thread_rwlock_t *rwp)
{int    status  = EBUSY;

 pthread_mutex_lock(&rwp->lock);
 /* available and no writers queued     */
 if ((rwp->state >= 0) && !rwp->waiters) {
   rwp->state++;
   status       = 0;
 }

 pthread_mutex_unlock(&rwp->lock);

 return(status);
}


void thread_rw_wrlock_cleanup(void *arg)
{thread_rwlock_t        *rwp    = (thread_rwlock_t *) arg;

 /*
  * Was the only queued writer and lock is available for readers.
  * Called through cancellation clean-up so lock is held at entry.
  */
 if ((--rwp->waiters == 0) && (rwp->state >= 0))
   pthread_cond_broadcast(&rwp->readers);

 pthread_mutex_unlock(&rwp->lock);

 return;
}


int thread_rw_wrlock(thread_rwlock_t *rwp) 
{pthread_mutex_t        *lkp    = &rwp->lock;

 pthread_mutex_lock(lkp);
 rwp->waiters++;                /* another writer queued                */
 pthread_cleanup_push(thread_rw_wrlock_cleanup, rwp);

 while (rwp->state)
   pthread_cond_wait(&rwp->writers, lkp);

 rwp->state     = -1;
 pthread_cleanup_pop(1);

 return(0);
}


int thread_rw_trywrlock(thread_rwlock_t *rwp)
{int    status  = EBUSY;

 pthread_mutex_lock(&rwp->lock);
 /* no readers, no writers, no writers queued   */
 if (!rwp->state && !rwp->waiters) {
   rwp->state   = -1;
   status               = 0;
 }

 pthread_mutex_unlock(&rwp->lock);

 return(status);
}


int thread_rw_unlock(thread_rwlock_t *rwp)
{pthread_mutex_lock(&rwp->lock);

 if (rwp->state == -1) {        /* writer releasing     */
   rwp->state   = 0;            /* mark as available    */

   if (rwp->waiters)            /* writers queued       */
     pthread_cond_signal(&rwp->writers);
   else
     pthread_cond_broadcast(&rwp->readers);
 } else {
   if (--rwp->state == 0)       /* no more readers      */
     pthread_cond_signal(&rwp->writers);
 }

 pthread_mutex_unlock(&rwp->lock);

 return(0);
}

/*
   ================================================================
                        Recursive Readers / Writer Lock
   ================================================================

   This allows both readers and writers to lock recursively. Readers
   essentially just get to have multiple locks (hence requiring the
   locks be reader-preference), while writers are
   recorded (as obviously required). Writers are also allowed to
   get recursive read locks (which makes it a little bit tricker).
*/

int thread_rrwlock_init(thread_rrwlock_t *rwp,
			thread_rrwlock_attr_t *attr) {
  int                    ptype;

  pthread_mutex_init(&rwp->lock, NULL);
  pthread_cond_init(&rwp->readers, NULL);
  pthread_cond_init(&rwp->writers, NULL);

  rwp->nreaders  = 0;
  rwp->nwriters  = 0;
  rwp->owner     = NULL_TID;
  rwp->waiters   = 0;

  return(0);
}


int thread_rrwlock_destroy(thread_rrwlock_t *rwp)
{
  pthread_mutex_destroy(&rwp->lock);
  pthread_cond_destroy(&rwp->readers);
  pthread_cond_destroy(&rwp->writers);

  return(0);
}


int thread_rrw_rdlock(thread_rrwlock_t *rwp) {
  pthread_mutex_t        *lkp    = &rwp->lock;

  pthread_mutex_lock(lkp);
  pthread_cleanup_push(pthread_mutex_unlock, lkp);
  /* active or queued writers            */
  while ((rwp->nreaders == 0) &&
	 ((rwp->nwriters > 0) && (!pthread_equal(owner, pthread_self()))) &&
	 ((rwp->nwriters > 0) || (rwp->waiters > 0)))
    pthread_cond_wait(&rwp->readers, lkp);

  /* Get here if there are (no readers, writers, waiters), or
     previous readers, or the writer is me */
  rwp->nreaders++;
  pthread_cleanup_pop(1);

  return(0);
}

void thread_rrw_wrlock_cleanup(void *arg) {
  thread_rrwlock_t        *rwp    = (thread_rrwlock_t *) arg;

  /*
   * Was the only queued writer and lock is available for readers.
   * Called through cancellation clean-up so lock is held at entry.
   */

  if (--rwp->waiters == 0)
    pthread_cond_broadcast(&rwp->readers);
  else
    pthread_cond_signal(&rwp->writers);    

  pthread_mutex_unlock(&rwp->lock);
  return;
}


int thread_rrw_wrlock(thread_rrwlock_t *rwp) {
  pthread_mutex_t       *lkp = &rwp->lock;
  pthread_t 		tid = pthread_self(); 

  pthread_mutex_lock(lkp);
  rwp->waiters++;                /* another writer queued                */
  pthread_cleanup_push(thread_rrw_wrlock_cleanup, rwp);

  while ((rwp->nreaders > 0) ||
	 ((rwp->nwriters > 0) && !pthread_equal(rwp->owner, tid)))
    pthread_cond_wait(&rwp->writers, lkp);

  rwp->nwriters++;
  rwp->owner = tid;
  pthread_cleanup_pop(1);	/* waiters--; unlock(lkp); */

  return(0);
}


int thread_rrw_unlock(thread_rrwlock_t *rwp) {
  
  pthread_mutex_lock(&rwp->lock);

  if (nreaders > 0) {
    rwp->nreaders--; 
    if ((rwp->nreaders == 0) && (rwp->nwriters == 0) && (rwp->waiters > 0))
      pthread_cond_signal(&rwp->writers);
      pthread_mutex_unlock(&rwp->lock);
      return(0);
  }

  if (rwp->nwriters > 0) {
    rwp->nwriters--;
    if (rwp->nwriters == 0) {
      rwp->owner = NULL_TID;	/* Advisory/debugging. Never used */
      if (rwp->waiters)
	pthread_cond_signal(&rwp->writers);
      else
	pthread_cond_broadcast(&rwp->readers);
      pthread_mutex_unlock(&rwp->lock);
      return(0);
    }
    else {
      abort(1);			/* Cannot happen */
    }
  }
}


/*
   ================================================================
                        Fifo Mutex Lock
   ================================================================

        Guarantees that all sleepers will get the mutex in correct
        order.
*/


int thread_fifo_mutex_init(thread_fifo_mutex_t *m,
                               pthread_mutexattr_t *attr)
{
  pthread_mutex_init(&(m->lock), NULL);
  m->waitersQueue = NULL;
  m->owned = FALSE;
  m->owner = NULL_TID;
}


int thread_fifo_mutex_destroy(thread_fifo_mutex_t *m)
{
  pthread_mutex_destroy(&(m->lock));
}


void fifo_queue_destroyer(void *arg)
{
  fifo_mutex_queue_t *fifo_mutex_queue = (fifo_mutex_queue_t *) arg;
  
  free(fifo_mutex_queue);
}


void fifo_key_initializer()
{
  pthread_key_create(&FIFO_MUTEX_QUEUE_KEY, fifo_queue_destroyer);
}

 

fifo_mutex_queue_t *get_fifo_mutex_queue()
{fifo_mutex_queue_t *fifo_mutex_queue =
                (fifo_mutex_queue_t *) pthread_getspecific(FIFO_MUTEX_QUEUE_KEY);
int i; 
 if (fifo_mutex_queue == NULL)
        {fifo_mutex_queue = (fifo_mutex_queue_t *) malloc(sizeof(fifo_mutex_queue_t));
        i++;
         fifo_mutex_queue->tid = pthread_self();
         sem_init(&fifo_mutex_queue->semaphore, FALSE, 0);
         pthread_setspecific(FIFO_MUTEX_QUEUE_KEY, fifo_mutex_queue);
        }
 return(fifo_mutex_queue);
}


int thread_fifo_mutex_lock(thread_fifo_mutex_t *m)
{pthread_t self = pthread_self();
 fifo_mutex_queue_t *fifo_mutex_queue = get_fifo_mutex_queue();

  pthread_mutex_lock(&(m->lock));
  if (!(m->owned))
    {m->owner = self;
     m->owned = TRUE;
     pthread_mutex_unlock(&(m->lock));
     return(0);}

  fifo_mutex_queue->next = NULL;
  if (m->waitersQueue == NULL)  
    m->waitersQueue = fifo_mutex_queue;
  else
    m->waitersQueueLast->next = fifo_mutex_queue;
  m->waitersQueueLast = fifo_mutex_queue;
  pthread_mutex_unlock(&(m->lock));
  SEM_WAIT(&(fifo_mutex_queue->semaphore));

  pthread_mutex_lock(&(m->lock));
  if (m->owned != -1) {abort(1);}  /* Cannot happen */
  m->owner = self;
  m->owned = TRUE;
  m->waitersQueue = m->waitersQueue->next;
  pthread_mutex_unlock(&(m->lock));
  return(0);
}


int thread_fifo_mutex_trylock(thread_fifo_mutex_t *m)
{

  pthread_mutex_lock(&(m->lock));
  if (!(m->owned))
    {m->owner = pthread_self();
     m->owned = TRUE;
     pthread_mutex_unlock(&(m->lock));
     return(0);}
  pthread_mutex_unlock(&(m->lock));
  return(EBUSY);
}


int thread_fifo_mutex_unlock(thread_fifo_mutex_t *m)
{
  pthread_mutex_lock(&(m->lock));
  m->owner = NULL_TID;

  if (m->waitersQueue == NULL)
    {m->owned = FALSE;
     pthread_mutex_unlock(&(m->lock));
     return(0);}

  m->owned = -1;
  sem_post(&(m->waitersQueue->semaphore));
  pthread_mutex_unlock(&(m->lock));
  return(0);
}



/*
   ================================================================
                        Simple (wrong) Fifo Mutex Lock
   ================================================================

        Guarantees that a sleeper will get the mutex, even if
        some other thread trys first.  But several sleeping threads
        will NOT be FIFO.  But the implementation is simple.



int thread_fifo_mutex_init(thread_fifo_mutex_t *m,
                               pthread_mutexattr_t *attr)
{
  pthread_mutex_init(&(m->lock), NULL);
  pthread_cond_init(&(m->cv), NULL);
  m->waiters = 0;
  m->owned = FALSE;
  m->owner = NULL_TID;
}


int thread_fifo_mutex_destroy(thread_fifo_mutex_t *m)
{
  pthread_mutex_destroy(&(m->lock));
  pthread_cond_destroy(&(m->cv));
}


int thread_fifo_mutex_lock(thread_fifo_mutex_t *m)
{

  pthread_mutex_lock(&(m->lock));
  if (!(m->owned))
    {m->owner = pthread_self();
     m->owned = TRUE;
     pthread_mutex_unlock(&(m->lock));
     return(0);}

  m->waiters++;
  /* Don't check for -1 the first time because
     that -1 is intended for a sleeper. 
  pthread_cond_wait(&(m->cv), &(m->lock));

  while (m->owned != -1)
    pthread_cond_wait(&(m->cv), &(m->lock));
  /* A spurious wakeup at just the wrong moment will give
     ownership to the wrong thread.  So we're only 99.999% FIFO.
     But NEVER incorrect. 
  m->owner = pthread_self();
  m->owned = TRUE;
  m->waiters--;
  pthread_mutex_unlock(&(m->lock));
  return(0);
}


int thread_fifo_mutex_trylock(thread_fifo_mutex_t *m)
{

  pthread_mutex_lock(&(m->lock));
  if (!(m->owned))
    {m->owner = pthread_self();
     m->owned = TRUE;
     pthread_mutex_unlock(&(m->lock));
     return(0);}
  pthread_mutex_unlock(&(m->lock));
  return(EBUSY);
}


int thread_fifo_mutex_unlock(thread_fifo_mutex_t *m)
{
  pthread_mutex_lock(&(m->lock));
  m->owner = NULL_TID;

  if (m->waiters == 0)
    {m->owned = FALSE;
     pthread_mutex_unlock(&(m->lock));
     return(0);}

  m->owned = -1;
  pthread_mutex_unlock(&(m->lock));
  pthread_cond_signal(&(m->cv));
  return(0);
}

*/



/*
   ================================================================
                        Debug Mutex Lock
   ================================================================

        For debugging programs.  Do a global replace of _mutex_ with
        _dmutex_ and then the dmutex will detect self deadlocks, 
        non-owner unlocks, and record the owner's TID for use from
        the debugger.

        If you call pthread_dmutex_init(), then a macro will place
        the dmutex on a global list and record its name.  If you only
        use PTHREAD_DMUTEX_INITIALIZER, then no recording.  You may
        choose specific names via pthread_dmutex_init1().

        print_dmutexes() will print out the state (and sleep queue)
        for all mutexes on the list.  From the debugger:
        (debugger) call print_dmutexes()

*/

pthread_mutex_t dmutex_queue_lock = PTHREAD_MUTEX_INITIALIZER;
dmutex_queue_t  *dmutex_queue;


void dmutex_key_initializer()
{
  pthread_key_create(&sleep_queue_key, NULL);
 }

void insert_dmutex(pthread_dmutex_t *m)
{dmutex_queue_t *dq;

 dq = (dmutex_queue_t *) malloc(sizeof(dmutex_queue_t));
 dq->mutex = m;
 pthread_mutex_lock(&dmutex_queue_lock);
 dq->next = dmutex_queue;
 dmutex_queue = dq;
 pthread_mutex_unlock(&dmutex_queue_lock);
}

void remove_dmutex(dmutex_queue_t **q, pthread_dmutex_t *m)
{dmutex_queue_t *dq, *dq1;

 pthread_mutex_lock(&dmutex_queue_lock);

 if ((*q)->mutex == m)
   {dq1 = dmutex_queue->next;
    *q = dq1->next;
    free(dq1);
  }
 else
   for (dq = *q; dq->next != NULL; dq = dq->next)
     if (dq->next->mutex == m)
       {dq1 = dq->next;
        dq->next = dq1->next;
        free(dq1);
        break;
      }

 pthread_mutex_unlock(&dmutex_queue_lock);
}

void remove_sleeper(sleep_queue_t **q, pthread_t tid)
{sleep_queue_t *dq, *dq1;

 if (pthread_equal((*q)->tid, tid))
   {
     *q = (*q)->next;
   }
 else
   for (dq = *q; dq->next != NULL; dq = dq->next)
     if (pthread_equal(dq->next->tid, tid))
       {dq1 = dq->next;
        dq->next = dq1->next;
        break;
      }

}


void print_dmutex(pthread_dmutex_t *m)
{sleep_queue_t  *sq;

 pthread_mutex_lock(&(m->lock));
 flockfile (stdout); 
   if (!m->owned)
   printf("%s ---- Times locked: %3d,  failed: %3d.  Sleepers: ( ",
          m->name, m->n_locked, m->n_failed);
 else
   printf("%s %s  Times locked: %3d,  failed: %3d.  Sleepers: ( ",
          m->name, thread_name(m->owner), m->n_locked, m->n_failed);
 
 for (sq = m->sleepers; sq != NULL; sq = sq->next)
   printf("%s ", thread_name(sq->tid));
 printf(")\n");
 funlockfile (stdout); 
 pthread_mutex_unlock(&(m->lock));
}



void print_dmutexes()
{dmutex_queue_t *dq;
 pthread_dmutex_t *m;

 pthread_mutex_lock(&dmutex_queue_lock);
 for (dq = dmutex_queue; dq != NULL; dq = dq->next)
   {m = dq->mutex;
    print_dmutex(m);
  }
 pthread_mutex_unlock(&dmutex_queue_lock);
}

  

int pthread_dmutex_init1(pthread_dmutex_t *m, pthread_mutexattr_t *attr,
                         char *name)
{
  pthread_mutex_init(&(m->lock), NULL);
  pthread_cond_init(&(m->cv), NULL);
  m->owned = FALSE;
  m->owner = NULL_TID;
  m->name = name;
  m->sleepers = NULL;
  m->n_locked = 0;
  m->n_failed = 0;
  insert_dmutex(m);
}

int pthread_dmutex_destroy(pthread_dmutex_t *m)
{
  pthread_mutex_destroy(&(m->lock));
  pthread_cond_destroy(&(m->cv));
  remove_dmutex(&dmutex_queue, m);
}



int pthread_dmutex_lock(pthread_dmutex_t *m)
{pthread_t tid = pthread_self();
 sleep_queue_t *dq = (sleep_queue_t *) pthread_getspecific(sleep_queue_key);

 if (dq == NULL)
   {dq = (sleep_queue_t *) malloc(sizeof(sleep_queue_t));
    pthread_setspecific(sleep_queue_key, dq);
    dq->tid = tid;
  }

 pthread_mutex_lock(&(m->lock));
 if (pthread_equal(m->owner, tid))
   {if (m->name)
      printf("Error! %s deadlocking dmutex %s.\n", thread_name(tid), m->name);
   else
     printf("Error! %s deadlocking dmutex x%x.\n", thread_name(tid), m);
    abort();    /* I don't want it to exit.  I want it to crash. */
  }
 while (m->owned)
   {dq->next = m->sleepers;
    m->sleepers = dq;
    m->n_failed++;
    pthread_cond_wait(&(m->cv), &(m->lock));
    remove_sleeper(&(m->sleepers), tid);
   }
 m->owner = tid;
 m->owned = TRUE;
 m->n_locked++;
 pthread_mutex_unlock(&(m->lock));
 return(0);
}


int pthread_dmutex_trylock(pthread_dmutex_t *m)
{pthread_t tid = pthread_self();

 pthread_mutex_lock(&(m->lock));
 if (!(m->owned))
    {m->owner = tid;
     m->owned = TRUE;
     m->n_locked++;
     pthread_mutex_unlock(&(m->lock));
     return(0);
   }
 m->n_failed++;
 pthread_mutex_unlock(&(m->lock));
 return(EBUSY);
}


int pthread_dmutex_unlock(pthread_dmutex_t *m)
{pthread_t tid = pthread_self();

 pthread_mutex_lock(&(m->lock));
 if (!pthread_equal(m->owner, tid))
   {if (m->name)
      printf("Error! %s unlocking dmutex %s owned by %s\n",
             thread_name(tid), m->name, thread_name(m->owner));
   else
     printf("Error! %s unlocking dmutex x%x owned by %s\n",
            thread_name(tid), m, thread_name(m->owner));
    abort();    /* I don't want it to exit.  I want it to crash. */
    }
   
 m->owned = FALSE;
 m->owner = NULL_TID;
 pthread_mutex_unlock(&(m->lock));
 pthread_cond_signal(&(m->cv));
 return(0);
}




/*
   ================================================================
                        Recursive Mutex Lock
   ================================================================

        Allow the same thread to lock the mutex recursively 
        (requiring the same number of unlocks).

        Use of these is NOT recommended for any program!  Write
        your program better so that you don't WANT them.
*/


int thread_recursive_mutex_init(thread_recursive_mutex_t *m,
                                    pthread_mutexattr_t *attr)
{
  pthread_mutex_init(&(m->lock), NULL);
  pthread_cond_init(&(m->cv), NULL);
  m->owned = FALSE;
  m->owner = NULL_TID;
  m->count = 0;
}


int thread_recursive_mutex_destroy(thread_recursive_mutex_t *m)
{
  pthread_mutex_destroy(&(m->lock));
  pthread_cond_destroy(&(m->cv));
}



int thread_recursive_mutex_lock(thread_recursive_mutex_t *m)
{pthread_t tid = pthread_self();

  pthread_mutex_lock(&(m->lock));
  while (m->owned && !pthread_equal(m->owner, tid))
    pthread_cond_wait(&(m->cv), &(m->lock));
 m->owner = tid;
 m->owned = TRUE;
 m->count++;
 pthread_mutex_unlock(&(m->lock));
 return(0);
}


int thread_recursive_mutex_trylock(thread_recursive_mutex_t *m)
{pthread_t tid = pthread_self();

 pthread_mutex_lock(&(m->lock));
 if (!m->owned || pthread_equal(m->owner, tid))
    {m->owner = tid;
     m->owned = TRUE;
     m->count++;
     pthread_mutex_unlock(&(m->lock));
     return(0);
   }
  pthread_mutex_unlock(&(m->lock));
  return(EBUSY);
}


int thread_recursive_mutex_unlock(thread_recursive_mutex_t *m)
{
  pthread_mutex_lock(&(m->lock));
  m->count--;
  if (m->count == 0)
    {m->owned = FALSE;
     m->owner = NULL_TID;
     pthread_mutex_unlock(&(m->lock));
     pthread_cond_signal(&(m->cv));
     return(0);
   }
  pthread_mutex_unlock(&(m->lock));
  return(0);
}


/*
   ================================================================
                        Barrier
   ================================================================

        Full barrier: all threads wait until the last thread arrives
        before waking up.
*/


int thread_barrier_init(thread_barrier_t *m, int i)
{
  pthread_mutex_init(&(m->lock), NULL);
  pthread_cond_init(&(m->cv), NULL);
  m->n_sleepers = 0;
  m->releasing = FALSE;
  m->count = i;
}


int thread_barrier_destroy(thread_barrier_t *m)
{
  pthread_mutex_destroy(&(m->lock));
  pthread_cond_destroy(&(m->cv));
}



int thread_barrier_wait(thread_barrier_t *m)
{int release = FALSE;

 pthread_mutex_lock(&(m->lock));
 while (m->releasing)           /* If previous cycle still releasing, wait */
   pthread_cond_wait(&(m->cv), &(m->lock));

 m->n_sleepers++;
 if (m->n_sleepers == m->count)
   release = m->releasing = TRUE;
 else
   while (!m->releasing)
     pthread_cond_wait(&(m->cv), &(m->lock));

 m->n_sleepers--;
 if (m->n_sleepers == 0)
   {m->releasing = FALSE;
    release = TRUE;             /* Wake up waiters (if any) for next cycle */
  }
 pthread_mutex_unlock(&(m->lock));

 if (release)
   pthread_cond_broadcast(&m->cv);
 return(0);
}


/*
   ================================================================
                        Barrier
   ================================================================

        Single barrier: Only one thread waits until the last thread
        arrives before waking up.
*/


int thread_single_barrier_init(thread_single_barrier_t *b, int i)
{
  pthread_mutex_init(&(b->lock), NULL);
  pthread_cond_init(&(b->cv), NULL);
  b->n_posters = 0;
  b->n_waiters = 0;
  b->releasing = FALSE;
  b->count = i;
}


int thread_single_barrier_destroy(thread_single_barrier_t *b)
{
  pthread_mutex_destroy(&(b->lock));
  pthread_cond_destroy(&(b->cv));
}



int thread_single_barrier_wait(thread_single_barrier_t *b) {
  int release = FALSE;

  b->n_waiters++;
  while (!b->releasing)
    pthread_cond_wait(&(b->cv), &(b->lock));

  b->n_waiters--;
  if (b->n_waiters == 0) {
    b->releasing = FALSE;
    release = TRUE;             /* Wake up waiters (if any) for next cycle */
    b->n_posters=0;
    }
  pthread_mutex_unlock(&(b->lock));

  if (release)
    pthread_cond_broadcast(&b->cv);
  return(0);
}




int thread_single_barrier_post(thread_single_barrier_t *b)
{int release = FALSE;

 pthread_mutex_lock(&(b->lock));
 while (b->releasing)           /* If previous cycle still releasing, wait */
   pthread_cond_wait(&(b->cv), &(b->lock));

 b->n_posters++;
 if (b->n_posters == b->count)
   {b->releasing = TRUE;
    release = TRUE;             /* Wake up waiters (if any) for next cycle */
  }
 pthread_mutex_unlock(&(b->lock));

 if (release)
   pthread_cond_broadcast(&b->cv);
 return(0);
}





/*
   ================================================================
                        Error Detection
   ================================================================

        For those functions which can return a "reasonable" runtime
        error, go ahead and detect them.  And then abort.
*/


int PTHREAD_CREATE(pthread_t *new_thread_ID,
                   const pthread_attr_t *attr,
                   void * (*start_func)(void *), void *arg)
{int err;
 pthread_t tid;

 if (!new_thread_ID)
   {printf("TID cannot be NULL.\n");
    abort();
  }
 if (err = pthread_create(new_thread_ID, attr, start_func, arg))
   {printf("%s\n", strerror(err));
    abort();
  }}

int PTHREAD_ATTR_INIT(pthread_attr_t *a)
{int err;
 if (err = pthread_attr_init(a))
   {printf("%s\n", strerror(err));
    abort();
  }}

int PTHREAD_CONDATTR_INIT(pthread_condattr_t *a)
{int err;
 if (err = pthread_condattr_init(a))
   {printf("%s\n", strerror(err));
    abort();
  }}

int PTHREAD_MUTEXATTR_INIT(pthread_mutexattr_t *a)
{int err;
 if (err = pthread_mutexattr_init(a))
   {printf("%s\n", strerror(err));
    abort();
  }}

int SEM_INIT(sem_t *sem, int pshared, unsigned int value)
{int err;
 if (err = sem_init(sem, pshared, value))
   {perror("");
    abort();
  }}


int SEM_WAIT(sem_t *arg)        /* Ignore signal interruptions */
{while (sem_wait(arg) != 0) {}}


/*
   ================================================================
                        Random useful functions
   ================================================================

*/

int delay_cycles_per_us = 8;            /* App. number for 110MHz SS4

/* sleep time will be quantized at 10ms, CPU time is VERY approximate. */

void delay(int sleep_ms, int cpu_us)
{int i, cycles = cpu_us*delay_cycles_per_us;
 volatile j=0;
 struct timespec tv;

 if (sleep_ms != 0)
   {tv.tv_sec = sleep_ms/1000;
    tv.tv_nsec = (sleep_ms%1000) * 1000000;
    nanosleep(&tv, NULL);
  }

 if (cpu_us != 0)
   for (i=0; i<cycles; i++) j++;
}


int calibrate_delay()
{
  delay_cycles_per_us = 8;
  iobench_start();
  delay(0, 1000000);
  iobench_end();
  DEBUG(iobench_report());
  DEBUG(printf("Calibration pass: %d delay cycles -> %f seconds\n", 
               delay_cycles_per_us, user_time));
  delay_cycles_per_us = (delay_cycles_per_us * 1.0) / user_time;

  iobench_start();
  delay(0, 1000000);
  iobench_end();
  DEBUG(iobench_report());
  DEBUG(printf("Verification pass: %d delay cycles -> %f seconds\n", 
               delay_cycles_per_us, user_time));
}

/* A barrier for one thread to wait for lots of others. */
void thread_single_barrier(sem_t *barrier, int count)
{
  while (count > 0)
    {SEM_WAIT(barrier);
     count--;
    }
}


/*
   ================================================================
                        Timed Mutex Lock
   ================================================================

        Just like a timed CV, only for a mutex!
*/


int thread_timed_mutex_init(thread_timed_mutex_t *m,
                                    pthread_mutexattr_t *attr)
{
  pthread_mutex_init(&(m->lock), NULL);
  pthread_cond_init(&(m->cv), NULL);
  m->owned = FALSE;
  m->owner = NULL_TID;
  m->abstime = 0;
}


int thread_timed_mutex_destroy(thread_timed_mutex_t *m)
{
  pthread_mutex_destroy(&(m->lock));
  pthread_cond_destroy(&(m->cv));
}



int thread_timed_mutex_lock(thread_timed_mutex_t *m,
                                const struct timespec *abstime)
{int err;
 pthread_t tid = pthread_self();

  pthread_mutex_lock(&(m->lock));
  while (m->owned)
    {err=pthread_cond_timedwait(&(m->cv), &(m->lock), abstime);
     if (err)
       {pthread_mutex_unlock(&(m->lock));
        return(err);
      }}
 m->owner = tid;
 m->owned = TRUE;
 pthread_mutex_unlock(&(m->lock));
 return(0);
}


int thread_timed_mutex_trylock(thread_timed_mutex_t *m)
{pthread_t tid = pthread_self();

 pthread_mutex_lock(&(m->lock));
 if (!m->owned)
    {m->owner = tid;
     m->owned = TRUE;
     pthread_mutex_unlock(&(m->lock));
     return(0);
   }
  pthread_mutex_unlock(&(m->lock));
  return(EBUSY);
}


int thread_timed_mutex_unlock(thread_timed_mutex_t *m)
{
  pthread_mutex_lock(&(m->lock));
  m->owned = FALSE;
  m->owner = NULL_TID;
  pthread_mutex_unlock(&(m->lock));
  pthread_cond_signal(&(m->cv));
}




/*
   ================================================================
                        Wait before fork()
   ================================================================

  The idea is that you count the number of threads you have, then have
the forker thread wait for them all to reach a fork-safe location (where
they will pause).  The forker then forks and wakes up all of the other
threads.

  The other threads will call thread_wait_for_forker() whenever they
are at a fork-safe location.  If no thread is waiting to fork, they
simply continue.  If there is one, they will then sleep until awaken.

  The forker thread (there can only be one!) calls thread_wait_for_others()
before forking, and thread_awake_others() afterwards.  The child does
not need to call thread_awake_others() unless it plans to call fork()
also.  (A bug w/spurious wakeups if child also forks?)

*/

struct _fork_waiter
{
  pthread_mutex_t       lock;
  pthread_cond_t        forker_cv;
  pthread_cond_t        others_cv;
  int                   runners;
  int                   owned;
  pthread_t             owner;
};

struct _fork_waiter fork_waiter =
{PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, PTHREAD_COND_INITIALIZER,
   0, FALSE, NULL_TID};


void thread_wait_for_others(int i)
{
  pthread_mutex_lock(&fork_waiter.lock);
  if (fork_waiter.owned)
    {printf("Fatal Error: Multiple calls to thread_wait_for_others!");
     abort();
   }
  fork_waiter.owned = TRUE;
  fork_waiter.owner = pthread_self();
  fork_waiter.runners=i;
  while (fork_waiter.runners>0)
    pthread_cond_wait(&fork_waiter.forker_cv, &fork_waiter.lock);
  pthread_mutex_unlock(&fork_waiter.lock);
}
  

void thread_awake_others()
{
  pthread_mutex_lock(&fork_waiter.lock);
  fork_waiter.owned = FALSE;
  fork_waiter.owner = NULL_TID;
  pthread_mutex_unlock(&fork_waiter.lock);
  pthread_cond_broadcast(&fork_waiter.others_cv);
}

void thread_wait_for_forker()
{
  pthread_mutex_lock(&fork_waiter.lock);
  if (fork_waiter.owned)
    {fork_waiter.runners--;
     while (fork_waiter.owned)
       {if (fork_waiter.runners == 0) pthread_cond_signal(&fork_waiter.forker_cv);
        pthread_cond_wait(&fork_waiter.others_cv, &fork_waiter.lock);
   }}
  pthread_mutex_unlock(&fork_waiter.lock);
}



/*
   ================================================================
                                Killer
   ================================================================
   */

void *killer(void *arg) {
  int time = (int) arg;
  sleep(time);
  printf("Killer thread timing out. (Program hung?)  Die!  Die!  Die!\n");
  exit(-1);
}



/*
   ================================================================
                                Spin locks
   ================================================================

  Two versions of spin locks.  The good versions (only defined for
  Solaris right now) will not flood the memory bus.

  The bad version of spin lock will flood the memory bus with ldstub
  ops.  Try setting BAD_SPIN_COUNT = 10^9 and then running this on
  a couple of CPUs, while trying to get work done on the other CPUs.

*/


#ifdef __sun2_5
const int SPIN_COUNT = 1000;            /* About .1us per loop, SS10 */

void spin_lock(pthread_mutex_t *m)
{int i;
 for (i = 0; i < SPIN_COUNT; i++)
   {if (m->pthread_mutex_lock.pthread_mutex_owner64 == 0) /* Check w/o ldstub */
      if (pthread_mutex_trylock(m) != EBUSY) 
        return;                         /* Got it! */
    /* Didn't get it, continue the loop */
    }
 pthread_mutex_lock(m);                 /* Give up and block */
}

#else
const int BAD_SPIN_COUNT = 100;         /* About 1us  per loop, SS10 */

void spin_lock(pthread_mutex_t *m)
{int i;
 for (i=0; i < BAD_SPIN_COUNT; i++)
   {if (pthread_mutex_trylock(m) != EBUSY)      /* Trash the memory bus! */
      return;    }                              /* got the lock! */
 pthread_mutex_lock(m);                         /* give up and block. */ 
 return;                                        /* got the lock after blocking! */
}



#endif




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

#ifdef __sun

struct stat statbuf;
struct prusage prusagebuf_start, prusagebuf_end;
int procfd;
double real_time, total_real_time, user_time, system_time, trap_time, wait_time;
unsigned long minor_pfs, major_pfs, input_blocks, output_blocks, iochars;

void iobench_start() {
  char pfile[80];

  sprintf(pfile, "/proc/%ld", getpid());
  procfd = open(pfile, O_RDONLY);

  ioctl(procfd, PIOCUSAGE, &prusagebuf_start);
}

void iobench_end() {
  ioctl(procfd, PIOCUSAGE, &prusagebuf_end);
  close(procfd);

  real_time = (double) prusagebuf_start.pr_tstamp.tv_sec +
    (double) prusagebuf_start.pr_tstamp.tv_nsec / NANOSEC;
  real_time = (double) prusagebuf_end.pr_tstamp.tv_sec +
    (double) prusagebuf_end.pr_tstamp.tv_nsec / NANOSEC - real_time;

  total_real_time = (double) prusagebuf_start.pr_rtime.tv_sec +
    (double) prusagebuf_start.pr_rtime.tv_nsec / NANOSEC;
  total_real_time = (double) prusagebuf_end.pr_rtime.tv_sec +
    (double) prusagebuf_end.pr_rtime.tv_nsec / NANOSEC - real_time;

  user_time = (double) prusagebuf_start.pr_utime.tv_sec +
    (double) prusagebuf_start.pr_utime.tv_nsec / NANOSEC;
  user_time = (double) prusagebuf_end.pr_utime.tv_sec +
    (double) prusagebuf_end.pr_utime.tv_nsec / NANOSEC - user_time;

  system_time = (double) prusagebuf_start.pr_stime.tv_sec +
    (double) prusagebuf_start.pr_stime.tv_nsec / NANOSEC;
  system_time = (double) prusagebuf_end.pr_stime.tv_sec +
    (double) prusagebuf_end.pr_stime.tv_nsec / NANOSEC - system_time;

  trap_time = (double) prusagebuf_start.pr_ttime.tv_sec +
    (double) prusagebuf_start.pr_ttime.tv_nsec / NANOSEC;
  trap_time = (double) prusagebuf_end.pr_ttime.tv_sec +
    (double) prusagebuf_end.pr_ttime.tv_nsec / NANOSEC - trap_time;

  wait_time = (double) prusagebuf_start.pr_wtime.tv_sec +
    (double) prusagebuf_start.pr_wtime.tv_nsec / NANOSEC;
  wait_time = (double) prusagebuf_end.pr_wtime.tv_sec +
    (double) prusagebuf_end.pr_wtime.tv_nsec / NANOSEC - wait_time;

  minor_pfs = prusagebuf_end.pr_minf - prusagebuf_start.pr_minf;
  major_pfs = prusagebuf_end.pr_majf - prusagebuf_start.pr_majf;
  input_blocks = prusagebuf_end.pr_inblk - prusagebuf_start.pr_inblk;
  output_blocks = prusagebuf_end.pr_oublk - prusagebuf_start.pr_oublk;
  /*  iochars = prusagebuf_end.pr_ioch - prusagebuf_start.pr_ioch;*/
}

void iobench_report() {
  fprintf(stderr, "\nProcess info:\n");
  fprintf(stderr, "  elapsed time  %g\n", real_time);
  /*  fprintf(stderr, "  total time    %g\n", total_real_time);*/
  fprintf(stderr, "  CPU time      %g\n", user_time + system_time);
  /*  fprintf(stderr, "  user time     %g\n", user_time);
  fprintf(stderr, "  system time   %g\n", system_time);
  fprintf(stderr, "  trap time     %g\n", trap_time);
  fprintf(stderr, "  wait time     %g\n",  wait_time);
  fprintf(stderr, "  pfs    major/minor  %6lu/%6lu\n", major_pfs, minor_pfs);
  fprintf(stderr, "  blocks input/output %6lu/%6lu\n",
          input_blocks, output_blocks); */
  /*  fprintf(stderr, "  char inp/out  %lu\n", iochars);*/
  fprintf(stderr, "\n");

  /*  fprintf(stderr, "%2.5g Mbytes/sec (real time)\n", iochars /
      real_time / 1e6);
      fprintf(stderr, "%2.5g Mbytes/sec (CPU time) \n", iochars /
      (user_time + system_time) / 1e6);*/

  fprintf(stderr, "%2.1f%% CPU usage\n\n", 100 * (user_time + system_time) /
          real_time + .05);
}

#else
void iobench_start() {}
void iobench_end() {}
void iobench_report() {}
#endif
