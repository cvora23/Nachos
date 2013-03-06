/*
 * Assign1Prob2.cc
 *
 *  Created on: Mar 3, 2013
 *      Author: cvora
 */

#include "Assign1Prob2.h"
#include <stdio.h>
/**
 * GLOBAL DATA STRUCTURES
 */
ItemInfo g_itemInfo[NO_OF_ITEM_TYPES];

CustomerInfo g_customerInfo[NO_OF_CUSTOMERS];

SalesManInfo g_salesmanInfo[NO_OF_SALESMAN];

/**
 * Locks, Condition Variables, Queue Wait Count for Customer-Trolley interaction
 */
int 		g_usedTrolleyCount;
int 		g_waitForTrolleyCount;
Lock* 		g_customerTrolleyLock;
Condition* 	g_customerTrolleyCV;

/**
 * Locks, Condition Variables for Customer-Salesman interaction
 */
Lock*		g_customerSalesmanLock[NO_OF_SALESMAN];
Condition*  g_customerSalesmanCV[NO_OF_SALESMAN];

/**
 * Locks,Condition Variables and Queue Length for Customer-Department Interaction
 */
Lock*		g_customerDepartmentLock[NO_OF_DEPARTMENT];
Condition*	g_customerDepartmentCV[NO_OF_DEPARTMENT];
int			g_departmentWaitQueue[NO_OF_DEPARTMENT];

/**
 * GLOBAL DATA STRUCTURES INIT FUNCTIONS
 */
static void initItemInfo()
{
    static bool firstCall = true;
    if(firstCall)
    {
		for (int i = 0;i<NO_OF_ITEM_TYPES;i++)
		{
			g_itemInfo[i].Price = Random()%MAX_PRICE_PER_ITEM + 1;
			g_itemInfo[i].departmentNo = (int)(i/NO_OF_ITEMS_PER_DEPARTMENT);
			g_itemInfo[i].shelfNo = i;
			g_itemInfo[i].noOfItems = Random()%MAX_NO_ITEMS_PER_SHELF + 1;
		}
		firstCall = false;
    }
}

void printItemInfo()
{
	for (int i = 0;i<NO_OF_ITEM_TYPES;i++)
	{
		DEBUG('p',"Item id is %d \n",i);
		DEBUG('p',"Item %d Price is %d \n",i,g_itemInfo[i].Price);
		DEBUG('p',"Item %d is in Department %d \n",i,g_itemInfo[i].departmentNo);
		DEBUG('p',"Item %d is in Shelf %d \n",i,g_itemInfo[i].shelfNo);
		DEBUG('p',"Item %d Total Stock no = %d \n",i,g_itemInfo[i].noOfItems);
	}
}

static void initCustomerInfo()
{
    static bool firstCall = true;
    if(firstCall)
    {
    	DEBUG('p',"CALLED ONCE \n");
		for(int i = 0;i<NO_OF_CUSTOMERS;i++)
		{
			g_customerInfo[i].money = Random()%MAX_AMT_PER_CUSTOMER + 1;
			g_customerInfo[i].type = CustomerType(Random()%2);
			g_customerInfo[i].noOfItems = Random()%MAX_NO_ITEMS_TO_BE_PURCHASED + 1;
			g_customerInfo[i].pCustomerShoppingList = new CustomerShoppingList[g_customerInfo[i].noOfItems];
		}
		firstCall = false;
    }
}

static void initCustomerShoppingList()
{
    static bool firstCall = true;
    if(firstCall)
    {
    	for(int i = 0;i<NO_OF_CUSTOMERS;i++)
    	{
			for(int j =0;j<g_customerInfo[i].noOfItems;j++)
			{
				g_customerInfo[i].pCustomerShoppingList[j].itemNo = Random()%NO_OF_ITEM_TYPES;
				g_customerInfo[i].pCustomerShoppingList[j].noOfItems =
						Random()%MAX_NO_ITEMS_TO_BE_PURCHASED_OF_EACH_TYPE + 1;
			}
    	}
		firstCall = false;
	}
}

void printCustomerInfo(int customerId)
{
	DEBUG('p',"Customer ID is %d\n",
			customerId);
	DEBUG('p',"Customer %d is of type %d \n",
			customerId,g_customerInfo[customerId].type);
	DEBUG('p',"Customer %d can spend %d amount on shopping \n",
			customerId,g_customerInfo[customerId].money);
	DEBUG('p',"Customer %d will purchase %d items today for shopping \n",
			customerId,g_customerInfo[customerId].noOfItems);
	printCustomerShoppingList(customerId);
}

