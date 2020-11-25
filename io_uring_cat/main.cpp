#include <cstring> //for memset
#include <cstdlib> //for exit

#include <stdio.h> //perror and printf
#include <unistd.h> //also needed for syscall stuff

#include <sys/stat.h> //fstat
#include <sys/uio.h> //readv/writev
#include <sys/ioctl.h> //ioctl
#include <sys/syscall.h> //syscall stuff parameters (as in like __NR_io_uring_enter/__NR_io_uring_setup)
#include <sys/mman.h> //for mmap

//the 3 below are needed for open and O_RDONLY, but more stuff too in this program (maybe) - the man page says to include these
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <linux/io_uring.h> //for the io_uring structs

typedef struct stat stat_struct;
typedef uint uint;

#define BLOCK_SIZE_IN_PROGRAM 4096
#define QUEUE_DEPTH 1 //queue depth, how many requests can be submitted in one call

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

////I think what this means is, you could pass literally any old array as the indirection array, mess around with it as much as you want, and then submit it whenever
////you feel like you're done, since that might require less code, and so be a bit simpler

// (Good example)
// ## i.e sqes[ sqring->array[ index ] ], where index = sqring->tail & (*sqring->ring_mask), so the ring buffer
// ## is an index into the sqring array/indirection array, which in turn contains the index into the SQEs

//ring mask with the head for cq ring and with the tail for sq ring is used to get the correct 
//index into the cqe to reap or the sqe to add events respectively

struct app_io_submission_queue_ring {
  uint *head;
  uint *tail;
  uint *ring_mask;
  uint *ring_entries;
  uint *flags;
  uint *array; //this is the indirection array
};

