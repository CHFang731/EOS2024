#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    int IP_fd;
    unsigned int x,y,data ;
    IP_fd = open("/dev/gcd",O_RDWR);
    if(IP_fd == -1){
	perror("cannot open the device gcd!!\n");
	exit(1);
    }
    printf("=========================\n\r");
    printf("Input the value of x and y:");
    scanf("%c %c",&x ,&y);
    printf("\n");
    if(x==0 || y==0)
	printf("Bye !");
    else
    {
	if(write(IP_fd, &x, sizeof(char))){
	    perror("write() error!\n");
	    close(IP_fd);
	    exit(1);
        }
	if(write(IP_fd, &y, sizeof(char))){
	    perror("write() error!\n");
	    close(IP_fd);
	    exit(1);
    	}

	if(read(IP_fd, &data, sizeof(char))){
		perror("read() error!\n");
		close(IP_fd);
		exit(1);
	}
	printf("GCD data = %d \n", data);

    }
    
	close(IP_fd);
	return 0;
}

