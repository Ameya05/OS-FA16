#include <xinu.h>
#include <fqueue.h>

struct fqentry	fqueuetab[NQENT];	/* Table of process queues	*/


/*------------------------------------------------------------------------
 *  dequeue  -  Remove and return the first process on a list
 *------------------------------------------------------------------------
 */
pid32	fdequeue(
	  qid16		q		/* ID queue to use		*/
	)
{
	pid32	pid;			/* ID of process removed	*/

	if (fisbadqid(q))
	{
		return SYSERR;
	} else if (isempty(q)) {
		return EMPTY;
	}

	pid = ffirstid(q);
	fqueuetab[pid].qprev = EMPTY;
	fqueuetab[pid].qnext = EMPTY;
	return pid;
}

/*------------------------------------------------------------------------
 *  enqueue  -  Insert a process at the tail of a queue
 *------------------------------------------------------------------------
 */
pid32	fenqueue(
	  pid32		pid,		/* ID of process to insert	*/
	  qid16		q		/* ID of queue to use		*/
	)
{
	qid16	tail, prev;		/* Tail & previous node indexes	*/

	if (isbadqid(q) || isbadpid(pid)) {
		return SYSERR;
	}

	tail = fqueuetail(q);
	prev = fqueuetab[tail].qprev;

	fqueuetab[pid].qnext  = tail;	/* Insert just before tail node	*/
	fqueuetab[pid].qprev  = prev;
	fqueuetab[prev].qnext = pid;
	fqueuetab[tail].qprev = pid;
	return pid;
}