void printCustomerShoppingList(int customerId)
{
	DEBUG('p',"Customer %d shopping list is as follows : \n",customerId);
	for(int j =0;j<g_customerInfo[customerId].noOfItems;j++)
	{
		DEBUG('p',"Item No: %d\n",
				g_customerInfo[customerId].pCustomerShoppingList[j].itemNo);
		DEBUG('p',"No of Items of Item Type:%d ===== %d\n",
				g_customerInfo[customerId].pCustomerShoppingList[j].itemNo,
				g_customerInfo[customerId].pCustomerShoppingList[j].noOfItems);
	}
}

void printCustomerShoppingListToFile(int customerId)
{
	FILE *file;
	file = fopen("customerShoppingList.txt","+a");
	if(file != NULL)
	{
		fprintf(file,"Customer %d shopping list is as follows : \n",customerId);
		for(int j =0;j<g_customerInfo[customerId].noOfItems;j++)
		{
			fprintf(file,"Item No: %d\n",
					g_customerInfo[customerId].pCustomerShoppingList[j].itemNo);
			fprintf(file,"No of Items of Item Type:%d ===== %d\n",
					g_customerInfo[customerId].pCustomerShoppingList[j].itemNo,
					g_customerInfo[customerId].pCustomerShoppingList[j].noOfItems);
		}
		fclose(file);
	}
	else
	{
		DEBUG('p',"File Open Error \n");
	}
}

void initSalesManInfo()
{
    static bool firstCall = true;
    if(firstCall)
    {
    	for(int i=0;i<NO_OF_SALESMAN;i++)
    	{
    		g_salesmanInfo[i].customerId = -1;
    		g_salesmanInfo[i].status = isBusy;
    		g_salesmanInfo[i].departmentNo = (int)(i/NO_OF_SALESMAN_PER_DEPARTMENT);
    		g_salesmanInfo[i].isComplainedAboutMissingItem = false;
    	}
		firstCall = false;
	}
}

void printSalesManInfo(int salesManId)
{

}

