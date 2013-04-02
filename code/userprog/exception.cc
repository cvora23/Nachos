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

void Sprintf_Syscall(unsigned int textToCreate,unsigned int textPassed,unsigned int lenTextPassed,int intPassed)
{
	/**
	 * Creating a known template buffer
	 */
	char templateBuf[] = "%s_%d";

	/**
	 * Validating textToCreate
	 */
	char sprintBuf[MAX_CHAR_SPRINTF];
	if(copyin(textToCreate,MAX_CHAR_SPRINTF-1,sprintBuf) == -1)
	{
		printf("%s: Bad Virtual Address \n",currentThread->getName());
		return ;
	}
	sprintBuf[MAX_CHAR_SPRINTF] = '\0';

	/**
	 * Validating textPassed
	 */
	char *tempTextPassedBuf = new char[lenTextPassed];
	if(tempTextPassedBuf == NULL)
	{
		printf("Sprintf_Syscall  ERROR: Unable to allocate memory of the heap for tempTextPassed buffer\n");
		return;
	}
	if(copyin(textPassed,lenTextPassed-1,tempTextPassedBuf) == -1)
	{
		printf("%s: Bad Virtual Address \n",currentThread->getName());
		return ;
	}
	tempTextPassedBuf[lenTextPassed] = '\0';

	/**
	 * Calling actual sprintf function
	 */
	sprintf(sprintBuf,"%s_%d",tempTextPassedBuf,intPassed);

	/**
	 * Finally copyout the sprintBuf to textToCreate
	 */
	copyout(textToCreate,MAX_CHAR_SPRINTF,sprintBuf);
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
	int lockId;

	if(lockNameLen<1 || lockNameLen>MAX_LOCK_NAME)
	{
		printf("LockName is invalid \n");
		return -1;
	}

	char *buf = new char[lockNameLen+1];

	if(buf == NULL)
	{
		printf("Memory unavailable of the heap \n");
		return -1;
	}

	if(vaddr<0 || (vaddr+lockNameLen)>=(currentThread->space)->addrSpaceSize)
	{
		delete [] buf;
		printf("Invalid Address passed \n");
		return -1;
	}

	if(copyin(vaddr,lockNameLen,buf) == -1)
	{
		printf("Invalid Address passed \n");
		delete [] buf;
		return -1;
	}
	buf[lockNameLen] = '\0';

	userLockTableLock->Acquire();
	if((lockId = userLockTable.lockBitMap->Find()) == -1)
	{
		printf("No space for new lock \n");
		delete[] buf;
		userLockTableLock->Release();
		return -1;
	}

	userLockTable.locks[lockId].lock = new Lock(buf);
	userLockTable.locks[lockId].addrSpace = currentThread->space;
	userLockTable.locks[lockId].isDeleted = false;
	userLockTable.locks[lockId].isToBeDeleted = false;
	userLockTable.locks[lockId].lockCounter = 0;

	userLockTableLock->Release();
	delete buf;
	return lockId;
}

void AcquireLock_Syscall(int lockId)
{
	userLockTableLock->Acquire();

	if(lockId<0 || lockId>MAX_LOCKS)
	{
		printf("Trying to acquire lock on invalid lockId = %d \n",lockId);
		userLockTableLock->Release();
		return;
	}

	if(userLockTable.locks[lockId].lock == NULL || userLockTable.locks[lockId].isDeleted)
	{
		printf("Tring to acquire lock on invalid lockId = %d. LockId does not exist \n",lockId);
		userLockTableLock->Release();
		return;
	}

	if(userLockTable.locks[lockId].addrSpace != currentThread->space)
	{
		printf("Address space mismatch.. Trying to Acquire Lock out of address space \n");
		userLockTableLock->Release();
		return;
	}

	if(!(userLockTable.locks[lockId].lock->isHeldByCurrentThread()))
	{
		userLockTable.locks[lockId].lockCounter++;
	}
	userLockTableLock->Release();

	userLockTable.locks[lockId].lock->Acquire();

	return;
}

