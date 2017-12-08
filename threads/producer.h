#ifndef PRODUCER_H
#define PRODUCER_H

#include "copyright.h"
#include "list.h"
#include "synch.h"

extern int currentSize;
extern int maxSize;

extern Lock* bufferLock;
extern Condition* producerLock;
extern Condition* consumerLock;

class Producer {
public:
    Producer(int id);	// set initial value
    ~Producer();   					// de-allocate
    int getId() { return id;}			// debugging assist
    void produce();

private:
    int id;        // useful for debugging

};


#endif