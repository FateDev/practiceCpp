#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/io_uring.h>

#define BLOCK_SZ 4096
#define QD 1

//memory barriers, prevents compiler from reordering instructions before the barrier to be after it and vice versa
#define read_barrier() __asm__ __volatile__("":::"memory")
#define write_barrier() __asm__ __volatile__("":::"memory")

int io_uring_setup(unsigned entries, io_uring_params *params){
  return (int) syscall(__NR_io_uring_setup, entries, params);
}

int io_uring_enter(int ringFd, unsigned readyToBeSubmittedSQEs, unsigned howManyToWaitForMin, unsigned flagsToModifyBehaviour){
  return (int) syscall(__NR_io_uring_enter, ringFd, readyToBeSubmittedSQEs, howManyToWaitForMin, flagsToModifyBehaviour, NULL); //final param is for the signal mask - what signals to block is called the signal mask
}

