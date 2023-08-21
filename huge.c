#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int usleep(useconds_t usec);

#define SIZE (1 << 21)
#define INLOOP_SIZE 1000

char *c = NULL;
int marker;

void *touch(void *unused)
{
	char *ptr = (char *)c;
	if (!ptr) {
		fprintf(stderr, "Failed to allocate memory\n");
		perror("");
		exit(2);
	}

	for (int i = 0; i < INLOOP_SIZE; i++) {
		ptr[0] = '.';
	}

	return NULL;
}

void *mfree(void *unused)
{
	if (!c)
		return NULL;

	for (int i = 0; i < INLOOP_SIZE; i++) {
		/* write(marker, "madvise", 6); */
		madvise(c, SIZE, MADV_DONTNEED);
	}

	return NULL;
}

void clear_ftrace(int fd)
{
	int value = 1;

	if (fd < 0) {
		perror("Failed to clear ftrace\n");
	}
	write(fd, &value, sizeof(value));
	close(fd);
}

int main()
{
	pthread_t thread1, thread2, thread3;

	int buffer = open("/sys/kernel/tracing/free_buffer", O_WRONLY);
	marker = open("/sys/kernel/tracing/trace_marker", O_WRONLY);

	while (1) {
		c = mmap(NULL, SIZE, PROT_READ | PROT_WRITE,
			 MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB,
			 -1, 0);

		if ((long unsigned) c == -1) {
			perror("Failed to allocate\n");
			continue;
		}

		/* clear_ftrace(buffer); */
		pthread_create(&thread1, NULL, mfree, NULL);
		pthread_create(&thread2, NULL, touch, NULL);

		pthread_join(thread1, NULL);
		pthread_join(thread2, NULL);
		munmap(c, SIZE);
		printf(".");
	}

}

