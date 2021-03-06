// system.h 
//	All global variables used in Nachos are defined here.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef SYSTEM_H
#define SYSTEM_H

#include "copyright.h"
#include "utility.h"
#include "thread.h"
#include "scheduler.h"
#include "interrupt.h"
#include "stats.h"
#include "timer.h"
#include "synch.h"

// Initialization and cleanup routines
extern void Initialize(int argc, char **argv); 	// Initialization,
						// called before anything else
extern void Cleanup();				// Cleanup, called when
						// Nachos is done.

extern Thread *currentThread;			// the thread holding the CPU
extern Thread *threadToBeDestroyed;  		// the thread that just finished
extern Scheduler *scheduler;			// the ready list
extern Interrupt *interrupt;			// interrupt status
extern Statistics *stats;			// performance metrics
extern Timer *timer;				// the hardware alarm clock

#ifdef USER_PROGRAM



#include "machine.h"

extern Machine* machine;	// user program memory and registers

#define MAX_LOCKS	500
#define MAX_CVS		2000
#define MAX_PROCESS 10

#define MAXFILENAME 				256
#define MAX_LOCK_NAME 				256
#define MAX_CV_NAME					256
#define MAX_CHAR_PRINTF				200
#define MAX_PROCESS_NAME_LEN		256
#define SWAPFILEBITMAP_SIZE 		10000

typedef enum _PageEvictionPolicy
{
	FIFO = 0,
	RANDOM
};

typedef struct _UserLock
{
	Lock* 		lock;
	AddrSpace* 	addrSpace;
	bool		isDeleted;
	bool 		isToBeDeleted;
	int 		lockCounter;
}UserLock;

typedef struct UserLockArray
{
	UserLock locks[MAX_LOCKS];
	BitMap*	lockBitMap;

	UserLockArray()
	{
		lockBitMap = new BitMap(MAX_LOCKS);
		for(int i=0;i<MAX_LOCKS;i++)
		{
			locks[i].lock = NULL;
			locks[i].addrSpace = NULL;
			locks[i].isDeleted = false;
			locks[i].isToBeDeleted = false;
			locks[i].lockCounter = 0;
		}
	}
}UserLockTable;

extern Lock* userLockTableLock;
extern UserLockTable userLockTable;

typedef struct _UserCondition
{
	Condition*		condition;
	AddrSpace* 		addrSpace;
	bool			isToBeDeleted;
	bool			isDeleted;
	int				conditionCounter;
}UserCondition;

typedef struct UserConditionArray
{
	UserCondition	conditions[MAX_CVS];
	BitMap*			conditionBitMap;

	UserConditionArray()
	{
		conditionBitMap = new BitMap(MAX_CVS);
		for(int i=0;i<MAX_CVS;i++)
		{
			conditions[i].condition = NULL;
			conditions[i].addrSpace = NULL;
			conditions[i].isDeleted = false;
			conditions[i].isToBeDeleted = false;
			conditions[i].conditionCounter = 0;
		}
	}

}UserConditionTable;

extern Lock* userConditionTableLock;
extern UserConditionTable userConditionTable;

extern int 		totalPagesReserved;
extern Lock* 	mainMemoryAccessLock;
extern BitMap*	mainMemoryBitMap;


typedef struct _ProcessTable
{
	AddrSpace*	addrSpace;
	int			activeThreadCounter;
	int			totalThreads;
	int			threadCounter;
}ProcessTable;

extern ProcessTable processTableArray[MAX_PROCESS];
extern Lock* processTableAccessLock;
extern BitMap* processTableBitMap;


/**
 * ADDITIONS FOR PROJECT 3 ----------------START --------------------
 */
class IPTClass: public TranslationEntry
{
public:
	int processId; // To store the process id of the entry in the TLB
};

extern Lock* IPTLock;
extern IPTClass IPT[NumPhysPages];

extern int pageEvictionPolicy;
extern List* pageEvictionQueue; // Queue for removing page for FIFO

extern OpenFile* swapFile;
extern BitMap* swapFileBitMap;
extern Lock* swapFileBitMapLock;

/**
 * ADDITIONS FOR PROJECT 3 ---------------- END --------------------
 */

#endif

#ifdef FILESYS_NEEDED 		// FILESYS or FILESYS_STUB 
#include "filesys.h"
extern FileSystem  *fileSystem;
#endif

#ifdef FILESYS
#include "synchdisk.h"
extern SynchDisk   *synchDisk;
#endif

#ifdef NETWORK
#include "post.h"
extern PostOffice* postOffice;
#endif

#endif // SYSTEM_H
