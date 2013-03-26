/*
 * conditionTest.c
 *
 *  Created on: Mar 25, 2013
 *      Author: cvora
 */

#include "syscall.h"

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

void testCase2()
{
	int lockId = -1,conditionId = -1;

	print("DESTROY CONDITION SYSTEM CALL TEST\n\n");

	print("PASSING CONDITION ID > MAX_CVS to destroy \n\n");
	destroyCondition(2001);
	print("------------------------------------------------------\n");

	print("PASSING CONDITION ID < 0 to destroy \n\n");
	destroyCondition(-1);
	print("------------------------------------------------------\n");

	print("TRYING TO DESTROY A CONDITION NOT EVEN CREATED \n\n");
	destroyCondition(0);
	print("------------------------------------------------------\n");

	print("TRYING TO DESTROY A CONDITION ALREADY IN USE \n\n");
	lockId = createLock((char*)"LockName",10);
	acquireLock(lockId);
	conditionId = createCondition((char*)"ConditionName",10);
	wait(conditionId,lockId);
	releaseLock(lockId);
	destroyCondition(conditionId);
	print("------------------------------------------------------\n");


	print("DESTROYING CONDITION SUCCESS CASE \n");
	conditionId = createCondition((char*)"ConditionName",10);
	destroyCondition(conditionId);
	print("------------------------------------------------------\n");

}

void testCase3()
{
	int lockId = -1;

	print("ACQUIRE LOCK SYSTEM CALL TEST\n\n");

	print("PASSING LOCK ID > MAX_LOCK to acquire \n\n");
	acquireLock(1000);
	print("------------------------------------------------------\n");

	print("PASSING LOCK ID < 0 to acquire \n\n");
	acquireLock(-1);
	print("------------------------------------------------------\n");

	print("TRYING TO acquire A LOCK NOT EVEN CREATED \n\n");
	acquireLock(0);
	print("------------------------------------------------------\n");

	print("TRYING TO acquire A LOCK already destroyed \n\n");
	lockId = createLock((char*)"LockName",10);
	destroyLock(lockId);
	acquireLock(lockId);
	print("------------------------------------------------------\n");


	print("ACQUIRING LOCK SUCCESS CASE \n");
	lockId = createLock((char*)"LockName",10);
	acquireLock(lockId);
	releaseLock(lockId);
	destroyLock(lockId);
	print("------------------------------------------------------\n");
}

int main()
{
	int testCaseNo;

	print("1 : CreateCondition system call \n");
	/**
	 * \todo: Fork and Exec needed for This test case
	 * 	print("2 : DestroyCondition system call \n");
	 */

	/**
	 * \todo: Fork and Exec needed for This test case
	 * 	print("3 : Wait Signal Broadcast system call \n");
	 */

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
		default:
		{
			print("Invalid Test Case No\n");
		}
		break;
	}
}


