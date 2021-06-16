/*
  Bounded queue implemented as fixed sized array.
  header file
  No threading support

  -jbs
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

// A fixed sized queue as a dynamic array
typedef struct {
    int count;  // # of elements in queue
    int front;  // index of next element to remove
    int next;   // index of next location to add to
    int size;   // capacity of the queue
    long long *data;  // array to hold queue's data
} BOUNDED;

// Initialize the bounded queue to the given size
void bounded_init(BOUNDED *q, int size);

// Display the meta-data and contents of the queue.
void bounded_display(BOUNDED *q);

// Attempt to add an item to the buffer
// returns true if successful, and false if there was no room
//bool bounded_put(BOUNDED *q, int value);
bool bounded_put(BOUNDED *q, long long value);

// Attempt to remove an item from the buffer, storing it where value points.
// returns true if successful, and false if there was nothing there.
bool bounded_get(BOUNDED *q, long long* value);

// If we're done with the queue, we should free the dynamic array.
void bounded_cleanup(BOUNDED *q);
