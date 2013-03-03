/*
 * Assign1Prob2.cc
 *
 *  Created on: Mar 3, 2013
 *      Author: cvora
 */

#include "Assign1Prob2.h"

void CustomerThread(int ThreadId)
{
    DEBUG('p', "%s Started !!!!!!! \n",currentThread->getName());
}

void GoodLoaderThread(int ThreadId)
{
    DEBUG('p', "%s Started !!!!!!! \n",currentThread->getName());

}

void SalesmanThread(int ThreadId)
{
    DEBUG('p', "%s Started !!!!!!! \n",currentThread->getName());

}

void CashierThread(int ThreadId)
{
    DEBUG('p', "%s Started !!!!!!! \n",currentThread->getName());

}

void ManagerThread(int ThreadId)
{
    DEBUG('p', "%s Started !!!!!!! \n",currentThread->getName());
}

void Problem2()
{
	Thread* t;
    DEBUG('p', "Entering Assign 1 Problem 2 !!!!!!! \n");
    DEBUG('p', "Starting to initialize all the Threads for Problem 2 !!!!!!! \n");

    DEBUG('p', "Starting Manager for Problem 2 !!!!!!! \n");

    char *threadName;

    for(int i = 0;i<NO_OF_MANAGERS;i++)
    {
    	threadName = new char[50];
        sprintf (threadName, "%s_%d", MANAGER_STRING, i);
    	t = new Thread(threadName);
        t->Fork((VoidFunctionPtr)ManagerThread,i);
    }
    for(int i = 0;i<NO_OF_CASHIERS;i++)
    {
    	threadName = new char[50];
        sprintf (threadName, "%s_%d", CASHIER_STRING, i);
    	t = new Thread(threadName);
    	t->Fork((VoidFunctionPtr)CashierThread,i);
    }
    for(int i = 0;i<NO_OF_SALESMAN;i++)
    {
    	threadName = new char[50];
        sprintf (threadName, "%s_%d", SALESMAN_STRING, i);
    	t = new Thread(threadName);
    	t->Fork((VoidFunctionPtr)SalesmanThread,i);
    }
    for(int i = 0;i<NO_OF_GOOD_LOADERS;i++)
    {
    	threadName = new char[50];
        sprintf (threadName, "%s_%d", GOOD_LOADERS_STRING, i);
    	t = new Thread(threadName);
    	t->Fork((VoidFunctionPtr)GoodLoaderThread,i);
    }
    for(int i = 0;i<NO_OF_CUSTOMERS;i++)
    {
    	threadName = new char[50];
        sprintf (threadName, "%s_%d", CUSTOMER_STRING, i);
    	t = new Thread(threadName);
    	t->Fork((VoidFunctionPtr)CustomerThread,i);
    }
}
