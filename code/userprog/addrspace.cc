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

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "noff.h"
#include "table.h"
#include "synch.h"

extern "C" { int bzero(char *, int); };

Table::Table(int s) : map(s), table(0), lock(0), size(s) {
    table = new void *[size];
    lock = new Lock("TableLock");
}

Table::~Table() {
    if (table) {
	delete table;
	table = 0;
    }
    if (lock) {
	delete lock;
	lock = 0;
    }
}

void *Table::Get(int i) {
    // Return the element associated with the given if, or 0 if
    // there is none.

    return (i >=0 && i < size && map.Test(i)) ? table[i] : 0;
}

int Table::Put(void *f) {
    // Put the element in the table and return the slot it used.  Use a
    // lock so 2 files don't get the same space.
    int i;	// to find the next slot

    lock->Acquire();
    i = map.Find();
    lock->Release();
    if ( i != -1)
	table[i] = f;
    return i;
}

void *Table::Remove(int i) {
    // Remove the element associated with identifier i from the table,
    // and return it.

    void *f =0;

    if ( i >= 0 && i < size ) {
	lock->Acquire();
	if ( map.Test(i) ) {
	    map.Clear(i);
	    f = table[i];
	    table[i] = 0;
	}
	lock->Release();
    }
    return f;
}

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
//	"executable" is the file containing the object code to load into memory
//
//      It's possible to fail to fully construct the address space for
//      several reasons, including being unable to allocate memory,
//      and being unable to read key parts of the executable.
//      Incompletely constructed address spaces have the member
//      constructed set to false.
//----------------------------------------------------------------------

AddrSpace::AddrSpace(OpenFile *executable) : fileTable(MaxOpenFiles)
{
    NoffHeader noffH;
    unsigned int size;
    int newProcessId;
    /**
     * ADDITIONS FOR PROJECT 3 ----------------START --------------------
     */
    executableFilePointer = executable;
    /**
     * ADDITIONS FOR PROJECT 3 ----------------END --------------------
     */
    processTableAccessLock->Acquire();

    if((newProcessId = processTableBitMap->Find()) == -1)
	{
    	printf("SYSTEM DOES NOT SUPPORT MORE THAN %d PROCESSES AT A TIME TO BE LOADED OR RUNNING.... \n",
    			MAX_PROCESS);
    	interrupt->Halt();
	}
    processId = newProcessId;
    processTableArray[processId].addrSpace = this;
    processTableArray[processId].totalThreads = 1;
    processTableArray[processId].threadCounter = 1;
    processTableArray[processId].activeThreadCounter = 1;

    processTableAccessLock->Release();

    pageTableLock = new Lock("pageTableLock");

    // Don't allocate the input or output to disk files
    fileTable.Put(0);
    fileTable.Put(0);

    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) && 
		(WordToHost(noffH.noffMagic) == NOFFMAGIC))
    	SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size ;

    numCodePages = divRoundUp(noffH.code.size,PageSize);
    numCodeDataPages = divRoundUp(noffH.code.size + noffH.initData.size,PageSize);

    numPages = divRoundUp(size, PageSize) + divRoundUp(UserStackSize,PageSize);
                                                // we need to increase the size
												// to leave room for the stack
    size = numPages * PageSize;
    addrSpaceSize = size;

#ifdef PROJECT2
    ASSERT(numPages + totalPagesReserved <= NumPhysPages);		// check we're not trying
						// to run anything too big --
						// at least until we have
						// virtual memory
#endif
    // zero out the entire address space, to zero the uninitialized data segment
    // and the stack segment
    //
#if 0
    bzero(machine->mainMemory, size);
#endif

    DEBUG('a', "Initializing address space, num pages %d, size %d\n", 
					numPages, size);

    pageTableLock->Acquire();

#ifdef PROJECT2
// first, set up the translation 
    pageTable = new TranslationEntry[numPages];
