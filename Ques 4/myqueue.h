/*
 * myqueue.h -- interface for queue ops
 */
#ifndef _MYQUEUE_H_
#define _MYQUEUE_H_

#include <malloc.h>
#include <stdio.h>
 struct mythread_queue;

typedef struct mythread_queue {
  void *item;
  struct mythread_queue *prev, *next;
} *mythread_queue_t;

#endif