void ReleaseLock_Syscall(int lockId)
{
	userLockTableLock->Acquire();

	if(lockId<0 || lockId>MAX_LOCKS)
	{
		printf("Trying to release lock on invalid lockId = %d \n",lockId);
		userLockTableLock->Release();
		return;
	}

	if(userLockTable.locks[lockId].lock == NULL || userLockTable.locks[lockId].isDeleted)
	{
		printf("Tring to release lock on invalid lockId = %d . LockId does not exist \n",lockId);
		userLockTableLock->Release();
		return;
	}

	if(userLockTable.locks[lockId].addrSpace != currentThread->space)
	{
		printf("Address space mismatch.. Trying to release Lock out of address space \n");
		userLockTableLock->Release();
		return;
	}

	if((userLockTable.locks[lockId].lock->isHeldByCurrentThread()))
	{
		userLockTable.locks[lockId].lockCounter--;
	}

	userLockTable.locks[lockId].lock->Release();

	if((userLockTable.locks[lockId].isToBeDeleted) && (userLockTable.locks[lockId].lockCounter == 0))
	{
		userLockTable.lockBitMap->Clear(lockId);
		userLockTable.locks[lockId].isDeleted = true;
		userLockTable.locks[lockId].isToBeDeleted = false;
		delete userLockTable.locks[lockId].lock;
		userLockTable.locks[lockId].lock = NULL;
		userLockTable.locks[lockId].addrSpace = NULL;
	}

	userLockTableLock->Release();

	return;
}

void DestroyLock_Syscall(int lockId)
{
	userLockTableLock->Acquire();

	if(lockId<0 || lockId>MAX_LOCKS)
	{
		printf("Trying to destroy lock on invalid lockId = %d \n",lockId);
		userLockTableLock->Release();
		return;
	}

	if(userLockTable.locks[lockId].lock == NULL || userLockTable.locks[lockId].isDeleted)
	{
		printf("Tring to destroy lock on invalid lockId = %d . LockId does not exist \n",lockId);
		userLockTableLock->Release();
		return;
	}

	if(userLockTable.locks[lockId].addrSpace != currentThread->space)
	{
		printf("Address space mismatch.. Trying to destroy Lock out of address space \n");
		userLockTableLock->Release();
		return;
	}

	userLockTable.locks[lockId].isToBeDeleted = true;
	if(userLockTable.locks[lockId].lockCounter > 0)
	{
		printf("Cannot Delete Lock. Its already in USE \n");
		userLockTableLock->Release();
		return;
	}

	userLockTable.lockBitMap->Clear(lockId);
	userLockTable.locks[lockId].isDeleted = true;
	userLockTable.locks[lockId].isToBeDeleted = false;
	delete userLockTable.locks[lockId].lock;
	userLockTable.locks[lockId].lock = NULL;
	userLockTable.locks[lockId].addrSpace = NULL;
	userLockTable.locks[lockId].lockCounter = 0;

	userLockTableLock->Release();

	return;
}

int CreateCondition_Syscall(unsigned int vaddr,int conditionNameLen)
{
	int conditionId;

	if(conditionNameLen<1 || conditionNameLen>MAX_CV_NAME)
	{
		printf("ConditionName is invalid \n");
		return -1;
	}

	char *buf = new char[conditionNameLen+1];

	if(buf == NULL)
	{
		printf("Memory unavailable of the heap \n");
		return -1;
	}

	if(vaddr<0 || (vaddr+conditionNameLen)>=(currentThread->space)->addrSpaceSize)
	{
		delete [] buf;
		printf("Invalid Address passed \n");
		return -1;
	}

	if(copyin(vaddr,conditionNameLen,buf) == -1)
	{
		printf("Invalid Address passed \n");
		delete [] buf;
		return -1;
	}
	buf[conditionNameLen] = '\0';

	userConditionTableLock->Acquire();
	if((conditionId = userConditionTable.conditionBitMap->Find()) == -1)
	{
		printf("No space for new condition \n");
		delete[] buf;
		userConditionTableLock->Release();
		return -1;
	}

	userConditionTable.conditions[conditionId].condition = new Condition(buf);
	userConditionTable.conditions[conditionId].addrSpace = currentThread->space;
	userConditionTable.conditions[conditionId].isDeleted = false;
	userConditionTable.conditions[conditionId].isToBeDeleted = false;
	userConditionTable.conditions[conditionId].conditionCounter = 0;

	userConditionTableLock->Release();
	delete buf;
	return conditionId;
}

