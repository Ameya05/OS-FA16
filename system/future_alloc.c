#include <xinu.h>
#include <future.h>

future* future_alloc(int future_flag){
  future *vfut;
  intmask  intrptmask;
  intrptmask = disable();
  vfut = (char *)getmem(sizeof(future));
  
  if( vfut != SYSERR ){
    vfut->state = FUTURE_EMPTY;
    restore(intrptmask);
    return vfut;
  }
  restore(intrptmask);
  return NULL;
}