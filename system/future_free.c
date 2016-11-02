#include <xinu.h>
#include <future.h>

syscall future_free(future* f){
  pid32 consPids,prodPids;

  //If State is either FUTURE_SHARED or FUTURE_QUEUE, perform additional cleanup steps
  if( f->flag == FUTURE_SHARED){
    //Empty the get_queue and kill the processes
    while(!isEmpty(f->get_queue)){
      consPids = dequeueFuture(f->get_queue);
      kill(consPids);
    }

    //Free the get_queue
    if( freemem((char *)f, sizeof(f->get_queue)) == SYSERR){
      return SYSERR;
    }

  }
  else if (f->flag == FUTURE_QUEUE){
    //Empty the get_queue, set_queue and kill the processes
    while(!isEmpty(f->get_queue)){
      consPids = dequeueFuture(f->get_queue);
      kill(consPids);
    }
    while(!isEmpty(f->set_queue)){
      prodPids = dequeueFuture(f->get_queue);
      kill(prodPids);
    }
    
    //Free the get_queue & set_queue
    if( freemem((char *)f, sizeof(f->get_queue)) == SYSERR){
      return SYSERR;
    }
    if( freemem((char *)f, sizeof(f->set_queue)) == SYSERR){
      return SYSERR;
    }    
  }

 if( freemem((char *)f, sizeof(future)) == SYSERR){
    return SYSERR;
  }
  return OK;
}