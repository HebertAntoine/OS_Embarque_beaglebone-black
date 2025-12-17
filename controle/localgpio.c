#define GPIO_SET 1
#define GPIO_CLEAR 4

#include<stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

int main(int argc, char * argv[]){
  int res=1;
  if (argc == 2) res= atoi(argv[1]); 
  int fd = open("/dev/kgpio",O_RDWR);
  res?ioctl(fd,GPIO_SET,0):ioctl(fd,GPIO_CLEAR,0);
  close(fd);
  return 0;
}
