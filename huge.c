#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <assert.h>


#define SIZE (1 << 21)
char *c = NULL;

int usleep(useconds_t usec);

void *alloc_huge_page(void *unused)
{
	c = mmap(NULL, SIZE, PROT_READ | PROT_WRITE,
		 MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB,
		 -1, 0);

	if ((long unsigned) c == -1) {
		/* perror("Failed to allocate\n"); */
		c =  NULL;
		return NULL;
	}
	char *ptr = (char *)c;
	ptr[0] = '.';

	if (!ptr) {
		fprintf(stderr, "Failed to allocate memory\n");
		perror("");
	}
}

void *mfree(void *unused)
{
	usleep(rand() % 10);
	if (!c)
		return NULL;

	madvise(c, SIZE, MADV_DONTNEED);
}

void *touch(void *unused)
{
	usleep(rand() % 300);
	if (!c)
		return NULL;

	char x = ((char *)c)[0];
	printf("%c", x);
}

int main()
{
	pthread_t thread1, thread2, thread3;

	srand(getpid());

	while (1) {
		pthread_create(&thread1, NULL, alloc_huge_page, NULL);
		pthread_create(&thread2, NULL, mfree, NULL);
		pthread_create(&thread3, NULL, touch, NULL);

		pthread_join(thread1, NULL);
		pthread_join(thread2, NULL);
		pthread_join(thread3, NULL);

		munmap(c, SIZE);
		c = NULL;
		/* fprintf(stderr, "."); */
	}

	fprintf(stderr, "Waiting thread1\n");
	fprintf(stderr, "Waiting thread2\n");

	getchar();

}

