#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <assert.h>

int usleep(useconds_t usec);

#define SIZE (1 << 21)
#define INLOOP_SIZE 100

char *c = NULL;

void *touch(void *unused)
{
	char *ptr = (char *)c;
	if (!ptr) {
		fprintf(stderr, "Failed to allocate memory\n");
		perror("");
	}

	for (int i = 0; i < INLOOP_SIZE; i++)
		ptr[0] = '.';

	return NULL;
}

void *mfree(void *unused)
{
	usleep(rand() % 10);
	if (!c)
		return NULL;

	for (int i = 0; i < INLOOP_SIZE; i++)
		madvise(c, SIZE, MADV_DONTNEED);

	return NULL;
}

int main()
{
	pthread_t thread1, thread2, thread3;

	srand(getpid());


	while (1) {
		c = mmap(NULL, SIZE, PROT_READ | PROT_WRITE,
			 MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB,
			 -1, 0);

		if ((long unsigned) c == -1) {
			perror("Failed to allocate\n");
			continue;
		}

		pthread_create(&thread1, NULL, mfree, NULL);
		pthread_create(&thread2, NULL, touch, NULL);

		pthread_join(thread1, NULL);
		pthread_join(thread2, NULL);
		munmap(c, SIZE);
		fprintf(stderr, ".");
	}


	getchar();

}

