#pragma once

#include "Lock.h"
#include "Util.h"

typedef struct Node
{
    struct Node *__prev;
    struct Node *__next;
} Node;


typedef struct
{
    Node __root;
    uint64_t __size;
    Lock __lock;
} Queue;


void queueInit(Queue *this);
uint64_t queueGetSize(Queue *this);
void queuePush(Queue *this, Node *pushNode);
Node *queuePop(Queue *this);
