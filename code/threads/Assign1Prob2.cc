/*
 * Assign1Prob2.cc
 *
 *  Created on: Mar 3, 2013
 *      Author: cvora
 */

#include "Assign1Prob2.h"

/**
 * GLOBAL DATA STRUCTURES
 */
ItemInfo g_ItemInfo[MAX_NO_OF_ITEMS];

CustomerInfo g_CustomerInfo[NO_OF_CUSTOMERS];

/**
 * GLOBAL DATA STRUCTURES INIT FUNCTIONS
 */
void initItemInfo()
{
	for (int i = 0;i<MAX_NO_OF_ITEMS;i++)
	{
		g_ItemInfo[i].Price = Random()%MAX_PRICE_PER_ITEM + 1;
		g_ItemInfo[i].departmentNo = Random()%NO_OF_DEPARTMENT;
		g_ItemInfo[i].shelfNo = Random()%NO_OF_SHELFS;
	}
}

static void initCustomerInfo()
{
	DEBUG('p',"CALLED ONCE \n");
	for(int i = 0;i<NO_OF_CUSTOMERS;i++)
	{
		g_CustomerInfo[i].money = Random()%MAX_AMT_PER_CUSTOMER + 1;
		g_CustomerInfo[i].type = CustomerType(Random()%NON_PRIVILEGED + 1);
		g_CustomerInfo[i].noOfItems = Random()%MAX_NO_ITEMS_TO_BE_PURCHASED + 1;
		g_CustomerInfo[i].pCustomerShoppingList = new CustomerShoppingList[g_CustomerInfo[i].noOfItems];
	}
}

static void initCustomerShoppingList()
{
	DEBUG('p',"CALLED ONCE \n");
	for(int i = 0;i<NO_OF_CUSTOMERS;i++)
	{
		for(int j =0;j<g_CustomerInfo[i].noOfItems;j++)
		{
			g_CustomerInfo[i].pCustomerShoppingList[j].itemNo = Random()%NO_OF_ITEM_TYPES;
			g_CustomerInfo[i].pCustomerShoppingList[j].noOfItems =
					Random()%MAX_NO_ITEMS_TO_BE_PURCHASED_OF_EACH_TYPE + 1;
		}
	}
}

void printCustomerInfo(int customerId)
{
	DEBUG('p',"Customer ID is %d\n",
			customerId);
	DEBUG('p',"Customer %d is of type %d \n",
			customerId,g_CustomerInfo[customerId].type);
	DEBUG('p',"Customer %d can spend %d amount on shopping \n",
			customerId,g_CustomerInfo[customerId].money);
	DEBUG('p',"Customer %d will purchase %d items today for shopping \n",
			customerId,g_CustomerInfo[customerId].noOfItems);
}

void printCustomerShoppingInfo(int customerId)
{
	DEBUG('p',"Customer %d shopping list is as follows : ",customerId);
	for(int j =0;j<g_CustomerInfo[customerId].noOfItems;j++)
	{
		DEBUG('p',"Item No: %d\n",
				g_CustomerInfo[j].pCustomerShoppingList[j].itemNo);
		DEBUG('p',"No of Items of Item Type:%d ===== %d\n",
				g_CustomerInfo[j].pCustomerShoppingList[j].itemNo,
				g_CustomerInfo[j].pCustomerShoppingList[j].noOfItems);
	}
}

void CustomerThread(int ThreadId)
{
    DEBUG('p', "%s Started !!!!!!! \n",currentThread->getName());
    initCustomerInfo();
    initCustomerShoppingList();
    initCustomerInfo();
    printCustomerInfo(ThreadId);
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

void startSimulation()
{
	Thread* t;
	RandomInit(NULL);

    DEBUG('p', "Entering Assign 1 Problem 2 !!!!!!! \n");
    DEBUG('p', "Starting to initialize all the Threads for Problem 2 !!!!!!! \n");

    DEBUG('p',"Number of Cashiers = %d \n",NO_OF_CASHIERS);
    DEBUG('p',"Number of Good Loaders = %d \n",NO_OF_GOOD_LOADERS);
    DEBUG('p',"Number of PrivilegedCustomers = %d \n",NO_OF_CUSTOMERS);
    DEBUG('p',"Number of Customers = %d \n",NO_OF_CUSTOMERS);
    DEBUG('p',"Number of Managers = %d \n",NO_OF_MANAGERS);
    DEBUG('p',"Number of DepartmentSalesmen = %d \n",NO_OF_SALESMAN);

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

void Problem2()
{
	startSimulation();
}
