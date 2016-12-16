#include <future.h>
#include <newnet.h>
/*Global variable for producer consumer*/

extern int32 n;

void consumer(int32 count);
void producer(int32 count);

extern sid32 produced, consumed;

uint future_prod(future *fut);
uint future_cons(future *fut);

uint nw_cons(future *fut);
uint nw_prods(future *fut);