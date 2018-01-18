//
// Created by hp on 04-01-2018.
//

#ifndef CODE_MEMORYMANAGER_H
#define CODE_MEMORYMANAGER_H


#include "bitmap.h"
#include "../threads/synch.h"

class MemoryManager {
private:
    int numPages;
    BitMap *bitMap;
    Lock *memoryLock;

public:
    MemoryManager(int numPages);

    ~MemoryManager();

    int AllocPage();

    void FreePage(int physPageNum);

    bool PageIsAllocated(int physPageNum);

    int NumOfFreePage();
};


#endif //CODE_MEMORYMANAGER_H
