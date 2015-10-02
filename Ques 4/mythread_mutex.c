#include <stdlib.h>
#include <stdio.h>
#include "mythread_mutex.h"
#include "mythread.h"

int mythread_mutex_init(mythread_mutex_t *mutex,const mythread_mutex_attr_t attr){
    (*mutex) = (mythread_mutex_t)malloc(sizeof(struct mythread_mutex));
    (*mutex)->lock = 0;
    (*mutex)->q = NULL;
    return 0;
}

int mythread_mutex_lock(mythread_mutex_t *mutex){
	mythread_enter_kernel();
    while(1)
    {
        if (mutex != NULL) {
            int x = 0;
            int *lock = &(*mutex)->lock;
            if(*lock == 1 && (*mutex)->q != NULL) 
            {
                mythread_block(&(*mutex)->q, 0);
            }   
            else
            {
                while (*lock == 1 && x < 50)
                   x++;
                if (compare_and_swap(lock, 1, 0) == 0) {
                    mythread_leave_kernel();
                return 0;
                }
                
                mythread_block(&(*mutex)->q,0);
            }
        }
        else {
            mythread_leave_kernel();
            return -1;
        }
    }

}
int mythread_mutex_unlock(mythread_mutex_t *mutex){
	mythread_enter_kernel();
    if ((*mutex)->q == NULL)
    {
	   (*mutex)->lock = 0;
    }
    else {
        (*mutex)->lock = 0;
        mythread_unblock(&(*mutex)->q,0);
    }
    mythread_leave_kernel();
    return 0;
}

int mythread_mutex_destroy(mythread_mutex_t *mutex){
	mythread_enter_kernel();
	free(*mutex);
    mythread_leave_kernel();
    return 0;
}

