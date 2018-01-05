//
// Created by hp on 05-01-2018.
//

#ifndef CODE_SYNCCONSOLE_H
#define CODE_SYNCCONSOLE_H

#include "../threads/synch.h"
#include "../machine/console.h"
#include "syscall.h"

class SyncConsole
{
private:
    Lock *consoleLock;
    Console *console;

public:
    SyncConsole();
    int Read(char *buffer, int size, OpenFileId id);
    void Write(char *buffer, int size, OpenFileId id);
};
#endif //CODE_SYNCCONSOLE_H
