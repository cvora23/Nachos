/*
 * lockTest.c
 *
 *  Created on: Mar 25, 2013
 *      Author: cvora
 */

#include "syscall.h"


void testCase1()
{
	int lockId = -1;
	int i;
	Print("CREATE LOCK SYSTEM CALL TEST\n\n");

	Print("PASSING LOCK NAME LENGTH > MAX_LOCK_NAME \n\n");
	lockId = CreateLock((char*)"LockName",400);
	Print("------------------------------------------------------\n");

	Print("PASSING LOCK NAME LENGTH < 0 \n\n");
	lockId = CreateLock((char*)"LockName",-4);
	Print("------------------------------------------------------\n");

	Print("PASSING INVALID VIRTUAL ADDR \n\n");
	lockId = CreateLock((char*)0xdeadbeef,10);
	Print("------------------------------------------------------\n");

	Print("CREATING MORE number of locks than MAX_LOCKS = 500 \n\n");
	for(i = 0;i<501;i++)
	{
		lockId = CreateLock((char*)"LockNo",10);
	}
	Print("------------------------------------------------------\n");

	Print("CREATING LOCK SUCCESS CASE \n");
	lockId = CreateLock((char*)"LockNo",10);
	Print("------------------------------------------------------\n");

}

void testCase2()
{
	int lockId = -1;

	Print("DESTROY LOCK SYSTEM CALL TEST\n\n");

	Print("PASSING LOCK ID > MAX_LOCK to destroy \n\n");
	DestroyLock(1000);
	Print("------------------------------------------------------\n");

	Print("PASSING LOCK ID < 0 to destroy \n\n");
	DestroyLock(-1);
	Print("------------------------------------------------------\n");

	Print("TRYING TO DESTROY A LOCK NOT EVEN CREATED \n\n");
	DestroyLock(0);
	Print("------------------------------------------------------\n");

	Print("TRYING TO DESTROY A LOCK ALREADY IN USE \n\n");
	lockId = CreateLock((char*)"LockName",10);
	Acquire(lockId);
	DestroyLock(lockId);
	Release(lockId);
	Print("------------------------------------------------------\n");


	Print("DESTROYING LOCK SUCCESS CASE \n");
	lockId = CreateLock((char*)"LockName",10);
	DestroyLock(lockId);
	Print("------------------------------------------------------\n");

}

void testCase3()
{
	int lockId = -1;

	Print("ACQUIRE LOCK SYSTEM CALL TEST\n\n");

	Print("PASSING LOCK ID > MAX_LOCK to acquire \n\n");
	Acquire(1000);
	Print("------------------------------------------------------\n");

	Print("PASSING LOCK ID < 0 to acquire \n\n");
	Acquire(-1);
	Print("------------------------------------------------------\n");

	Print("TRYING TO acquire A LOCK NOT EVEN CREATED \n\n");
	Acquire(0);
	Print("------------------------------------------------------\n");

	Print("TRYING TO acquire A LOCK already destroyed \n\n");
	lockId = CreateLock((char*)"LockName",10);
	DestroyLock(lockId);
	Acquire(lockId);
	Print("------------------------------------------------------\n");


	Print("ACQUIRING LOCK SUCCESS CASE \n");
	lockId = CreateLock((char*)"LockName",10);
	Acquire(lockId);
	Release(lockId);
	DestroyLock(lockId);
	Print("------------------------------------------------------\n");
}

void testCase4()
{
	int lockId = -1;

	Print("RELEASE LOCK SYSTEM CALL TEST\n\n");

	Print("PASSING LOCK ID > MAX_LOCK to release \n\n");
	Release(1000);
	Print("------------------------------------------------------\n");

	Print("PASSING LOCK ID < 0 to release \n\n");
	Release(-1);
	Print("------------------------------------------------------\n");

	Print("TRYING TO release A LOCK NOT EVEN CREATED \n\n");
	Release(0);
	Print("------------------------------------------------------\n");

	Print("RELEASE LOCK SUCCESS CASE \n");
	lockId = CreateLock((char*)"LockName",10);
	Acquire(lockId);
	Release(lockId);
	DestroyLock(lockId);
	Print("------------------------------------------------------\n");
}

int main()
{
	int testCaseNo;

	Print("1 : CreateLock system call \n");
	Print("2 : DestroyLock system call \n");
	Print("3 : AcquireLock system call \n");
	Print("4 : ReleaseLock system call \n");

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
		default:
		{
			Print("Invalid Test Case No\n");
		}
		break;
	}
}


