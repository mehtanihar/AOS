#include <stdio.h>
#include <unistd.h>
#include <linux/unistd.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sched.h>
#include <signal.h>
#include <setjmp.h>
#include <errno.h>
#include <assert.h>
#include <math.h>
#include "gt_include.h"


#define ROWS 256
#define COLS ROWS
#define SIZE COLS

#define NUM_CPUS 2
#define NUM_GROUPS NUM_CPUS
#define PER_GROUP_COLS (SIZE/NUM_GROUPS)

#define NUM_THREADS 128
#define PER_THREAD_ROWS (SIZE/NUM_THREADS)


/* A[SIZE][SIZE] X B[SIZE][SIZE] = C[SIZE][SIZE]
 * Let T(g, t) be thread 't' in group 'g'.
 * T(g, t) is responsible for multiplication :
 * A(rows)[(t-1)*SIZE -> (t*SIZE - 1)] X B(cols)[(g-1)*SIZE -> (g*SIZE - 1)] */

typedef struct matrix
{
	int m[SIZE][SIZE];

	int rows;
	int cols;
	unsigned int reserved[2];
} matrix_t;


typedef struct __uthread_arg
{
	matrix_t *_A, *_B, *_C;
	unsigned int reserved0;

	unsigned int tid;
	unsigned int gid;
	int start_row; /* start_row -> (start_row + PER_THREAD_ROWS) */
	int start_col; /* start_col -> (start_col + PER_GROUP_COLS) */
	int weight;
	int cap;
	int tslice;
	int rate_limit;
	int end_row;

}uthread_arg_t;

struct timeval tv1;

static void generate_matrix(matrix_t *mat, int val)
{

	int i,j;
	mat->rows = SIZE;
	mat->cols = SIZE;
	for(i = 0; i < mat->rows;i++)
		for( j = 0; j < mat->cols; j++ )
		{
			mat->m[i][j] = val;
		}
	return;
}

static void print_matrix(matrix_t *mat)
{
	int i, j;

	for(i=0;i<mat->rows;i++)
	{
		for(j=0;j<mat->cols;j++)
			printf(" %d ",mat->m[i][j]);
		printf("\n");
	}

	return;
}


static void * uthread_mulmat(void *p)
{
	int i, j, k;
	int start_row, end_row;
	int start_col, end_col;
	unsigned int cpuid;
	struct timeval tv2;

#define ptr ((uthread_arg_t *)p)

	i=0; j= 0; k=0;

	start_row = ptr->start_row;
	end_row = (ptr->start_row + PER_THREAD_ROWS);

#ifdef GT_GROUP_SPLIT
	start_col = ptr->start_col;
	end_col = (ptr->start_col + PER_THREAD_ROWS);
#else
	start_col = 0;
	end_col = SIZE;
#endif
#ifdef GT_THREADS
	cpuid = kthread_cpu_map[kthread_apic_id()]->cpuid;
	fprintf(stderr, "\nThread(id:%d, group:%d, cpu:%d) started",ptr->tid, ptr->gid, cpuid);
#else
	fprintf(stderr, "\nThread(id:%d, group:%d) started",ptr->tid, ptr->gid);
#endif

	for(i = start_row; i < end_row; i++)
		for(j = start_col; j < end_col; j++)
			for(k = 0; k < SIZE; k++)
				ptr->_C->m[i][j] += ptr->_A->m[i][k] * ptr->_B->m[k][j];

#ifdef GT_THREADS
	fprintf(stderr, "\nThread(id:%d, group:%d, cpu:%d) finished (TIME : %lu s and %lu us)\n",
			ptr->tid, ptr->gid, cpuid, (tv2.tv_sec - tv1.tv_sec), (tv2.tv_usec - tv1.tv_usec));
#else
	gettimeofday(&tv2,NULL);
	fprintf(stderr, "\nThread(id:%d, group:%d) finished (TIME : %lu s and %lu us)\n",
			ptr->tid, ptr->gid, (tv2.tv_sec - tv1.tv_sec), (tv2.tv_usec - tv1.tv_usec));
#endif

#undef ptr
	return 0;
}



static void * custom_uthread_mulmat(void *p)
{
	int i, j, k;
	int start_row, end_row;
	int start_col, end_col;
	unsigned int cpuid;
	struct timeval tv2;
	srand(time(NULL));  
	int yield_interval = 10;
	int yield_count = 0;
	int yield_max = 2;
	
#define ptr ((uthread_arg_t *)p)

	i=0; j= 0; k=0;

	start_row = ptr->start_row;
	end_row = ptr->end_row;//(ptr->start_row + (int)(ptr->_A->rows / (NUM_THREADS/16) ));

#ifdef GT_GROUP_SPLIT
	start_col = ptr->start_col;
	end_col = (ptr->start_col + (int)(ptr->_A->rows / (NUM_THREADS/16) ));
#else
	start_col = 0;
	end_col = end_row;//ptr->_B->cols;
#endif

#ifdef GT_THREADS
	cpuid = kthread_cpu_map[kthread_apic_id()]->cpuid;
fprintf(stderr, "\nThread(id:%d, group:%d, cpu:%d) started",ptr->tid, ptr->gid, cpuid);
#else
	fprintf(stderr, "\nThread(id:%d, group:%d) started",ptr->tid, ptr->gid);
#endif



	for(i = start_row; i < end_row; i++){
		for(j = start_col; j < end_col; j++){
			for(k = 0; k < end_row; k++){

				if(ptr->tid % yield_interval == 0 && yield_count < yield_max){
					yield_count += 1;
					gt_yield();
					
				}
				ptr->_C->m[i][j] += ptr->_A->m[i][k] * ptr->_B->m[k][j];
			}
		}
	}
#ifdef GT_THREADS
	fprintf(stderr, "\nThread(id:%d, group:%d, cpu:%d) finished (TIME : %lu s and %lu us)\n",
			ptr->tid, ptr->gid, cpuid, (tv2.tv_sec - tv1.tv_sec), (tv2.tv_usec - tv1.tv_usec));
#else
	gettimeofday(&tv2,NULL);
	fprintf(stderr, "\nThread(id:%d, group:%d) finished (TIME : %lu s and %lu us)\n",
			ptr->tid, ptr->gid, (tv2.tv_sec - tv1.tv_sec), (tv2.tv_usec - tv1.tv_usec));
#endif

#undef ptr
	return 0;
}


