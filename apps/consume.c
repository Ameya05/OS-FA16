#include <xinu.h>
#include <prodcons.h>

/*  
*  Consumer function to consume values "count" times 
*/
void consumer(int32 count) {

	if (count <1 )
		return;

	for (int32 i=0; i < count ; i++)
	{
		wait(produced);
		printf("\nconsumed : %d", n );
		signal(consumed);
	}
	printf("\n");
}