	#include <stdio.h>
	#include <stdlib.h>
	#include <pthread.h>
	#include <stdint.h>
	
	struct cacheline_separate {
		unsigned int x;	
		unsigned char pad[64];
		unsigned int y;
	}__attribute__((aligned(64))); 
	
	struct cacheline_same {
		unsigned int x;	
		unsigned int y;
		unsigned char pad[64];
	}__attribute__((aligned(64))); 

	struct cacheline_separate c_sep;	
	struct cacheline_same c_same;
	static inline uint64_t
	rdtsc(void)
	{	
		uint32_t tsc_l, tsc_h;

		asm volatile("rdtsc\n\t"
                :"=a"(tsc_l), "=d"(tsc_h)
                ::"memory");

		return ((uint64_t)(tsc_h) << 32) | tsc_l;
	}

	static void *thread_1(void *ptr)
	{
		int opt = atoi(ptr);
		int *x, *y;
		uint64_t count = 0x10000000;
		int val;
		uint64_t time1, time2;	
	
		printf("\n Thread_1:%d", opt);
		
		if (opt == 1) {
			x = &c_same.x;
			y = &c_same.y;
		} else {
			x = &c_sep.x;
			y = &c_sep.y;	
		}

		time1 = rdtsc();
		while (count--) {
			*x = *x + 100;
		}
		time2 = rdtsc();

		printf("\nThread_1 time:%lu", time2 - time1);
	}	
 
	static void *thread_2(void *ptr)
	{
		int opt = atoi(ptr);
		int *x, *y;
		uint64_t count = 0x10000000;
		int val;
		uint64_t time1, time2;	
	
		printf("\n Thread_2:%d", opt);
		
		if (opt == 1) {
			x = &c_same.x;
			y = &c_same.y;
		} else {
			x = &c_sep.x;
			y = &c_sep.y;	
		}

		time1 = rdtsc();
		while (count--) {
			*y = *y + 100;
		}
		time2 = rdtsc();

		printf("\nThread_2 time:%lu", time2 - time1);
	}

	
	main(int argc, char *argv[])
	{
	     pthread_t thread1, thread2;
	     const char *message1 = "Thread 1";
	     const char *message2 = "Thread 2";
	     int  iret1, iret2;
		
		if (argc < 2) {
			printf("\nUsage, prog1 1/2");
			printf("\n 1 = same cacheline, 2 = different cacheline");
			return;
		}
	
		 printf("\n Separate, x:%lx y:%lx", &c_sep.x, &c_sep.y);	 
		 printf("\n Same, x:%lx y:%lx", &c_same.x, &c_same.y);	 
	    /* Create independent threads each of which will execute function */
	 
	     iret1 = pthread_create( &thread1, NULL, thread_1, (void*) argv[1]);
	     if(iret1)
	     {
	         fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
	         exit(EXIT_FAILURE);
	     }
	 
	     iret2 = pthread_create( &thread2, NULL, thread_2, (void*) argv[1]);
	     if(iret2)
	     {
	         fprintf(stderr,"Error - pthread_create() return code: %d\n",iret2);
	         exit(EXIT_FAILURE);
	     }
	 
	     printf("pthread_create() for thread 1 returns: %d\n",iret1);
	     printf("pthread_create() for thread 2 returns: %d\n",iret2);
	 
	     /* Wait till threads are complete before main continues. Unless we  */
	     /* wait we run the risk of executing an exit which will terminate   */
	     /* the process and all threads before the threads have completed.   */
	 
	     pthread_join( thread1, NULL);
	     pthread_join( thread2, NULL);
	 
	     exit(EXIT_SUCCESS);
	}
	 