void Wait_Syscall(int conditionId,int lockId)
{
	userLockTableLock->Acquire();
	userConditionTableLock->Acquire();

	if(lockId<0 || lockId>MAX_LOCKS || conditionId<0 || conditionId>MAX_CVS)
	{
		printf("Error in Wait System call: lockId = %d or ConditionId = %d out of bounds \n",
				lockId,conditionId);
		userLockTableLock->Release();
		userConditionTableLock->Release();
		return;
	}

	if(userLockTable.locks[lockId].lock == NULL ||
			userLockTable.locks[lockId].isDeleted ||
			userConditionTable.conditions[conditionId].condition == NULL ||
			userConditionTable.conditions[conditionId].isDeleted)
	{
		printf("Error in Wait System call: lock or CV is not valid \n");
		userLockTableLock->Release();
		userConditionTableLock->Release();
		return;
	}

	if(userLockTable.locks[lockId].addrSpace != currentThread->space ||
			userConditionTable.conditions[conditionId].addrSpace != currentThread->space)
	{
		printf("Error in Wait System call: Lock or CV does not belong to current thread address space\n");
		userLockTableLock->Release();
		userConditionTableLock->Release();
		return;
	}

	userConditionTable.conditions[conditionId].conditionCounter++;
	userLockTableLock->Release();
	userConditionTableLock->Release();

	(userConditionTable.conditions[conditionId].condition)->Wait(userLockTable.locks[lockId].lock);
	return;
}

void Signal_Syscall(int conditionId,int lockId)
{
	userLockTableLock->Acquire();
	userConditionTableLock->Acquire();

	if(lockId<0 || lockId>MAX_LOCKS || conditionId<0 || conditionId>MAX_CVS)
	{
		printf("Error in Signal System call: lockId = %d or ConditionId = %d out of bounds \n",
				lockId,conditionId);
		userLockTableLock->Release();
		userConditionTableLock->Release();
		return;
	}

	if(userLockTable.locks[lockId].lock == NULL ||
			userLockTable.locks[lockId].isDeleted ||
			userConditionTable.conditions[conditionId].condition == NULL ||
			userConditionTable.conditions[conditionId].isDeleted)
	{
		printf("Error in Signal System call: lock or CV is not valid \n");
		userLockTableLock->Release();
		userConditionTableLock->Release();
		return;
	}

	if(userLockTable.locks[lockId].addrSpace != currentThread->space ||
			userConditionTable.conditions[conditionId].addrSpace != currentThread->space)
	{
		printf("Error in Signal System call: Lock or CV does not belong to current thread address space\n");
		userLockTableLock->Release();
		userConditionTableLock->Release();
		return;
	}

	(userConditionTable.conditions[conditionId].condition)->Signal(userLockTable.locks[lockId].lock);

	if(userConditionTable.conditions[conditionId].conditionCounter>0)
	{
		userConditionTable.conditions[conditionId].conditionCounter--;
	}

	if(userConditionTable.conditions[conditionId].isToBeDeleted &&
			userConditionTable.conditions[conditionId].conditionCounter == 0)
	{
		userConditionTable.conditionBitMap->Clear(conditionId);
		delete userConditionTable.conditions[conditionId].condition;
		userConditionTable.conditions[conditionId].condition = NULL;
		userConditionTable.conditions[conditionId].isDeleted = true;
		userConditionTable.conditions[conditionId].isToBeDeleted = false;
	}
	userLockTableLock->Release();
	userConditionTableLock->Release();
	return;
}

