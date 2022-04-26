#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>           /* For O_* constants */
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

static inline uint64_t __cputime_get(void)
{
        unsigned long l, h;

        asm volatile("rdtsc" : "=a" (l), "=d" (h));

        return ((uint64_t) h << 32) | l;
}

int main()
{
        int fd;
        void *mem = NULL;
        uint32_t size = 0x1000000;
	/* path should be always start with "/" */
        char shm_name[] = "/shm_test";
        uint64_t t;

        /* create */
        shm_unlink(shm_name);

        fd = shm_open(shm_name, O_RDWR | O_CREAT | O_EXCL | O_TRUNC, 0666);
        if (fd < 0)
                return -1;

	/* set shm size */
        t = __cputime_get();
        if (ftruncate(fd, getpagesize() + size)) {
                close(fd);
                shm_unlink(shm_name);
                return -1;
        }
        printf("ftruncate() takes %llu\n", __cputime_get() - t);

	/* mmap */
        t = __cputime_get();
        mem = mmap(NULL, getpagesize() + size, PROT_READ | PROT_WRITE,
                        MAP_SHARED, fd, 0);
        if (mem == MAP_FAILED) {
                close(fd);
                shm_unlink(shm_name);
                return -1;
        }
        printf("mmap() takes %llu\n", __cputime_get() - t);

        close(fd);

        /* zero memory region */
        t = __cputime_get();
        memset(mem, 0, size);
        printf("memset() takes %llu\n", __cputime_get() - t);

        /*
        munmap(mem, size);
        shm_unlink(shm_name);
        */

        /*
        while (1) {
                sleep(1);
        }
        */
        return 0;
}

