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
    Lock __lock;
} Queue;


void queueInit(Queue *this);
bool queueEmpty(Queue *this);
void queuePush(Queue *this, Node *pushNode);
Node *queuePop(Queue *this);
