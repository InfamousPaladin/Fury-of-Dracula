// The following program file has adapted for use in this assignment from the
// COMP2521 labs. It was written by UNSW staff and for the purpose of the 
// assignment, and has been used to utilise as an ADT.

// Queue.h has been adapted from COMP2521 lab05 2020T2.


// Queue.h; // interface to Queue ADT
// Written by John Shepherd, March 2013

#ifndef QUEUE_H
#define QUEUE_H

#include "Item.h"

typedef struct QueueRep *Queue;

Queue newQueue(); // create new empty queue
void dropQueue(Queue); // free memory used by queue
void showQueue(Queue); // display as 3 > 5 > 4 > ...
void QueueJoin(Queue,Item); // add item on queue
Item QueueLeave(Queue); // remove item from queue
int QueueIsEmpty(Queue); // check for no items

#endif
