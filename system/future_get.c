#include <xinu.h>
#include <future.h>

syscall future_get(future *f, int *value){
  
  intmask inmask;
  inmask = disable();
  if(f->flag == FUTURE_EXCLUSIVE){
	if(f->state == FUTURE_VALID){
	        f->state = FUTURE_EMPTY;
		*value = *(f->value);
	        f->pid = NULL;
	}
	else if(f->state == FUTURE_EMPTY){
		f->state = FUTURE_WAITING;
		f->pid = getpid();
		suspend( f->pid );
		*value = *(f->value);
	}
  }
  else if (f->flag == FUTURE_SHARED){

	if(f->state == FUTURE_EMPTY){
  		f->state = FUTURE_WAITING;
  		enqueueFuture(getpid(),f->get_queue);
  		suspend(getpid());
		*value = *(f->value);
  	}
  	else if(f->state == FUTURE_WAITING){
  		enqueueFuture(getpid(),f->get_queue);
		suspend(getpid());
		*value = *(f->value);
	}
	else if(f->state == FUTURE_VALID){
		*value = *(f->value);
	}
  }
  else if(f->flag == FUTURE_QUEUE){
	if(f->state == FUTURE_VALID){
		pid32 prodPid = dequeueFuture(f->set_queue);
  		 if (isEmpty(f->get_queue) && isEmpty(f->set_queue)){
			f->state = FUTURE_EMPTY;
		}
		resume(prodPid);
		*value = *(f->value);
	}
	else if(f->state == FUTURE_EMPTY){
  		f->state = FUTURE_WAITING;
  		enqueueFuture(getpid(),f->get_queue);
  		suspend(getpid());
		*value = *(f->value);
  	}
	else if(f->state == FUTURE_WAITING){
		enqueueFuture(getpid(),f->get_queue);
		suspend(getpid());
		*value = *(f->value);
	}
  }

  restore(inmask);
  return OK;
}