#include <fcntl.h>
#include <stdio.h>

#define MAX_FILE_SIZE 8388608

char write_buf[MAX_FILE_SIZE];

int main() {
	int i, i_fd, o_fd;
  	i_fd = open("file.txt", O_RDONLY);
	read(i_fd, write_buf, sizeof(write_buf));
	close(i_fd);

	o_fd = open("/dev/onebyte", O_RDWR);
	if (o_fd == -1) {
		printf("Error opening device\n");
		exit(-1);
	}

	int number_of_bytes = write(o_fd, write_buf, sizeof(write_buf));
	printf("Number of bytes written: %d\n", number_of_bytes);

	close(o_fd);
	return 0;
}
