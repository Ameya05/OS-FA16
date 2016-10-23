#include <xinu.h>
#include <future.h>

syscall future_free(future* f){
  freemem((char *)f, sizeof(future));
  return OK;
}