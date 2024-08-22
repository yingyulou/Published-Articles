#pragma once

#include "Queue.h"
#include "Lock.h"
#include "Util.h"

void queueInit(Queue *this)
{
    this->__root.__prev = this->__root.__next = &this->__root;

    lockInit(&this->__lock);
}


bool queueEmpty(Queue *this)
{
    uint64_t RFLAGS = lockAcquire(&this->__lock);

    bool emptyBool = this->__root.__next == &this->__root;

    lockRelease(&this->__lock, RFLAGS);

    return emptyBool;
}


void queuePush(Queue *this, Node *pushNode)
{
    uint64_t RFLAGS = lockAcquire(&this->__lock);

    pushNode->__prev = this->__root.__prev;
    pushNode->__next = &this->__root;

    this->__root.__prev->__next = pushNode;
    this->__root.__prev = pushNode;

    lockRelease(&this->__lock, RFLAGS);
}


Node *queuePop(Queue *this)
{
    uint64_t RFLAGS = lockAcquire(&this->__lock);

    Node *popNode = this->__root.__next;

    popNode->__next->__prev = &this->__root;
    this->__root.__next = popNode->__next;

    lockRelease(&this->__lock, RFLAGS);

    return popNode;
}