#endif
    /**
     * ADDITIONS FOR PROJECT 3 ----------------START --------------------
     */
    pageTable = new newTranslationEntry[numPages];

    /**
     * 1: Nothing gets pre-loaded into main Memory..
     * 2: Will wait for Page Fault Exception and then handle page fault exceptions
     */

    /**
     * ADDITIONS FOR PROJECT 3 ----------------END --------------------
     */
    for (unsigned int i = 0; i < numPages; i++)
    {
#ifdef PROJECT2
    	mainMemoryAccessLock->Acquire();
#endif
		pageTable[i].virtualPage = i;	// for now, virtual page # = phys page #

#if 0
		pageTable[i].physicalPage = i;
#endif

#ifdef PROJECT2
		pageTable[i].physicalPage = mainMemoryBitMap->Find();
		if(pageTable[i].physicalPage == -1)
		{
			printf("%s : No Memory available for allocation \n",currentThread->getName());
	    	mainMemoryAccessLock->Release();
			interrupt->Halt();
		}
		DEBUG('b',"Found Physical Page %d for Process %d from Main Memory Bit Map \n",
				pageTable[i].physicalPage,processId);
		//bzero(machine->mainMemory + pageTable[i].physicalPage* PageSize,PageSize);
		DEBUG('a',"Initializing PHYSICAL PAGE = %d, VIRTUAL PAGE = %d \n",pageTable[i].physicalPage,i);
#endif

		if(i < numCodeDataPages)
		{
			pageTable[i].physicalPage = -1;
		}
#ifdef PROJECT2
		pageTable[i].valid = TRUE;
#endif
	    /**
	     * ADDITIONS FOR PROJECT 3 ----------------START --------------------
	     */
		pageTable[i].valid = FALSE;
	    /**
	     * ADDITIONS FOR PROJECT 3 ----------------END --------------------
	     */
		pageTable[i].use = FALSE;
		pageTable[i].dirty = FALSE;
		pageTable[i].readOnly = FALSE;  // if the code segment was entirely on
						// a separate page, we could set its
						// pages to be read-only
#ifdef PROJECT2
		if(i<numCodeDataPages)
		{
	    	executable->ReadAt(&(machine->mainMemory[pageTable[i].physicalPage*PageSize]),
	    			PageSize,(noffH.code.inFileAddr + (i*PageSize)));
	    	DEBUG('a',"Page copied to PageTable at Physical Addr: 0x%x . Code/Data of size %d "
	    			"copied from 0x%x \n",pageTable[i].physicalPage*PageSize,PageSize,
	    			(noffH.code.inFileAddr + i*PageSize));
			totalPagesReserved++;
		}
		mainMemoryAccessLock->Release();
#endif

	    /**
	     * ADDITIONS FOR PROJECT 3 ----------------START --------------------
	     */

		if(i < numCodeDataPages)
		{
			pageTable[i].location = EXECUTABLE;
			pageTable[i].byteOffSet = noffH.code.inFileAddr + (i*PageSize);
		}
		else
		{
			pageTable[i].location = NEITHER;
		}


	    /**
	     * ADDITIONS FOR PROJECT 3 ----------------END --------------------
	     */


    }

    pageTableLock->Release();

// then, copy in the code and data segments into memory

#if 0
    if (noffH.code.size > 0)
    {
        DEBUG('a', "Initializing code segment, at 0x%x, size %d\n", 
			noffH.code.virtualAddr, noffH.code.size);
        executable->ReadAt(&(machine->mainMemory[noffH.code.virtualAddr]),
			noffH.code.size, noffH.code.inFileAddr);
    }
    if (noffH.initData.size > 0)
    {
        DEBUG('a', "Initializing data segment, at 0x%x, size %d\n", 
			noffH.initData.virtualAddr, noffH.initData.size);
        executable->ReadAt(&(machine->mainMemory[noffH.initData.virtualAddr]),
			noffH.initData.size, noffH.initData.inFileAddr);
    }


    for(unsigned int i=0;i<numCodeDataPages;i++)
    {
    	executable->ReadAt(&(machine->mainMemory[pageTable[i].physicalPage*PageSize]),
    			PageSize,(noffH.code.inFileAddr + (i*PageSize)));
    	DEBUG('a',"Page copied to PageTable at Physical Addr: 0x%x . Code/Data of size %d "
    			"copied from 0x%x \n",pageTable[i].physicalPage*PageSize,PageSize,
    			(noffH.code.inFileAddr + i*PageSize));
		totalPagesReserved++;
    }
#endif

}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
//
// 	Dealloate an address space.  release pages, page tables, files
// 	and file tables
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
    delete pageTable;
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
    DEBUG('a', "Initializing stack register to 0x%x\n", numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState() 
{
    /**
     * ADDITIONS FOR PROJECT 3 ----------------START --------------------
     */

	IPTLock->Acquire();
	//Disable the interrupts: Modifying TLB
	IntStatus old=interrupt->SetLevel(IntOff);

	//Invalidating all entries on TLB during context switch
	for(int i=0; i<TLBSize; i++)
	{
		if(machine->tlb[i].valid==TRUE)
		{
			//copy the dirty bit from the TLB to IPT
			IPT[machine->tlb[i].physicalPage].dirty=machine->tlb[i].dirty;
		}
		machine->tlb[i].valid = FALSE;
	}

	//Restoring the interrupts: Modification to TLB completed
	interrupt->SetLevel(old);
	IPTLock->Release();

    /**
     * ADDITIONS FOR PROJECT 3 ----------------END --------------------
     */

}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState() 
{
#ifdef PROJECT2
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
#endif
}
