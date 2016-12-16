#include <xinu.h>
#include <prodcons.h>

void producer(int count){  
	
	while(n < count){

		sleep(1);

		wait(consumed);
		n++;		
		printf("Produced value : %d \n", n);

		signal(produced);
		
	}

	printf("Producer stopped.\n");
}