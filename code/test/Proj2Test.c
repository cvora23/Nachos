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
	Acquire(lockNumber1);
	myID=threadID;
	threadID=threadID+1;
	Print2("Thread : %d is waiting on lockId : %d \n",myID,lockNumber1);
	threadsWaiting=threadsWaiting+1;
	Wait(cvNumber1,lockNumber1);
	Print1("Thread %d Coming Out of Wait\n",myID);
	Release(lockNumber1);
	Exit(0);
}

void testfunc1()
{
	int myID;
	Acquire(lockNumber1);
	myID=threadID;
	threadID=threadID+1;
	Print1("Thread : %d to send Signal\n",myID);
	Signal(cvNumber1,lockNumber1);
	threadsWaiting=threadsWaiting-1;
	Release(lockNumber1);
	Exit(0);

}

void testfunc2()
{
	int myID;
	Acquire(lockNumber1);
	myID=threadID;
	threadID=threadID+1;
	Print2("Thread : %d to send Broadcast to %d more waiting threads\n",myID,threadsWaiting);
	Broadcast(cvNumber1,lockNumber1);
	Release(lockNumber1);
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

void forktest()
{
	Print("Inside the forked function\n");
	Exit(0);
}

void testCase3()
{
	int i,rv;

	Print("Fork System Call Test\n");
	Print("-------------------------------------------");

	Print("Forking a Thread out of Address Space \n ");
	Fork(0xdeadbeef);

	Print("Forking 3 threads:\n");
	for(i=0;i<3;i++)
	{
		Print1("Thread %d forked\n",i);
		Fork(forktest);
	}

	Exit(0);
}

void testCase4()
{
	int rv;
	Print("EXEC SYSTEM CALL TEST\n\n");
	Print("--------------------------------------------------------\n");
	Print("NEGATIVE TEST CASES\n");
	Print("--------------------------------------------------------\n");

	Print("1.Invalid Call:\nFile name provided to the syscall was invalid\n");
	rv = Exec("../test/dudenoway",14);
	Print("--------------------------------------------------------\n");

#if 0
	Print("2.Invalid Address-space used:\nAddress-space used is out of the range\n");
	rv = Exec((char *)0xffffff00,10);
#endif

	Print("--------------------------------------------------------\n");
	Print("POSITIVE TEST CASES\n");
	Print("--------------------------------------------------------\n");
	Print("1.Executing Executables\nThe same executable is twice executed\n\n");
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

	Print("EXIT SYSTEM CALL TEST\n\n");
	Print("--------------------------------------------------------\n");
	Print("POSITIVE TEST CASES\n");
	Print("--------------------------------------------------------\n");
	Print("1.Exiting a Last Thread in Process\nSimulating Last thread in Exiting process\n\n");
	rv = Exec("../test/exec",12);
	for(i=0;i<10;i++)
	{
		Yield();
	}
	Print("--------------------------------------------------------\n");
	Print("2. Exiting a Last Thread in Last Process\nSimulating Last thread in Last process\n\n");
	Exit(0);
}

void testCase9()
{
	Print("Walmart Market Simulation \n");

	Exec("../test/WalmartSimulation",25);
	Exec("../test/WalmartSimulation",25);

	Exit(0);
}

void testCase12()
{
	Print("Jigar Market Simulation \n\n");

	Exec("../test/Problem2",16);

	Exit(0);
}

int main()
{
	int testCaseNo;

	Print("1 : CreateCondition System Call \n");
	Print("2: Wait Signal BroadCast System Call \n");
	Print("3: Fork System Call\n");
	Print("4: Exec System Call\n");
	Print("5: Exit System Call\n");
	Print("6: Sprintf System Call\n");
	Print("7: PrintStringInt System Call\n");
	Print("8: PrintStringIntInt System Call\n");
	Print("9: Simulate Walmart Simulation One Process \n");
	Print("10: Simulate Walmart Simulation 2 Proces \n");
	Print("11: Print1 System call \n");
	Print("12: Simulate Jigar Simulation One Process \n");
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
#if 0
			testCase6();
#endif
		}
		break;
		case 7:
		{
#if 0
			testCase7();
#endif
		}
		break;
		case 8:
		{
#if 0
			testCase8();
#endif
		}
		break;
		case 9:
		{
			testCase9();
		}
		break;
		case 11:
		{
#if 0
			testCase11();
#endif
		}
		break;
		case 12:
		{
			testCase12();
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


