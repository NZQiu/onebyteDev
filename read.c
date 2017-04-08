#include <fcntl.h>
#include <stdio.h>

#define MAX_BUF_SIZE 4194304

char read_buf[MAX_BUF_SIZE];

int main() {
	int i, fd;
  	fd = open("/dev/onebyte", O_RDONLY);
  	if (fd == -1) {
    		printf("Error in opening char device \n");
    		exit(-1);
  	}
  	read(fd, read_buf, sizeof(read_buf));
  	printf(read_buf);

  	char head[1000];
  	for(int i=0; i< 999; i++){
      		head[i] = read_buf[i];
  	}
  	head[999] = '\0';
  	printf("\nHead: %s\n", head);

  	close(fd);
  	return 0;
}
