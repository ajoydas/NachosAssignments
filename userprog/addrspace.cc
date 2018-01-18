// addrspace.cc 
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -N -T 0 option 
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you haven't implemented the file system yet, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include <cstdio>
#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "../machine/machine.h"
#include "../threads/utility.h"
#include "../machine/sysdep.h"
#include "../filesys/openfile.h"
#include "memorymanager.h"
#include "../machine/interrupt.h"
#include "../threads/system.h"
#include <vector>

using namespace std;
//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the 
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void 
SwapHeader (NoffHeader *noffH)
{
	noffH->noffMagic = WordToHost(noffH->noffMagic);
	noffH->code.size = WordToHost(noffH->code.size);
	noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
	noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
	noffH->initData.size = WordToHost(noffH->initData.size);
	noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
	noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
	noffH->uninitData.size = WordToHost(noffH->uninitData.size);
	noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
	noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//	Assumes that the object code file is in NOFF format.
//
//	First, set up the translation from program memory to physical 
//	memory.  For now, this is really simple (1:1), since we are
//	only uniprogramming, and we have a single unsegmented page table
//
//	"executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

extern MemoryManager *memoryManager;
AddrSpace::AddrSpace(OpenFile *exec)
{
    this->executable = exec;
    unsigned int i, j, size;

    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) && 
		(WordToHost(noffH.noffMagic) == NOFFMAGIC))
    	SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

// how big is address space?
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size 
			+ UserStackSize;	// we need to increase the size
						// to leave room for the stack
    numPages = divRoundUp(size, PageSize);
    size = numPages * PageSize;

//    ASSERT(numPages <= NumPhysPages);		// check we're not trying
						// to run anything too big --
						// at least until we have
						// virtual memory

    DEBUG('a', "Initializing address space, num pages %d, size %d\n", 
					numPages, size);
// first, set up the translation

//    vector<int> pages;
    pageTable = new TranslationEntry[numPages];
    for (i = 0; i < numPages; i++) {
        pageTable[i].virtualPage = i;	// for now, virtual page # = phys page #

/*        int physicalPage = memoryManager->AllocPage();
        DEBUG('m',"VPN: %d PPN: %d\n", i, physicalPage);

        if(physicalPage == -1){
            DEBUG('m', "Not enough memory.Freeing up allocated pages....");
            for(int k=0; k <pages.size();k++)
            {
                memoryManager->FreePage(pages[k]);
            }
            ASSERT(false);
        }
        pages.push_back(physicalPage);*/

        pageTable[i].physicalPage = -1;
        pageTable[i].valid = false;
        pageTable[i].use = false;
        pageTable[i].dirty = false;
        pageTable[i].readOnly = false;  // if the code segment was entirely on
					// a separate page, we could set its 
					// pages to be read-only
    }
    
// zero out the entire address space, to zero the unitialized data segment 
// and the stack segment
//    bzero(machine->mainMemory, size);

// then, copy in the code and data segments into memory

    /*IntStatus prevState = interrupt->SetLevel(IntOff);
    for(j=0; j< numPages; j++)
    {
        bzero(&machine->mainMemory[PageSize * pageTable[j].physicalPage], PageSize);
    }

    unsigned int pagesCode, pagesData, startAddr, physAddr;
    pagesCode= divRoundUp(noffH.code.size, PageSize);
    pagesData= divRoundUp(noffH.initData.size, PageSize);

    if (noffH.code.size > 0) {
        DEBUG('a', "Initializing code segment, at 0x%x, size %d\n",
              noffH.code.virtualAddr, noffH.code.size);
        startAddr = noffH.code.virtualAddr;
        for(j=0; j< pagesCode; j++){
            physAddr = virtualToPhysAddr(startAddr);
            char *physBuffer = &(machine->mainMemory[physAddr]);
            int psize = PageSize;
            int fileOffset = noffH.code.inFileAddr + j * PageSize;
            executable->ReadAt(physBuffer, psize, fileOffset);
            startAddr += PageSize;
        }
    }
    if (noffH.initData.size > 0) {
        DEBUG('a', "Initializing data segment, at 0x%x, size %d\n",
              noffH.initData.virtualAddr, noffH.initData.size);
        startAddr = noffH.initData.virtualAddr;
        for(j=0; j< pagesData; j++){
            physAddr = virtualToPhysAddr(startAddr);
            char *physBuffer = &(machine->mainMemory[physAddr]);
            int psize = PageSize;
            int fileOffset = noffH.initData.inFileAddr + j * PageSize;
            executable->ReadAt(physBuffer, psize, fileOffset);
            startAddr += PageSize;
        }
    }

    interrupt->SetLevel(prevState);*/
}