matrix_t A, B, C;

static void init_matrices()
{
	generate_matrix(&A, 1);
	generate_matrix(&B, 1);
	generate_matrix(&C, 0);

	return;
}


uthread_arg_t uargs[NUM_THREADS];
uthread_t utids[NUM_THREADS];

int main(int argc, char* argv[])
{
	if (argc < 2) {
        fprintf(stderr, "Invalid arguments" );
        return 1;
    }

	num_cpus = (int)sysconf(_SC_NPROCESSORS_CONF);
	int SchedulerChoice = atoi(argv[1]);
	if(SchedulerChoice == 1){


		uthread_arg_t *uarg;
		int inx;

		gtthread_app_init(SchedulerChoice);
		int credits[4] = {25, 50, 75, 100};
		init_matrices();
		int matrix_sizes[4] = {32, 64, 128, 256};

		int i,j, credit, matrix_size;
		for(i = 0; i < 4; i++){
			credit = credits[i];
			for(j = 0; j < 4; j++){
				matrix_size = matrix_sizes[j];
				
				for(inx=0; inx<NUM_THREADS/16; inx++)
				{
					int id = 32*i + 8*j + inx;
					uarg = &uargs[id];
					uarg->_A = &A;
					uarg->_B = &B;
					uarg->_C = &C;

					uarg->tid = id;


					uarg->start_row = 0;//(inx * (int)(matrix_size)/(NUM_THREADS/16));
					uarg->end_row = matrix_size;
					
					uarg->gid = (id % num_cpus);

					#ifdef GT_GROUP_SPLIT
						/* Wanted to split the columns by groups !!! */
						uarg->start_col = (uarg->gid * (int(matrix_size/num_cpus)));
					#endif

					uarg->weight = credit;
					uarg->tslice = credit;
					uarg->cap = 0;

					uthread_create(&utids[id], custom_uthread_mulmat, uarg, uarg->gid, uarg->weight, uarg->cap, uarg->tslice, uarg->rate_limit, SchedulerChoice);
				}


			}
		}


		gtthread_app_exit();

		//Mean calculation
		float running_means[NUM_THREADS/8] = {0.0};
		float total_means[NUM_THREADS/8] = {0.0};
		float running_std[NUM_THREADS/8] = {0.0};
		float total_std[NUM_THREADS/8] = {0.0};
		for(int i = 0; i < NUM_THREADS; i++){
			
			running_means[(int)i/8] = running_means[(int)i/8] + (running_thread_time[i]/8.0);
			total_means[(int)i/8] = total_means[(int)i/8] + (total_thread_time[i]/8.0);
			running_std[(int)i/8] +=  (running_thread_time[i])*(running_thread_time[i]/8.0);
			total_std[(int)i/8] +=  (total_thread_time[i])*(total_thread_time[i]/8.0);			

		}


		//Std calculation

		for(int i=0; i<NUM_THREADS/8; i++){
			running_std[i] = sqrt(running_std[i] - running_means[i]*running_means[i]);
			total_std[i] = sqrt(total_std[i] - total_means[i]*total_means[i]);
			printf("\n Process id: %d credits: %d matrix_size: %d mean_run_time %f mean_total_time %f std_run_time %f std_total_time %f", i, credits[(int)(i/4)], matrix_sizes[i%4],
			running_means[i],total_means[i], running_std[i], total_std[i]);
		}
		




	}
	else{

		uthread_arg_t *uarg;
		int inx;

		gtthread_app_init(SchedulerChoice);

		init_matrices();

		gettimeofday(&tv1,NULL);

		for(inx=0; inx<NUM_THREADS; inx++)
		{
			uarg = &uargs[inx];
			uarg->_A = &A;
			uarg->_B = &B;
			uarg->_C = &C;

			uarg->tid = inx;

			uarg->gid = (inx % NUM_GROUPS);

			uarg->start_row = (inx * PER_THREAD_ROWS);
		#ifdef GT_GROUP_SPLIT
			/* Wanted to split the columns by groups !!! */
			uarg->start_col = (uarg->gid * PER_GROUP_COLS);
		#endif

			uthread_create(&utids[inx], uthread_mulmat, uarg, uarg->gid, 0,0,0,0, SchedulerChoice);
		}

		gtthread_app_exit();


	}


	//print_matrix(&C);
	// fprintf(stderr, "********************************");
	return(0);
}
