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

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include <stdio.h>
#include <iostream>

using namespace std;

int copyin(unsigned int vaddr, int len, char *buf) {
    // Copy len bytes from the current thread's virtual address vaddr.
    // Return the number of bytes so read, or -1 if an error occors.
    // Errors can generally mean a bad virtual address was passed in.
    bool result;
    int n=0;			// The number of bytes copied in
    int *paddr = new int;

    while ( n >= 0 && n < len) {
      result = machine->ReadMem( vaddr, 1, paddr );
      while(!result) // FALL 09 CHANGES
	  {
   			result = machine->ReadMem( vaddr, 1, paddr ); // FALL 09 CHANGES: TO HANDLE PAGE FAULT IN THE ReadMem SYS CALL
	  }	
      
      buf[n++] = *paddr;
     
      if ( !result ) {
	//translation failed
	return -1;
      }

      vaddr++;
    }

    delete paddr;
    return len;
}

int copyout(unsigned int vaddr, int len, char *buf) {
    // Copy len bytes to the current thread's virtual address vaddr.
    // Return the number of bytes so written, or -1 if an error
    // occors.  Errors can generally mean a bad virtual address was
    // passed in.
    bool result;
    int n=0;			// The number of bytes copied in

    while ( n >= 0 && n < len) {
      // Note that we check every byte's address
      result = machine->WriteMem( vaddr, 1, (int)(buf[n++]) );

      if ( !result ) {
	//translation failed
	return -1;
      }

      vaddr++;
    }

    return n;
}

void Create_Syscall(unsigned int vaddr, int len) {
    // Create the file with the name in the user buffer pointed to by
    // vaddr.  The file name is at most MAXFILENAME chars long.  No
    // way to return errors, though...
    char *buf = new char[len+1];	// Kernel buffer to put the name in

    if (!buf) return;

    if( copyin(vaddr,len,buf) == -1 ) {
	printf("%s","Bad pointer passed to Create\n");
	delete buf;
	return;
    }

    buf[len]='\0';

    fileSystem->Create(buf,0);
    delete[] buf;
    return;
}

int Open_Syscall(unsigned int vaddr, int len) {
    // Open the file with the name in the user buffer pointed to by
    // vaddr.  The file name is at most MAXFILENAME chars long.  If
    // the file is opened successfully, it is put in the address
    // space's file table and an id returned that can find the file
    // later.  If there are any errors, -1 is returned.
    char *buf = new char[len+1];	// Kernel buffer to put the name in
    OpenFile *f;			// The new open file
    int id;				// The openfile id

    if (!buf) {
	printf("%s","Can't allocate kernel buffer in Open\n");
	return -1;
    }

    if( copyin(vaddr,len,buf) == -1 ) {
	printf("%s","Bad pointer passed to Open\n");
	delete[] buf;
	return -1;
    }

    buf[len]='\0';

    f = fileSystem->Open(buf);
    delete[] buf;

    if ( f ) {
	if ((id = currentThread->space->fileTable.Put(f)) == -1 )
	    delete f;
	return id;
    }
    else
	return -1;
}

void Write_Syscall(unsigned int vaddr, int len, int id) {
    // Write the buffer to the given disk file.  If ConsoleOutput is
    // the fileID, data goes to the synchronized console instead.  If
    // a Write arrives for the synchronized Console, and no such
    // console exists, create one. For disk files, the file is looked
    // up in the current address space's open file table and used as
    // the target of the write.
    
    char *buf;		// Kernel buffer for output
    OpenFile *f;	// Open file for output

    if ( id == ConsoleInput) return;
    
    if ( !(buf = new char[len]) ) {
	printf("%s","Error allocating kernel buffer for write!\n");
	return;
    } else {
        if ( copyin(vaddr,len,buf) == -1 ) {
	    printf("%s","Bad pointer passed to to write: data not written\n");
	    delete[] buf;
	    return;
	}
    }

    if ( id == ConsoleOutput) {
      for (int ii=0; ii<len; ii++) {
	printf("%c",buf[ii]);
      }

    } else {
	if ( (f = (OpenFile *) currentThread->space->fileTable.Get(id)) ) {
	    f->Write(buf, len);
	} else {
	    printf("%s","Bad OpenFileId passed to Write\n");
	    len = -1;
	}
    }

    delete[] buf;
}

int Read_Syscall(unsigned int vaddr, int len, int id) {
    // Write the buffer to the given disk file.  If ConsoleOutput is
    // the fileID, data goes to the synchronized console instead.  If
    // a Write arrives for the synchronized Console, and no such
    // console exists, create one.    We reuse len as the number of bytes
    // read, which is an unnessecary savings of space.
    char *buf;		// Kernel buffer for input
    OpenFile *f;	// Open file for output

    if ( id == ConsoleOutput) return -1;
    
    if ( !(buf = new char[len]) ) {
	printf("%s","Error allocating kernel buffer in Read\n");
	return -1;
    }

    if ( id == ConsoleInput) {
      //Reading from the keyboard
      scanf("%s", buf);

      if ( copyout(vaddr, len, buf) == -1 ) {
	printf("%s","Bad pointer passed to Read: data not copied\n");
      }
    } else {
	if ( (f = (OpenFile *) currentThread->space->fileTable.Get(id)) ) {
	    len = f->Read(buf, len);
	    if ( len > 0 ) {
	        //Read something from the file. Put into user's address space
  	        if ( copyout(vaddr, len, buf) == -1 ) {
		    printf("%s","Bad pointer passed to Read: data not copied\n");
		}
	    }
	} else {
	    printf("%s","Bad OpenFileId passed to Read\n");
	    len = -1;
	}
    }

    delete[] buf;
    return len;
}

