// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include <cstdio>
#include "copyright.h"
#include "system.h"
#include "producer.h"
#include "consumer.h"
#include "synch.h"

//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------

void
SimpleThread(int which)
{
    int num;
    
    for (num = 0; num < 5; num++) {
	printf("*** thread %d looped %d times\n", which, num);
        currentThread->Yield();
    }
}

//----------------------------------------------------------------------
// ThreadTest
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

/*
void
ThreadTest()
{
    DEBUG('t', "Entering SimpleTest");

    Thread *t = new Thread("forked thread");

    t->Fork(reinterpret_cast<VoidFunctionPtr>(SimpleThread), reinterpret_cast<void *>(1));
    SimpleThread(0);
}
*/

//extern int buffer[100];
/*extern int currentSize;
extern int maxSize;

extern Lock* bufferLock;
extern Condition* producerLock;
extern Condition* consumerLock;*/
int currentSize;
int maxSize;

Lock* bufferLock;
Condition* producerLock;
Condition* consumerLock;

void producerThread(int id)
{
    printf("Running producer.....\n");
    Producer* producer = new Producer(id);
    producer->produce();
}

void consumerThread(int id)
{
    printf("Running consumer.....\n");
    Consumer* consumer = new Consumer(id);
    consumer->consume();
}

void
ThreadTest()
{
    DEBUG('t', "Running Producer-Consumer");
    currentSize= 1;
    maxSize = 100;
    bufferLock = new Lock("Buffer Lock");
    producerLock = new Condition("Producer Lock");
    consumerLock = new Condition("Consumer Lock");

    Thread *producers[10], *consumers[10];
    for(int i=0;i<2;i++)
    {
        producers[i] = new Thread("Producer thread"+i+1);
        producers[i]->Fork(reinterpret_cast<VoidFunctionPtr>(producerThread)
                , reinterpret_cast<void *>(i + 1));

    }

    for(int i=0;i<2;i++)
    {
        consumers[i] = new Thread("Producer thread"+i+1);
        consumers[i]->Fork(reinterpret_cast<VoidFunctionPtr>(consumerThread)
                , reinterpret_cast<void *>(i + 1));
    }

}

