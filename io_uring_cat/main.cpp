#include <cstring> //for memset
#include <cstdlib> //for exit

#include <stdio.h> //perror and printf
#include <unistd.h> //also needed for syscall stuff

#include <sys/stat.h> //fstat
#include <sys/uio.h> //readv/writev
#include <sys/ioctl.h> //ioctl
#include <sys/syscall.h> //syscall stuff parameters (as in like __NR_io_uring_enter/__NR_io_uring_setup)
#include <sys/mman.h> //for mmap

#include <linux/io_uring.h> //for the io_uring structs

typedef struct stat stat;

#define BLOCK_SIZE 4096
#define QD 1 //queue depth, how many requests can be submitted in one call

//memory barriers, prevents compiler from reordering instructions before the barrier to be after it and vice versa
#define read_barrier() __asm__ __volatile__("":::"memory")
#define write_barrier() __asm__ __volatile__("":::"memory")

//structs

//https://kernel.dk/io_uring.pdf
//For the CQ ring, the array of cqes is a part of the CQ ring itself. Since the SQ ring is an index of 
//values into the sqe array, the sqe array must be mapped separately by the application.

//https://unixism.net/2020/04/io-uring-by-example-part-1-introduction/
//While the completion queue ring directly indexes the shared array of CQEs, the submission ring has
//an indirection array in between. The submission side ring buffer is an index into this array, which
//in turn contains the index into the SQEs. This is useful for certain applications that embed submission
//requests inside of internal data structures. This setup allows them to submit multiple submission
//entries in one go while allowing them to adopt io_uring more easily.

// (Good example)
// ## i.e sqes[ sqring->array[ index ] ], where index = sqring->tail & (*sqring->ring_mask), so the ring buffer
// ## is an index into the sqring array/indirection array, which in turn contains the index into the SQEs

//ring mask with the head for cq ring and with the tail for sq ring is used to get the correct 
//index into the cqe to reap or the sqe to add events respectively

struct app_io_submission_queue_ring {
  unsigned *head;
  unsigned *tail;
  unsigned *ring_mask;
  unsigned *ring_entries;
  unsigned *flags;
  unsigned *array; //this is the indirection array
};

struct app_io_completion_queue_ring {
  unsigned *head;
  unsigned *tail;
  unsigned *ring_mask;
  unsigned *ring_entries;
};

struct submitter {
  int ring_fd;
  app_io_submission_queue_ring submission_queue_ring;
  io_uring_sqe *sqes;
  app_io_completion_queue_ring completion_queue_ring;
};

struct file_info {
  int file_size;
  iovec iovecs[]; //for readv/writev
};

//syscall wrappers

int io_uring_setup(unsigned entries, io_uring_params *params){
  return (int) syscall(__NR_io_uring_setup, entries, params);
}

int io_uring_enter(int ring_fd, unsigned ready_to_be_submitted_SQEs, unsigned how_many_to_wait_for_min, unsigned flags_to_modify_behaviour){
  return (int) syscall(__NR_io_uring_enter, ring_fd, ready_to_be_submitted_SQEs, how_many_to_wait_for_min, flags_to_modify_behaviour, NULL); //final param is for the signal mask - what signals to block is called the signal mask
}

//get file size
int get_file_size(int fd){
  stat file_stat;

  if(fstat(fd, &file_stat) < 0){
    perror("fstat get_file_size");
    exit(1);
  }

  if(S_ISBLK(file_stat.st_mode)){ //true if the file is a block special file (a device like a disk)
    unsigned long long size_bytes;
    if(ioctl(fd, BLKGETSIZE64, &size_bytes) != 0){ //gives size of a disk in bytes as opposed to returning number of 512 byte blocks, which is what BLKGETSIZE does instead of BLKGETSIZE64
      perror("ioctl get_file_size");
      return -1;
    }

    return size_bytes;
  }else if(S_ISREG(file_stat.st_mode)){ //if it's a regular file, just returns size in bytes using file_stat.st_size
    return file_stat.st_size;
  }

  return -1;
}

int app_setup_io_uring(submitter *sub){
  app_io_submission_queue_ring *s_ring = &sub->submission_queue_ring;
  app_io_completion_queue_ring *c_ring = &sub->completion_queue_ring;
  
  io_uring_params params;
  void *sq_ptr, *cq_ptr; //for the return of mmap

  std::memset(&params, 0, sizeof(params)); //initialises params, could set some flags now, but don't in this case

  sub->ring_fd = io_uring_setup(QD, &params);

  //sq_entries and cq_entries are filled out by the kernel, telling us how many sqe entries the ring supports, cq_entries tells us how big the CQ ring is
  
  //both of these use the offset of the final entry in their offset structures + the sizes of the arrays in those structures (be it the indirection or cqe array)
  //to determine how much memory is needed to be mapped into the application memory space
  int s_ring_size = params.sq_off.array + params.sq_entries * sizeof(unsigned);
  int c_ring_size = params.cq_off.cqes + params.cq_entries * sizeof(io_uring_cqe);

  if(params.features & IORING_FEAT_SINGLE_MMAP){ //possible to map submission and complextion buffers with a single mmap call since Linux kernel version 5.4
    //so this just maximises and equates the sizes, to ensure enough is available for both
    if(c_ring_size > s_ring_size)
      s_ring_size = c_ring_size;
    
    c_ring_size = s_ring_size;
  }
  
  sq_ptr = mmap(0, s_ring_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, sub->ring_fd, IORING_OFF_SQ_RING);
  //1st param allows the kernel to choose at which page aligned address to create the mapping
  //2nd param is the length of the mapping, the 3rd param is saying that pages may be read or written,
  //the 4th param says this mapping is visible to other processes mapping the same region, 5th param is the fd/file descriptor which is being mapped in
  //the 6th param is the offset inside the object that's being mapped, at which point the mmap'ing starts,
  //so the region in the object `offset` to `offset+length` is mapped in - in this case that is 0
}

int main(){
  
}