void Close_Syscall(int fd) {
    // Close the file associated with id fd.  No error reporting.
    OpenFile *f = (OpenFile *) currentThread->space->fileTable.Remove(fd);

    if ( f ) {
      delete f;
    } else {
      printf("%s","Tried to close an unopen file\n");
    }
}

void Yield_Syscall()
{
	currentThread->Yield();
	return;
}

int CreateLock_Syscall(unsigned int vaddr,int lockNameLen)
{
	char *buf = new char[lockNameLen+1];
	int lockId;

	if(buf == NULL)
	{
		printf("Memory unavailable of the heap \n");
		return -1;
	}

	if(lockNameLen<1 || lockNameLen>MAX_LOCK_NAME)
	{
		printf("LockName is invalid \n");
		delete [] buf;
		return -1;
	}

	if(vaddr<0 || (vaddr+lockNameLen)>=(currentThread->space)->addrSpaceSize)
	{
		printf("Invalid Address passed \n");
		return -1;
	}

	if(copyin(vaddr,lockNameLen,buf) == -1)
	{
		delete [] buf;
		return -1;
	}
	buf[lockNameLen] = '\0';

	userLockTableLock->Acquire();
	if((lockId = userLockTableLock.lockBitMap->Find()) == -1)
	{
		printf("No space for new lock \n");
		delete[] buf;
		userLockTableLock->Release();
		return -1;
	}

	userLockTable.locks[lockId].lock = new Lock(buf);

	userLockTable.locks[lockId].space = currentThread->space;
	userLockTable.locks[lockId].isDeleted = false;
	userLockTable.locks[lockId].isToBeDeleted = false;
	userLockTable.locks[lockId].lockCounter = 0;

	userLockTableLock->Release();
	return lockId;
}

void AcquireLock_Syscall(int lockId)
{
	userLockTableLock->Acquire();

	if(lockId<0 || lockId>MAX_LOCKS)
	{
		printf("Trying to acquire lock on invalid lockId \n");
		userLockTableLock->Release();
		return;
	}

	if(userLockTableLock.locks[lockId].lock == NULL || userLockTableLock.locks[lockId].isDeleted)
	{
		printf("Tring to acquire lock on invalid lockId. LockId does not exist \n");
		userLockTableLock->Release();
		return;
	}

	if(userLockTableLock.locks[lockId].addrSpace != currentThread->space)
	{
		printf("Address space mismatch.. Trying to Acquire Lock out of address space \n");
		userLockTableLock->Release();
		return;
	}

	if(!(userLockTableLock.locks[lockId].lock->isHeldByCurrentThread()))
	{
		userLockTableLock.locks[lockId].lockCounter++;
	}
	userLockTableLock->Release();

	userLockTableLock.locks[lockId].lock->Acquire();

	return;
}

void ReleaseLock_Syscall(int lockId)
{
	userLockTableLock->Acquire();

	if(lockId<0 || lockId>MAX_LOCKS)
	{
		printf("Trying to release lock on invalid lockId \n");
		userLockTableLock->Release();
		return;
	}

	if(userLockTableLock.locks[lockId].lock == NULL || userLockTableLock.locks[lockId].isDeleted)
	{
		printf("Tring to release lock on invalid lockId. LockId does not exist \n");
		userLockTableLock->Release();
		return;
	}

	if(userLockTableLock.locks[lockId].addrSpace != currentThread->space)
	{
		printf("Address space mismatch.. Trying to release Lock out of address space \n");
		userLockTableLock->Release();
		return;
	}

	if((userLockTableLock.locks[lockId].lock->isHeldByCurrentThread()))
	{
		userLockTableLock.locks[lockId].lockCounter--;
	}

	userLockTableLock.locks[lockId].lock->Release();

	if((userLockTableLock.locks[lockId].isToBeDeleted) && (userLockTableLock.locks[lockId].lockCounter == 0))
	{
		userLockTableLock.lockBitMap->Clear(lockId);
		userLockTableLock.locks[lockId].isDeleted = true;
		userLockTableLock.locks[lockId].isToBeDeleted = false;
		delete userLockTableLock.locks[lockId].lock;
		userLockTableLock.locks[lockId].lock = NULL;
		userLockTableLock.locks[lockId].addrSpace = NULL;
		userLockTableLock.locks[lockId].lockCounter = 0;
	}

	userLockTableLock->Release();

	return;
}

