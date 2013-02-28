// synch.cc 
//	Routines for synchronizing threads.  Three kinds of
//	synchronization routines are defined here: semaphores, locks 
//   	and condition variables (the implementation of the last two
//	are left to the reader).
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation.  We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts.  While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synch.h"
#include "system.h"

//TODO: DOCUMENT ERROR CONDITIONS PROPERLY BEFORE PUSHING CHANGES TO SERVER

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	Initialize a semaphore, so that it can be used for synchronization.
//
//	"debugName" is an arbitrary name, useful for debugging.
//	"initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

Semaphore::Semaphore(char* debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new List;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	De-allocate semaphore, when no longer needed.  Assume no one
//	is still waiting on the semaphore!
//----------------------------------------------------------------------

Semaphore::~Semaphore()
{
    delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
// 	Wait until semaphore value > 0, then decrement.  Checking the
//	value and decrementing must be done atomically, so we
//	need to disable interrupts before checking the value.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
//----------------------------------------------------------------------

void
Semaphore::P()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
    
    while (value == 0) { 			// semaphore not available
	queue->Append((void *)currentThread);	// so go to sleep
	currentThread->Sleep();
    } 
    value--; 					// semaphore available, 
						// consume its value
    
    (void) interrupt->SetLevel(oldLevel);	// re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::V
// 	Increment semaphore value, waking up a waiter if necessary.
//	As with P(), this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that threads
//	are disabled when it is called.
//----------------------------------------------------------------------

void
Semaphore::V()
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    thread = (Thread *)queue->Remove();
    if (thread != NULL)	   // make thread ready, consuming the V immediately
	scheduler->ReadyToRun(thread);
    value++;
    (void) interrupt->SetLevel(oldLevel);
}

// Dummy functions -- so we can compile our later assignments 
// Note -- without a correct implementation of Condition::Wait(), 
// the test case in the network assignment won't work!


//----------------------------------------------------------------------
// Lock::Lock
// 	Initialize a Lock, so that it can be used for synchronization.
//
//	"debugName" is an arbitrary name, useful for debugging. Copy that
//  into local name.
//	initialize lockWaitQueue -  Wait Queue used with current lock
//  initialize currentLockOwner - current thread owning the lock
//  initialize mState - current state of the lock
//----------------------------------------------------------------------
Lock::Lock(char* debugName)
{
	name = new char[strlen(debugName) + 1];
	strncpy(name, debugName, strlen(debugName) + 1);
	lockWaitQueue = new List;
	currentLockOwner = NULL;
	mState = FREE;
}

//----------------------------------------------------------------------
// Lock::Lock
// 	De-allocate Lock, when no longer needed. Assume no one is still
//  waiting on the Lock!
//  De-allocate all the dynamically allocated member variables of Lock
//	Class.
//----------------------------------------------------------------------
Lock::~Lock()
{
	delete name;
	delete lockWaitQueue;
}

//----------------------------------------------------------------------
// Lock::Acquire
// 	Trying to Acquire a lock for synchronization.
//
//----------------------------------------------------------------------
void Lock::Acquire()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    if(isHeldByCurrentThread())
    {
        DEBUG('t', "Current Thread is the lock Owner.... Wasting your time buddy !!!! \n");
        (void) interrupt->SetLevel(oldLevel);
        return;
    }
    else if(mState == FREE)
    {
    	mState = BUSY;
    	currentLockOwner = currentThread;
    }
    else
    {
    	lockWaitQueue->Append((void *)currentThread);
    	currentThread->Sleep();
    }

    (void) interrupt->SetLevel(oldLevel);

    DEBUG('t', "Lock Acquire DONE !!!! \n");

}

//----------------------------------------------------------------------
// Lock::Release
// 	Trying to Release a lock used for synchronization.
//
//----------------------------------------------------------------------
void Lock::Release()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    if(!isHeldByCurrentThread())
    {
        DEBUG('t', "Current Thread is not the lock Owner to Release the lock"
        		".... Wasting your time buddy !!!! \n");
        (void) interrupt->SetLevel(oldLevel);
        return;
    }
    else if(!lockWaitQueue->IsEmpty())
    {
    	Thread* firstWaitingThreadForLock = (Thread*)lockWaitQueue->Remove();
    	if(firstWaitingThreadForLock != NULL)
    	{
        	mState = BUSY;
			currentLockOwner = firstWaitingThreadForLock;
			scheduler->ReadyToRun(firstWaitingThreadForLock);
    	}
    }
    else
    {
    	mState = FREE;
    	currentLockOwner = NULL;
    }
    (void) interrupt->SetLevel(oldLevel);
}

