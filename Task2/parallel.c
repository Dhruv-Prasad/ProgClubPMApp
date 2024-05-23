// C program to multiply two matrices parallelly
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <sys/time.h>

// Struct to store arguments for multiplication
// I need this because pthread_create only takes one argument for input to a function
struct arguments
{
	long long int *m1ptr; // Pointer to first matrix
	long long int *m2ptr; // Pointer to second matrix
	long long int *mptr; // Pointer to product matrix
	long long int N;
	long long int start; // Index of first element to be calculated by thread
	long long int end; // Index of last element to be calculated by thread
};

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
void *multiply_matrix_parallel(void *arg_structure)
{
	// Getting the data from the void pointer
	// I am doing it this way since pthread_create can only pass a void pointer argument to it's function
	struct arguments *argsp = (struct arguments *)arg_structure;
	long long int *m1ptr=argsp->m1ptr; 
	long long int *m2ptr=argsp->m2ptr; 
	long long int *mptr=argsp->mptr;
	long long int N=argsp->N;
	long long int start_index=argsp->start;
	long long int end_index=argsp->end;

	// Setting location of first element
	long long int *currentptr;
	currentptr=mptr+start_index;
	long long int rownum=start_index/N;
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
		*currentptr=sum;
		sum=0;
		currentptr++;
		colnum++;
		if((colnum%N)!=colnum) // Reset column number to 0 after N-1 and increment row number
		{
			colnum=colnum%N;
			rownum++;
		}
	}
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
	int N = atoi(argv[1]);
	int N_square = N*N;

	// Set seed using current time
	srand(time(NULL));
	
	// Decide how many threads to create
	// My scomputer can run 8 threads concurrently at max, so I will create 7 threads at max as 1 is the main thread
	// 8, 16 and 32 threads didn't give me any boost anyways
	int num_of_threads = fmin(N_square, 8);

	// Generate two random matrices of size N x N
	// Generate 0 matrix of size N x N to store ans
	long long int *m1ptr = malloc(N_square*sizeof(long long int));
	long long int *m2ptr = malloc(N_square*sizeof(long long int));
	long long int *ansptr = malloc(N_square*sizeof(long long int));
	initialize_random_matrix(m1ptr, N);
	initialize_random_matrix(m2ptr, N);
	initialize_0matrix(ansptr, N);

	// Initialize the arguments structs
	struct arguments my_args[num_of_threads];
	for(int i = 0; i<(num_of_threads); i++)
	{
		my_args[i].mptr=ansptr;
		my_args[i].m1ptr=m1ptr;
		my_args[i].m2ptr=m2ptr;
		my_args[i].N=N;
	}

	// Start clock to measure run time for multiplication of the matrices
	struct timeval start, end;
	gettimeofday(&start, NULL);

	// Create the threads and evaluate num_of_threads parts of ans matrix in parallel
	pthread_t threadId[num_of_threads];
	for(int i = 0; i<(num_of_threads-1); i++)
	{
		// Set correct start index and end index of argument structs
		my_args[i].start=(i*(N_square/num_of_threads));
		my_args[i].end=(((i+1)*(N_square/num_of_threads))-1);

		// Create a thread to run wrapper function parallely for each thread id
        pthread_create(&threadId[i], NULL, multiply_matrix_parallel, (void *)&my_args[i]);
    }

	// I had to evaluate the last part separately to cover the entire ans matrix
	my_args[(num_of_threads-1)].start=(((num_of_threads-1)*(N_square/num_of_threads)));
	my_args[(num_of_threads-1)].end=((N_square)-1);
	pthread_create( &threadId[num_of_threads-1], NULL, multiply_matrix_parallel, (void *)&my_args[(num_of_threads-1)]);
	
	// Wait for all threads to complete
	for(int i = 0; i<num_of_threads; i++)
	{
        pthread_join(threadId[i], NULL);
    }

	// End clock
	gettimeofday(&end, NULL);

	// Store the two random matrices and product matrix in a file for verification of result
	// Skip if size > 1000 as it takes up time
	FILE *fptr = fopen("p.txt","w");
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

	// Free allocated memory
	free(m1ptr);
	free(m2ptr);
	free(ansptr);

	// Calculate run time
	double t = (double)((end.tv_usec - start.tv_usec)/1000) + (double)((end.tv_sec - start.tv_sec)*1000);

	// Print run time
	printf("Multiplication time is %lf ms\n", t);

	return 0;
}
