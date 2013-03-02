// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#ifdef CHANGED
#include "synch.h"
#endif
//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------

void
SimpleThread(int which)
{
    int num;
    
    for (num = 0; num < 2; num++)
    {
    	printf(" \n Thread Name: ");
    	currentThread->Print();
    	printf("\n*** thread %d looped %d times\n", which, num);
        currentThread->Yield();
    }
}

//----------------------------------------------------------------------
// ThreadTest
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest()
{
    DEBUG('t', "Entering SimpleTest");

    Thread *t = new Thread("forked thread");

    t->Fork(SimpleThread, 1);
    SimpleThread(0);
}

#ifdef CHANGED
// --------------------------------------------------
// Test Suite
// --------------------------------------------------


// --------------------------------------------------
// Test 1 - see TestSuite() for details
// --------------------------------------------------
Semaphore t1_s1("t1_s1",0);       // To make sure t1_t1 acquires the
                                  // lock before t1_t2
Semaphore t1_s2("t1_s2",0);       // To make sure t1_t2 Is waiting on the
                                  // lock before t1_t3 releases it
Semaphore t1_s3("t1_s3",0);       // To make sure t1_t1 does not release the
                                  // lock before t1_t3 tries to acquire it
Semaphore t1_done("t1_done",0);   // So that TestSuite knows when Test 1 is
                                  // done
Lock t1_l1("t1_l1");		  // the lock tested in Test 1

// --------------------------------------------------
// t1_t1() -- test1 thread 1
//     This is the rightful lock owner
// --------------------------------------------------
void t1_t1() {
    t1_l1.Acquire();
    t1_s1.V();  // Allow t1_t2 to try to Acquire Lock

    printf ("%s: Acquired Lock %s, waiting for t3\n",currentThread->getName(),
	    t1_l1.getName());
    t1_s3.P();
    printf ("%s: working in CS\n",currentThread->getName());
    for (int i = 0; i < 1000000; i++) ;
    printf ("%s: Releasing Lock %s\n",currentThread->getName(),
	    t1_l1.getName());
    t1_l1.Release();
    t1_done.V();
}

// --------------------------------------------------
// t1_t2() -- test1 thread 2
//     This thread will wait on the held lock.
// --------------------------------------------------
void t1_t2() {

    t1_s1.P();	// Wait until t1 has the lock
    t1_s2.V();  // Let t3 try to acquire the lock

    printf("%s: trying to acquire lock %s\n",currentThread->getName(),
	    t1_l1.getName());
    t1_l1.Acquire();

    printf ("%s: Acquired Lock %s, working in CS\n",currentThread->getName(),
	    t1_l1.getName());
    for (int i = 0; i < 10; i++)
	;
    printf ("%s: Releasing Lock %s\n",currentThread->getName(),
	    t1_l1.getName());
    t1_l1.Release();
    t1_done.V();
}

// --------------------------------------------------
// t1_t3() -- test1 thread 3
//     This thread will try to release the lock illegally
// --------------------------------------------------
void t1_t3() {

    t1_s2.P();	// Wait until t2 is ready to try to acquire the lock

    t1_s3.V();	// Let t1 do it's stuff
    for ( int i = 0; i < 3; i++ )
    {
    	printf("%s: Trying to release Lock %s\n",currentThread->getName(),
	       t1_l1.getName());
    	t1_l1.Release();
    }
}

// --------------------------------------------------
// Test 2 - see TestSuite() for details
// --------------------------------------------------
Lock t2_l1("t2_l1");		// For mutual exclusion
Condition t2_c1("t2_c1");	// The condition variable to test
Semaphore t2_s1("t2_s1",0);	// To ensure the Signal comes before the wait
Semaphore t2_done("t2_done",0);     // So that TestSuite knows when Test 2 is
                                  // done

// --------------------------------------------------
// t2_t1() -- test 2 thread 1
//     This thread will signal a variable with nothing waiting
// --------------------------------------------------
void t2_t1() {
    t2_l1.Acquire();
    printf("%s: Lock %s acquired, signalling %s\n",currentThread->getName(),
	   t2_l1.getName(), t2_c1.getName());
    t2_c1.Signal(&t2_l1);
    printf("%s: Releasing Lock %s\n",currentThread->getName(),
	   t2_l1.getName());
    t2_l1.Release();
    t2_s1.V();	// release t2_t2
    t2_done.V();
}

