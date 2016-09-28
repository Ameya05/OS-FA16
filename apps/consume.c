#include <xinu.h>
#include <prodcons.h>

/*  
*  Consumer function to consume values "count" times 
*/
void consumer(int count) {

	if (count <1 )
		return;

	for (int i=0; i < count ; i++)
	{
		printf("\nconsumed : %d", n );
	}
	printf("\n");
}