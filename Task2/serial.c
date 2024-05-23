// C program to multiply two matrices serially
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

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

// Function to multiply two NxN matrices and return a pointer to the resulting matrix
void *multiply_matrix_serial(long long int *m1ptr, long long int *m2ptr, long long int *mptr, long long int N)
{
	long long int rownum=0;
	long long int colnum=0;
	long long int sum=0;
	for(long long int i=0; i<(N*N); i++) // Iterates through every element of product matrix
	{
		for(long long int j=0; j<N; j++) // Find the value of current element in product matrix
		{
			// Cpq = Ap1*B1q + Ap2*B2q + ... + ApN*BNq
			// where C=A*B, p=rownum, q=colnum
			sum+=(*(m1ptr+(N*rownum)+j))*(*(m2ptr+colnum+(N*j)));
		}
		*mptr=sum;
		sum=0;
		mptr++;
		colnum++;
		if((colnum%N)!=colnum) // Reset column number to 0 after N-1 and increment row number
		{
			colnum=colnum%N;
			rownum++;
		}
	}
	mptr -= (N * N);
}

// Driver code
int main(int argc, char *argv[])
{
	// Take N as input via command line argument
	// Perror if invalid inputs are given
	if(argc!=2)
	{
		perror("Give exactly one integer argument");
		exit(EXIT_FAILURE);
	}
	long long int N = atoi(argv[1]);
	
	// Set seed using current time
	srand(time(NULL));

	// Generate two random matrices of size N x N and a 0 matrix of size N x N to store the product
	long long int *m1ptr = malloc((N*N)*sizeof(long long int));
	long long int *m2ptr = malloc((N*N)*sizeof(long long int));
	initialize_random_matrix(m1ptr, N);
	initialize_random_matrix(m2ptr, N);
	long long int *ansptr = malloc((N*N)*sizeof(long long int));
	initialize_0matrix(ansptr, N);

	// Start clock to measure CPU run time of matrix multiplication, do the multiplication, end the clock
	struct timeval start, end;
	gettimeofday(&start, NULL);
	multiply_matrix_serial(m1ptr, m2ptr, ansptr, N);
	gettimeofday(&end, NULL);

	// Store the two random matrices and product matrix in a file for verification of result
	// Skip if size > 1000 as it takes up time
	FILE *fptr = fopen("s.txt","w");
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

	// Free the pointers to the three matrices
	free(m1ptr);
	free(m2ptr);
	free(ansptr);

	// Calculate run time
	double t = (double)((end.tv_usec - start.tv_usec)/1000) + (double)((end.tv_sec - start.tv_sec)*1000);

	// Print run time
	printf("Multiplication time is %lf ms\n", t);

	return 0;
}