void Broadcast_Syscall(int conditionId,int lockId)
{
	userLockTableLock->Acquire();
	userConditionTableLock->Acquire();

	if(lockId<0 || lockId>MAX_LOCKS || conditionId<0 || conditionId>MAX_CVS)
	{
		printf("Error in Broadcast System call: lockId or ConditionId out of bounds \n");
		userLockTableLock->Release();
		userConditionTableLock->Release();
		return;
	}

	if(userLockTable.locks[lockId].lock == NULL ||
			userLockTable.locks[lockId].isDeleted ||
			userConditionTable.conditions[conditionId].condition == NULL ||
			userConditionTable.conditions[conditionId].isDeleted)
	{
		printf("Error in Broadcast System call: lock or CV is not valid \n");
		userLockTableLock->Release();
		userConditionTableLock->Release();
		return;
	}

	if(userLockTable.locks[lockId].addrSpace != currentThread->space ||
			userConditionTable.conditions[conditionId].addrSpace != currentThread->space)
	{
		printf("Error in Broadcast System call: Lock or CV does not belong to current thread address space\n");
		userLockTableLock->Release();
		userConditionTableLock->Release();
		return;
	}

	if(userConditionTable.conditions[conditionId].conditionCounter>0)
	{
		userConditionTable.conditions[conditionId].conditionCounter = 0;
	}

	(userConditionTable.conditions[conditionId].condition)->Broadcast(userLockTable.locks[lockId].lock);


	if(userConditionTable.conditions[conditionId].isToBeDeleted &&
			userConditionTable.conditions[conditionId].conditionCounter == 0)
	{
		userConditionTable.conditionBitMap->Clear(conditionId);
		delete userConditionTable.conditions[conditionId].condition;
		userConditionTable.conditions[conditionId].condition = NULL;
		userConditionTable.conditions[conditionId].isDeleted = true;
		userConditionTable.conditions[conditionId].isToBeDeleted = false;
	}
	userLockTableLock->Release();
	userConditionTableLock->Release();
	return;
}

void DestroyCondition_Syscall(int conditionId)
{
	userConditionTableLock->Acquire();
	if(conditionId<0 || conditionId>MAX_CVS)
	{
		printf("Trying to destroy condition on invalid conditionId \n");
		userConditionTableLock->Release();
		return;
	}

	if(userConditionTable.conditions[conditionId].condition == NULL ||
			userConditionTable.conditions[conditionId].isDeleted)
	{
		printf("Tring to destroy condition on invalid conditionId. conditionId does not exist \n");
		userConditionTableLock->Release();
		return;
	}

	if(userConditionTable.conditions[conditionId].addrSpace != currentThread->space)
	{
		printf("Address space mismatch.. Trying to destroy Condition out of address space \n");
		userConditionTableLock->Release();
		return;
	}

	userConditionTable.conditions[conditionId].isToBeDeleted = true;
	if(userConditionTable.conditions[conditionId].conditionCounter > 0)
	{
		printf("Cannot Delete Condition. Its already in USE \n");
		userConditionTableLock->Release();
		return;
	}

	userConditionTable.conditionBitMap->Clear(conditionId);
	userConditionTable.conditions[conditionId].isDeleted = true;
	userConditionTable.conditions[conditionId].isToBeDeleted = false;
	delete userConditionTable.conditions[conditionId].condition;
	userConditionTable.conditions[conditionId].condition = NULL;
	userConditionTable.conditions[conditionId].addrSpace = NULL;
	userConditionTable.conditions[conditionId].conditionCounter = 0;

	userConditionTableLock->Release();

	return;

}

void Print_Syscall(unsigned int vaddr)
{
	char *printBuf = new char[MAX_CHAR_PRINTF];
	if(copyin(vaddr,MAX_CHAR_PRINTF - 1,printBuf) == -1)
	{
		printf("%s: Bad Virtual Address \n",currentThread->getName());
		return ;
	}
	printf(printBuf);
	delete printBuf;
}

void PrintString_Syscall(unsigned int vaddr,unsigned int string)
{
	char *printBuf = new char[MAX_CHAR_PRINTF];
	if(copyin(vaddr,MAX_CHAR_PRINTF - 1,printBuf) == -1)
	{
		printf("%s: Bad Virtual Address \n",currentThread->getName());
		return ;
	}

	char *tempStringBuf = new char[MAX_CHAR_PRINTF];
	if(copyin(string,MAX_CHAR_PRINTF - 1,tempStringBuf) == -1)
	{
		printf("%s: Bad Virtual Address \n",currentThread->getName());
		return ;
	}

	printf(printBuf,tempStringBuf);
	delete tempStringBuf;
	delete printBuf;
}

