//
// Created by hp on 04-01-2018.
//

#include "memorymanager.h"
#include "../machine/sysdep.h"

MemoryManager::MemoryManager(int numPages)
{
    this->numPages = numPages;
    bitMap = new BitMap(numPages);
    memoryLock = new Lock("Memory Lock");
    processMap = new int[numPages];
    entries = new TranslationEntry[numPages];
}

int MemoryManager::AllocPage() {
    memoryLock->Acquire();
    int allocated =  bitMap->Find();
    memoryLock->Release();
    return allocated;
}


int MemoryManager::Alloc(int processNo, TranslationEntry &entry) {
    memoryLock->Acquire();
    int allocated =  bitMap->Find();
//    printf("allocated: %d", allocated);
    processMap[allocated] = processNo;
    entries[allocated] = entry;
    memoryLock->Release();
    return allocated;
}

int MemoryManager::AllocByForce() {
    memoryLock->Acquire();
    int allocated = Random()%numPages;
    DEBUG('v', "Randomly allocated page: %d", allocated);

    long least_time = entries[allocated].time;
    for (int i = 0; i < numPages; i++) {
        if(entries[i].time < least_time && !entries[i].dirty){
            least_time = entries[i].time;
            allocated= i;
        }
    }
    memoryLock->Release();
    return allocated;
}

void MemoryManager::FreePage(int physPageNum) {
    memoryLock->Acquire();
    bitMap->Clear(physPageNum);
    memoryLock->Release();
}

bool MemoryManager::PageIsAllocated(int physPageNum) {
    memoryLock->Acquire();
    bool res = bitMap->Test(physPageNum);
    memoryLock->Release();
    return res;
}

int MemoryManager::NumOfFreePage() {
    memoryLock->Acquire();
    int num = bitMap->NumClear();
    memoryLock->Release();
    return num;
}

MemoryManager::~MemoryManager() {
    delete bitMap;
}
