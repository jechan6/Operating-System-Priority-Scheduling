/* 
 * This file is derived from source code for the Pintos
 * instructional operating system which is itself derived
 * from the Nachos instructional operating system. The 
 * Nachos copyright notice is reproduced in full below. 
 *
 * Copyright (C) 1992-1996 The Regents of the University of California.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose, without fee, and
 * without written agreement is hereby granted, provided that the
 * above copyright notice and the following two paragraphs appear
 * in all copies of this software.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO
 * ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE
 * AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF CALIFORNIA
 * HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
 * BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
 * MODIFICATIONS.
 *
 * Modifications Copyright (C) 2017 David C. Harrison. All rights reserved.
 */

#include <stdio.h>
#include <string.h>

#include "threads/lock.h"
#include "threads/interrupt.h"
#include "threads/thread.h"

/* 
 * Initializes LOCK.  A lock can be held by at most a single
 * thread at any given time.  Our locks are not "recursive", that
 * is, it is an error for the thread currently holding a lock to
 * try to acquire that lock.
 *
 * A lock is a specialization of a semaphore with an initial
 * value of 1.  The difference between a lock and such a
 * semaphore is twofold.  First, a semaphore can have a value
 * greater than 1, but a lock can only be owned by a single
 * thread at a time.  Second, a semaphore does not have an owner,
 * meaning that one thread can "down" the semaphore and then
 * another one "up" it, but with a lock the same thread must both
 * acquire and release it.  When these restrictions prove
 * onerous, it's a good sign that a semaphore should be used,
 * instead of a lock. 
 */
void
lock_init(struct lock *lock)
{
    ASSERT(lock != NULL);

    lock->holder = NULL;
    lock->donated = false;
    
    lock->priority = 0;
    semaphore_init(&lock->semaphore, 1);
}

compare_lock_priority(const struct list_elem *a,
                             const struct list_elem *b,
                             void *aux UNUSED) {
    struct lock *aa = list_entry(a, struct lock, donation_elem);
    struct lock *bb = list_entry(b, struct lock, donation_elem);
    return aa->priority > bb->priority;
}

/* 
 * Acquires LOCK, sleeping until it becomes available if
 * necessary.  The lock must not already be held by the current
 * thread.
 *
 * This function may sleep, so it must not be called within an
 * interrupt handler.  This function may be called with
 * interrupts disabled, but interrupts will be turned back on if
 * we need to sleep. 
 */
void
lock_acquire(struct lock *lock)
{
    ASSERT(lock != NULL);
    ASSERT(!intr_context());
    ASSERT(!lock_held_by_current_thread(lock));
    
    if(lock->holder != NULL ){
        
        lock->donated = true;
        lock->holder->lowered = true;
        //printf("priority: %d\n", lock->holder->priority);
        //printf("size: %d\n", list_size(&lock->holder->d_list));
        if(lock->priority == 0){
          
            lock->priority = lock->holder->priority;
        }
   
        lock->d_priority = thread_current()->priority;
        lock->holder->priority = thread_current()->priority;
        thread_current()->lock = lock;
        list_insert_ordered(&lock->holder->d_list,&lock->donation_elem,compare_lock_priority ,NULL);
        
     
        //list_push_back(&lock->holder->d_list,&lock->donation_elem);
        //list_sort(&lock->holder->d_list);
        
        
        
        //donate_priority();                          0['              
    }
  
    semaphore_down(&lock->semaphore);
    thread_current()->lock= NULL;
      
    
    lock->holder = thread_current();

}

/* 
 * Tries to acquires LOCK and returns true if successful or false
 * on failure.  The lock must not already be held by the current
 * thread.
 *
 * This function will not sleep, so it may be called within an
 * interrupt handler. 
 */
