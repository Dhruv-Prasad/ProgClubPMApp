#include "task_queue.h"
#include <stdlib.h>
#include <string.h> // For memcpy

void queue_initialize(queue *q) // Queue constructor function
{
    q->num_of_objects = 0;
    q->tail = NULL;
}

void queue_add(queue *q, const task *data_vp) // Adds an object holding data_vp to the end
{
    // Allocate memory for the object to be added
    object_in_queue *new_object = (object_in_queue *)malloc(sizeof(object_in_queue));
    new_object->data = malloc(sizeof(task));

    // Copy the void pointer to the new object's data member
    memcpy(new_object->data, data_vp, sizeof(task));

    // Attach the new object to the queue
    if(q->num_of_objects == 0) // The new object is both the first and last object if it is the only one
    {
        q->tail = new_object; // Set tail
        q->tail->next = new_object; // Set head
    }
    else
    {
        new_object->next = q->tail->next; // Set head
        q->tail->next = new_object; // Point second last object's next member to the new object
        q->tail = new_object; // Set tail
    }

    q->num_of_objects++;
}

int queue_pop(queue *q, task *return_p) // Pops the first object in queue and copies it's data to return_vp
{
    if(q->num_of_objects > 0) // Checks if queue is empty
    {
        object_in_queue *temp = q->tail->next; // Set temp to current head
        memcpy(return_p, temp->data, sizeof(task)); // Copy the data of current head to return_vp
        // Set head to the second object, effectively popping the current head
        if(q->num_of_objects > 1)
        {
            q->tail->next = q->tail->next->next;
        }
        else // If there is only one object...
        {
            q->tail->next = NULL;
            q->tail = NULL;
        }

        q->num_of_objects--;

        // Free the popped out object
        free(temp->data);
        free(temp);
        return 0;
    }
    else // For errpr checking, if the queue is empty
    {
        return -1;
    }
}

void queue_clear(queue *q) // Queue destructor function
{
    object_in_queue *temp;

    while(q->num_of_objects > 0) // Free all the objects
    {
        temp = q->tail->next; // Set temp to current head
        q->tail->next = temp->next; // Set head to second object
        // Free the head
        free(temp->data);
        free(temp);
        q->num_of_objects--;
    }

    q->tail->next = NULL;
    q->tail = NULL;
}

long long int queue_size(const queue *q) // Returns the num_of_objects
{
    return q->num_of_objects;
}