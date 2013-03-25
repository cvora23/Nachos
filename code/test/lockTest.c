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
	print("CREATE LOCK SYSTEM CALL TEST\n\n");

	print("PASSING LOCK NAME LENGTH > MAX_LOCK_NAME \n\n");
	lockId = createLock((char*)"LockName",400);
	print("------------------------------------------------------\n");

	print("PASSING LOCK NAME LENGTH < 0 \n\n");
	lockId = createLock((char*)"LockName",-4);
	print("------------------------------------------------------\n");

	print("PASSING INVALID VIRTUAL ADDR \n\n");
	lockId = createLock((char*)0xdeadbeef,10);
	print("------------------------------------------------------\n");

	print("CREATING MORE number of locks than MAX_LOCKS = 500 \n\n");
	for(i = 0;i<501;i++)
	{
		lockId = createLock((char*)"LockNo",10);
	}
	print("------------------------------------------------------\n");

	print("CREATING LOCK SUCCESS CASE \n");
	createLock((char*)"LockNo",10);
	print("------------------------------------------------------\n");

}

void testCase2()
{
	int lockId = -1;

	print("DESTROY LOCK SYSTEM CALL TEST\n\n");

	print("PASSING LOCK ID > MAX_LOCK to destroy \n\n");
	destroyLock(1000);
	print("------------------------------------------------------\n");

	print("PASSING LOCK ID < 0 to destroy \n\n");
	destroyLock(-1);
	print("------------------------------------------------------\n");

	print("TRYING TO DESTROY A LOCK NOT EVEN CREATED \n\n");
	destroyLock(0);
	print("------------------------------------------------------\n");

	print("TRYING TO DESTROY A LOCK ALREADY IN USE \n\n");
	lockId = createLock((char*)"LockName",10);
	acquireLock(lockId);
	destroyLock(lockId);
	releaseLock(lockId);
	print("------------------------------------------------------\n");


	print("DESTROYING LOCK SUCCESS CASE \n");
	lockId = createLock((char*)"LockName",10);
	destroyLock(lockId);
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

void testCase4()
{
	int lockId = -1;

	print("RELEASE LOCK SYSTEM CALL TEST\n\n");

	print("PASSING LOCK ID > MAX_LOCK to release \n\n");
	releaseLock(1000);
	print("------------------------------------------------------\n");

	print("PASSING LOCK ID < 0 to release \n\n");
	releaseLock(-1);
	print("------------------------------------------------------\n");

	print("TRYING TO release A LOCK NOT EVEN CREATED \n\n");
	releaseLock(0);
	print("------------------------------------------------------\n");

	print("RELEASE LOCK SUCCESS CASE \n");
	lockId = createLock((char*)"LockName",10);
	acquireLock(lockId);
	releaseLock(lockId);
	destroyLock(lockId);
	print("------------------------------------------------------\n");
}

int main()
{
	int testCaseNo;

	print("1 : CreateLock system call \n");
	print("2 : AcquireLock system call \n");
	print("3 : ReleaseLock system call \n");
	print("4 : DestroyLock system call \n");

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
	}
}


