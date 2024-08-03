#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    int IP_fd;
    unsigned char data = 0x25;
    IP_fd = open("/dev/des",O_RDWR);
    if(IP_fd == -1){
	perror("cannot open the device aes!!\n");
	exit(1);
    }
    if(write(IP_fd, &data, sizeof(char))){
	perror("write() error!\n");
	close(IP_fd);
	exit(1);
    }
	close(IP_fd);
	return 0;
}