void PrintStringInt_Syscall(unsigned int vaddr,unsigned int string,int var)
{
	char *printBuf = new char[MAX_CHAR_PRINTF];
	if(copyin(vaddr,MAX_CHAR_PRINTF - 1,printBuf) == -1)
	{
		printf("%s: Bad Virtual Address \n",currentThread->getName());
		return ;
	}

	char *tempStringBuf = new char[MAX_CHAR_PRINTF];
	if(copyin(string,MAX_CHAR_PRINTF - 1,tempStringBuf) == -1)
	{
		printf("%s: Bad Virtual Address \n",currentThread->getName());
		return ;
	}

	printf(printBuf,tempStringBuf,var);
	delete tempStringBuf;
	delete printBuf;
}

void PrintStringIntInt_Syscall(unsigned int vaddr,unsigned int string,int var1,int var2)
{
	char *printBuf = new char[MAX_CHAR_PRINTF];
	if(copyin(vaddr,MAX_CHAR_PRINTF - 1,printBuf) == -1)
	{
		printf("%s: Bad Virtual Address \n",currentThread->getName());
		return ;
	}

	char *tempStringBuf = new char[MAX_CHAR_PRINTF];
	if(copyin(string,MAX_CHAR_PRINTF - 1,tempStringBuf) == -1)
	{
		printf("%s: Bad Virtual Address \n",currentThread->getName());
		return ;
	}

	printf(printBuf,tempStringBuf,var1,var2);
	delete tempStringBuf;
	delete printBuf;
}

void Print1_Syscall(unsigned int vaddr,int arg1)
{
	char *printBuf = new char[MAX_CHAR_PRINTF];
	if(copyin(vaddr, MAX_CHAR_PRINTF - 1 , printBuf)==-1){
		printf("%s: Bad Virtual address\n",currentThread->getName());
		return;
	}
	printf(printBuf, arg1);
	delete printBuf;
}

void Print2_Syscall(unsigned int vaddr,int arg1,int arg2)
{
	char *printBuf = new char[MAX_CHAR_PRINTF];
	if(copyin(vaddr, MAX_CHAR_PRINTF - 1 , printBuf)==-1){
		printf("%s: Bad Virtual address\n",currentThread->getName());
		return;
	}
	printf(printBuf, arg1, arg2);
	delete printBuf;
}

void Print3_Syscall(unsigned int vaddr,int arg1,int arg2,int arg3)
{
	char *printBuf = new char[MAX_CHAR_PRINTF];
	if(copyin(vaddr, MAX_CHAR_PRINTF - 1 , printBuf)==-1){
		printf("%s: Bad Virtual address\n",currentThread->getName());
		return;
	}
	printf(printBuf, arg1, arg2,arg3);
	delete printBuf;
}

int Scan_Syscall()
{
	int scanVal;
	printf("Please enter an integer value to be scanned \n");
	scanf("%d",&scanVal);
	return scanVal;
}

void newKernelThread(unsigned int vaddr)
{
	machine->WriteRegister(PCReg,vaddr);

	machine->WriteRegister(NextPCReg,vaddr+4);

    machine->WriteRegister(StackReg, (currentThread->stackRegVirtualPage) * PageSize - 16);

    (currentThread->space)->RestoreState();

    machine->Run();

    ASSERT(false);
}