void CustomerThread(int ThreadId)
{
    DEBUG('p', "%s enters the SuperMarket !!!!!!! \n",currentThread->getName());

    //printCustomerInfo(ThreadId);
    printCustomerShoppingListToFile(ThreadId);
    /**
     * Local Variable for Customer Thread.
     */
    int currentItemNoFromShoppingList;
    int currentDepartmentNoForItem;
    int salesManStartForDepartment;
    int salesManEndForDepartment;
    int mySalesMan = -1;

    /*************************************CUSTOMER-TROLLEY INTERACTION STARTS HERE*********************************************/

    /**
     * Starting to get in line to get a shopping trolley
     */
    g_customerTrolleyLock->Acquire();
    DEBUG('p',"%s gets in line for a trolley\n",currentThread->getName());
    if(g_usedTrolleyCount<MAX_NO_OF_TROLLEYS)
    {
    	g_usedTrolleyCount++;
    }
    else
    {
    	g_waitForTrolleyCount++;
    	g_customerTrolleyCV->Wait(g_customerTrolleyLock);
    	g_waitForTrolleyCount--;
    	g_usedTrolleyCount++;
    }
    DEBUG('p',"%s has a trolley for shopping\n",currentThread->getName());
    g_customerTrolleyLock->Release();

    /*************************************CUSTOMER-TROLLEY INTERACTION ENDS HERE*********************************************/

    /*************************************CUSTOMER-SALESMAN INTERACTION STARTS HERE*********************************************/


    /**
     * Customer will start the shopping now.
     */
    for(int i=0;i<g_customerInfo[ThreadId].noOfItems;i++)
    {
    	/**
    	 * Customer will now find Department for particular item no
    	 */
    	DEBUG('p',"%s wants to shop Item %d in Department %d \n",
    			currentThread->getName(),
    			g_customerInfo[ThreadId].pCustomerShoppingList[i].itemNo,
    			g_itemInfo[g_customerInfo[ThreadId].pCustomerShoppingList[i].itemNo].departmentNo);

    	currentItemNoFromShoppingList = g_customerInfo[ThreadId].pCustomerShoppingList[i].itemNo;
    	currentDepartmentNoForItem =
    			g_itemInfo[g_customerInfo[ThreadId].pCustomerShoppingList[i].itemNo].departmentNo;
    	salesManStartForDepartment = currentDepartmentNoForItem*NO_OF_SALESMAN_PER_DEPARTMENT;
    	salesManEndForDepartment = salesManStartForDepartment + NO_OF_SALESMAN_PER_DEPARTMENT;
    	/**
    	 * Customer will now start interacting with Department Salesman for the item
    	 */

    	for(int salesmanIndex=salesManStartForDepartment;
    			salesmanIndex<salesManEndForDepartment;salesmanIndex++)
    	{
    		/**
    		 * First Check if salesman for department is free.
    		 * IF YES:
    		 * 1: Change his status to Busy
    		 * 2: Signal Him Assuming He is Waiting
    		 * 3: Wait For a Greeting From Him
    		 * 4: Customer will then start Interacting with Salesman
    		 */

    		g_customerSalesmanLock[salesmanIndex]->Acquire();
    		if(g_salesmanInfo[salesmanIndex].status == isFree)
    		{
    			mySalesMan = salesmanIndex;
    			g_salesmanInfo[salesmanIndex].status = isBusy;
    			g_salesmanInfo[salesmanIndex].customerId = ThreadId;
    			break;
    		}
    		g_customerSalesmanLock[salesmanIndex]->Release();
    	}

    	if(mySalesMan == -1)
    	{
    		/**
    		 * First Check if salesman for department is free.
    		 * IF NO:
       		 * 5: Wait in the line for particular department.
       		 * 6: For a particular department line, whichever Salesman is free
       		 * 	  will signal the customer
       		 * 7: Check which Salesman signaled you and start interacting with him.
       		 * 8: Get out the line now.
       		 *
       		 * 	  Customer will then start interacting with the Salesman
       		 */
    		g_customerDepartmentLock[currentDepartmentNoForItem]->Acquire();
    		g_departmentWaitQueue[currentDepartmentNoForItem]++;
    		DEBUG('p',"%s gets in line for the Department %d \n",currentThread->getName(),
    				currentDepartmentNoForItem);
    		g_customerDepartmentCV[currentDepartmentNoForItem]->
    		Wait(g_customerDepartmentLock[currentDepartmentNoForItem]);

        	for(int salesmanIndex=salesManStartForDepartment;
        			salesmanIndex<salesManEndForDepartment;salesmanIndex++)
        	{
        		/**
        		 * \todo: ASSUMPTION HERE THAT FIRST SALESMAN WHO SIGNALED WILL BE OUR
        		 * SALESMAN (Will Work For Now)
        		 */
    			g_customerSalesmanLock[salesmanIndex]->Acquire();
        		if(g_salesmanInfo[salesmanIndex].status == salesmanSignalToCustomer)
        		{
        			mySalesMan = salesmanIndex;
        			g_salesmanInfo[salesmanIndex].status = isBusy;
        			break;
        		}
        		g_customerSalesmanLock[salesmanIndex]->Release();
        	}
        	g_customerDepartmentLock[currentDepartmentNoForItem]->Release();
    	}

    	g_customerSalesmanCV[mySalesMan]->Signal(g_customerSalesmanLock[mySalesMan]);
    	g_customerSalesmanCV[mySalesMan]->Wait(g_customerSalesmanLock[mySalesMan]);
    	g_customerSalesmanCV[mySalesMan]->Signal(g_customerSalesmanLock[mySalesMan]);

    	g_customerSalesmanLock[mySalesMan]->Release();

		DEBUG('p',"%s is interacting with DepartmentSalesman %d from department %d \n",
				currentThread->getName(),mySalesMan,currentDepartmentNoForItem);

		/**
		 * Customer started interaction with Department Salesman and will now
		 * start shopping from his shopping list.
		 */

        /*************************************CUSTOMER-SALESMAN INTERACTION ENDS HERE*********************************************/
    }
}

