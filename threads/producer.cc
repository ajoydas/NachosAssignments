#include <cstdio>
#include "copyright.h"
#include "producer.h"
#include "system.h"
#include "../machine/interrupt.h"
#include "../machine/sysdep.h"


Producer::Producer(int id) {
    this->id = id;
}

Producer::~Producer() {

}

void Producer::produce() {

    bufferLock->Acquire();
    while (currentSize == maxSize)
    {
        printf("Producer: %d is sleeping",getId());
        producerLock->Wait(bufferLock);
    }

    currentSize++;
    printf("Producer: %d produced an item Current size: %d",id,currentSize);

    int i=0;
    while (i!=10000)i++;

    if(currentSize == 1)
    {
        printf("Producer: %d is waking a consumer",id);
        consumerLock->Signal(bufferLock);
    }
    bufferLock->Release();

    i=0;
    while (i!=10000)i++;
}

