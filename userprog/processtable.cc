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
    return allocated+1;
}

void* ProcessTable::Get(int pid) {
    int index = pid-1;
    void *process = NULL;
    tableLock->Acquire();
    if(bitMap->Test(index))
    {
        process = processes[index];
    }
    tableLock->Release();
    return process;
}

void ProcessTable::Release(int pid) {
    int index = pid-1;
    tableLock->Acquire();
    if(bitMap->Test(index))
    {
        // clear object or not
        bitMap->Clear(index);
    }
    tableLock->Release();
}

ProcessTable::~ProcessTable() {
    delete bitMap;
    delete [] processes;
}

int ProcessTable::numberOfRunningProcess() {
    tableLock->Acquire();
    int total = size - bitMap->NumClear();
    tableLock->Release();
    return total;
}