// --------------------------------------------------
// t2_t2() -- test 2 thread 2
//     This thread will wait on a pre-signalled variable
// --------------------------------------------------
void t2_t2() {
    t2_s1.P();	// Wait for t2_t1 to be done with the lock
    t2_l1.Acquire();
    printf("%s: Lock %s acquired, waiting on %s\n",currentThread->getName(),
	   t2_l1.getName(), t2_c1.getName());
    t2_c1.Wait(&t2_l1);
    printf("%s: Releasing Lock %s\n",currentThread->getName(),
	   t2_l1.getName());
    t2_l1.Release();
}
// --------------------------------------------------
// Test 3 - see TestSuite() for details
// --------------------------------------------------
Lock t3_l1("t3_l1");		// For mutual exclusion
Condition t3_c1("t3_c1");	// The condition variable to test
Semaphore t3_s1("t3_s1",0);	// To ensure the Signal comes before the wait
Semaphore t3_done("t3_done",0); // So that TestSuite knows when Test 3 is
                                // done

// --------------------------------------------------
// t3_waiter()
//     These threads will wait on the t3_c1 condition variable.  Only
//     one t3_waiter will be released
// --------------------------------------------------
void t3_waiter() {
    t3_l1.Acquire();
    t3_s1.V();		// Let the signaller know we're ready to wait
    printf("%s: Lock %s acquired, waiting on %s\n",currentThread->getName(),
	   t3_l1.getName(), t3_c1.getName());
    t3_c1.Wait(&t3_l1);
    printf("%s: freed from %s\n",currentThread->getName(), t3_c1.getName());
    t3_l1.Release();
    t3_done.V();
}


// --------------------------------------------------
// t3_signaller()
//     This threads will signal the t3_c1 condition variable.  Only
//     one t3_signaller will be released
// --------------------------------------------------
void t3_signaller() {

    // Don't signal until someone's waiting

    for ( int i = 0; i < 5 ; i++ )
	t3_s1.P();
    t3_l1.Acquire();
    printf("%s: Lock %s acquired, signalling %s\n",currentThread->getName(),
	   t3_l1.getName(), t3_c1.getName());
    t3_c1.Signal(&t3_l1);
    printf("%s: Releasing %s\n",currentThread->getName(), t3_l1.getName());
    t3_l1.Release();
    t3_done.V();
}

// --------------------------------------------------
// Test 4 - see TestSuite() for details
// --------------------------------------------------
Lock t4_l1("t4_l1");		// For mutual exclusion
Condition t4_c1("t4_c1");	// The condition variable to test
Semaphore t4_s1("t4_s1",0);	// To ensure the Signal comes before the wait
Semaphore t4_done("t4_done",0); // So that TestSuite knows when Test 4 is
                                // done

// --------------------------------------------------
// t4_waiter()
//     These threads will wait on the t4_c1 condition variable.  All
//     t4_waiters will be released
// --------------------------------------------------
void t4_waiter() {
    t4_l1.Acquire();
    t4_s1.V();		// Let the signaller know we're ready to wait
    printf("%s: Lock %s acquired, waiting on %s\n",currentThread->getName(),
	   t4_l1.getName(), t4_c1.getName());
    t4_c1.Wait(&t4_l1);
    printf("%s: freed from %s\n",currentThread->getName(), t4_c1.getName());
    t4_l1.Release();
    t4_done.V();
}


// --------------------------------------------------
// t2_signaller()
//     This thread will broadcast to the t4_c1 condition variable.
//     All t4_waiters will be released
// --------------------------------------------------
void t4_signaller() {

    // Don't broadcast until someone's waiting

    for ( int i = 0; i < 5 ; i++ )
	t4_s1.P();
    t4_l1.Acquire();
    printf("%s: Lock %s acquired, broadcasting %s\n",currentThread->getName(),
	   t4_l1.getName(), t4_c1.getName());
    t4_c1.Broadcast(&t4_l1);
    printf("%s: Releasing %s\n",currentThread->getName(), t4_l1.getName());
    t4_l1.Release();
    t4_done.V();
}
// --------------------------------------------------
// Test 5 - see TestSuite() for details
// --------------------------------------------------
Lock t5_l1("t5_l1");		// For mutual exclusion
Lock t5_l2("t5_l2");		// Second lock for the bad behavior
Condition t5_c1("t5_c1");	// The condition variable to test
Semaphore t5_s1("t5_s1",0);	// To make sure t5_t2 acquires the lock after
                                // t5_t1

