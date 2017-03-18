#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
/*
 * Vector 55
 */
#define LAPIC_START	0xfee00000

static uint64_t
rdtsc(void)
{
    uint32_t tsc_l, tsc_h;

    asm volatile("rdtsc\n\t"
                :"=a"(tsc_l), "=d"(tsc_h)
                ::"memory");

    return ((uint64_t)(tsc_h) << 32) | tsc_l;
}

static void
memory_access(volatile uint32_t *addr)
{
	int cnt = 10000;
	uint32_t val;
	uint64_t tsc1, tsc2, diff;
	
	tsc1 = rdtsc();
	while (cnt--) {
		val += *addr;
	}
	tsc2 = rdtsc();

	diff = tsc2 - tsc1;

	printf("\n Time taken %lu", diff);	
		
}

int
main(int argc, char *argv[])
{

	void *addr;
	uint8_t mode;
	int fd;

	if (argc != 2) {
		printf("\nInvalid argument count");
		printf("\n Usage: cache_test mode");
		printf("\n       mode 0 = cached, 1 = uncached");
		return -1;
	}

	mode = (uint8_t)strtol(argv[1], NULL, 10);

	if (mode == 1) {
		printf("\n Uncached access");
		fd = open("/dev/mem", O_RDWR);

	 	if (!fd) {
        	printf("\nError: Unable to open /dev/mem");
        	return -1;
    	}

		addr =  mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, LAPIC_START);	

	 	if (addr == MAP_FAILED) {
        	printf("\nFailed to mmap physical address:%lx", LAPIC_START);
        	return -1;
    	}
	} else {
		printf("\n Cached access");
		addr = malloc(4096);
		if (!addr) {
			printf("\nMalloc failed !");
			return -1;
		}
	}

	memory_access(addr);
	printf("\n");

	if (mode == 1)
		close(fd);

	return 0;
}
