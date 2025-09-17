#pragma once

#include "Queue.h"
#include "Lock.h"
#include "Util.h"

void queueInit(Queue *this)
{
    this->__root.__prev = &this->__root;
    this->__root.__next = &this->__root;
    this->__size = 0;

    lockInit(&this->__lock);
}


uint64_t queueGetSize(Queue *this)
{
    lockAcquire(&this->__lock);

    uint64_t queueSize = this->__size;

    lockRelease(&this->__lock);

    return queueSize;
}


void queuePush(Queue *this, Node *pushNode)
{
    lockAcquire(&this->__lock);

    pushNode->__prev = this->__root.__prev;
    pushNode->__next = &this->__root;

    this->__root.__prev->__next = pushNode;
    this->__root.__prev = pushNode;

    this->__size++;

    lockRelease(&this->__lock);
}


Node *queuePop(Queue *this)
{
    Node *popNode = 0;

    lockAcquire(&this->__lock);

    if (this->__size)
    {
        popNode = this->__root.__next;

        popNode->__next->__prev = &this->__root;
        this->__root.__next = popNode->__next;

        this->__size--;
    }

    lockRelease(&this->__lock);

    return popNode;
}
