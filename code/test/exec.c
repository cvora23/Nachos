/*
 * exec.c
 *
 *  Created on: Mar 27, 2013
 *      Author: cvora
 */

#include "syscall.h"

int lockId=-1;

int main()
{
	print("Execution Started\n");
	lockId = CreateLock((char *)"testlock",8);
	Acquire(lockId);
	Release(lockId);
	Exit(0);


}