void SalesmanThread(int ThreadId)
{
    DEBUG('p', "%s Started !!!!!!! \n",currentThread->getName());
    DEBUG('p',"%s will be working for Department %d \n",
    		currentThread->getName(),g_salesmanInfo[ThreadId].departmentNo);

    int myDepartmentNo = g_salesmanInfo[ThreadId].departmentNo;

    while(1)
    {
    	/**
    	 * Check to see if there is some one in department line
    	 * If YES:
    	 * 1:	Signal to that customer
    	 */
    	g_customerDepartmentLock[myDepartmentNo]->Acquire();

    	if(g_departmentWaitQueue[myDepartmentNo]>0)
    	{
    		g_departmentWaitQueue[myDepartmentNo]--;
    		/**
    		 * Very Important to acquire the g_customerSalesmanLock before signaling the
    		 * customer waiting on  g_customerDepartmentCV with g_customerDepartmentLock because:
    		 * In case we signal on  g_customerDepartmentCV with g_customerDepartmentLock and release
    		 * the g_customerDepartmentLock there is a chance of CS and customer acquiring the
    		 * g_customerSalesmanLock and then salesman and customer waiting for signals from each other
    		 * in other words LiveLock Situation
    		 */
    		g_customerSalesmanLock[ThreadId]->Acquire();
    		g_salesmanInfo[ThreadId].status = salesmanSignalToCustomer;

    		g_customerDepartmentCV[myDepartmentNo]->Signal(g_customerDepartmentLock[myDepartmentNo]);
    		g_customerDepartmentLock[myDepartmentNo]->Release();

    		g_customerSalesmanCV[ThreadId]->Wait(g_customerSalesmanLock[ThreadId]);
    		g_customerSalesmanCV[ThreadId]->Signal(g_customerSalesmanLock[ThreadId]);

    		DEBUG('p',"%s welcomes Customer %d to Department %d \n",
    				currentThread->getName(),g_salesmanInfo[ThreadId].customerId,myDepartmentNo);

    		g_customerSalesmanCV[ThreadId]->Wait(g_customerSalesmanLock[ThreadId]);
    		g_customerSalesmanLock[ThreadId]->Release();

    	}
        else
        {
        	g_customerDepartmentLock[myDepartmentNo]->Release();
        }

    	g_customerDepartmentLock[myDepartmentNo]->Acquire();

    	if(g_departmentWaitQueue[myDepartmentNo] == 0)
    	{
    		g_customerSalesmanLock[ThreadId]->Acquire();
    		g_salesmanInfo[ThreadId].status = isFree;

    		g_customerDepartmentLock[myDepartmentNo]->Release();

    		g_customerSalesmanCV[ThreadId]->Wait(g_customerSalesmanLock[ThreadId]);

    		if(g_salesmanInfo[ThreadId].isComplainedAboutMissingItem == false)
    		{
    			g_salesmanInfo[ThreadId].status = salesmanSignalToCustomer;
    			g_customerSalesmanCV[ThreadId]->Signal(g_customerSalesmanLock[ThreadId]);

        		DEBUG('p',"%s welcomes Customer %d to Department %d \n",
        				currentThread->getName(),g_salesmanInfo[ThreadId].customerId,myDepartmentNo);

    			g_customerSalesmanCV[ThreadId]->Wait(g_customerSalesmanLock[ThreadId]);
        		g_customerSalesmanLock[ThreadId]->Release();
    		}
    		else
    		{
    			/**
    			 * Nothing for now
    			 */
    		}
    	}
    	else
    	{
    		g_customerDepartmentLock[myDepartmentNo]->Release();
    	}
    }



}

void GoodLoaderThread(int ThreadId)
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

void initLockCvForSimulation()
{
	g_usedTrolleyCount = 0;
	g_waitForTrolleyCount = 0;

	g_customerTrolleyLock = new Lock((char*)CUSTOMERTROLLEYLOCK_STRING);
	g_customerTrolleyCV =new Condition((char*)CUSTOMERTROLLEYCV_STRING);

	char* lockName;
	char* cvName;

	for(int i=0;i<NO_OF_SALESMAN;i++)
	{
		lockName = new char[50];
        sprintf (lockName, "%s_%d", CUSTOMERSALESMANLOCK_STRING, i);
        g_customerSalesmanLock[i] = new Lock(lockName);

        cvName = new char[50];
        sprintf (cvName, "%s_%d", CUSTOMERSALESMANCV_STRING, i);
        g_customerSalesmanCV[i] = new Condition(cvName);
	}

	for(int i=0;i<NO_OF_DEPARTMENT;i++)
	{
		lockName = new char[50];
        sprintf (lockName, "%s_%d", CUSTOMERDEPARTMENTLOCK_STRING, i);
        g_customerDepartmentLock[i] = new Lock(lockName);

        cvName = new char[50];
        sprintf (cvName, "%s_%d", CUSTOMERDEPARTMENTCV_STRING, i);
        g_customerDepartmentCV[i] = new Condition(cvName);

        g_departmentWaitQueue[i] = 0;
	}

}

void startSimulation()
{
	Thread* t;
	RandomInit(time(NULL));

    DEBUG('p', "Entering Assign 1 Problem 2 !!!!!!! \n");
    DEBUG('p', "Starting to initialize all the Threads for Problem 2 !!!!!!! \n");

    DEBUG('p',"Number of Cashiers = %d \n",NO_OF_CASHIERS);
    DEBUG('p',"Number of Good Loaders = %d \n",NO_OF_GOOD_LOADERS);
    DEBUG('p',"Number of PrivilegedCustomers = %d \n",NO_OF_CUSTOMERS);
    DEBUG('p',"Number of Customers = %d \n",NO_OF_CUSTOMERS);
    DEBUG('p',"Number of Managers = %d \n",NO_OF_MANAGERS);
    DEBUG('p',"Number of DepartmentSalesmen = %d \n",NO_OF_SALESMAN);

    initItemInfo();
    //printItemInfo();

    initCustomerInfo();
    initCustomerShoppingList();

    initSalesManInfo();

    initLockCvForSimulation();

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
