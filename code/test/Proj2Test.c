/*
 * conditionTest.c
 *
 *  Created on: Mar 25, 2013
 *      Author: cvora
 */

#include "syscall.h"

#define CUSTOMERSALESMANLOCK_STRING	(const char*)"CustomerSalesmanLock"

void testCase1()
{
	int conditionId = -1;
	int i;
	print("CREATE CONDITION SYSTEM CALL TEST\n\n");

	print("PASSING CONDITION NAME LENGTH > MAX_CV_NAME \n\n");
	conditionId = createCondition((char*)"ConditionName",400);
	print("------------------------------------------------------\n");

	print("PASSING CONDITION NAME LENGTH < 0 \n\n");
	conditionId = createCondition((char*)"ConditionName",-4);
	print("------------------------------------------------------\n");

	print("PASSING INVALID VIRTUAL ADDR \n\n");
	conditionId = createCondition((char*)0xdeadbeef,10);
	print("------------------------------------------------------\n");

	print("CREATING MORE number of Conditions than MAX_CVS = 2000 \n\n");
	for(i = 0;i<2001;i++)
	{
		conditionId = createCondition((char*)"ConditionNo",10);
	}
	print("------------------------------------------------------\n");

	print("CREATING CONDITION SUCCESS CASE \n");
	conditionId = createCondition((char*)"ConditionNo",10);
	print("------------------------------------------------------\n");
}


/* TEST FUNCTIONS FOR TEST CASE 2*/

int lockNumber1,cvNumber1,threadsWaiting=0;
int threadID=0;

void testfunc()
{
	int myID;
	acquireLock(lockNumber1);
	myID=threadID;
	threadID=threadID+1;
	print2("Thread : %d is waiting on lockId : %d \n",myID,lockNumber1);
	threadsWaiting=threadsWaiting+1;
	wait(cvNumber1,lockNumber1);
	print1("Thread %d Coming Out of Wait\n",myID);
	releaseLock(lockNumber1);
	Exit(0);
}

void testfunc1()
{
	int myID;
	acquireLock(lockNumber1);
	myID=threadID;
	threadID=threadID+1;
	print1("Thread : %d to send Signal\n",myID);
	signal(cvNumber1,lockNumber1);
	threadsWaiting=threadsWaiting-1;
	releaseLock(lockNumber1);
	Exit(0);

}

void testfunc2()
{
	int myID;
	acquireLock(lockNumber1);
	myID=threadID;
	threadID=threadID+1;
	print2("Thread : %d to send Broadcast to %d more waiting threads\n",myID,threadsWaiting);
	broadcast(cvNumber1,lockNumber1);
	releaseLock(lockNumber1);
	Exit(0);

}

void testCase2()
{
	int negativeCVId=-1;
	int negativeLockId=-1;
	int hugeLockId=9999;
	int hugeCVId=9999;

	print("WAIT/SIGNAL/BROADCAST SYSTEM CALL TEST \n");

	print(" WAITING on invalid LOCK/CONDITION .. Passing -VE CONDITION ID and LOCK ID\n");
	wait(negativeCVId,negativeLockId);
	print(" WAITING on invalid LOCK/CONDITION .. Passing CONDITION ID > MAX_CVS and LOCK ID > MAX_LOCKS\n");
	wait(hugeCVId,hugeLockId);

	print(" SIGNAL on invalid LOCK/CONDITION .. Passing -VE CONDITION ID and LOCK ID\n");
	signal(negativeCVId,negativeLockId);
	print(" SIGNAL on invalid LOCK/CONDITION .. Passing CONDITION ID > MAX_CVS and LOCK ID > MAX_LOCKS\n");
	signal(hugeCVId,hugeLockId);

	print(" BROADCAST on invalid LOCK/CONDITION .. Passing -VE CONDITION ID and LOCK ID\n");
	broadcast(negativeCVId,negativeLockId);
	print(" BROADCAST on invalid LOCK/CONDITION .. Passing CONDITION ID > MAX_CVS and LOCK ID > MAX_LOCKS\n");
	broadcast(hugeCVId,hugeLockId);

	print("SIGNAL , BROADCAST, WAIT SUCCESS CASE\n");
	print("FORKING THREE THREADS TO SIMULATE Wait/Signal/Broadcast System calls \n");

	lockNumber1 = createLock("lock1",5);
	cvNumber1 = createCondition("cond1",5);

	Fork(testfunc);
	Fork(testfunc);
	Fork(testfunc);
	Yield();
	Fork(testfunc1);
	Yield();
	Yield();
	Yield();
	Yield();
	Yield();
	Fork(testfunc2);
	Exit(0);


}

