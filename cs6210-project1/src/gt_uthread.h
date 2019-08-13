#ifndef __GT_UTHREAD_H
#define __GT_UTHREAD_H

/* User-level thread implementation (using alternate signal stacks) */

typedef unsigned int uthread_t;
typedef unsigned int uthread_group_t;

/* uthread states */
#define UTHREAD_INIT 0x01
#define UTHREAD_RUNNABLE 0x02
#define UTHREAD_RUNNING 0x04
#define UTHREAD_CANCELLED 0x08
#define UTHREAD_DONE 0x10

/* uthread struct : has all the uthread context info */
typedef struct uthread_struct
{
	
	int uthread_state; /* UTHREAD_INIT, UTHREAD_RUNNABLE, UTHREAD_RUNNING, UTHREAD_CANCELLED, UTHREAD_DONE */
	int uthread_priority; /* uthread running priority */
	int cpu_id; /* cpu it is currently executing on */
	int last_cpu_id; /* last cpu it was executing on */
	
	uthread_t uthread_tid; /* thread id */
	uthread_group_t uthread_gid; /* thread group id  */
	void * (*uthread_func)(void*);
	void *uthread_arg;
	void *exit_status; /* exit status */
	int reserved1;
	int reserved2;
	int reserved3;
	int weight, cap, credit, rate_limit;
	
	sigjmp_buf uthread_env; /* 156 bytes : save user-level thread context*/
	stack_t uthread_stack; /* 12 bytes : user-level thread stack */
	TAILQ_ENTRY(uthread_struct) uthread_runq;
	struct timeval created_time, finished_time, curr_start_time, curr_end_time,
	total_run_time, curr_run_time;
	int is_preempted;

} uthread_struct_t;

struct __kthread_runqueue;
extern void uthread_schedule(uthread_struct_t * (*kthread_best_sched_uthread)(struct __kthread_runqueue *));

extern int uthread_create(uthread_t *u_tid, void * (*u_func)(void *), void *u_arg, uthread_group_t u_gid, 
int weight, int cap, int credit, int rate_limit, int SchedulerChoice);
extern void gt_yield();
#endif
