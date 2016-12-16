#include <xinu.h>
#include <prodcons.h>

 void consumer(int count){
  
	while(1){

		sleep(1);

		wait(produced);
		printf("consumed value : %d \n", n);
		
		if(n == count){
			break;
		}

		signal(consumed);
	}

	printf("Consumer stopped.\n");
	
	printf("Deleting semaphores.\n");

	semdelete(produced);
	semdelete(consumed);

	printf("Semaphores deleted.\n");
}