	#include <stdio.h>
	#include <stdlib.h>
	#include <stdint.h>
	
	static inline uint64_t
	rdtsc(void)
	{	
		uint32_t tsc_l, tsc_h;

		asm volatile("rdtsc\n\t"
                :"=a"(tsc_l), "=d"(tsc_h)
                ::"memory");

		return ((uint64_t)(tsc_h) << 32) | tsc_l;
	}

	main(int argc, char *argv[])
	{
		volatile uint32_t x = ~0U;
		uint64_t start = rdtsc();
	
		asm volatile("" : : : "memory");
		while (x--);
		asm volatile("" : : : "memory");
		printf("\n Ticks=%u\n", rdtsc() - start);
		return 0;
	}
	 
