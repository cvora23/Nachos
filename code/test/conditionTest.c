/*
 * conditionTest.c
 *
 *  Created on: Jun 6, 2013
 *      Author: cvora
 */


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
	Print("CREATE CONDITION SYSTEM CALL TEST\n\n");

	Print("PASSING CONDITION NAME LENGTH > MAX_CV_NAME \n\n");
	conditionId = CreateCondition((char*)"ConditionName",400);
	Print("------------------------------------------------------\n");

	Print("PASSING CONDITION NAME LENGTH < 0 \n\n");
	conditionId = CreateCondition((char*)"ConditionName",-4);
	Print("------------------------------------------------------\n");

	Print("PASSING INVALID VIRTUAL ADDR \n\n");
	conditionId = CreateCondition((char*)0xdeadbeef,10);
	Print("------------------------------------------------------\n");

	Print("CREATING MORE number of Conditions than MAX_CVS = 2000 \n\n");
	for(i = 0;i<2001;i++)
	{
		conditionId = CreateCondition((char*)"ConditionNo",10);
		Print1("CONDITION ID RECEIVED IS %d\n",conditionId);

	}
	Print("------------------------------------------------------\n");

	Print("CREATING CONDITION SUCCESS CASE \n");
	conditionId = CreateCondition((char*)"ConditionNo",10);
	Print("------------------------------------------------------\n");
}


/* TEST FUNCTIONS FOR TEST CASE 2*/

int lockNumber1,cvNumber1,threadsWaiting=0;
int threadID=0;

void testfunc()
{
	int myID;
	myID=threadID;
	threadID=threadID+1;

	Acquire(lockNumber1);
	Print1("THREAD %d \n",myID);

	Print2("Thread : %d is waiting on lockId : %d \n",myID,lockNumber1);
	threadsWaiting=threadsWaiting+1;
	Wait(cvNumber1,lockNumber1);
	Print1("THREAD %d \n",myID);

	Print1("Thread %d Coming Out of Wait\n",myID);
	Release(lockNumber1);
	Print1("THREAD %d \n",myID);

	Exit(0);
}

void testfunc1()
{
	int myID;
	Acquire(lockNumber1);
	Print("THREAD 4 \n");
	myID=threadID;
	threadID=threadID+1;
	Print1("Thread : %d to send Signal\n",myID);
	Signal(cvNumber1,lockNumber1);
	Print("THREAD 1 \n");
	threadsWaiting=threadsWaiting-1;
	Release(lockNumber1);
	Print("THREAD 1 \n");
	Exit(0);
}

void testfunc2()
{
	int myID;
	Acquire(lockNumber1);
	Print("THREAD 5 \n");
	myID=threadID;
	threadID=threadID+1;
	Print2("Thread : %d to send Broadcast to %d more waiting threads\n",myID,threadsWaiting);
	Broadcast(cvNumber1,lockNumber1);
	Print("THREAD 2 \n");
	Release(lockNumber1);
	Print("THREAD 2 \n");
	Exit(0);

}

void testCase2()
{
	int negativeCVId=-1;
	int negativeLockId=-1;
	int hugeLockId=9999;
	int hugeCVId=9999;

	Print("WAIT/SIGNAL/BROADCAST SYSTEM CALL TEST \n");

	Print(" WAITING on invalid LOCK/CONDITION .. Passing -VE CONDITION ID and LOCK ID\n");
	Wait(negativeCVId,negativeLockId);
	Print(" WAITING on invalid LOCK/CONDITION .. Passing CONDITION ID > MAX_CVS and LOCK ID > MAX_LOCKS\n");
	Wait(hugeCVId,hugeLockId);

	Print(" SIGNAL on invalid LOCK/CONDITION .. Passing -VE CONDITION ID and LOCK ID\n");
	Signal(negativeCVId,negativeLockId);
	Print(" SIGNAL on invalid LOCK/CONDITION .. Passing CONDITION ID > MAX_CVS and LOCK ID > MAX_LOCKS\n");
	Signal(hugeCVId,hugeLockId);

	Print(" BROADCAST on invalid LOCK/CONDITION .. Passing -VE CONDITION ID and LOCK ID\n");
	Broadcast(negativeCVId,negativeLockId);
	Print(" BROADCAST on invalid LOCK/CONDITION .. Passing CONDITION ID > MAX_CVS and LOCK ID > MAX_LOCKS\n");
	Broadcast(hugeCVId,hugeLockId);

	Print("SIGNAL , BROADCAST, WAIT SUCCESS CASE\n");
	Print("FORKING THREE THREADS TO SIMULATE Wait/Signal/Broadcast System calls \n");

	lockNumber1 = CreateLock("lock1",5);
	cvNumber1 = CreateCondition("cond1",5);

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

int main()
{
	int testCaseNo;

	Print("1 : CreateCondition System Call \n");
	Print("2: Wait Signal BroadCast System Call \n");
	Print("Enter your choice \n");
	testCaseNo = Scan();

	Print("\n\n");

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
		default:
		{
			Print("Invalid Test Case No\n");
		}
		break;
	}
	Exit(0);
}