void Fork_Syscall(unsigned int vaddr)
{
	int numPages;
	TranslationEntry* tempPageTableForDel;

	if(vaddr > currentThread->space->addrSpaceSize)
	{
		DEBUG('a',"Fork ERROR: Invalid virtual address passed \n");
		return;
	}

	processTableAccessLock->Acquire();

	(currentThread->space)->pageTableLock->Acquire();

	numPages = (currentThread->space)->numPages;

	TranslationEntry* tempPageTable = new TranslationEntry[numPages + 8];

    for (int i = 0; i < numPages; i++)
    {
    	tempPageTable[i].virtualPage = (currentThread->space)->pageTable[i].virtualPage;
    	tempPageTable[i].physicalPage = (currentThread->space)->pageTable[i].physicalPage;
		tempPageTable[i].valid = (currentThread->space)->pageTable[i].valid ;
		tempPageTable[i].use = (currentThread->space)->pageTable[i].use ;
		tempPageTable[i].dirty = (currentThread->space)->pageTable[i].dirty ;
		tempPageTable[i].readOnly = (currentThread->space)->pageTable[i].readOnly ;
						// if the code segment was entirely on
						// a separate page, we could set its
						// pages to be read-only
    }

	mainMemoryAccessLock->Acquire();

    for(int i = numPages;i<numPages+8;i++)
    {


    	tempPageTable[i].virtualPage = i;	// for now, virtual page # = phys page #
    	tempPageTable[i].physicalPage = mainMemoryBitMap->Find();
    	if(tempPageTable[i].physicalPage == -1)
    	{
			printf("%s : No Memory available for allocation \n",currentThread->getName());
	    	mainMemoryAccessLock->Release();
			interrupt->Halt();
    	}
    	tempPageTable[i].valid = TRUE;
    	tempPageTable[i].use = FALSE;
    	tempPageTable[i].dirty = FALSE;
    	tempPageTable[i].readOnly = FALSE;  // if the code segment was entirely on
						// a separate page, we could set its
						// pages to be read-only
    }

	mainMemoryAccessLock->Release();

    (currentThread->space)->numPages = numPages + 8;
    (currentThread->space)->addrSpaceSize = (currentThread->space)->numPages * PageSize;

    tempPageTableForDel = (currentThread->space)->pageTable;
    (currentThread->space)->pageTable = tempPageTable;

    delete [] tempPageTableForDel;

    (currentThread->space)->RestoreState();

    Thread* newForkedThread = new Thread("newForkedThread");
    newForkedThread->space = currentThread->space;
    newForkedThread->stackRegVirtualPage = (newForkedThread->space)->numPages;

    (currentThread->space)->pageTableLock->Release();


    newForkedThread->threadId = processTableArray[(currentThread->space)->processId].threadCounter;

    DEBUG('a',"Process %d 's Thread %d forked a New Thread %d \n",(currentThread->space)->processId,
    		currentThread->threadId,newForkedThread->threadId);

    processTableArray[(currentThread->space)->processId].activeThreadCounter++;
    processTableArray[(currentThread->space)->processId].totalThreads++;
    processTableArray[(currentThread->space)->processId].threadCounter++;

	processTableAccessLock->Release();

	newForkedThread->Fork((VoidFunctionPtr)newKernelThread,vaddr);

	return;
}

void newExecProcess(unsigned int vaddr)
{
	currentThread->space->InitRegisters();
	currentThread->space->RestoreState();

	machine->Run();

	ASSERT(false);
}

SpaceId Exec_Syscall(unsigned int vaddr,int len)
{
	char* execBuf;

	if(len <1 || len>MAX_PROCESS_NAME_LEN)
	{
		printf("Exe_Syscall : Illegal Executable file name \n");
	}
	execBuf = new char[len+1];

	if(execBuf == NULL)
	{
		printf("Exec_Syscall : Temp Buffer cant be created \n");
		return -1;
	}

	if(copyin(vaddr,len,execBuf) == -1)
	{
		printf("Exec_Syscall : Bad Virtual Address \n");
		delete [] execBuf;
		return -1;
	}
	execBuf[len] = '\0';

	OpenFile* executable = fileSystem->Open(execBuf);

	AddrSpace* space;

	if(executable == NULL)
	{
		printf("Exec_Syscall : Unable to open executable file passed \n");
		delete [] execBuf;
		return -1;
	}

	Thread* firstNewThreadForExecProcess = new Thread("firstNewThreadForExecProcess");
	firstNewThreadForExecProcess->space = new AddrSpace(executable);
	firstNewThreadForExecProcess->threadId = 0;
	firstNewThreadForExecProcess->stackRegVirtualPage = (firstNewThreadForExecProcess->space)->numPages;

	//delete executable;
	//delete [] execBuf;

	DEBUG('a',"Process %d Thread %d Exec_Syscall: New Process created with Process Id \n",
			currentThread->space->processId,currentThread->threadId,
			firstNewThreadForExecProcess->space->processId);

	firstNewThreadForExecProcess->Fork((VoidFunctionPtr)newExecProcess,vaddr);

	return firstNewThreadForExecProcess->space->processId;
}

