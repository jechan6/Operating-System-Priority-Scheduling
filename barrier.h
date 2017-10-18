#ifndef BARRIER_H
#define BARRIER_H

/* Optimization barrier. */
#define barrier() asm volatile ("" : : : "memory")

#endif /* UCSC CMPS111 */
