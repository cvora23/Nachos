/*
 * Assign1Prob2.cc
 *
 *  Created on: Mar 3, 2013
 *      Author: cvora
 */

#include "Assign1Prob2.h"

#define NO_OF_MANAGERS				1
#define NO_OF_CASHIERS				1
#define NO_OF_DEPARTMENT			5
#define NO_OF_SALESMAN				(NO_OF_DEPARTMENT*3)
#define NO_OF_GOOD_LOADERS			5
#define NO_OF_CUSTOMERS				30

#define space_conc(str1, str2) #str1 " " #str2

#define MANAGER_STRING				(const char*)"MANAGER"
#define CASHIER_STRING				(const char*)"CASHIER"
#define DEPARTMENT_STRING			(const char*)"DEPARTMENT"
#define SALESMAN_STRING				(const char*)"SALESMAN"
#define GOOD_LOADERS_STRING			(const char*)"GOOD LOADER"
#define CUSTOMER_STRING				(const char*)"CUSTOMER"

void CustomerThread(int ThreadId)
{
    DEBUG('p', "%s Started !!!!!!! \n",currentThread->name);

}

void GoodLoaderThread(int ThreadId)
{
    DEBUG('p', "%s Started !!!!!!! \n",currentThread->name);

}

void SalesmanThread(int ThreadId)
{
    DEBUG('p', "%s Started !!!!!!! \n",currentThread->name);

}

void CashierThread(int ThreadId)
{
    DEBUG('p', "%s Started !!!!!!! \n",currentThread->name);

}

void ManagerThread(int ThreadId)
{
    DEBUG('p', "%s Started !!!!!!! \n",currentThread->name);
}

void Problem2()
{
	Thread* t;
    DEBUG('p', "Entering Assign 1 Problem 2 !!!!!!! \n");
    DEBUG('p', "Starting to initialize all the Threads for Problem 2 !!!!!!! \n");

    DEBUG('p', "Starting Manager for Problem 2 !!!!!!! \n");

    for(int i = 0;i<NO_OF_MANAGERS;i++)
    {
    	t = new Thread(space_conc(MANAGER_STRING,i));
        t->Fork((VoidFunctionPtr)ManagerThread,i);
    }
    for(int i = 0;i<NO_OF_CASHIERS;i++)
    {
    	t = new Thread(space_conc(CASHIER_STRING,i));
        t->Fork((VoidFunctionPtr)CashierThread,i);
    }
    for(int i = 0;i<NO_OF_SALESMAN;i++)
    {
    	t = new Thread(space_conc(SALESMAN_STRING,i));
        t->Fork((VoidFunctionPtr)SalesmanThread,i);
    }
    for(int i = 0;i<GOOD_LOADERS_STRING;i++)
    {
    	t = new Thread(space_conc(GOOD_LOADERS_STRING,i));
        t->Fork((VoidFunctionPtr)GoodLoaderThread,i);
    }
    for(int i = 0;i<NO_OF_CUSTOMERS;i++)
    {
    	t = new Thread(space_conc(CUSTOMER_STRING,i));
        t->Fork((VoidFunctionPtr)CustomerThread,i);
    }
}