// --------------------------------------------------
// t5_t1() -- test 5 thread 1
//     This thread will wait on a condition under t5_l1
// --------------------------------------------------
void t5_t1() {
    t5_l1.Acquire();
    t5_s1.V();	// release t5_t2
    printf("%s: Lock %s acquired, waiting on %s\n",currentThread->getName(),
	   t5_l1.getName(), t5_c1.getName());
    t5_c1.Wait(&t5_l1);
    printf("%s: Releasing Lock %s\n",currentThread->getName(),
	   t5_l1.getName());
    t5_l1.Release();
}

// --------------------------------------------------
// t5_t1() -- test 5 thread 1
//     This thread will wait on a t5_c1 condition under t5_l2, which is
//     a Fatal error
// --------------------------------------------------
void t5_t2() {
    t5_s1.P();	// Wait for t5_t1 to get into the monitor
    t5_l1.Acquire();
    t5_l2.Acquire();
    printf("%s: Lock %s acquired, signalling %s\n",currentThread->getName(),
	   t5_l2.getName(), t5_c1.getName());
    t5_c1.Signal(&t5_l2);
    printf("%s: Releasing Lock %s\n",currentThread->getName(),
	   t5_l2.getName());
    t5_l2.Release();
    printf("%s: Releasing Lock %s\n",currentThread->getName(),
	   t5_l1.getName());
    t5_l1.Release();
}

// --------------------------------------------------
// TestSuite1()
//     This is the main thread of the test suite.  It runs the
//     following tests:
//
//       1.  Show that a thread trying to release a lock it does not
//           hold does not work
// --------------------------------------------------
void TestSuite1()
{
    Thread *t;
    char *name;
    int i;

    // Test 1

    printf("Starting Test 1\n");

    t = new Thread("t1_t1");
    t->Fork((VoidFunctionPtr)t1_t1,0);

    t = new Thread("t1_t2");
    t->Fork((VoidFunctionPtr)t1_t2,0);

    t = new Thread("t1_t3");
    t->Fork((VoidFunctionPtr)t1_t3,0);

    // Wait for Test 1 to complete
    for (  i = 0; i < 2; i++ )
	t1_done.P();
}

// --------------------------------------------------
// TestSuite2()
//     This is the main thread of the test suite.  It runs the
//     following tests:
//
//       2.  Show that Signals are not stored -- a Signal with no
//           thread waiting is ignored
// --------------------------------------------------
void TestSuite2()
{
    Thread *t;
    char *name;
    int i;
    // Test 2

    printf("Starting Test 2.  Note that it is an error if thread t2_t2\n");
    printf("completes\n");

    t = new Thread("t2_t1");
    t->Fork((VoidFunctionPtr)t2_t1,0);

    t = new Thread("t2_t2");
    t->Fork((VoidFunctionPtr)t2_t2,0);

    // Wait for Test 2 to complete
    t2_done.P();
}

// --------------------------------------------------
// TestSuite3()
//     This is the main thread of the test suite.  It runs the
//     following tests:
//
//       3.  Show that Signal only wakes 1 thread
// --------------------------------------------------
void TestSuite3()
{
    Thread *t;
    char *name;
    int i;
    // Test 3

    printf("Starting Test 3\n");

    for (  i = 0 ; i < 5 ; i++ ) {
	name = new char [20];
	sprintf(name,"t3_waiter%d",i);
	t = new Thread(name);
	t->Fork((VoidFunctionPtr)t3_waiter,0);
    }
    t = new Thread("t3_signaller");
    t->Fork((VoidFunctionPtr)t3_signaller,0);

    // Wait for Test 3 to complete
    for (  i = 0; i < 2; i++ )
	t3_done.P();
}

// --------------------------------------------------
// TestSuite4()
//     This is the main thread of the test suite.  It runs the
//     following tests:
//
//	 	 4.  Show that Broadcast wakes all waiting threads
// --------------------------------------------------
void TestSuite4()
{
    Thread *t;
    char *name;
    int i;
    // Test 4

    printf("Starting Test 4\n");

    for (  i = 0 ; i < 5 ; i++ ) {
	name = new char [20];
	sprintf(name,"t4_waiter%d",i);
	t = new Thread(name);
	t->Fork((VoidFunctionPtr)t4_waiter,0);
    }
    t = new Thread("t4_signaller");
    t->Fork((VoidFunctionPtr)t4_signaller,0);

    // Wait for Test 4 to complete
    for (  i = 0; i < 6; i++ )
	t4_done.P();
}

