
#ifndef MYTHREAD_MUTEX_H
#define MYTHREAD_MUTEX_H

#include "myqueue.h"

typedef struct mythread_mutex {
   int lock;          
   mythread_queue_t q;
}*mythread_mutex_t;
typedef struct mythread_mutex_attr
{
	int value;
}*mythread_mutex_attr_t;

extern int mythread_mutex_init(mythread_mutex_t *mutex,const mythread_mutex_attr_t attr);
extern int mythread_mutex_lock(mythread_mutex_t *mutex);
extern int mythread_mutex_unlock(mythread_mutex_t *mutex);
extern int mythread_mutex_destroy(mythread_mutex_t *mutex);
#endif