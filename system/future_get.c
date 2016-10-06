#include <xinu.h>
#include <future.h>

syscall future_get(future *f, int *value){
  
  intmask inmask;
  inmask = disable();
  
  if(f->state == FUTURE_VALID){
    *value = *(f->value);
    restore(inmask);
    return OK;
  }
  
  if(f->state == FUTURE_EMPTY){
    f->state = FUTURE_WAITING;
    f->pid = getpid();
    suspend( f->pid );
    *value = *(f->value);
  }
  
  restore(inmask);
  return OK;
}