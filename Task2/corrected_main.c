#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

volatile int counter = 0;
int num_increments = 0;
pthread_mutex_t mutex;

void *increment_counter(void *arg) 
{
    for (int i = 0; i < num_increments; i++) 
    {
        pthread_mutex_lock(&mutex);
        counter++;
        pthread_mutex_unlock(&mutex);
    }
}

int main(int argc, char *argv[]) 
{
    if (argc != 2) 
    {
        fprintf(stderr, "Usage: %s <num_increments>\n", argv[0]);
        return EXIT_FAILURE;
    }

    num_increments = atoi(argv[1]);

    pthread_t thread1, thread2;

    pthread_create(&thread1, NULL, increment_counter, &num_increments);
    pthread_create(&thread2, NULL, increment_counter, &num_increments);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("Final counter value: %d\n", counter);

    return 0;
}