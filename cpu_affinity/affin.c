	#include <stdio.h>
	#include <stdlib.h>
	#include <pthread.h>
	#include <stdint.h>
	

	static void *thread_1(void *ptr)
	{
		while (1) {
			//printf("\nThread 1");
			//sleep(1);
		}
	}	
 
	static void *thread_2(void *ptr)
	{
		while (1) {
			//printf("\nThread 2");
			//sleep(1);
		}
	}

	static void *thread_3(void *ptr)
	{
		while (1) {
			//printf("\nThread 3");
			//sleep(1);
		}
	}

	static void *thread_4(void *ptr)
	{
		while (1) {
			//printf("\nThread 4");
			//sleep(1);
		}
	}
	
	main(int argc, char *argv[])
	{
	     pthread_t thread1, thread2, thread3, thread4;
	     int  iret;
		
	 
	     iret = pthread_create( &thread1, NULL, thread_1, (void*)NULL);
	     if(iret)
	     {
	         fprintf(stderr,"Error - pthread_create() return code: %d\n",iret);
	         exit(EXIT_FAILURE);
	     }
	 
	     iret = pthread_create( &thread2, NULL, thread_2, (void*)NULL);
	     if(iret)
	     {
	         fprintf(stderr,"Error - pthread_create() return code: %d\n",iret);
	         exit(EXIT_FAILURE);
	     }
	     
		iret = pthread_create( &thread3, NULL, thread_3, (void*)NULL);
	    if(iret) {
	         fprintf(stderr,"Error - pthread_create() return code: %d\n",iret);
	         exit(EXIT_FAILURE);
	     }
	 
	     iret = pthread_create( &thread4, NULL, thread_4, (void*)NULL);
	     if(iret) {
	         fprintf(stderr,"Error - pthread_create() return code: %d\n",iret);
	         exit(EXIT_FAILURE);
	     }
	 
	     /* Wait till threads are complete before main continues. Unless we  */
	     /* wait we run the risk of executing an exit which will terminate   */
	     /* the process and all threads before the threads have completed.   */
	 
	     pthread_join( thread1, NULL);
	     pthread_join( thread2, NULL);
	     pthread_join( thread3, NULL);
	     pthread_join( thread4, NULL);
	 
	     exit(EXIT_SUCCESS);
	}
	 