struct app_io_completion_queue_ring {
  uint *head;
  uint *tail;
  uint *ring_mask;
  uint *ring_entries;
  io_uring_cqe *cqes;
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

int io_uring_setup(uint entries, io_uring_params *params){
  return (int) syscall(__NR_io_uring_setup, entries, params);
}

int io_uring_enter(int ring_fd, uint ready_to_be_submitted_SQEs, uint how_many_to_wait_for_min, uint flags_to_modify_behaviour){
  return (int) syscall(__NR_io_uring_enter, ring_fd, ready_to_be_submitted_SQEs, how_many_to_wait_for_min, flags_to_modify_behaviour, NULL); //final param is for the signal mask - what signals to block is called the signal mask
}

//get file size
int get_file_size(int fd){
  stat_struct file_stat;

  if(fstat(fd, &file_stat) < 0){
    perror("fstat get_file_size");
    exit(1);
  }

  if(S_ISBLK(file_stat.st_mode)){ //true if the file is a block special file (a device like a disk)
    uint long long size_bytes;
    if(ioctl(fd, BLKGETSIZE64, &size_bytes) != 0){ //gives size of a disk in bytes as opposed to returning number of 512 byte blocks, which is what BLKGETSIZE does instead of BLKGETSIZE64
      perror("ioctl get_file_size");
      exit(-1);
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
  char *sq_ptr, *cq_ptr; //for the return of mmap

  std::memset(&params, 0, sizeof(params)); //initialises params, could set some flags now, but don't in this case

  sub->ring_fd = io_uring_setup(QUEUE_DEPTH, &params);
  if(sub->ring_fd < 0){
      perror("io_uring_setup");
      exit(1);
  }
  //sq_entries and cq_entries are filled out by the kernel, telling us how many sqe entries the ring supports, cq_entries tells us how big the CQ ring is
  
  //both of these use the offset of the final entry in their offset structures + the sizes of the arrays in those structures (be it the indirection or cqe array)
  //to determine how much memory is needed to be mapped into the application memory space
  int s_ring_size = params.sq_off.array + params.sq_entries * sizeof(uint);
  int c_ring_size = params.cq_off.cqes + params.cq_entries * sizeof(io_uring_cqe);

  if(params.features & IORING_FEAT_SINGLE_MMAP){ //possible to map submission and complextion buffers with a single mmap call since Linux kernel version 5.4
    //so this just maximises and equates the sizes, to ensure enough is available for both
    if(c_ring_size > s_ring_size)
      s_ring_size = c_ring_size;
    
    c_ring_size = s_ring_size;
  }
  
  sq_ptr = (char*)mmap(0, s_ring_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, sub->ring_fd, IORING_OFF_SQ_RING);
  //1st param allows the kernel to choose at which page aligned address to create the mapping
  //2nd param is the length of the mapping, the 3rd param is saying that pages may be read or written,
  //the 4th param says this mapping is visible to other processes mapping the same region, 5th param is the fd/file descriptor which is being mapped in
  //the 6th param is the offset inside the object that's being mapped, at which point the mmap'ing starts,
  //so the region in the object `offset` to `offset+length` is mapped in - in this case the offset is 0

  if(sq_ptr == MAP_FAILED){
    perror("mmap sq_ptr");
    exit(1);
  }

  if(params.features & IORING_FEAT_SINGLE_MMAP){ //the above about being able to get by with mmap'ing just once
    cq_ptr = sq_ptr;
  }else{
    cq_ptr = (char*)mmap(0, c_ring_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, sub->ring_fd, IORING_OFF_CQ_RING);
    if(cq_ptr == MAP_FAILED){
      perror("mmap cq_ptr");
      exit(1);
    }
  }

  //put the correct offsets for the mapped fields into our submission queue struct, and cast from char to uint
  s_ring->head = (uint*)(sq_ptr + params.sq_off.head);
  s_ring->tail = (uint*)(sq_ptr + params.sq_off.tail);
  s_ring->ring_mask = (uint*)(sq_ptr + params.sq_off.ring_mask);
  s_ring->ring_entries = (uint*)(sq_ptr + params.sq_off.ring_entries);
  s_ring->flags = (uint*)(sq_ptr + params.sq_off.flags);
  s_ring->array = (uint*)(sq_ptr + params.sq_off.array);
  
  //mapping in the region for the submission queue indirection array
  sub->sqes = (struct io_uring_sqe*)mmap(0, params.sq_entries * sizeof(struct io_uring_sqe), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, sub->ring_fd, IORING_OFF_SQES);
  if (sub->sqes == MAP_FAILED) {
    perror("mmap");
    return 1;
  }

  //put the correct offsets for the mapped fields into our completion queue struct, and cast from char to uint
  c_ring->head = (uint*)(cq_ptr + params.cq_off.head);
  c_ring->tail = (uint*)(cq_ptr + params.cq_off.tail);
  c_ring->ring_mask = (uint*)(cq_ptr + params.cq_off.ring_mask);
  c_ring->ring_entries = (uint*)(cq_ptr + params.cq_off.ring_entries);
  c_ring->cqes = (struct io_uring_cqe*)(cq_ptr + params.cq_off.cqes);
  
  return 0; //success
}

void output(char *buffer, int length){ //prints buffer to stdout
  while(length--){
    fputc(*buffer++, stdout);
  }
}

void read_from_completion_queue(submitter *sub){
  file_info *info;
  app_io_completion_queue_ring *c_ring = &sub->completion_queue_ring;
  io_uring_cqe *cqe;
  uint head; //the value of the head of the ring will be in here

  head = *c_ring->head;

  while(true){ //in a ring buffer, head == tail means the buffer is empty
    read_barrier(); //so that read instructions before and after this STAY before and after this even after compilation (compilation could mix things up for optimisation) - (remember read_barrier and write_barrier do the same thing though, in this program anyway, since we define the operations at the top of the file)

    cqe = &c_ring->cqes[head & *sub->completion_queue_ring.ring_mask]; //get the index of the entry
    //the ring buffer has to be a power of two to exploit this trick:
    //ring mask = ring size - 1
    //if ring size is a power of 2, it'll look something like 00100000
    //therefore ring mask would look like this 00011111
    //so when you do head & ring mask, it ensures you never exceed the length of the buffer
    //you can do this with modulo, but that requires division, which is slower
    //subtracting the size of the array whenever head gets too big using an if statement can risk branch misprediction, and also give a performance hit
    //using bitwise AND has a negligible effect on performance

    info = (file_info*)cqe->user_data; //retrieve the user data

    if(cqe->res < 0)
      printf("Error: %s\n", strerror(abs(cqe->res))); //will print the error as a string

    //below 3 lines retrieves the number of blocks correctly, which is also the amount of iovec structures being used (one for each block)
    uint blocks = info->file_size / BLOCK_SIZE_IN_PROGRAM;
    
    if(info->file_size % BLOCK_SIZE_IN_PROGRAM)
      blocks++;

    for(int i = 0; i < blocks; i++)
      output((char*)info->iovecs[i].iov_base, info->iovecs[i].iov_len); //retrieve the pointer to the data, and the length of the data, and output it to the console

    if(head != *c_ring->tail)
      break;

    head++; //increment the local copy of the head index, since we've just consumed an entry in the completion queue
  }
  
  *c_ring->head = head;
  write_barrier();
  //make sure none of the writes after this happen before it via the compiler mixing it up (remember read_barrier and write_barrier do the same thing though, in this program anyway, since we define the operations at the top of the file)
}

int submit_to_submission_queue(char *file_path, submitter *sub){
  file_info *info;

  int fd = open(file_path, O_RDONLY); //open the file, return fd/file descriptor for it
  if(fd < 0){
    perror("open fd");
    exit(1);
  }

  app_io_submission_queue_ring *s_ring = &sub->submission_queue_ring;
  
  int file_size = get_file_size(fd);
  if(file_size < 0){
    perror("file size error");
    exit(1);
  }
  uint bytes_left_to_read = file_size;
  uint blocks = file_size/BLOCK_SIZE_IN_PROGRAM;
  if(file_size % BLOCK_SIZE_IN_PROGRAM) //non 0 remainder means an extra block is required to cover the rest of the data
    blocks++;

  info = (file_info*)malloc(sizeof(file_info) + sizeof(iovec) * blocks); //allocates enough memory for all the iovecs and the file info struct itself
  if(!info){
    fprintf(stderr, "Unable to allocate memory\n");
    exit(1);
  }
  info->file_size = file_size;

  uint current_block = 0;
  while(bytes_left_to_read){
    int bytes_to_read_this_iteration = BLOCK_SIZE_IN_PROGRAM;
    if(bytes_left_to_read < BLOCK_SIZE_IN_PROGRAM){
      bytes_to_read_this_iteration = bytes_left_to_read;
    }
    bytes_left_to_read -= bytes_to_read_this_iteration;

    void *buffer;
    //read the man page for this, basically allocates stuff, but the address of the allocated 
    //memory is a multiple of the second argument (which must be a power of two)
    if(posix_memalign(&buffer, BLOCK_SIZE_IN_PROGRAM, BLOCK_SIZE_IN_PROGRAM)){
      perror("posix_memalign");
      exit(1);
    }

    info->iovecs[current_block].iov_len = bytes_to_read_this_iteration;
    info->iovecs[current_block++].iov_base = buffer;
  }
  
  //now we will add the entry to the tail of the submission queue ring buffer
  uint tail = *s_ring->tail;
  uint next_tail = tail + 1;

  read_barrier(); //to ensure the order of writes
  
  uint index = tail & *sub->submission_queue_ring.ring_mask; //retrieve the index in which we will insert our submission
  
  io_uring_sqe *sqe = &sub->sqes[index]; //store a pointer to our entry's location
  sqe->fd = fd; //the fd of the file we opened
  sqe->flags = 0; //no flags
  sqe->opcode = IORING_OP_READV; //the operation we'd like to have done
  sqe->addr = (unsigned long)info->iovecs;
  sqe->len = blocks; //for a vectored IO transfer, like readv in our case, `len` is the number of vectors we are submitting (the number of iovecs we're submitting)
  sqe->off = 0; //the offset at which the operation should happen in the file
  sqe->user_data = (unsigned long long)info; //we use the user_data field to store a pointer to info, we use it for reading the data in read_from_completion_queue

  //since there is this indirection array, and we're submitting just one element, we just set the value as index, at the index in the array `index`
  s_ring->array[index] = index;
  
  tail = next_tail; //since we've now made 1 entry, the tail has been updated to the next tail

  if(*s_ring->tail != tail){
    *s_ring->tail = tail;
    write_barrier(); //ensure the proper ordering of writes, making sure the updated tail happens no later than this operation
  }

  int return_code = io_uring_enter(sub->ring_fd, 1, 1, IORING_ENTER_GETEVENTS);
  //param 1: the fd returned by io_uring_setup(), param 2: the number of SQEs ready to be submitted, param 3: the number of SQEs to wait for as a minimum,
  //param 4: flags, the system call will attempt to wait for 1 event (as we specified) to be completed before returning

  if(return_code < 0){
    perror("io_uring_enter in submit_to_submission_queue");
    exit(1);
  }

  return 0; //success
}

int main(int argc, char *argv[]){
  submitter *sub;

  if(argc < 2){
    printf("Usage: %s filename1 filename2 filename3 ...\n", argv[0]);
    return 1;
  }

  sub = (submitter*)malloc(sizeof(submitter));
  if(!sub){
    perror("malloc for sub in the main function");
    return 1;
  }
  memset(sub, 0, sizeof(submitter));

  if(app_setup_io_uring(sub)){
    printf("Unable to setup io_uring\n");
    return 1;
  }

  for(int i = 1; i < argc; i++){ //goes through all the file arguments and basically prints them via readv
    if(submit_to_submission_queue(argv[i], sub)){ //submit a file for reading
      printf("Error reading file\n");
      exit(1);
    }
    read_from_completion_queue(sub); //print it to the command line
  }
}