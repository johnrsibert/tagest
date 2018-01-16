#ifndef __adi_thread_h__
#define __adi_thread_h__
#include "coff_t.h"

#include "thread_extensions.h"
//#include <pthread.h>

typedef void* (*solver_fn_t)(void*);

typedef struct solver_description 
{
  solver_fn_t solver_functionp;
  void* argsp;
  struct solver_description* next;
} solver_description_t;
  
typedef struct
{
  int thread_no;
  int ij;
  coff_t* parent;
} rc_solver_args_t;

/*
typedef struct
{
  int num_active;
  pthread_cond_t thread_exit_cv;
  pthread_mutex_t mutex;
} active_thread_info_t;
*/

/*
class adi_threads
{
private:
  int num_threads;
  int max_queue_size;

  int do_not_block_when_full;
  ivector threads;
  int max_threads;
  int cur_queue_size;
  solver_description_t* queue_head;
  solver_description_t* queue_tail;
  pthread_mutex_t queue_lock;
  pthread_cond_t queue_not_empty;
  pthread_cond_t queue_not_full;
  pthread_cond_t queue_empty;
  int queue_closed;
  int shutdown;

  void (*solverp)(void*);
  solver_fn_t solver_function;
  rc_solver_args_t* argsp;
  int thrno;
  adi_threads* parent;
  coff_t* coff_ptr;

public:
  adi_threads(coff_t* coffp, const int max_threads);
  ~adi_threads(void);

  void* executive(void* t);
  static void* s_executive(void* t);
//  int run_solver(solver_fn_t solverp, rc_solver_args_t* argsp, const int thrno);
  int run_solver(solver_fn_t solverp, const int ij); //, coff_t* coffp);

};   
*/
#endif // __adi_thread_h__
