#pragma once

#include "Util.h"

typedef struct Node
{
    struct Node *__prev;
    struct Node *__next;
} Node;


typedef struct
{
    Node root;
} Queue;


void queueInit(Queue *this);
bool queueEmpty(Queue *this);
void queuePush(Queue *this, Node *pushNode);
Node *queuePop(Queue *this);