// --------------------------------------------------
// TestSuite5()
//     This is the main thread of the test suite.  It runs the
//     following tests:
//
//       5.  Show that Signaling a thread waiting under one lock
//           while holding another is a Fatal error
//
// --------------------------------------------------
void TestSuite5()
{
    Thread *t;
    char *name;
    int i;
    // Test 5

    printf("Starting Test 5.  Note that it is an error if thread t5_t1\n");
    printf("completes\n");

    t = new Thread("t5_t1");
    t->Fork((VoidFunctionPtr)t5_t1,0);

    t = new Thread("t5_t2");
    t->Fork((VoidFunctionPtr)t5_t2,0);
}


/**
 * TestSuite6() Producer Consumer Implementation only with locks:
 * 	1:	Producers add characters to a buffer.
 * 	2: 	Consumers remove characters from buffer.
 * 	3:	Characters will removed in the same order added.
 */

/**
 * Size of Buffer
 */
#define BUFSIZE 256
/*
 * Buffer used to read from and write to
 */
char buffer[BUFSIZE];
/**
 * Head and Tail of the buffer
 */
int head = 0,tail = 0;
/**
 * Lock used to protect the buffer for mutual exclusion
 */
Lock BufferLock("BufferLock");
/**
 * Counter to keep track of number of bytes added to Buffer
 */
int count;
/**
 * put function to add characters to protected buffer
 */
void put(char c)
{
	BufferLock.Acquire();
	count++;
	buffer[head] = c;
	head++;
	if(head == BUFSIZE)
	{
		head = 0;
	}
	BufferLock.Release();
}
/**
 * get function to remove characters from protected buffer
 */
char get()
{
	char c;
	BufferLock.Acquire();
	count--;
	c = buffer[tail];
	tail++;
	if(tail == BUFSIZE)
	{
		tail = 0;
	}
	BufferLock.Release();
	return c;
}

/**
 * Producer Thread - Adds character to a buffer
 */
void Producer()
{
    DEBUG('t', "Producer Started !!!!! \n");
    char* putBuffer = "Hello World";
    while(*putBuffer)
    {
    	put(*putBuffer);
    	*putBuffer++;
    }
    DEBUG('t', "Producer Done !!!! \n");
}

/**
 * Consumer Thread - Removes character from a buffer
 */
void Consumer()
{
    DEBUG('t', "Consumer Started !!!!! \n");
    char bufPtr;
    while(count != 0)
    {
    	bufPtr = get();
    	DEBUG('t',"%c",bufPtr);
    }
    DEBUG('t', "\nConsumer Done !!!!! \n");
}


void TestSuite6()
{
    Thread *t;
    char *name;

    DEBUG('t', "Entering TestSuite6 !!!!!!! \n");

    t = new Thread("Producer");
    t->Fork((VoidFunctionPtr)Producer,0);

    t = new Thread("Consumer");
    t->Fork((VoidFunctionPtr)Consumer,0);

}

/**
 * TestSuite7() Producer Consumer Implementation only with locks:
 * 	1:	Producers add characters to a buffer.
 * 	2: 	Consumers remove characters from buffer.
 * 	3:	Characters will removed in the same order added.
 * 	4:  Test also covers the empty/full cases with protected buffer.
 */

/**
 * Size of Buffer
 */
#define T7_BUFSIZE 5
/*
 * Buffer used to read from and write to
 */
char T7_buffer[T7_BUFSIZE];
/**
 * Head and Tail of the buffer
 */
int T7_head = 0,T7_tail = 0;
/**
 * Lock used to protect the buffer for mutual exclusion
 */
Lock T7_BufferLock("T7_BufferLock");
/**
 * Counter to keep track of number of bytes added to Buffer
 */
int T7_count;
/**
 * End the test now
 */
int T7_end = 0;
/**
 * put function to add characters to protected buffer
 */
