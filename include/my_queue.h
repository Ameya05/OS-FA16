
//Custom implementation of queue for storing pids

typedef struct queueNode{
  pid32 pid;
  struct queueNode* next;
} queueNode;

typedef struct queueFuture{
  struct queueNode *head;
} queueFuture;