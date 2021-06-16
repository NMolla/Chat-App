/*
  wrapper.h
 */

#include "bounded.h"
#include <pthread.h>

typedef struct {
    BOUNDED bounded;
    pthread_mutex_t mutex;   // mutex to protect the queue
    pthread_cond_t not_full; // wait here if queue is full
    pthread_cond_t not_empty; // wait here if queue is empty
} SAFE_BOUNDED;

// Initialize the bounded queue to the given size
void S_bounded_init(SAFE_BOUNDED *q, int size);

// Display the meta-data and contents of the queue.
void S_bounded_display(SAFE_BOUNDED *q);

// Adds an item to the buffer. Will wait if no room
//void S_bounded_put(SAFE_BOUNDED *q, int value); //1
void S_bounded_put(SAFE_BOUNDED *q, long long value);

// Removes an item from the buffer, returning the value. 
// Will wait if no room
//int S_bounded_get(SAFE_BOUNDED *q); //2
long long S_bounded_get(SAFE_BOUNDED *q);

// If we're done with the queue, we should free the dynamic array.
void S_bounded_cleanup(SAFE_BOUNDED *q);
