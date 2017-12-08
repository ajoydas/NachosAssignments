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

    int count=1;
    while (count) {
//        printf("Running produce of.....%d\n",id);
        bufferLock->Acquire();
        while (currentSize == maxSize)
        {
            printf("Producer: %d is sleeping\n",getId());
            producerLock->Wait(bufferLock);
        }

        currentSize++;
        printf("Producer: %d produced an item\n",id);
        printf("Current size: %d\n",currentSize);

        int i=0;
        while (i!=50000000)i++;

        if(currentSize == 1)
        {
            printf("Producer: %d is waking a consumer\n",id);
            consumerLock->Signal(bufferLock);
        }
        bufferLock->Release();

        i=0;
        while (i!=50000000)i++;

//        printf("Ending produce of.....%d\n",id);
        currentThread->Yield();
    }
}

