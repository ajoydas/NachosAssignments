// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include <cstdio>
#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "../machine/machine.h"
#include "../threads/utility.h"
#include "../threads/system.h"
#include "../filesys/openfile.h"
#include "memorymanager.h"
#include "addrspace.h"
#include "processtable.h"
#include "../machine/sysdep.h"
#include "syncconsole.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------
extern MemoryManager *memoryManager;
extern ProcessTable *processTable;
extern SyncConsole *syncConsole;

void SysCallHaltHandler();

void SysCallExecHandler(int arg1);

void SysCallExitHandler();

void forwardPC();

void SysCallReadHandler(int buffer, int size);

void SysCallWriteHandler(int buffer, int size);

void forkFuncForProgram(int pid) {
    Thread *thread = (Thread *)(processTable->Get(pid));
    thread->space->InitRegisters();		// set the initial register values
    thread->space->RestoreState();		// load page table register
    DEBUG('a', "Forking new thread with pid: %d\n", pid);
    machine->Run();
    ASSERT(false);
}

SpaceId Exec(char *name) {
    OpenFile* executable = fileSystem->Open(name);
    if (executable != NULL)
    {
        // Set up a new thread and alloc address space for it.
        Thread* thread = new Thread(name);
        thread->space =  new AddrSpace(executable);
        int pid =  processTable->Alloc(thread);
        thread->spaceId = pid;
        DEBUG('a', "Current thread %d-> initiating %s with Pid: %d\n", currentThread->spaceId, name, pid);
        if(pid == 0)
        {
            printf("Process creation failed! Maximum process limit exceeded.\n");
            ASSERT(false);
        }else {
            DEBUG('a', "Forking new thread with pid: %d\n", pid);
            thread->Fork(reinterpret_cast<VoidFunctionPtr>(forkFuncForProgram),
                         reinterpret_cast<void *>(pid));
        }

        //delete executable;
        return pid;
    }
    // Can't open executable file, so return -1.
    machine->WriteRegister(2, -1);
    return -1;
}

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);
    int arg1 = machine->ReadRegister(4);
    int arg2 = machine->ReadRegister(5);
    int arg3 = machine->ReadRegister(6);
    int arg4 = machine->ReadRegister(7);

    DEBUG('a', "System Call happened.\n %d %d\n", which, type);

    switch (which)
    {
        case SyscallException:
            switch (type)
            {
                case SC_Halt:
                    SysCallHaltHandler();
                    break;

                case SC_Exec:
                    SysCallExecHandler(arg1);
                    break;

                case SC_Exit:
                    SysCallExitHandler();
                    break;

                case SC_Read:
                    SysCallReadHandler(arg1, arg2);
                    break;

                case SC_Write:
                    SysCallWriteHandler(arg1, arg2);
                    break;

                default:
                    printf("Unexpected syscall happened, %d %d\n", which, type);
                    break;
            }
            break;
        case PageFaultException:
            {
                stats->numPageFaults++;
                DEBUG('v', "Page fault exception for process: %d\n", currentThread->spaceId);
                int faultAddr = machine->ReadRegister(39);
                int vpn = faultAddr/PageSize;
                int physPageNo;
                if (memoryManager->NumOfFreePage() > 0) {
                    physPageNo = memoryManager->Alloc(currentThread->spaceId, &(machine->pageTable[vpn]));

                } else {
                    physPageNo = memoryManager->AllocByForce();
//                    memoryManager->processMap[physPageNo] = currentThread->spaceId;
//                    memoryManager->entries[physPageNo] = &(machine->pageTable[vpn]);
                }
                currentThread->space->loadIntoFreePage(faultAddr, physPageNo);
            }
            break;
        default:
            printf("Unexpected user mode exception %d %d\n", which, type);
            ASSERT(false);
    }
}


void SysCallHaltHandler() {
    DEBUG('a', "Shutdown, initiated by user program.\n");
    interrupt->Halt();
}



void forwardPC() {
    int pc = machine->ReadRegister(PCReg);
    machine->WriteRegister(PrevPCReg, pc);
    pc = machine->ReadRegister(NextPCReg);
    machine->WriteRegister(PCReg, pc);
    pc += 4;
    machine->WriteRegister(NextPCReg, pc);
}

void SysCallExecHandler(int arg1) {
    char filename[100];
    int i = 0;

    // Get the executable file name from user space.
    do
    {
        machine->ReadMem(arg1 + i, 1, (int*)&filename[i]);
    }while(filename[i++] != '\0');
    filename[0]='.';

    DEBUG('a', "Trying to execute Process name: %s\n", filename);
    int pid = Exec(filename);
    if(pid == -1)
    {
        printf("File %s not found.\n",filename);
        ASSERT(false);
    }
    else
    {
        machine->WriteRegister(2, pid);
    }

    forwardPC();
}



//void Exit(int status) {
//    DEBUG('a', "Exit Code: %d", status);
//    for(int i=0; i< machine->pageTableSize;i++)
//    {
//        memoryManager->FreePage(machine->pageTable[i].physicalPage);
//    }
//
//    processTable->Release(currentThread->spaceId);
//    if(processTable->numberOfRunningProcess() == 0) {
//        printf("No more processes to run. Halting.....");
//        interrupt->Halt();
//    } else{
//        DEBUG('a',"System call Exit for Pid: %d",currentThread->spaceId);
//        currentThread->Finish();
//    }
//}

void SysCallExitHandler() {
    int status = machine->ReadRegister(4);
//    Exit(arg);
    printf("Exit Code: %d\n", status);
    DEBUG('a', "From current threadId: %d\n", currentThread->spaceId);
    for(int i=0; i< machine->pageTableSize;i++)
    {
        if(machine->pageTable[i].valid){
            DEBUG('v', "Freeing virtual: %d physical: %d\n",machine->pageTable[i].virtualPage,
                  machine->pageTable[i].physicalPage);
            memoryManager->FreePage(machine->pageTable[i].physicalPage);
        }
    }

    processTable->Release(currentThread->spaceId);
    if(processTable->numberOfRunningProcess() == 0) {
        printf("No more processes to run. Halting.....\n");
        interrupt->Halt();
    } else{
        DEBUG('a',"System call Exit for Pid: %d\n",currentThread->spaceId);
        currentThread->Finish();
    }
    forwardPC();
}


int Read(int buffer, int size, OpenFileId id)
{
    syncConsole->Read(buffer, size, id);
}

void SysCallReadHandler(int buffer, int size) {
    DEBUG('a', "Reading from buffer: %d of size:%d\n", buffer, size);
    int bytesRead = Read(buffer, size, 0);
    DEBUG('a', "Read from buffer: %d of size:%d\n", buffer, bytesRead);
    machine->WriteRegister(2, bytesRead);
    forwardPC();
}


void Write(int buffer, int size, OpenFileId id){
    syncConsole->Write(buffer, size, id);
}

void SysCallWriteHandler(int buffer, int size) {
    DEBUG('a', "Writing to buffer: %d of size:%d\n", buffer, size);
    Write(buffer, size, 0);
    DEBUG('a', "Wrote to buffer: %d of size:%d\n", buffer, size);
    machine->WriteRegister(2, 1);
    forwardPC();
}

































