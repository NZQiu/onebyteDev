#include <fcntl.h>
#include <stdio.h>

#define MAX_FILE_SIZE 8388608

char mega_file_buf[MAX_FILE_SIZE];

int main() {
	int i, test_fd, fd;
	test_fd = open("five_mb.txt", O_RDONLY);
	read(test_fd, mega_file_buf, sizeof(mega_file_buf));
	close(test_fd);

	fd = open("/dev/onebyte", O_RDWR);
	if (fd == -1) {
		printf("Error in opening char device \n");
		exit(-1);
	}
	printf("writing to device.\n");

	int written_count = write(fd, mega_file_buf, sizeof(mega_file_buf));
	printf("Number of chars written: %d\n", written_count);

	close(fd);
	return 0;
}
