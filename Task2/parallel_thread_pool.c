// C program to multiply two matrices parallelly using a thread pool and a queue
#include "task_queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

// Decide how many threads to create
// My computer can run 8 threads concurrently at max, so I will create 7 threads at max as 1 is the main calling thread
#define num_of_threads 7
#define minimum_task_size 128

// Global variables used by queue related functions
queue q; // Task queue
pthread_mutex_t mutex_queue;

// Function to generate a random single digit
int single_digit_rand()
{
	return ((rand())%10);
}

// Function to initialize an NxN matrix with random numbers
void initialize_random_matrix(long long int *mptr, long long int N)
{
	for(long long int i=0; i<(N*N); i++)
	{
		*mptr = single_digit_rand();
		mptr++;
	}
}

// Function to initialize an NxN matrix with 0s
void initialize_0matrix(long long int *mptr, long long int N)
{
	for(long long int i=0; i<(N*N); i++)
	{
		*mptr = 0;
		mptr++;
	}
}

// Function to print out an NxN matrix in a file
void fprint_matrix(long long int *mptr, long long int N, FILE *fptr)
{
	for(long long int i=0; i<(N*N); i++)
	{
		fprintf(fptr, "%lld " ,*mptr);
		if((i%N)==(N-1)) // New rows in new lines
			fprintf(fptr, "\n");
		mptr++;
	}
}

// Function to multiply two NxN matrices
void multiply_matrix_parallel(task *args_task)
{   
    // Getting the arguments from the void pointer
    long long int start_index = args_task->start;
    long long int end_index = args_task->end;
	long long int N = args_task->N;
	long long int *m1ptr = args_task->m1ptr;
	long long int *m2ptr = args_task->m2ptr;

	// Setting location of first element
	long long int *currentptr;
	currentptr=(args_task->mptr)+start_index;
	long long int rownum=(long long int)(start_index/N);
	long long int colnum=(start_index%N);
	long long int sum=0;

	// Iterate through elements of product matrix from start_index to end_index
	for(long long int i=start_index; i<=end_index; i++)
	{
		for(long long int j=0; j<N; j++) // Find the value of current element in product matrix
		{
			// Cpq = Ap1*B1q + Ap2*B2q + ... + ApN*BNq
			// where C=A*B, p=rownum, q=colnum
			sum+=(*(m1ptr+(N*rownum)+j))*(*(m2ptr+colnum+(N*j)));
		}

		// Store the sum and reset it
		*currentptr=sum;
		sum=0;

		// Increment the current element pointer and column number
		currentptr++;
		colnum++;

		if((colnum%N)!=colnum) // Reset column number to 0 after N-1 and increment row number
		{
			colnum=colnum%N;
			rownum++;
		}
	}
	free(args_task); // Free the memory allocated by thread_wait function
}

// Function called by pthread_create
// The function and agument types is void * just so it is compatible with pthread_create
void* thread_wait(void *args) // Each thread pops tasks from the queue and exectutes them until no items are left in queue
{
	while(1)
	{
		pthread_mutex_lock(&mutex_queue); // Locks the mutex so other threads cannot pop tasks simultaneously
		if(queue_size(&q)==0) // Check if queue is empty
		{
			pthread_mutex_unlock(&mutex_queue);
			pthread_exit(NULL); // Exit the thread
		}
		task *copy = malloc(sizeof(task)); // Get a task from queue and store it in 'copy'
		queue_pop(&q, copy);
		pthread_mutex_unlock(&mutex_queue); // Unlock the mutex for other threads
		multiply_matrix_parallel(copy); // Pass 'copy' task to the matrix multiplication function
	}
	return NULL;
}

// Driver code
int main(int argc, char *argv[])
{
	// Take N as input via command line argument and perror otherwise
	// N square is used many times
	if(argc!=2)
	{
		perror("Give exactly one integer argument");
		exit(EXIT_FAILURE);
	}
	long long int N = atoi(argv[1]);
	long long int N_square = N*N;

	// Generate two random matrices of size N x N
	// Generate 0 matrix of size N x N to store ans
	long long int *m1ptr = malloc(N_square*sizeof(long long int));
	long long int *m2ptr = malloc(N_square*sizeof(long long int));
	long long int *ansptr = malloc(N_square*sizeof(long long int));
	initialize_random_matrix(m1ptr, N);
	initialize_random_matrix(m2ptr, N);
	initialize_0matrix(ansptr, N);	

    // Decide number of parts to break the ans matrix into
    long long int num_of_structs;
    num_of_structs=fmin(N_square,fmax(num_of_threads,(N_square/minimum_task_size)));
    
	// Start clock to measure run time for multiplication of the matrices
	clock_t start = clock();

    // Initialize the queue and put all the tasks in it
	queue_initialize(&q);
	for(long long int i = 0; i<(num_of_structs-1); i++)
	{
		// Create a copy of task and initialize it
		task *to_be_added = malloc(sizeof(task));
		to_be_added->m1ptr = m1ptr;
		to_be_added->m2ptr = m2ptr;
		to_be_added->mptr = ansptr;
		to_be_added->N = N;
		to_be_added->start = (i*(N_square/num_of_structs));
		to_be_added->end = (((i+1)*(N_square/num_of_structs))-1);
        queue_add(&q, to_be_added); // Add the task to the queue and then free the copy
		free(to_be_added);
	}
	// I had to evaluate the last part separately to cover the entire ans matrix
	task *to_be_added = malloc(sizeof(task));
	to_be_added->m1ptr = m1ptr;
	to_be_added->m2ptr = m2ptr;
	to_be_added->mptr = ansptr;
	to_be_added->N = N;
	to_be_added->start=(((num_of_structs-1)*(N_square/num_of_structs)));
	to_be_added->end=((N_square)-1);
    queue_add(&q, to_be_added);
	free(to_be_added);

	pthread_mutex_init(&mutex_queue, NULL); // Initialize the mutex to be used by thread_wait function

	// Create the threads
    pthread_t thread_id[num_of_threads];
    for(long long int i = 0; i<(num_of_threads); i++)
	{
        pthread_create(&thread_id[i], NULL, &thread_wait, NULL);
    }

	// Wait for all threads to complete
	for(long long int i = 0; i<num_of_threads; ++i)
	{
        pthread_join(thread_id[i], NULL);
    }

	// End clock
	clock_t end = clock();

	// Store the two random matrices and product matrix in a file for verification of result
	// Skip if size > 1000 as it takes up time
	FILE *fptr = fopen("p_tpool_out.txt","w");
	if(N<=1000)
	{
		fprintf(fptr, "First Matrix:\n");
		fprint_matrix(m1ptr, N, fptr);
		fprintf(fptr, "Second Matrix:\n");
		fprint_matrix(m2ptr, N, fptr);
		fprintf(fptr, "Product of the two matrices:\n");
		fprint_matrix(ansptr, N, fptr);
	}
	fclose(fptr);

	// Free allocated memory and destroy mutex
	free(m1ptr);
	free(m2ptr);
	free(ansptr);
    pthread_mutex_destroy(&mutex_queue);

	// Calculate run time
	double t = ((double)(end - start)*1000) / CLOCKS_PER_SEC;

	// Print run time
	printf("Multiplication time is %lf ms\n", t);

	return 0;
}
