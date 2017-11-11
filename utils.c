
#include <stdio.h>
#include <string.h>

#include "threads/lock.h"
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/utils.h"
/*
 * Utility/helper file for lock and threads
 */
void
utils_donate_priority(struct lock *lock) {
    struct thread *cur = thread_current();
  
    //struct thread *lock_donated = lock->holder->donatedTo;
    
    struct lock *donated = NULL;
    lock->donated = true;
    lock->holder->lowered = true;
    


    if(lock->priority == 0){

        lock->priority = lock->holder->priority;
    }
    
    lock->d_priority = cur->priority;
    lock->holder->priority = cur->priority;
    cur->lock = lock;
    donated = cur->lock;


    //chain donation
    while(donated->holder != NULL && lock->holder != NULL) {
        donated->holder->priority = lock->holder->priority;

        donated = donated->holder->lock;


    }
}

void utils_check_priority(struct lock *l){
    int frontp = 0;
    int backp = 0;
    if(!list_empty(&thread_current()->d_list)) {
        struct lock *lock2 = list_entry( list_back(&(thread_current()->d_list)), struct lock, donation_elem);
        struct lock *lock3 = list_entry( list_front(&(thread_current()->d_list)), struct lock, donation_elem);
        frontp = lock2->priority;
        backp = lock3->priority;
    }
    list_remove(&l->donation_elem);
 
    l->holder->priority = l->priority;
    utils_check_donation(l,frontp, backp);
}
void utils_check_donation(struct lock *l, int frontp, int backp) {
    if(list_empty(&l->holder->d_list)) {      
        if(l->holder->orig_priority < l->holder->priority) 
           l->holder->priority = l->holder->orig_priority;     
    } else {
       struct lock *lock = list_entry( list_front(&thread_current()->d_list), struct lock, donation_elem);
       if(lock->d_priority > l->holder->priority ) {    
           if(frontp != backp) {
               l->holder->priority = lock->d_priority;
           }                  
       }        
    }
}
void utils_set_priority(int new_priority){
    int old_priority = thread_current()->priority;
    thread_current()->real_priority = new_priority;
    thread_current()->priority = new_priority;

    if(old_priority > thread_current()->priority) {

        if(!list_empty(&thread_current()->d_list)) {

            struct lock *t = list_entry(list_front(&thread_current()->d_list),
                struct lock, donation_elem);
            //printf("priority: %d\n", t->priority);
            if(t->priority > thread_current()->priority)
                thread_current()->priority = t->priority;

        }
    }
    int current_priority = thread_current()->priority;

    if(!list_empty(&ready_list)) {
        struct thread *t = list_entry(list_back(&ready_list), struct thread, elem);
        if(current_priority < t->priority)
            thread_yield();

    }
}


bool utils_compare_priority(const struct list_elem *a,
                             const struct list_elem *b,
                             void *aux UNUSED) {
    struct thread *aa = list_entry(a, struct thread, elem);
    struct thread *bb = list_entry(b, struct thread, elem);
    return aa->priority > bb->priority;
}
bool utils_compare_sem_priority(const struct list_elem *a,
                             const struct list_elem *b,
                             void *aux UNUSED) {
    struct semaphore *athread = list_entry(a, struct semaphore,elem); 
   
    struct semaphore *bthread = list_entry(b, struct semaphore,elem);

    return athread->priority > bthread->priority;
}
bool
utils_compare_lock_priority(const struct list_elem *a,
                             const struct list_elem *b,
                             void *aux UNUSED) {
    struct lock *aa = list_entry(a, struct lock, donation_elem);
    struct lock *bb = list_entry(b, struct lock, donation_elem);
    return aa->priority > bb->priority;
}