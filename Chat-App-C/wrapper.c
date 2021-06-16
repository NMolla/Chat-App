/*
  Thread safe wrapper for 
  Bounded queue implemented as fixed sized array.
  put and get will wait as needed

  No error checking!
 */

#include "wrapper.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

// Initialize the bounded queue to the given size
void S_bounded_init(SAFE_BOUNDED *q, int size) {
    // Delegate to BOUNDED's init
    bounded_init(&q->bounded, size);
    // And initialize our mutex
    pthread_mutex_init(&q->mutex, NULL);
    // Dynamically initialize the condition variable
    pthread_cond_init(&q->not_empty, NULL);
    // Dynamically initialize the condition variable
    pthread_cond_init(&q->not_full, NULL);   
}

// Display the meta-data and contents of the queue.
void S_bounded_display(SAFE_BOUNDED *q) {
    // Acquire our lock
    pthread_mutex_lock(&q->mutex);

    // Delegate to BOUNDED's display
    bounded_display(&q->bounded);
    
    // Release the lock
    pthread_mutex_unlock(&q->mutex);
}

// Adds an item to the buffer. Will wait if no room
//void S_bounded_put(SAFE_BOUNDED *q, int value) {
void S_bounded_put(SAFE_BOUNDED *q, long long value) {
    // Acquire our lock
    pthread_mutex_lock(&q->mutex);

    // Wait if needed till there is space
    while (q->bounded.size == q->bounded.count) {
        pthread_cond_wait(&q->not_full, &q->mutex);
    }

    // Delegate to BOUNDED's put
    bounded_put(&q->bounded, value);

    //     End of critical region
    // Wake up anyone who was waiting because we were empty.
    pthread_cond_broadcast(&q->not_empty);
    // And allow them entry to the critical region
    pthread_mutex_unlock(&q->mutex);
}

// Removes an item from the buffer, returning the value. 
// Will wait if no room. Returns -1 if lock fails.
//int S_bounded_get(SAFE_BOUNDED *q) {
long long S_bounded_get(SAFE_BOUNDED *q) {
    // Acquire our lock
    pthread_mutex_lock(&q->mutex);

    // Wait for space as needed
    while (q->bounded.count == 0) {
        pthread_cond_wait(&q->not_empty, &q->mutex);
    }

    // Delegate to BOUNDED's get
    long long value;
    bounded_get(&q->bounded, &value);
    
    //void** value_ptr=NULL;
    //bounded_get(&q->bounded, value_ptr);

    //     End of critical region
    // Wake up anyone who was waiting because we were empty.
    pthread_cond_broadcast(&q->not_full);
    // And allow them entry to the critical region
    pthread_mutex_unlock(&q->mutex);

    return value;
}

// If we're done with the queue, we should free the dynamic array.
void S_bounded_cleanup(SAFE_BOUNDED *q) {
    // Delegate to BOUNDED's cleanup
    bounded_cleanup(&q->bounded);

    // And cleanup our mutex
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->not_empty);
    pthread_cond_destroy(&q->not_full);
}
