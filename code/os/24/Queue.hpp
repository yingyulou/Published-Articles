#pragma once

#include "Queue.h"
#include "Util.h"

void queueInit(Queue *this)
{
    this->__root.__prev = this->__root.__next = &this->__root;
}


bool queueEmpty(Queue *this)
{
    uint64_t RFLAGS;

    __asm__ __volatile__("pushf; pop %0; cli": "=g"(RFLAGS));

    bool emptyBool = this->__root.__next == &this->__root;

    __asm__ __volatile__("push %0; popf":: "g"(RFLAGS));

    return emptyBool;
}


void queuePush(Queue *this, Node *pushNode)
{
    uint64_t RFLAGS;

    __asm__ __volatile__("pushf; pop %0; cli": "=g"(RFLAGS));

    pushNode->__prev = this->__root.__prev;
    pushNode->__next = &this->__root;

    this->__root.__prev->__next = pushNode;
    this->__root.__prev = pushNode;

    __asm__ __volatile__("push %0; popf":: "g"(RFLAGS));
}


Node *queuePop(Queue *this)
{
    uint64_t RFLAGS;

    __asm__ __volatile__("pushf; pop %0; cli": "=g"(RFLAGS));

    Node *popNode = this->__root.__next;

    popNode->__next->__prev = &this->__root;
    this->__root.__next = popNode->__next;

    __asm__ __volatile__("push %0; popf":: "g"(RFLAGS));

    return popNode;
}