//----------------------------------------------------------------------
// Lock::isHeldByCurrentThread
// 	Check to see if current thread owns the lock
//
//----------------------------------------------------------------------
bool Lock::isHeldByCurrentThread(void)
{
	return ((currentThread == (Thread*)currentLockOwner) ? true:false);
}

//----------------------------------------------------------------------
// Condition::Condition
// 	Initialize a Condition, so that it can be used for mutual exclusion.
//
//	"debugName" is an arbitrary name, useful for debugging. Copy that
//  into local name.
//	initialize cvWaitQueue -  Wait Queue used with current Condition
//	Variable
//  initialize cvWaitLock -   lock associated with this Condition
//	Variable
//----------------------------------------------------------------------
Condition::Condition(char* debugName)
{
	name = new char[strlen(debugName) + 1];
	strncpy(name, debugName, strlen(debugName) + 1);
	cvWaitQueue = new List;
	cvWaitLock = new Lock(debugName);
}

//----------------------------------------------------------------------
// Condition::Condition
// 	De-allocate Lock, when no longer needed. Assume no one is still
//  waiting on the Lock!
//  De-allocate all the dynamically allocated member variables of
// 	Condition Class.
//----------------------------------------------------------------------
Condition::~Condition()
{
	delete name;
	delete cvWaitQueue;
	delete cvWaitLock;
}

//----------------------------------------------------------------------
// Condition::Wait
// 	Waiting for a particular condition to occur. Till then sleep.
//----------------------------------------------------------------------
void Condition::Wait(Lock* conditionLock)
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    if(conditionLock == NULL)
    {
        DEBUG('t', "ConditionalLock passed is invalid and NULL \n");
        (void) interrupt->SetLevel(oldLevel);
        return;
    }
    else if(!(conditionLock->isHeldByCurrentThread()))
    {
        DEBUG('t', "ConditionalLock is not acquired by you to call Wait on a Condition \n");
        (void) interrupt->SetLevel(oldLevel);
        return;
    }
    else if(cvWaitLock == NULL)
    {
    	cvWaitLock = conditionLock;
    }
    else if(!isValidCvWaitLock(conditionLock))
    {
        DEBUG('t', "ConditionalLock passed is never meant to be used with given Condition Variable \n");
        (void) interrupt->SetLevel(oldLevel);
        return;
    }

    conditionLock->Release();
	cvWaitQueue->Append((void *)currentThread);
	currentThread->Sleep();
	cvWaitLock->Acquire();
    (void) interrupt->SetLevel(oldLevel);
}

//----------------------------------------------------------------------
// Condition::Signal
// 	Signaling single thread waiting this Condition to occur.
//----------------------------------------------------------------------
void Condition::Signal(Lock* conditionLock)
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    if(conditionLock == NULL)
    {
        DEBUG('t', "ConditionalLock passed is invalid and NULL \n");
        (void) interrupt->SetLevel(oldLevel);
        return;
    }
    else if(!(conditionLock->isHeldByCurrentThread()))
    {
        DEBUG('t', "ConditionalLock is not acquired by you to call Signal on a Condition \n");
        (void) interrupt->SetLevel(oldLevel);
        return;
    }

    else if(cvWaitQueue->IsEmpty())
    {
        DEBUG('t', " Wait Queue for Condition Variable is Empty... Returning  !!!!! \n");
        (void) interrupt->SetLevel(oldLevel);
        return;
    }
    else if(!isValidCvWaitLock(conditionLock))
    {
        DEBUG('t', "ConditionalLock passed is never meant to be used with given Condition Variable \n");
        (void) interrupt->SetLevel(oldLevel);
        return;
    }

    Thread* firstWaitingThreadForCV = (Thread*)cvWaitQueue->Remove();
	scheduler->ReadyToRun(firstWaitingThreadForCV);

	if(cvWaitQueue->IsEmpty())
	{
		cvWaitLock = NULL;
	}

    (void) interrupt->SetLevel(oldLevel);
}

//----------------------------------------------------------------------
// Condition::Broadcast
// 	Broadcasting all threads waiting this Condition to occur.
//----------------------------------------------------------------------
void Condition::Broadcast(Lock* conditionLock)
{
	while(!cvWaitQueue->IsEmpty())
	{
		Signal(conditionLock);
	}
}

//----------------------------------------------------------------------
// Condition::isValidCvWaitLock
// 	Checking validity of the lock used with this Condition Variable.
//----------------------------------------------------------------------
bool Condition::isValidCvWaitLock(Lock* conditionLock)
{
	return (cvWaitLock == conditionLock) ? true : false;
}