void T7_put(char c)
{
	T7_BufferLock.Acquire();
	while(T7_count == T7_BUFSIZE)
	{
		T7_BufferLock.Release();
		currentThread->Yield();
		T7_BufferLock.Acquire();
	}
	T7_count++;
	T7_buffer[T7_head] = c;
	T7_head++;
	if(T7_head == T7_BUFSIZE)
	{
		T7_head = 0;
	}
	T7_BufferLock.Release();
}
/**
 * get function to remove characters from protected buffer
 */
char T7_get()
{
	char c;
	T7_BufferLock.Acquire();
	while(T7_count == 0)
	{
		T7_BufferLock.Release();
		currentThread->Yield();
		T7_BufferLock.Acquire();
	}
	T7_count--;
	c = T7_buffer[T7_tail];
	T7_tail++;
	if(T7_tail == T7_BUFSIZE)
	{
		T7_tail = 0;
	}
	T7_BufferLock.Release();
	return c;
}

/**
 * Producer Thread - Adds character to a buffer
 */
void T7_Producer()
{
    DEBUG('t', "T7_Producer Started !!!!! \n");
    char* putBuffer = "Hello World";
    while(*putBuffer)
    {
    	T7_put(*putBuffer);
    	*putBuffer++;
    }
    DEBUG('t', "T7_Producer Done !!!! \n");
    T7_end = 1;
}

/**
 * Consumer Thread - Removes character from a buffer
 */
void T7_Consumer()
{
    DEBUG('t', "T7_Consumer Started !!!!! \n");
    char bufPtr;
    while(T7_end != 1)
    {
    	bufPtr = T7_get();
    	DEBUG('t',"%c",bufPtr);
    }
    DEBUG('t', "\nT7_Consumer Done !!!!! \n");
}


void TestSuite7()
{
    Thread *t;
    char *name;

    DEBUG('t', "Entering TestSuite7 !!!!!!! \n");

    t = new Thread("T7_Producer");
    t->Fork((VoidFunctionPtr)T7_Producer,0);

    t = new Thread("T7_Consumer");
    t->Fork((VoidFunctionPtr)T7_Consumer,0);

}


/**
 * TestSuite7() Producer Consumer Implementation only with locks:
 * 	1:	Producers add characters to a buffer.
 * 	2: 	Consumers remove characters from buffer.
 * 	3:	Characters will removed in the same order added.
 * 	4:  Test also covers the empty/full cases with protected buffer.
 *
 * 	Synchronization mechanisms need more than just mutual exclusion;
 * 	also need a way to wait for another thread to do something
 * 	(e.g., wait for a character to be added to the buffer)
 *
 * 	Condition Variables added to make sure we don't need to call yield for
 * 	synchronization.
 */

/**
 * Size of Buffer
 */
#define T8_BUFSIZE 5
/*
 * Buffer used to read from and write to
 */
char T8_buffer[T8_BUFSIZE];
/**
 * Head and Tail of the buffer
 */
int T8_head = 0,T8_tail = 0;
/**
 * Lock used to protect the buffer for mutual exclusion
 */
Lock T8_BufferLock("T8_BufferLock");
/**
 * not Empty Condition used to protect the buffer for synchronization
 */
Condition T8_notEmptyCondition("T8_notEmptyCondition");
/**
 * not Full Condition used to protect the buffer for synchronization
 */
Condition T8_notFullCondition("T8_notFullCondition");
/**
 * Counter to keep track of number of bytes added to Buffer
 */
int T8_count;
/**
 * End the test now
 */
int T8_end = 0;
/**
 * put function to add characters to protected buffer
 */
void T8_put(char c)
{
	T8_BufferLock.Acquire();
	while(T8_count == T8_BUFSIZE)
	{
		T8_notFullCondition.Wait(&T8_BufferLock);
	}
	T8_count++;
	T8_buffer[T8_head] = c;
	T8_head++;
	if(T8_head == T8_BUFSIZE)
	{
		T8_head = 0;
	}
	T8_notEmptyCondition.Signal(&T8_BufferLock);
	T8_BufferLock.Release();
}
/**
 * get function to remove characters from protected buffer
 */
char T8_get()
{
	char c;
	T8_BufferLock.Acquire();
	while(T8_count == 0)
	{
		T8_notEmptyCondition.Wait(&T8_BufferLock);
	}
	T8_count--;
	c = T8_buffer[T8_tail];
	T8_tail++;
	if(T8_tail == T8_BUFSIZE)
	{
		T8_tail = 0;
	}
	T8_notFullCondition.Signal(&T8_BufferLock);
	T8_BufferLock.Release();
	return c;
}

