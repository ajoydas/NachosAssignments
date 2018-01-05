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


int SyncConsole::Read(char *buffer, int size, OpenFileId id) {
    consoleLock->Acquire();
    DEBUG('a', "Reading from SyncConsole.....\n");
    char data;
    int dest;
    int bytesRead = 0;
    for(int i=0; i< size; i++){
        syncReadAvail->P();
        data = console->GetChar();
        dest = atoi(buffer + i);
        if(data != '\n'){
            machine->WriteMem(dest, 1, data);
            DEBUG('a', "Dest :%d and buffer+i:%d\n", dest, atoi(buffer + i));
            bytesRead++;
        }
        else
        {
            machine->WriteMem(dest, 1, NULL);
            break;
        }
    }
    consoleLock->Release();
    return bytesRead;
}

void SyncConsole::Write(char *buffer, int size, OpenFileId id) {
    consoleLock->Acquire();
    DEBUG('a', "Writing in SyncConsole.....\n");
    for(int i=0; i<size; i++){
        int src = atoi(buffer + i);
        int* dataInt;
        machine->ReadMem(src, 1, dataInt);
        char data = *((char *) dataInt);
        console->PutChar(data);
        if(data == '\0' || data =='\n') break;
        syncWriteDone->P();
    }
    consoleLock->Release();
}





