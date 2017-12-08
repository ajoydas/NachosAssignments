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

    int count=1;
    while (count) {
        printf("Running consume of.....%d\n",id);
        bufferLock->Acquire();
        while (currentSize == 0)
        {
            printf("Consumer: %d is sleeping\n",getId());
            consumerLock->Wait(bufferLock);
        }

        currentSize--;
        printf("Consumer: %d consumed an item. Current size: %d\n",id,currentSize);

        int i=0;
        while (i!=50000000)i++;

        if(currentSize == maxSize-1)
        {
            printf("Consumer: %d is waking a producer\n",id);
            consumerLock->Signal(bufferLock);
        }
        bufferLock->Release();

        i=0;
        while (i!=50000000)i++;

        printf("Ending produce of.....%d\n",id);
        currentThread->Yield();
    }
}
