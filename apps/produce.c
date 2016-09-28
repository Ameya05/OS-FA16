#include <xinu.h>
#include <prodcons.h>

/*
* Producer function to produce values "count" times
*/

void producer(int count) {
	
	if (count <1 )
		return;

	for (int i=0 ; i< count ; i++)
	{
		n = i ;
		printf("\nproduced : %d", n );
	}
}