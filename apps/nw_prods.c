#include <xinu.h>
#include <prodcons.h>

uint nw_prods(future *fut) {

	printf("In Network comm using Future producer \n");
	int i,j;
	j = netserver();
	for(i=0; i<2000; i++){
		j += i;
	}
	future_set(fut, &j);
	return OK;
}