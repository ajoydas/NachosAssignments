// progtest.cc 
//	Test routines for demonstrating that Nachos can load
//	a user program and execute it.  
//
//	Also, routines for testing the Console hardware device.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include <cstdio>
#include "system.h"
#include "console.h"
#include "addrspace.h"
#include "synch.h"
#include "../filesys/openfile.h"
#include "../machine/sysdep.h"
#include "memorymanager.h"
#include "../machine/machine.h"
#include "../threads/system.h"
#include "../machine/console.h"
#include "../threads/synch.h"
#include "processtable.h"
#include "syncconsole.h"

//----------------------------------------------------------------------
// StartProcess
// 	Run a user program.  Open the executable, load it into
//	memory, and jump to it.
//----------------------------------------------------------------------

MemoryManager *memoryManager;
ProcessTable *processTable;
SyncConsole *syncConsole;
int totalNumOfProcess = 10;

void
StartProcess(const char *filename)
{
    DEBUG('a', "Trying to execute Process name:%s\n", filename);
    OpenFile *executable = fileSystem->Open(filename);

    if (executable == NULL) {
	printf("Unable to open file %s\n", filename);
	return;
    }

    memoryManager = new MemoryManager(NumPhysPages);
    processTable = new ProcessTable(totalNumOfProcess);
    syncConsole = new SyncConsole();

    currentThread->space = new AddrSpace(executable);
    currentThread->spaceId = processTable->Alloc(currentThread);
    DEBUG('a', "Init process with Pid = %d created.\n", currentThread->spaceId);

    delete executable;			// close file

    currentThread->space->InitRegisters();		// set the initial register values
    currentThread->space->RestoreState();		// load page table register

    machine->Run();			// jump to the user progam
    ASSERT(false);			// machine->Run never returns;
					// the address space exits
					// by doing the syscall "exit"
}

// Data structures needed for the console test.  Threads making
// I/O requests wait on a Semaphore to delay until the I/O completes.

static Console *console;
static Semaphore *readAvail;
static Semaphore *writeDone;

//----------------------------------------------------------------------
// ConsoleInterruptHandlers
// 	Wake up the thread that requested the I/O.
//----------------------------------------------------------------------

static void ReadAvail(void* arg) { readAvail->V(); }
static void WriteDone(void* arg) { writeDone->V(); }

//----------------------------------------------------------------------
// ConsoleTest
// 	Test the console by echoing characters typed at the input onto
//	the output.  Stop when the user types a 'q'.
//----------------------------------------------------------------------

void 
ConsoleTest (const char *in, const char *out)
{
    char ch;

    console = new Console(in, out, ReadAvail, WriteDone, 0);
    readAvail = new Semaphore("read avail", 0);
    writeDone = new Semaphore("write done", 0);
    
    for (;;) {
	readAvail->P();		// wait for character to arrive
	ch = console->GetChar();
	console->PutChar(ch);	// echo it!
	writeDone->P() ;        // wait for write to finish
	if (ch == 'q') return;  // if q, quit
    }
}
