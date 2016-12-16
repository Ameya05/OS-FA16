/* queue.h - firstid, firstkey, isempty, lastkey, nonempty		*/

/* Queue structure declarations, constants, and inline functions	*/

/* Default # of queue entries: 1 per process plus 2 for ready list plus	*/
/*			2 for sleep list plus 2 per semaphore		*/

#define FNPROC 15

#ifndef FNQENT
#define FNQENT	(FNPROC + 6)
#endif

#define	EMPTY	(-1)		/* Null value for qnext or qprev index	*/
#define	MAXKEY	0x7FFFFFFF	/* Max key that can be stored in queue	*/
#define	MINKEY	0x80000000	/* Min key that can be stored in queue	*/

typedef struct	fqentry	{		/* One per process plus two per list	*/
	pid32	qpid;		/* Key on which the queue is ordered	*/
	qid16	qnext;		/* Index of next process or tail	*/
	qid16	qprev;		/* Index of previous process or head	*/
} queue;

extern	struct fqentry	fqueuetab[];

/* Inline queue manipulation functions */

#define	fqueuehead(q)	(q)
#define	fqueuetail(q)	((q) + 1)
#define	ffirstid(q)	(fqueuetab[fqueuehead(q)].qnext)
#define	flastid(q)	(fqueuetab[fqueuetail(q)].qprev)
#define	fisempty(q)	(ffirstid(q) >= FNPROC)
#define	fnonempty(q)	(ffirstid(q) <  FNPROC)
#define	ffirstkey(q)	(fqueuetab[ffirstid(q)].qpid)
#define	flastkey(q)	(fqueuetab[flastid(q)].qpid)

/* Inline to check queue id assumes interrupts are disabled */

#define	fisbadqid(x) (((int32)(x) < 0) || (int32)(x) >= NQENT-1)