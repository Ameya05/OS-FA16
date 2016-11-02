
#include <xinu.h>
#include <future.h>

syscall future_set(future *f, int *value){

  intmask inmask;
  inmask = disable();
  if(f->flag == FUTURE_EXCLUSIVE){
	  if (f->state == FUTURE_EMPTY){
	    f->state = FUTURE_VALID;
	    f->value = value;
	  }
	  
	  if(f->state == FUTURE_WAITING){
	    f->state = FUTURE_VALID;
	    f->value = value;
	    resume(f->pid);
	  }
  }
  else if(f->flag == FUTURE_SHARED){
  	if(f->state == FUTURE_EMPTY){
		f->state = FUTURE_VALID;
		f->value = value;
	}
	else if(f->state == FUTURE_WAITING){
		 f->state = FUTURE_VALID;
		f->value = value;
		
		//Resume all waiting consumer processes
		while(f->get_queue->head->next != NULL){
			pid32 consPid = dequeueFuture(f->get_queue);
			resume(consPid);
		}
	}
  }
  else if(f->flag == FUTURE_QUEUE){
  	if(f->state == FUTURE_EMPTY){
		f->state = FUTURE_VALID;
		f->value = value;
		enqueueFuture(getpid(),f->set_queue);
		suspend(getpid());
	}
	else if(f->state == FUTURE_WAITING){
		pid32 consPid = dequeueFuture(f->get_queue);
		f->value = value;
		 if (isEmpty(f->get_queue) && isEmpty(f->set_queue)){
			f->state = FUTURE_EMPTY;
		}
		resume(consPid);
	}
	else if(f->state == FUTURE_VALID){
		enqueueFuture(getpid(),f->set_queue);
		suspend(getpid());
		f->value = value;
	}
  }
  restore(inmask);
  return OK;
}