#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>

//needed for IO things. Attention that this is different from kernel mode
int lcd;

#define SCULL_IOC_MAGIC 'k'
#define SCULL_HELLO _IO(SCULL_IOC_MAGIC, 1)
#define SCULL_MSG_WRITE _IOW(SCULL_IOC_MAGIC, 2, int)
#define SCULL_MSG_READ _IOR(SCULL_IOC_MAGIC, 3, int)

void test()
{
	int k, i, sum;
	char s[3];
	memset(s, '2', sizeof(s));
	printf("test begin!\n");
	k = write(lcd, s, sizeof(s));
	printf("written = %d\n", k);
	k = ioctl(lcd, SCULL_HELLO);
	printf("result = %d\n", k);

	char *dev_msg = "User message from user process!";
	k = ioctl(lcd, SCULL_MSG_WRITE, dev_msg);
	printf("dev msg write result = %d. msg address = %d\n", k, dev_msg);

	char user_msg[1024] = {'\0'};
	k = ioctl(lcd, SCULL_MSG_READ, user_msg);
	printf("user msg read result = %d. msg address = %d\n", k, user_msg);
	printf("user msg = %s\n", user_msg);
}

int main(int argc, char **argv)
{
	lcd = open("/dev/onebyte", O_RDWR);
	if (lcd == -1) {
		perror("unable to open lcd");
		exit(EXIT_FAILURE);
	}

	test();
	close(lcd);
	return 0;
}