void Exit_Syscall(int status)
{
	int physPageToClear;

	if(status != 0)
	{
		printf("User Program Result Returned %d \n",status);
	}

	processTableAccessLock->Acquire();

	if((processTableBitMap->NumClear() == (MAX_PROCESS -1)) &&
			((processTableArray[(currentThread->space)->processId].totalThreads) == 1) &&
			((processTableArray[(currentThread->space)->processId].activeThreadCounter) == 1))
	{
		printf("Exit_Syscall : Process %d Thread %d : Last thread of last process exiting.... BYE BYE NACHOS !!!!\n",
				currentThread->space->processId,currentThread->threadId);
		interrupt->Halt();
	}

	else if(((processTableArray[(currentThread->space)->processId].totalThreads) == 1) &&
			((processTableArray[(currentThread->space)->processId].activeThreadCounter) == 1))
	{
		int numPages = (currentThread->space)->numPages;
		AddrSpace *oldAddrSpace = currentThread->space;

		(currentThread->space)->pageTableLock->Acquire();
		mainMemoryAccessLock->Acquire();

		for(int i=0;i<numPages;i++)
		{
			physPageToClear = (currentThread->space)->pageTable[i].physicalPage;
			if(physPageToClear != -1)
			{
				mainMemoryBitMap->Clear(physPageToClear);
			}
		}

		mainMemoryAccessLock->Release();

		processTableBitMap->Clear((currentThread->space)->processId);

		(currentThread->space)->pageTableLock->Release();

		delete currentThread->space;

		processTableAccessLock->Release();

		userLockTableLock->Acquire();

		for(int lockId = 0;lockId<MAX_LOCKS;lockId++)
		{
			if((userLockTable.lockBitMap->Test(lockId)) &&
					(userLockTable.locks[lockId].addrSpace == oldAddrSpace))
			{
				DestroyLock_Syscall(lockId);
			}
		}

		userLockTableLock->Release();

		userConditionTableLock->Acquire();

		for(int cvId = 0;cvId<MAX_CVS;cvId++)
		{
			if((userConditionTable.conditionBitMap->Test(cvId)) &&
					(userConditionTable.conditions[cvId].addrSpace == oldAddrSpace))
			{
				DestroyCondition_Syscall(cvId);
			}
		}

		userConditionTableLock->Release();

		currentThread->Finish();
	}

	else
	{
		(currentThread->space)->pageTableLock->Acquire();
		processTableArray[(currentThread->space)->processId].activeThreadCounter--;
		processTableArray[(currentThread->space)->processId].totalThreads--;

		mainMemoryAccessLock->Acquire();

		for(int i = (currentThread->stackRegVirtualPage)-8;i<(currentThread->stackRegVirtualPage);i++)
		{
			physPageToClear = (currentThread->space)->pageTable[i].physicalPage;
			if(physPageToClear != -1)
			{
				mainMemoryBitMap->Clear(physPageToClear);
			}
		}

		mainMemoryAccessLock->Release();

		(currentThread->space)->pageTableLock->Release();

		processTableAccessLock->Release();

		currentThread->Finish();
	}

}

int GetRand_Syscall()
{
	int returnScanVal;
	returnScanVal=(int)rand();
	return returnScanVal;
}


