#ifndef __GT_INCLUDE_H
#define __GT_INCLUDE_H

#include "gt_signal.h"
#include "gt_spinlock.h"
#include "gt_tailq.h"
#include "gt_bitops.h"

#include "gt_uthread.h"
#include "gt_pq.h"
#include "gt_kthread.h"

/* Shared arrays for storing execution and total times */
unsigned int total_thread_time[128];
unsigned int running_thread_time[128];
unsigned int num_cpus;
int kthreads_initialized;

#endif