void DestroyLock_Syscall(int lockId)
{
	userLockTableLock->Acquire();

	if(lockId<0 || lockId>MAX_LOCKS)
	{
		printf("Trying to destroy lock on invalid lockId \n");
		userLockTableLock->Release();
		return;
	}

	if(userLockTableLock.locks[lockId].lock == NULL || userLockTableLock.locks[lockId].isDeleted)
	{
		printf("Tring to destroy lock on invalid lockId. LockId does not exist \n");
		userLockTableLock->Release();
		return;
	}

	if(userLockTableLock.locks[lockId].addrSpace != currentThread->space)
	{
		printf("Address space mismatch.. Trying to destroy Lock out of address space \n");
		userLockTableLock->Release();
		return;
	}

	userLockTableLock.locks[lockId].isToBeDeleted = true;
	if(userLockTableLock.locks[lockId].lockCounter > 0)
	{
		printf("Cannot Delete Lock. Its already in USE \n");
		userLockTableLock->Release();
		return;
	}

	userLockTableLock.lockBitMap->Clear(lockId);
	userLockTableLock.locks[lockId].isDeleted = true;
	userLockTableLock.locks[lockId].isToBeDeleted = false;
	delete userLockTableLock.locks[lockId].lock;
	userLockTableLock.locks[lockId].lock = NULL;
	userLockTableLock.locks[lockId].addrSpace = NULL;
	userLockTableLock.locks[lockId].lockCounter = 0;

	userLockTableLock->Release();

	return;
}

void Print_Syscall(unsigned int vaddr)
{
	char printBuf[MAX_CHAR_PRINTF];
	if(copyin(vaddr,MAX_CHAR_PRINTF - 1,printBuf) == -1)
	{
		printf("%s: Bad Virtual Address \n",currentThread->getName());
		return ;
	}
	printf(printBuf);
}

int Scan_Syscall()
{
	int scanVal;
	printf("Please enter an integer value to be scanned \n");
	scanf("%d",&scanVal);
	return scanVal;
}

void ExceptionHandler(ExceptionType which) {
    int type = machine->ReadRegister(2); // Which syscall?
    int rv=0; 	// the return value from a syscall

    if ( which == SyscallException ) {
	switch (type) {
	    default:
		DEBUG('a', "Unknown syscall - shutting down.\n");
	    case SC_Halt:
		DEBUG('a', "Shutdown, initiated by user program.\n");
		interrupt->Halt();
		break;
	    case SC_Create:
		DEBUG('a', "Create syscall.\n");
		Create_Syscall(machine->ReadRegister(4), machine->ReadRegister(5));
		break;
	    case SC_Open:
		DEBUG('a', "Open syscall.\n");
		rv = Open_Syscall(machine->ReadRegister(4), machine->ReadRegister(5));
		break;
	    case SC_Write:
		DEBUG('a', "Write syscall.\n");
		Write_Syscall(machine->ReadRegister(4),
			      machine->ReadRegister(5),
			      machine->ReadRegister(6));
		break;
	    case SC_Read:
		DEBUG('a', "Read syscall.\n");
		rv = Read_Syscall(machine->ReadRegister(4),
			      machine->ReadRegister(5),
			      machine->ReadRegister(6));
		break;
	    case SC_Close:
		DEBUG('a', "Close syscall.\n");
		Close_Syscall(machine->ReadRegister(4));
		break;

	    case SC_Yield:
		DEBUG('a',"Yield syscall. \n");
		Yield_Syscall();
		break;

	    case SC_CreateLock:
	    {
	    	DEBUG('a',"Create Lock syscall \n");
	    	rv = CreateLock_Syscall(machine->ReadRegister(4),machine->ReadRegister(5));
	    }
	    break;

	    case SC_AcquireLock:
	    {
	    	DEBUG('a',"Acquire Lock syscall \n");
	    	AcquireLock_Syscall(machine->ReadRegister(4));
	    }
	    break;

	    case SC_ReleaseLock:
	    {
	    	DEBUG('a',"Release Lock syscall \n");
	    	ReleaseLock_Syscall(machine->ReadRegister(4));
	    }
	    break;

	    case SC_DestroyLock:
	    {
	    	DEBUG('a',"Destroy Lock syscall \n");
	    	DestroyLock_Syscall(machine->ReadRegister(4));
	    }
	    break;

	    case SC_Print:
	    {
	    	DEBUG('a',"Print syscall \n");
	    	Print_Syscall(machine->ReadRegister(4));
	    }
	    break;

	    case SC_Scan:
	    {
	    	DEBUG('a',"Scan syscall \n");
	    	rv = Scan_Syscall();
	    }
	    break;
	}

	// Put in the return value and increment the PC
	machine->WriteRegister(2,rv);
	machine->WriteRegister(PrevPCReg,machine->ReadRegister(PCReg));
	machine->WriteRegister(PCReg,machine->ReadRegister(NextPCReg));
	machine->WriteRegister(NextPCReg,machine->ReadRegister(PCReg)+4);
	return;
    } else {
      cout<<"Unexpected user mode exception - which:"<<which<<"  type:"<< type<<endl;
      interrupt->Halt();
    }
}