void ExceptionHandler(ExceptionType which) {
    int type = machine->ReadRegister(2); // Which syscall?
    int rv=0; 	// the return value from a syscall

    int arg1 = machine->ReadRegister(4);
    int arg2 = machine->ReadRegister(5);
    int arg3 = machine->ReadRegister(6);
    int arg4 = machine->ReadRegister(7);

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
		Create_Syscall(arg1,arg2);
		break;
	    case SC_Open:
		DEBUG('a', "Open syscall.\n");
		rv = Open_Syscall(arg1,arg2);
		break;
	    case SC_Write:
		DEBUG('a', "Write syscall.\n");
		Write_Syscall(arg1,arg2,arg3);
		break;
	    case SC_Read:
		DEBUG('a', "Read syscall.\n");
		rv = Read_Syscall(arg1,arg2,arg3);
		break;
	    case SC_Close:
		DEBUG('a', "Close syscall.\n");
		Close_Syscall(arg1);
		break;

	    case SC_Yield:
		DEBUG('a',"Yield syscall. \n");
		Yield_Syscall();
		break;

	    case SC_CreateLock:
	    {
	    	DEBUG('a',"Create Lock syscall \n");
	    	rv = CreateLock_Syscall(arg1,arg2);
	    }
	    break;

	    case SC_AcquireLock:
	    {
	    	DEBUG('a',"Acquire Lock syscall \n");
	    	AcquireLock_Syscall(arg1);
	    }
	    break;

	    case SC_ReleaseLock:
	    {
	    	DEBUG('a',"Release Lock syscall \n");
	    	ReleaseLock_Syscall(arg1);
	    }
	    break;

	    case SC_DestroyLock:
	    {
	    	DEBUG('a',"Destroy Lock syscall \n");
	    	DestroyLock_Syscall(arg1);
	    }
	    break;

	    case SC_CreateCondition:
	    {
	    	DEBUG('a',"Create Condition syscall \n");
	    	rv = CreateCondition_Syscall(arg1,arg2);
	    }
	    break;

	    case SC_DestroyCondition:
	    {
	    	DEBUG('a',"Destroy Condition syscall \n");
	    	DestroyCondition_Syscall(arg1);
	    }
	    break;

	    case SC_Signal:
	    {
	    	DEBUG('a',"Signal syscall \n");
	    	Signal_Syscall(arg1,arg2);
	    }
	    break;

	    case SC_Wait:
	    {
	    	DEBUG('a',"Wait syscall \n");
	    	Wait_Syscall(arg1,arg2);
	    }
	    break;

	    case SC_Broadcast:
	    {
	    	DEBUG('a',"Broadcast syscall \n");
	    	Broadcast_Syscall(arg1,arg2);
	    }
	    break;

	    case SC_Print:
	    {
	    	DEBUG('a',"Print syscall \n");
	    	Print_Syscall(arg1);
	    }
	    break;

	    case SC_PrintString:
	    {
	    	DEBUG('a',"PrintString syscall \n");
	    	PrintString_Syscall(arg1,arg2);
	    }
	    break;

	    case SC_PrintStringInt:
	    {
	    	DEBUG('a',"PrintStringInt syscall \n");
	    	PrintStringInt_Syscall(arg1,arg2,arg3);
	    }
	    break;

	    case SC_PrintStringIntInt:
	    {
	    	DEBUG('a',"PrintStringIntInt syscall \n");
	    	PrintStringIntInt_Syscall(arg1,arg2,arg3,arg4);
	    }
	    break;

	    case SC_Print1:
	    {
	    	DEBUG('a',"Print1 syscall \n");
	    	Print1_Syscall(arg1,arg2);
	    }
	    break;

	    case SC_Print2:
	    {
	    	DEBUG('a',"Print2 syscall \n");
	    	Print2_Syscall(arg1,arg2,arg3);
	    }
	    break;

	    case SC_Print3:
	    {
	    	DEBUG('a',"Print3 syscall \n");
	    	Print3_Syscall(arg1,arg2,arg3,arg4);
	    }
	    break;

	    case SC_Sprintf:
	    {
	    	DEBUG('a',"Sprintf syscall\n");
	    	Sprintf_Syscall(arg1,arg2,arg3,arg4);
	    }
	    break;
	    case SC_Scan:
	    {
	    	DEBUG('a',"Scan syscall \n");
	    	rv = Scan_Syscall();
	    }
	    break;
	    case SC_Fork:
	    {
	    	DEBUG('a',"Fork syscall \n");
	    	Fork_Syscall(arg1);
	    }
	    break;
	    case SC_Exec:
	    {
	    	DEBUG('a',"Exec syscall \n");
	    	rv = Exec_Syscall(arg1,arg2);
	    }
	    break;
	    case SC_Exit:
	    {
	    	DEBUG('a',"Exit syscall \n");
	    	Exit_Syscall(arg1);
	    }
	    break;
	    case SC_GetRand:
	    {
	    	DEBUG('a',"GetRand syscall \n");
	    	rv = GetRand_Syscall();
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
