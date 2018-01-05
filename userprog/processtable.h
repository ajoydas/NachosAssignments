//
// Created by hp on 04-01-2018.
//

#ifndef CODE_PROCESSTABLE_H
#define CODE_PROCESSTABLE_H

#include "bitmap.h"
#include "../threads/synch.h"

class ProcessTable {
private:
    int size;
    Lock *tableLock;
    void **processes;
    BitMap *bitMap;
public:
    ProcessTable(int size);

    ~ProcessTable();

    int Alloc(void *object);

    void *Get(int index);

    void Release(int index);

    int numberOfRunningProcess();
};


#endif //CODE_PROCESSTABLE_H
