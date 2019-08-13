#ifndef __GT_PQ_H
#define __GT_PQ_H

#define MAX_UTHREAD_PRIORITY 32
#define MAX_UTHREAD_GROUPS 32
#define DEFAULT_UTHREAD_PRIORITY 16

TAILQ_HEAD(uthread_head, uthread_struct);
typedef struct uthread_head uthread_head_t;

typedef struct prio_struct
{
	unsigned int group_mask; /* mask(i) : groups with atleast one thread */
	unsigned int reserved[3];

	uthread_head_t group[MAX_UTHREAD_GROUPS]; /* array(i) : uthreads from uthread_group 'i' */
} prio_struct_t;


typedef struct __runqueue
{
	unsigned int uthread_mask; /* mask : prio levels with atleast one uthread */
	unsigned int uthread_tot; /* cnt : Tot num of uthreads in the runq (all priorities) */
	uthread_group_t min_uthread_group; /* NOT USED : group (in the runqueue) with minimum uthreads (but GT 0) */

	unsigned int uthread_prio_tot[MAX_UTHREAD_PRIORITY]; /* array(i) : Tot num of uthreads at priority 'i' */

	unsigned int uthread_group_mask[MAX_UTHREAD_GROUPS]; /* array(i) : prio levels with atleast one uthread from uthread_group 'i' */
	unsigned int uthread_group_tot[MAX_UTHREAD_GROUPS]; /* array(i) : Tot num of uthreads in uthread_group 'i' */

	prio_struct_t prio_array[MAX_UTHREAD_PRIORITY];
} runqueue_t;

/* XXX: Move it to gt_sched.h later */


/* NOTE: kthread active/expires/zombie use the same link(uthread_runq) in uthread_struct.
 * This is perfectly fine since active/expires/zombie are mutually exclusive. */

/* NOTE: Each kthread simulates a virtual processor.
 * Since we are running only one uthread on a kthread
 * at any given time, we (typically) do not need a runqlock 
 * when executing uthreads. But, runqlock is necessary to allow 
 * a uthread_create from any kthread to post a uthread to 
 * any kthread's runq.
 * There is not much performance penalty (only one additional check), 
 * since we are using spinlocks. */

/* NOTE: We can have locks at finer granularity than kthread_runqlock.
 * For instance, a lock for each queue. But, for the current design,
 * it will be an overkill. So, we go with one *GIANT* lock for the
 * entire kthread runqueue */
typedef struct __kthread_runqueue
{
	runqueue_t *active_runq;
	runqueue_t *expires_runq;
	gt_spinlock_t kthread_runqlock;

	uthread_struct_t *cur_uthread;	/* current running uthread (not in active/expires) */
	unsigned int reserved0;
	uthread_head_t zombie_uthreads;

	runqueue_t runqueues[2];
} kthread_runqueue_t;

/* only lock protected versions are exported */
extern void init_runqueue(runqueue_t *runq);
extern void add_to_runqueue(runqueue_t *runq, gt_spinlock_t *runq_lock, uthread_struct_t *u_elem);
extern void rem_from_runqueue(runqueue_t *runq, gt_spinlock_t *runq_lock, uthread_struct_t *u_elem);
extern void switch_runqueue(runqueue_t *from_runq, gt_spinlock_t *from_runqlock, 
				runqueue_t *to_runq, gt_spinlock_t *to_runqlock, uthread_struct_t *u_elem);


/* kthread runqueue */
extern void kthread_init_runqueue(kthread_runqueue_t *kthread_runq);

/* Find the highest priority uthread.
 * Called by kthread handling VTALRM. */
extern uthread_struct_t *sched_find_best_uthread(kthread_runqueue_t *kthread_runq);

/* Find the highest priority uthread from uthread_group u_gid.
 * Called by kthread handling SIGUSR1(RELAYED signal).
 * Also globally sets the penalty, if choosing a lower priority uthread. */
extern uthread_struct_t *sched_find_best_uthread_group(kthread_runqueue_t *kthread_runq);


#endif
