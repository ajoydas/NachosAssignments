//
// Created by hp on 04-01-2018.
//

#ifndef CODE_MEMORYMANAGER_H
#define CODE_MEMORYMANAGER_H


#include "bitmap.h"
#include "../threads/synch.h"
#include "../machine/translate.h"


class SwapSpace{
public:
    int virtualPageNo;
    int processId;
    char* swapPageData;
    SwapSpace(){
        processId = -1;
        virtualPageNo = -1;
        swapPageData = new char[PageSize];
    }
};

class MemoryManager {
private:
    int numPages;
    BitMap *bitMap;
    Lock *memoryLock;

public:
    int *processMap;
    TranslationEntry **entries;

    MemoryManager(int numPages);

    ~MemoryManager();

    int AllocPage();

    void FreePage(int physPageNum);

    bool PageIsAllocated(int physPageNum);

    int NumOfFreePage();

    int Alloc(int processNo, TranslationEntry *entry);

    int AllocByForce();
};


#endif //CODE_MEMORYMANAGER_H