void forktest()
{
	print("Inside the forked function\n");
	Exit(0);
}

void testCase3()
{
	int i,rv;

	print("Fork System Call Test\n");
	print("-------------------------------------------");

	print("Forking a Thread out of Address Space \n ");
	Fork(0xdeadbeef);

	print("Forking 3 threads:\n");
	for(i=0;i<3;i++)
	{
		print1("Thread %d forked\n",i);
		Fork(forktest);
	}

	Exit(0);
}

void testCase4()
{
	int rv;
	print("EXEC SYSTEM CALL TEST\n\n");
	print("--------------------------------------------------------\n");
	print("NEGATIVE TEST CASES\n");
	print("--------------------------------------------------------\n");

	print("1.Invalid Call:\nFile name provided to the syscall was invalid\n");
	rv = Exec("../test/dudenoway",14);
	print("--------------------------------------------------------\n");

#if 0
	print("2.Invalid Address-space used:\nAddress-space used is out of the range\n");
	rv = Exec((char *)0xffffff00,10);
#endif

	print("--------------------------------------------------------\n");
	print("POSITIVE TEST CASES\n");
	print("--------------------------------------------------------\n");
	print("1.Executing Executables\nThe same executable is twice executed\n\n");
	rv = Exec("../test/exec",12);
	rv = Exec("../test/exec",12);
	Exit(0);

}

/**
 * The testcases to prove the correctness of the "Exit" implementation
 */
void testCase5()
{
	int i,rv;

	print("EXIT SYSTEM CALL TEST\n\n");
	print("--------------------------------------------------------\n");
	print("POSITIVE TEST CASES\n");
	print("--------------------------------------------------------\n");
	print("1.Exiting a Last Thread in Process\nSimulating Last thread in Exiting process\n\n");
	rv = Exec("../test/exec",12);
	for(i=0;i<10;i++)
	{
		Yield();
	}
	print("--------------------------------------------------------\n");
	print("2. Exiting a Last Thread in Last Process\nSimulating Last thread in Last process\n\n");
	Exit(0);
}

void testCase6()
{
	char lockName[256];
	print("SPRINTF SYSTEM CALL TEST \n");
	print("--------------------------------------------------------\n");

#if 1
    sprintf(lockName, CUSTOMERSALESMANLOCK_STRING,21, 1);
    printString("Lock Name returned after sprintf test is %s \n",lockName);

#endif

}

void testCase7()
{
	printStringInt("%s has found ITEM_%d and placed ITEM_%d in the trolley \n",CUSTOMERSALESMANLOCK_STRING,5);
}

void testCase8()
{
	printStringIntInt("%s has found ITEM_%d and placed ITEM_%d in the trolley \n",CUSTOMERSALESMANLOCK_STRING,
						5,5);
}

int main()
{
	int testCaseNo;

	print("1 : CreateCondition System Call \n");
	print("2: Wait Signal BroadCast System Call \n");
	print("3: Fork System Call\n");
	print("4: Exec System Call\n");
	print("5: Exit System Call\n");
	print("6: Sprintf System Call\n");
	print("7: PrintStringInt System Call\n");
	print("8: PrintStringIntInt System Call\n");

	print("Enter your choice \n");
	testCaseNo = scan();

	print("\n\n");

	switch(testCaseNo)
	{
		case 1:
		{
			testCase1();
		}
		break;
		case 2:
		{
			testCase2();
		}
		break;
		case 3:
		{
			testCase3();
		}
		break;
		case 4:
		{
			testCase4();
		}
		break;
		case 5:
		{
			testCase5();
		}
		break;
		case 6:
		{
			testCase6();
		}
		break;
		case 7:
		{
			testCase7();
		}
		break;
		case 8:
		{
			testCase8();
		}
		break;
		default:
		{
			print("Invalid Test Case No\n");
		}
		break;
	}
}


