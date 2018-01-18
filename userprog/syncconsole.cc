//
// Created by hp on 05-01-2018.
//

#include <cstdlib>
#include "syncconsole.h"
#include "system.h"

static Semaphore *syncReadAvail;
static Semaphore *syncWriteDone;

static void isReadAvail() {
    syncReadAvail->V();
}

static void isWriteDone() {
    syncWriteDone->V();
}

SyncConsole::SyncConsole(){
    this->console = new Console(NULL, NULL, reinterpret_cast<VoidFunctionPtr>(isReadAvail),
                                reinterpret_cast<VoidFunctionPtr>(isWriteDone), 0);
    consoleLock = new Lock("Sync Console Lock");
    syncReadAvail = new Semaphore("read avail", 0);
    syncWriteDone = new Semaphore("write done", 0);
}


int SyncConsole::Read(int buffer, int size, OpenFileId id) {
    consoleLock->Acquire();
    DEBUG('a', "Reading from SyncConsole.....\n");
    char data;
    int dest;
    int bytesRead = 0;
    for(int i=0; i< size; i++){
        syncReadAvail->P();
        data = console->GetChar();
        dest = buffer + i;

        machine->WriteMem(dest, 1, data);
        DEBUG('a', "Dest :%d and buffer+i:%d -- %c\n", dest, buffer + i, data);
        bytesRead++;

        if(data == '\n'){
            break;
        }
    }
    consoleLock->Release();
    return bytesRead;
}

void SyncConsole::Write(int buffer, int size, OpenFileId id) {
    consoleLock->Acquire();
    DEBUG('a', "Writing in SyncConsole..... %d , size: %d\n", buffer,size);
    for(int i=0; i<size; i++){
        int src = buffer + i;
        int dataInt;
        machine->ReadMem(src, 1, &dataInt);
        char data = (char) dataInt;
        console->PutChar(data);
        if(data == '\0' || data =='\n') break;
        syncWriteDone->P();
    }
    consoleLock->Release();
}





