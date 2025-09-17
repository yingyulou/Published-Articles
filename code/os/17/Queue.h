#pragma once

#include "Util.h"

typedef struct Node
{
    struct Node *__prev;
    struct Node *__next;
} Node;


typedef struct
{
    Node __root;
    uint32_t __size;
} Queue;


void queueInit(Queue *this);
uint32_t queueGetSize(Queue *this);
void queuePush(Queue *this, Node *pushNode);
Node *queuePop(Queue *this);
