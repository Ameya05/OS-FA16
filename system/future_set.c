
#include <xinu.h>
#include <future.h>

syscall future_set(future *f, int *value){
  
  intmask inmask;
  inmask = disable();

  if (f->state == FUTURE_EMPTY){
    f->state = FUTURE_VALID;
    f->value = value;
    restore(inmask);
    return OK;
  }
  
  if(f->state == FUTURE_WAITING){
    f->state = FUTURE_VALID;
    f->value = value;
    resume(f->pid);
    restore(inmask);
    return OK;
  }
  
  restore(inmask);
  return SYSERR;
}