#include <xinu.h>
#include <prodcons.h>

/*
* Producer function to produce values "count" times
*/

void producer(int32 count) {
	
	if (count <1 )
		return;

	for (int32 i=0 ; i< count ; i++)
	{
		wait(consumed);
		n = i ;
		printf("\nproduced : %d", n );
		signal(produced);
	}
}