bool
lock_try_acquire(struct lock *lock)
{
    ASSERT(lock != NULL);
    ASSERT(!lock_held_by_current_thread(lock));

    bool success = semaphore_try_down(&lock->semaphore);
    if (success) {
        
        lock->holder = thread_current();
    }
    return success;
}
void check_priority(struct lock *l){
    int frontp;
    int backp;
    if(!list_empty(&thread_current()->d_list)) {
        struct lock *lock2 = list_entry( list_back(&(thread_current()->d_list)), struct lock, donation_elem);
        struct lock *lock3 = list_entry( list_front(&(thread_current()->d_list)), struct lock, donation_elem);
        frontp = lock2->priority;
        backp = lock3->priority;
        //printf(" ORIG: %d\n", lock2->priority);
       // printf(" ORIG: %d\n", lock3->priority);
    }
//     printf(" ORIG: %d\n", lock2->priority);
//        printf(" old PRIORITY: %d\n", l->priority);
//     printf("HOLDER PRIORITY: %d\n", l->holder->priority);
//    }
    list_remove(&l->donation_elem);
    //int orig = lock2->holder->orig_priority;
        
   
    //list_sort(&l->holder->d_list, compare_lock_priority, NULL);
    //thread_current()->priority = l->priority;
    
    //thread_current()->priority = thread_current()->orig_priority;
    l->holder->priority = l->priority;
    if(list_empty(&l->holder->d_list)) {
        //printf(" PRIORITY: %d\n",l->holder->orig_priority);
        
        if(l->holder->orig_priority < l->holder->priority) 
           l->holder->priority = l->holder->orig_priority;
       
        
        
    } else {
        //list_sort(&l->holder->d_list, compare_lock_priority, NULL);
       struct lock *lock = list_entry( list_front(&thread_current()->d_list), struct lock, donation_elem);
    
       
       //printf(" D PRIORITY: %d\n", lock->d_priority);
       //printf("SIZE: %zu\n", list_size(&l->holder->d_list));
       //list_sort(&l->holder->d_list, compare_lock_priority, NULL);
       if(lock->d_priority > l->holder->priority ) {
            //printf("SIZE: %zu\n", list_size(&thread_current()->d_list));
          //printf("cur PRIORITY: %d\n", thread_current()->priority);
          // printf(" PRIORITY: %s\n",thread_current()->name);
           if(frontp != backp) {
               l->holder->priority = lock->d_priority;
           }
          
          //printf("CURRENT PRIORITY: %d\n", lock->d_priority);
            //thread_current()->priority = lock->priority;
       } 
        //printf("CURRENT PRIORITY: %d\n", thread_current()->priority);
        
    }
        //thread_current()->priority = thread_current()->orig_priority;
    
   
    
}
/* 
 * Releases LOCK, which must be owned by the current thread.
 *
 * An interrupt handler cannot acquire a lock, so it does not
 * make sense to try to release a lock within an interrupt
 * handler. 
 */
void
lock_release(struct lock *lock)
{
    ASSERT(lock != NULL);
    ASSERT(lock_held_by_current_thread(lock));
    struct thread *t = thread_current();
    enum intr_level old_level = intr_disable();
    if(lock->donated ) {
        
        
        lock->donated = false;

       
       // if(lock->holder->real_priority != 0) {
        //printf("LOCK PRIORITY: %d\n", lock->holder->priority);
        //lock->d_priority = lock->holder->priority;
        //release_lock(lock);
       
        //printf(" current priorit: %d\n", lock->holder->priority);
         //lock->holder->priority = lock->priority;
        
        check_priority(lock);
        //list_remove(&lock->donation_elem);
        //list_remove(&lock->donation_elem);
       
      
        
        
      
       // } 
        if(lock->holder->lowered) {
            lock->holder->lowered = false;
            if(lock->holder->real_priority != 0)
                lock->holder->priority = lock->holder->real_priority;
        }
       
     
        //release_priority();
        
    } 
    //printf("CURRENT PRIORITY: %zu\n", list_size(&lock->holder->d_list));
    lock->holder = NULL;
    
 

    semaphore_up(&lock->semaphore);
    intr_set_level(old_level);
  
}

/* 
 * Returns true if the current thread holds LOCK, false otherwise.  
 * Note that testing whether some other thread holds a lock would be racy. 
 */
bool
lock_held_by_current_thread(const struct lock *lock)
{
    ASSERT(lock != NULL);
    return lock->holder == thread_current();
}
