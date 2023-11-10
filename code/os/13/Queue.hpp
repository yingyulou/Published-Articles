#pragma once

#include "Queue.h"
#include "Util.h"

void queueInit(Queue *this)
{
    this->root.__prev = this->root.__next = &this->root;
}


bool queueEmpty(Queue *this)
{
    __asm__ __volatile__("pushf; cli");

    bool emptyBool = this->root.__next == &this->root;

    __asm__ __volatile__("popf");

    return emptyBool;
}


void queuePush(Queue *this, Node *pushNode)
{
    __asm__ __volatile__("pushf; cli");

    pushNode->__prev = this->root.__prev;
    pushNode->__next = &this->root;

    this->root.__prev->__next = pushNode;
    this->root.__prev = pushNode;

    __asm__ __volatile__("popf");
}


Node *queuePop(Queue *this)
{
    __asm__ __volatile__("pushf; cli");

    Node *popNode = this->root.__next;

    popNode->__next->__prev = &this->root;
    this->root.__next = popNode->__next;

    __asm__ __volatile__("popf");

    return popNode;
}
