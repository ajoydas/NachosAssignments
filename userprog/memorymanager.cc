//
// Created by hp on 04-01-2018.
//

#include "memorymanager.h"

MemoryManager::MemoryManager(int numPages)
{
    this->numPages = numPages;
    bitMap = new BitMap(numPages);
    memoryLock = new Lock("Memory Lock");
}

int MemoryManager::AllocPage() {
    memoryLock->Acquire();
    int allocated =  bitMap->Find();
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
