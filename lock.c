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
    semaphore_init(&lock->semaphore, 1);
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

    semaphore_down(&lock->semaphore);
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

    lock->holder = NULL;
    semaphore_up(&lock->semaphore);
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