/**
 * Producer Thread - Adds character to a buffer
 */
void T8_Producer()
{
    DEBUG('t', "T8_Producer Started !!!!! \n");
    char* putBuffer = "Hello World";
    while(*putBuffer)
    {
    	T8_put(*putBuffer);
    	*putBuffer++;
    }
    DEBUG('t', "T8_Producer Done !!!! \n");
    T8_end = 1;
}

/**
 * Consumer Thread - Removes character from a buffer
 */
void T8_Consumer()
{
    DEBUG('t', "T8_Consumer Started !!!!! \n");
    char bufPtr;
    while(T8_end != 1)
    {
    	bufPtr = T8_get();
    	DEBUG('t',"%c",bufPtr);
    }
    DEBUG('t', "\nT8_Consumer Done !!!!! \n");
}


void TestSuite8()
{
    Thread *t;
    char *name;

    DEBUG('t', "Entering TestSuite7 !!!!!!! \n");

    t = new Thread("T8_Producer");
    t->Fork((VoidFunctionPtr)T8_Producer,0);

    t = new Thread("T8_Consumer");
    t->Fork((VoidFunctionPtr)T8_Consumer,0);

}

// --------------------------------------------------
// TestSuite()
//     This is the main thread of the test suite.  It runs the
//     following tests:
//
//       1.  Show that a thread trying to release a lock it does not
//           hold does not work
//
//       2.  Show that Signals are not stored -- a Signal with no
//           thread waiting is ignored
//
//       3.  Show that Signal only wakes 1 thread
//
//	 	 4.  Show that Broadcast wakes all waiting threads
//
//       5.  Show that Signaling a thread waiting under one lock
//           while holding another is a Fatal error
//
//           Fatal errors terminate the thread in question.
// --------------------------------------------------

void TestSuite()
{
    Thread *t;
    char *name;
    int i;

    // Test 1

    printf("Starting Test 1\n");

    t = new Thread("t1_t1");
    t->Fork((VoidFunctionPtr)t1_t1,0);

    t = new Thread("t1_t2");
    t->Fork((VoidFunctionPtr)t1_t2,0);

    t = new Thread("t1_t3");
    t->Fork((VoidFunctionPtr)t1_t3,0);

    // Wait for Test 1 to complete
    for (  i = 0; i < 2; i++ )
	t1_done.P();

    // Test 2

    printf("Starting Test 2.  Note that it is an error if thread t2_t2\n");
    printf("completes\n");

    t = new Thread("t2_t1");
    t->Fork((VoidFunctionPtr)t2_t1,0);

    t = new Thread("t2_t2");
    t->Fork((VoidFunctionPtr)t2_t2,0);

    // Wait for Test 2 to complete
    t2_done.P();

    // Test 3

    printf("Starting Test 3\n");

    for (  i = 0 ; i < 5 ; i++ ) {
	name = new char [20];
	sprintf(name,"t3_waiter%d",i);
	t = new Thread(name);
	t->Fork((VoidFunctionPtr)t3_waiter,0);
    }
    t = new Thread("t3_signaller");
    t->Fork((VoidFunctionPtr)t3_signaller,0);

    // Wait for Test 3 to complete
    for (  i = 0; i < 2; i++ )
	t3_done.P();

    // Test 4

    printf("Starting Test 4\n");

    for (  i = 0 ; i < 5 ; i++ ) {
	name = new char [20];
	sprintf(name,"t4_waiter%d",i);
	t = new Thread(name);
	t->Fork((VoidFunctionPtr)t4_waiter,0);
    }
    t = new Thread("t4_signaller");
    t->Fork((VoidFunctionPtr)t4_signaller,0);

    // Wait for Test 4 to complete
    for (  i = 0; i < 6; i++ )
	t4_done.P();

    // Test 5

    printf("Starting Test 5.  Note that it is an error if thread t5_t1\n");
    printf("completes\n");

    t = new Thread("t5_t1");
    t->Fork((VoidFunctionPtr)t5_t1,0);

    t = new Thread("t5_t2");
    t->Fork((VoidFunctionPtr)t5_t2,0);

}
#endif
