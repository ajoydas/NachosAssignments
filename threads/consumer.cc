#include <cstdio>
#include "copyright.h"
#include "consumer.h"
#include "system.h"
#include "../machine/interrupt.h"
#include "../machine/sysdep.h"


Consumer::Consumer(int id) {
    this->id = id;
}

Consumer::~Consumer() {

}

void Consumer::consume(){
    bufferLock->Acquire();
    while (currentSize == 0)
    {
        printf("Consumer: %d is sleeping",getId());
        consumerLock->Wait(bufferLock);
    }

    currentSize--;
    printf("Consumer: %d consumed an item. Current size: %d",id,currentSize);

    int i=0;
    while (i!=100000)i++;

    if(currentSize == maxSize-1)
    {
        printf("Consumer: %d is waking a producer",id);
        consumerLock->Signal(bufferLock);
    }
    bufferLock->Release();

    i=0;
    while (i!=100000)i++;
}
