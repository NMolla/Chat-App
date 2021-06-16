/*
  Bounded queue implemented as fixed sized array.
  No threading support
 */

#include "bounded.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// Initialize the bounede queue to the given size
void bounded_init(BOUNDED *q, int size) {
    q->data = malloc(size * sizeof(long long));     // Allocate the array
    q->size = size;  // Remember the size    
    q->count = 0;    // Note that it is empty
    q->front = 0;    // The position to find the front element, if there is one.
    q->next = 0;     // The position to store the next item, if there is space.
}

// Display the meta-data and contents of the queue.
void bounded_display(BOUNDED *q) {
    fprintf(stderr, "size: %d, count: %d, front: %d, next: %d\n",
            q->size, q->count, q->front, q->next);
    int pos = q->front;
    for (int i = 0; i < q->count; ++i) {
        fprintf(stderr, "%lld ", q->data[pos]);
        if (++pos == q->size) pos = 0;
    }
    fprintf(stderr, "\n");
}

// Attempt to add an item to the buffer
// returns true if successful, and false if there was no room
bool bounded_put(BOUNDED *q, long long value) {
    // "If" the queue is full, return false
    if (q->size == q->count) return false;

    // Ok, there is space.  Store it.
    q->data[q->next] = value;
    //    and bump next
    ++q->next;
    //    but wrap if we fall off the end
    if (q->next == q->size) q->next = 0;
    //    and finally, bump the count
    ++q->count;

    return true;
}

// Attempt to remove an item from the buffer, storing it where value points.
// returns true if successful, and false if there was nothing there.
bool bounded_get(BOUNDED *q, long long* value) {
    // Return false if the queue is empty
    if (q->count == 0) return false;

    *value = q->data[q->front];
    //q->data[q->front] = -1;
    ++q->front;
    if (q->front == q->size) q->front = 0;
    --q->count;
    return true;
}

// If we're done with the queue, we should free the dynamic array.
void bounded_cleanup(BOUNDED *q) {
    free(q->data);
}
