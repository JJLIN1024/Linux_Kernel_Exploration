#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main() {
	int dev = open("/dev/chardev", O_RDONLY);
	if(dev == -1) {
		printf("[Error]: file opening failed!");
		return -1;
	}

	printf("[SUCCESS]: file opening successful!");
	close(dev);
	return 1;
}
