#include <xinu.h>
#include <my_queue.h>

void enqueueFuture(pid32 pid, struct queueFuture *futureQ) {
	queueNode *newNode;
	newNode = (queueNode *)getmem(sizeof(struct queueNode));
	newNode->next = NULL;
	newNode->pid = pid;
	
	if (futureQ->head->next == NULL) {
		futureQ->head->next = newNode;
	}
	else {
		queueNode *tempNode = futureQ->head->next;
		while(tempNode->next!= NULL)
		{
			tempNode = (tempNode)->next;
		}
		tempNode->next = newNode;
	}
}

pid32 dequeueFuture(struct queueFuture *futureQ)
{
	pid32 pid;
	pid = 0;

	// If empty queue, return 0
	if(isEmpty(futureQ))
	{
		return pid;
	}
	
	//get PID from the node to be deleted
	queueNode *nodeToDelete;
	nodeToDelete = futureQ->head->next ;
	pid = nodeToDelete->pid;

	//Reset head to next node
	futureQ->head->next = nodeToDelete->next;

	freemem((char *)nodeToDelete, sizeof(*nodeToDelete));
	return pid;

}

int isEmpty(struct queueFuture *queueF) {
	if (queueF->head->next == NULL)
		return 1;
	else
		return 0;
}