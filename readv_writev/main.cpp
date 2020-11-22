#include <unistd.h>
#include <sys/stat.h>
#include <linux/fs.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <sys/uio.h>

#include <iostream>
#include <cstring>

typedef struct stat stat_t;
typedef unsigned long long u64;

int get_file_size(int fd){
  stat_t file_stat;
  
  if(fstat(fd, &file_stat) < 0){
    perror("fstat");
    exit(1);
  }

  if(S_ISBLK(file_stat.st_mode)){ 
    u64 bytes;
    if(ioctl(fd, BLKGETSIZE64, &bytes) < 0){ //gives size of a disk in bytes
      perror("ioctl BLKGETSIZE");
      exit(1);
    }
    return bytes;
  }else if(S_ISREG(file_stat.st_mode)){ //is a regular file
    return file_stat.st_size;
  }
  return -1; //if unable to find size - would be a surprise
}

void printStuff(char *buf, int length){
  while(length--){
    fputc(*buf++, stdout); //dereferences the buffer of length 'length', while incrementing it each time, and printing one character to the screen, in effect printing the entire buffer
  }
}

struct iovec_info {
  iovec_info(iovec *ivcs, int blks) : iovecs(ivcs), blocks(blks) {}
  int blocks;
  iovec *iovecs;
};

iovec_info readv_in_blocks(const char* filename, int block_size){
  int fd = open(filename, O_RDONLY);
  int file_size = get_file_size(fd);
  int bytes_remaining = file_size;

  int blocks = (int)bytes_remaining/block_size;
  if(bytes_remaining % block_size) blocks++;
  int current_block = 0;

  auto iovecs = (iovec*)std::malloc(sizeof(iovec) * blocks);

  while(bytes_remaining){
    int bytes_to_read = bytes_remaining - std::max(0, bytes_remaining - block_size);
    bytes_remaining = std::max(0, bytes_remaining - block_size);

    std::cout << bytes_to_read << "\n";

    void *buffer;
    if(posix_memalign(&buffer, block_size, block_size)){ //NULL, which is numerically 0, is returned if it fails, otherwise positive number of bytes
      perror("posix memalign");
      exit(1);
    }

    iovecs[current_block].iov_base = buffer;
    iovecs[current_block++].iov_len = bytes_to_read;
  }

  int rc = readv(fd, iovecs, blocks);

  close(fd);

  return iovec_info(iovecs, blocks);
}

int main(){
  auto a_txt = readv_in_blocks("a.txt", 128);
  auto b_txt = readv_in_blocks("b.txt", 128);
  auto c_txt = readv_in_blocks("c.txt", 128);

  int fd = open("lorem.txt", O_WRONLY | O_CREAT);
  writev(fd, a_txt.iovecs, a_txt.blocks);
  writev(fd, b_txt.iovecs, b_txt.blocks);
  writev(fd, c_txt.iovecs, c_txt.blocks);
}
