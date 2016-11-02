#include <xinu.h>
#include <future.h>

future* future_alloc(int future_flag){
  future *vfut;
  intmask  intrptmask;
  intrptmask = disable();
  vfut = (future *)getmem(sizeof(future));
  
  if( vfut != NULL ){
    vfut->state = FUTURE_EMPTY;
    vfut->flag = future_flag;

    if(future_flag == FUTURE_SHARED)
    {
       //Create Future and Get Queue in it
      queueFuture *newQ;
      newQ = (queueFuture *)getmem(sizeof(struct queueFuture));
      vfut->get_queue = newQ;

      //Create new node
      queueNode *newGetNode;
      newGetNode = (queueNode *)getmem(sizeof(struct queueNode));
      newGetNode->pid = 0;
      newGetNode->next = NULL;

      //Set the node as head and tail of the get_queue in vfut
      vfut->get_queue->head = newGetNode;
    }
    else if (future_flag == FUTURE_QUEUE)
    {
      //Create Future and Get & Set queues in it
      queueFuture *newGetQ, *newSetQ ;
      newGetQ = (queueFuture *)getmem(sizeof(struct queueFuture));
      newSetQ = (queueFuture *)getmem(sizeof(struct queueFuture));
      vfut->get_queue = newGetQ;
      vfut->set_queue = newSetQ;

      //Create new node for Get queue
      queueNode *newGetNode;
      newGetNode = (queueNode *)getmem(sizeof(struct queueNode));
      newGetNode->pid = 0;
      newGetNode->next = NULL;

      //Create new node for Set queue
      queueNode *newSetNode;
      newSetNode = (queueNode *)getmem(sizeof(struct queueNode));
      newSetNode->pid = 0;
      newSetNode->next = NULL;

      //Set the head and tail nodes of the get_queue and set_queue in vfut
      vfut->get_queue->head = newGetNode;
      vfut->set_queue->head = newSetNode;
    }

    restore(intrptmask);
    return vfut;
  }
  restore(intrptmask);
  return NULL;
}