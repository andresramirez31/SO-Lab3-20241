/**
 * @defgroup   SAXPY saxpy
 *
 * @brief      This file implements an iterative saxpy operation
 * 
 * @param[in] <-p> {vector size} 
 * @param[in] <-s> {seed}
 * @param[in] <-n> {number of threads to create} 
 * @param[in] <-i> {maximum itertions} 
 *
 * @author     Danny Munera
 * @date       2020
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>
#include <pthread.h>
#include <bits/getopt_core.h>

typedef struct {
    int start;
    int end;
} ThreadArg;

// Variables to obtain command line parameters
unsigned int seed = 1;
int p = 100000;
int n_threads = 2;
int max_iters = 1000;

// Variables to perform SAXPY operation
double* X;
double a;
double* Y;
double* Y_avgs;
int i, it;

// Variables to get execution time
struct timeval t_start, t_end;
double exec_time;


// Variables to control the threads
pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;

void *saxpyCalculation(void *);

int main(int argc, char* argv[]){
	
  	
  
	
	// Variables to partition the work in threads
	int part; 
	
	
	

	// Getting input values
	int opt;
	while((opt = getopt(argc, argv, ":p:s:n:i:")) != -1){  
		switch(opt){  
			case 'p':  
			printf("vector size: %s\n", optarg);
			p = strtol(optarg, NULL, 10);
			assert(p > 0 && p <= 2147483647);
			break;  
			case 's':  
			printf("seed: %s\n", optarg);
			seed = strtol(optarg, NULL, 10);
			break;
			case 'n':  
			printf("threads number: %s\n", optarg);
			n_threads = strtol(optarg, NULL, 10);
			break;  
			case 'i':  
			printf("max. iterations: %s\n", optarg);
			max_iters = strtol(optarg, NULL, 10);
			break;  
			case ':':  
			printf("option -%c needs a value\n", optopt);  
			break;  
			case '?':  
			fprintf(stderr, "Usage: %s [-p <vector size>] [-s <seed>] [-n <threads number>] [-i <maximum itertions>]\n", argv[0]);
			exit(EXIT_FAILURE);
		}  
	}  

	part = max_iters / n_threads;
	// Variables to initialize parallelization
	pthread_t t[n_threads];
	ThreadArg args[n_threads];
	srand(seed);

	printf("p = %d, seed = %d, n_threads = %d, max_iters = %d\n", \
	 p, seed, n_threads, max_iters);	

	// initializing data
	X = (double*) malloc(sizeof(double) * p);
	Y = (double*) malloc(sizeof(double) * p);
	Y_avgs = (double*) malloc(sizeof(double) * max_iters);

	for(i = 0; i < p; i++){
		X[i] = (double)rand() / RAND_MAX;
		Y[i] = (double)rand() / RAND_MAX;
	}
	for(i = 0; i < max_iters; i++){
		Y_avgs[i] = 0.0;
	}
	a = (double)rand() / RAND_MAX;

#ifdef DEBUG
	printf("vector X= [ ");
	for(i = 0; i < p-1; i++){
		printf("%f, ",X[i]);
	}
	printf("%f ]\n",X[p-1]);

	printf("vector Y= [ ");
	for(i = 0; i < p-1; i++){
		printf("%f, ", Y[i]);
	}
	printf("%f ]\n", Y[p-1]);

	printf("a= %f \n", a);	
#endif

gettimeofday(&t_start, NULL);
for(int j = 0; j < n_threads; j++){
	args[j].start = part * j;
	args[j].end = part * (j + 1);
	pthread_create(&t[j], NULL, saxpyCalculation , &args[j]);
}

for(int j = 0; j < n_threads; j++){
	pthread_join(t[j], NULL);	
}
gettimeofday(&t_end, NULL);


#ifdef DEBUG
	printf("RES: final vector Y= [ ");
	for(i = 0; i < p-1; i++){
		printf("%f, ", Y[i]);
	}
	printf("%f ]\n", Y[p-1]);
#endif
	
	// Computing execution time
	exec_time = (t_end.tv_sec - t_start.tv_sec) * 1000.0;  // sec to ms
	exec_time += (t_end.tv_usec - t_start.tv_usec) / 1000.0; // us to ms
	printf("Execution time: %f ms \n", exec_time);
	printf("Last 3 values of Y: %f, %f, %f \n", Y[p-3], Y[p-2], Y[p-1]);
	printf("Last 3 values of Y_avgs: %f, %f, %f \n", Y_avgs[max_iters-3], Y_avgs[max_iters-2], Y_avgs[max_iters-1]);
	return 0;
}	


void *saxpyCalculation(void *arg){

	/*
	 *	Function to parallelize 
	 */
	ThreadArg* thread_arg = (ThreadArg*) arg;
	
	//SAXPY iterative SAXPY mfunction
	for(it = thread_arg->start; it < thread_arg->end; it++){
		
		for(i = 0; i < p; i++){
			pthread_mutex_lock(&m1);
			Y[i] = Y[i] + a * X[i];
			Y_avgs[it] += Y[i];
			pthread_mutex_unlock(&m1);
		}
		Y_avgs[it] = Y_avgs[it] / p;
		
	}
	
	sleep(1);
	pthread_exit(0);
}