void AddrSpace::loadIntoFreePage(int addr, int physicalPageNo){
//    int physicalPageNo=memoryManager->AllocPage();
    int vpn = addr/PageSize;
    pageTable[vpn].physicalPage = physicalPageNo;
    pageTable[vpn].valid = true;

    DEBUG('v',"\n\n\n");
    DEBUG('v',"code size = %d, code infileAddr: %d, data size %d undata size = %d\n\n",
          noffH.code.size, noffH.code.inFileAddr, noffH.initData.size, noffH.uninitData.size);

    int codeOffset=0, codeSize=0, initDataOffset=0, initDataSize=0,
            uninitDataOffset=0, uninitDataSize=0;
    int physAddr = physicalPageNo*PageSize;
    if(noffH.code.virtualAddr <= addr && (noffH.code.virtualAddr + noffH.code.size) > addr){
        codeOffset = (addr - noffH.code.virtualAddr)/PageSize;
        codeSize = noffH.code.size - codeOffset*PageSize;
        codeSize = min(codeSize, PageSize);
        if(noffH.code.size > 0){
            DEBUG('v',"___code = %d___\n", codeSize);
            executable->ReadAt(&machine->mainMemory[physAddr], codeSize,
                               noffH.code.inFileAddr + codeOffset*PageSize);
        }
        if(codeSize < PageSize){
            initDataSize = PageSize - codeSize;
            initDataSize = min(initDataSize, noffH.initData.size);
            DEBUG('v',"___code mid init data = %d___\n", initDataSize);
            if(noffH.initData.size > 0){
                executable->ReadAt(&machine->mainMemory[physAddr + codeSize],
                                   initDataSize, noffH.initData.inFileAddr);
            }
        }
        if((codeSize + initDataSize) < PageSize){
            int freePageSize = PageSize - codeSize - initDataSize;
            DEBUG('v',"___code mid uninit data = %d___\n", freePageSize);
            bzero(&machine->mainMemory[physAddr + codeSize + initDataSize], freePageSize);
        }
    }
    else if(noffH.initData.virtualAddr <= addr && (noffH.initData.virtualAddr + noffH.initData.size) > addr){
        initDataOffset = (addr - noffH.initData.virtualAddr)/PageSize;
        initDataSize = min(noffH.initData.size - initDataOffset*PageSize, PageSize);
        if(noffH.initData.size > 0 ){
            DEBUG('v',"___init data = %d___\n", initDataSize);
            executable->ReadAt(&machine->mainMemory[physAddr], initDataSize, noffH.initData.inFileAddr + initDataOffset*PageSize);
        }
        if(initDataSize < PageSize){
            int freePageSize = PageSize - initDataSize;
            DEBUG('v',"___init mid uninit data = %d___\n", freePageSize);
            bzero(&machine->mainMemory[physAddr + initDataSize], freePageSize);
        }
    }
    else if(noffH.uninitData.virtualAddr <= addr && (noffH.uninitData.virtualAddr + noffH.uninitData.size) > addr){
        DEBUG('v',"___uninit  data = %d___\n", PageSize);
        bzero(&machine->mainMemory[physAddr], PageSize);
    }
    else{
        DEBUG('v',"___zero = %d___\n", PageSize);
        bzero(&machine->mainMemory[physAddr], PageSize);
    }
    printf("\n\n\n");
}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
    delete executable;
//    for(int i=0; i<numPages; i++)
//    {
//        memoryManager->FreePage(pageTable[i].physicalPage);
//    }
    delete []pageTable;
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------

void
AddrSpace::InitRegisters()
{
    int i;

    for (i = 0; i < NumTotalRegs; i++)
	machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister(PCReg, 0);	

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister(NextPCReg, 4);

   // Set the stack register to the end of the address space, where we
   // allocated the stack; but subtract off a bit, to make sure we don't
   // accidentally reference off the end!
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState() 
{}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState() 
{
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
}



unsigned int
AddrSpace::virtualToPhysAddr(int virtualAddr){
    unsigned int virtualPageNum = (unsigned)virtualAddr/PageSize;
    unsigned int offset = virtualAddr%PageSize;
    unsigned int pageFrame = pageTable[virtualPageNum].physicalPage;
    unsigned int physicalAddr = pageFrame*PageSize + offset;
    return physicalAddr;

}