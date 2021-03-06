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

int lockNumber1,cvNumber1;
int threadID=2;

void testfunc()
{

#if 1

	int myID;
	myID=threadID;
	threadID=threadID+1;


	Print("START --------------------- testfunc \n");

	Print2("THREAD %d TRYING TO ACQUIRE LOCK %d \n",myID,lockNumber1);
	Acquire(lockNumber1);
	Print2("THREAD %d ACQUIRED LOCK %d \n",myID,lockNumber1);

	Print2("Thread : %d is waiting on lockId : %d \n",myID,lockNumber1);
	Wait(cvNumber1,lockNumber1);

	Print1("Thread %d Coming Out of Wait\n",myID);

	Print2("THREAD %d RELEASING LOCK %d\n",myID,lockNumber1);
	Release(lockNumber1);
	Print2("THREAD %d RELEASED LOCK %d\n",myID,lockNumber1);

	Print1("THREAD %d is done !!!!!!!!!!!!!!!!!! \n",myID);

#endif
	Print("END -----------------------------------  testfunc \n");

	/*
	Exit(0);
	*/

	Yield();
	Yield();
	Yield();
	Yield();
	Yield();
	Yield();
	Yield();
	Yield();
	Yield();
	Exit(0);


}

void testfunc1()
{
#if 1
	int myID;
	myID=threadID;
	threadID=threadID+1;

	Print("START --------------------- testfunc1 \n");

	Print2("THREAD %d TRYING TO ACQUIRE LOCK %d \n",myID,lockNumber1);
	Acquire(lockNumber1);
	Print2("THREAD %d ACQUIRED LOCK %d \n",myID,lockNumber1);

	Print2("Thread : %d to send Signal on LOCK %d \n",myID,lockNumber1);
	Signal(cvNumber1,lockNumber1);

	Print2("THREAD %d RELEASING LOCK %d\n",myID,lockNumber1);
	Release(lockNumber1);
	Print2("THREAD %d RELEASED LOCK %d\n",myID,lockNumber1);

	Print1("THREAD %d is done !!!!!!!!!!!!!!!!!! \n",myID);

#endif

	Print("END -----------------------------------  testfunc1 \n");

	Yield();
	Yield();
	Yield();
	Yield();
	Yield();
	Yield();
	Yield();
	Yield();
	Yield();

	/*
	Exit(0);
	 */

	Exit(0);
}

void testCase2()
{
	int negativeCVId=-1;
	int negativeLockId=-1;
	int hugeLockId=9999;
	int hugeCVId=9999;
#if 0
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
#endif

	lockNumber1 = CreateLock("lock1",5);
	Print1("LOCK ID RECEIVED IS %d\n",lockNumber1);
	cvNumber1 = CreateCondition("cond1",5);
	Print1("CONDITION ID RECEIVED IS %d\n",cvNumber1);

	Fork(testfunc);
	Fork(testfunc1);
	Print("TestCase 2 Exiting \n");

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
	/**
	 * Exit(0);
	 */

}




