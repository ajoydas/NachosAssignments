//
// Created by hp on 04-01-2018.
//

#include "processtable.h"

ProcessTable::ProcessTable(int size)
{
    this->size = size;
    bitMap = new BitMap(size);
    tableLock = new Lock("Process Table");
    processes = new void *[size];
    for(int i=0;i<size;i++)
    {
        processes[i] = NULL;
    }
}


int ProcessTable::Alloc(void *object) {
    tableLock->Acquire();
    int allocated =  bitMap->Find();
    if(allocated != -1)
    {
        processes[allocated] = object;
    }
    tableLock->Release();
    return allocated;
}

void* ProcessTable::Get(int index) {
    void *process = NULL;
    tableLock->Acquire();
    if(bitMap->Test(index))
    {
        process = processes[index];
    }
    tableLock->Release();
    return process;
}

void ProcessTable::Release(int index) {
    tableLock->Acquire();
    if(bitMap->Test(index))
    {
        delete processes[index];
        bitMap->Clear(index);
    }
    tableLock->Release();
}

ProcessTable::~ProcessTable() {
    delete bitMap;
    delete [] processes;
}
