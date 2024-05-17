// Header file
// Queue is first in first out
// It holds 'task', a struct with arguments required for my matrix multiplication function

#ifndef TASK_QUEUE_H_INCLUDED // Include guard
#define TASK_QUEUE_H_INCLUDED

#include <stddef.h>

typedef struct task // Struct with arguments required for my matrix multiplication function
{
  long long int *m1ptr; // Pointer to first matrix
  long long int *m2ptr; // Pointer to second matrix
  long long int *mptr; // Pointer to product matrix
  long long int N; // Number of rows/columns in the square matrix
  long long int num_of_structs; // Number of parts the matrix has been broken into
  long long int start; // Index of first element to be calculated by thread
  long long int end; // Index of last element to be calculated by thread
}
task;

typedef struct object_in_queue // Typedef the queue member object so I don't need to always type struct
{
  task *data; // Pointer to the task
  struct object_in_queue *next; // pointer to next object
}
object_in_queue;

typedef struct queue // The actual queue
{
  int num_of_objects;
  object_in_queue *tail; // Pointer to the last object in queue
}
queue;

// I have implemented the queue such that the tail object's next points to the first object in the queue

void queue_initialize(queue *q); // Queue constructor function

void queue_add(queue *q, const task *data_vp); // Adds an object holding data_vp to the end

int queue_pop(queue *q, task *return_p); // Pops the first object in queue and copies it's data to return_vp

void queue_clear(queue *q); // Queue destructor function

long long int queue_size(const queue *q); // Returns the num_of_objects

#include "task_queue.c" // Include the implementation file

#endif