/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   utils.h
 * Author: pintos
 *
 * Created on November 10, 2017, 6:20 PM
 */

#ifndef UTILS_H
#define UTILS_H


void utils_donate_priority(struct lock *);


void utils_check_priority(struct lock *);
void utils_set_priority(int);
bool utils_compare_priority (const struct list_elem *a,
                             const struct list_elem *b,
                             void *aux);
bool utils_compare_sem_priority (const struct list_elem *a,
                             const struct list_elem *b,
                             void *aux);
bool utils_compare_lock_priority(const struct list_elem *a,
                             const struct list_elem *b,
                             void *aux UNUSED); 
void utils_check_donation(struct lock *l, int frontp, int backp);
#endif /* UTILS_H */

