#include <xinu.h>
#include <prodcons.h>

uint nw_cons(future *fut) 
{
	int i, status;	
	status = future_get(fut, &i);

	if (status < 1) 
	{
		printf("future_get failed\n");
		return -1;
	}
	printf("It produced : %d \n", i);
	return OK;
}