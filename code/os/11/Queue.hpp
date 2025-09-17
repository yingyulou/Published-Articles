#pragma once

#include "Queue.h"
#include "Util.h"

void queueInit(Queue *this)
{
    this->__root.__prev = &this->__root;
    this->__root.__next = &this->__root;
    this->__size = 0;
}


uint32_t queueGetSize(Queue *this)
{
    return this->__size;
}


void queuePush(Queue *this, Node *pushNode)
{
    pushNode->__prev = this->__root.__prev;
    pushNode->__next = &this->__root;

    this->__root.__prev->__next = pushNode;
    this->__root.__prev = pushNode;

    this->__size++;
}


Node *queuePop(Queue *this)
{
    Node *popNode = 0;

    if (this->__size)
    {
        popNode = this->__root.__next;

        popNode->__next->__prev = &this->__root;
        this->__root.__next = popNode->__next;

        this->__size--;
    }

    return popNode;
}
