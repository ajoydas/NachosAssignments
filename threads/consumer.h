#ifndef CONSUMER_H
#define CONSUMER_H

#include "copyright.h"
#include "list.h"
#include "synch.h"

extern int currentSize;
extern int maxSize;

extern Lock* bufferLock;
extern Condition* producerLock;
extern Condition* consumerLock;


class Consumer {
public:
    Consumer(int id);	// set initial value
    ~Consumer();   					// de-allocate
    int getId() { return id;}			// debugging assist
    void consume();

private:
    int id;        // useful for debugging

};


#endif