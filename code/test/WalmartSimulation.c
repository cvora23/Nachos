/*
 * WalmartSimulation.c
 *
 *  Created on: Mar 28, 2013
 *      Author: cvora
 */

#include "syscall.h"

/**
 * Declare the Defines here
 */

/*************************************DEFINES DECLARATION**********************************************/

#ifndef NO_OF_DEPARTMENT
#define NO_OF_DEPARTMENT								5
#endif

#define NO_OF_SHELFS									(NO_OF_DEPARTMENT*3)
#define NO_OF_ITEM_TYPES								NO_OF_SHELFS
#define NO_OF_ITEMS_PER_DEPARTMENT						(NO_OF_ITEM_TYPES/NO_OF_DEPARTMENT)
#define MAX_NO_ITEMS_TO_BE_PURCHASED					10
#define MAX_NO_ITEMS_TO_BE_PURCHASED_OF_EACH_TYPE 		5
#define MAX_AMT_PER_CUSTOMER							1000
#define MAX_PRICE_PER_ITEM								200
#define MAX_NO_ITEMS_PER_SHELF							1000
#define MAX_NO_OF_TROLLEYS								100
#define NO_OF_GOODLOADER_WAIT_QUEUE						1
#define MIN_GOODS_LOADER_WALKINGTIME					20
#define MAX_GOODS_LOADER_WALKINGTIME					40
#define MAX_CASHIER_LINE_LENGTH							9999
#define MANAGER_RANDOM_SLEEP_TIME						1000

#define NO_OF_ITEMS_TO_SHOP								5

/**
 * STORE MANAGEMENT INFO
 */
#ifndef NO_OF_MANAGERS
#define NO_OF_MANAGERS									1
#endif

#ifndef NO_OF_CASHIERS
#define NO_OF_CASHIERS									5
#endif

#define NO_OF_SALESMAN									(NO_OF_DEPARTMENT*3)
#define NO_OF_SALESMAN_PER_DEPARTMENT					(NO_OF_SALESMAN/NO_OF_DEPARTMENT)

#ifndef NO_OF_GOOD_LOADERS
#define NO_OF_GOOD_LOADERS								5
#endif

#ifndef NO_OF_CUSTOMERS
#define NO_OF_CUSTOMERS									30
#endif

/*************************************DEFINES DECLARATION**********************************************/

/*************************************GLOBAL DATA STRUCTS DEFINITION**********************************************/

typedef enum _bool
{
	false = 0,
	true
}bool;

typedef struct _ItemInfo
{
	int 	Price;
	int 	shelfNo;
	int 	departmentNo;
	int		noOfItems;
}ItemInfo;


typedef struct _CustomerShoppingList
{
	int 	 itemNo;
	int		 noOfItems;
}CustomerShoppingList;

typedef enum _CustomerType
{
	PRIVILEGED = 0,
	NON_PRIVILEGED
}CustomerType;

typedef struct _CustomerInfo
{
	CustomerType 			type;
	int 					money;
	int						noOfItems;
	bool					hasEnoughMoneyForShopping;
	bool					isDoneShopping;
	CustomerShoppingList	pCustomerShoppingList[NO_OF_ITEMS_TO_SHOP];
}CustomerInfo;


typedef enum _SalesManStatus
{
	salesmanIsFree = 0,
	salesmanIsBusy,
	salesmanSignalToCustomerFromDeptWaitQueue,
	salesmanSignalToCustomerFromDeptComplainWaitQueue
}SalesManStatus;

typedef struct _SalesManInfo
{
	SalesManStatus status;
	int	 departmentNo;
	int customerId;
	int itemToRestock;
}SalesManInfo;

typedef enum _GoodLoaderStatus
{
	goodLoaderIsFree = 0,
	goodLoaderIsBusy,
	goodLoaderSignalToSalesman
}GoodLoaderStatus;

typedef struct _GoodLoaderInfo
{
	GoodLoaderStatus status;
	int salesmanId;
	int itemToRestock;
}GoodLoaderInfo;

typedef enum _CashierStatus
{
	cashierIsFree = 0,
	cashierIsBusy,
	cashierSignalToCustomer
}CashierStatus;

typedef struct _CashierInfo
{
	CashierStatus	status;
	int 			customerId;
	int 			bill;
	int 			totalSalesMoney;
}CashierInfo;

typedef struct _ManagerInfo
{
	int 			cashierId;
	int 			customerId;
	int 			customerBill;
	int				totalRevenue;
}ManagerInfo;

/*************************************GLOBAL DATA STRUCTS DEFINITION**********************************************/

ItemInfo 		g_itemInfo[NO_OF_ITEM_TYPES];

CustomerInfo 	g_customerInfo[NO_OF_CUSTOMERS];

SalesManInfo 	g_salesmanInfo[NO_OF_SALESMAN];

GoodLoaderInfo 	g_goodLoaderInfo[NO_OF_GOOD_LOADERS];

CashierInfo		g_cashierInfo[NO_OF_CASHIERS];

ManagerInfo		g_managerInfo;

int g_noOfCustomersLeft = 0;
int g_customerListLeft[NO_OF_CUSTOMERS];
int simulationEnd = 0;

int g_customerThreadCounter = 0;
int g_salesmanThreadCounter = 0;
int g_goodsLoaderThreadCounter = 0;
int g_cashierThreadCounter = 0;

/*****************************************LOCKS**************************************************************/
unsigned int printLock;

unsigned int g_customersLeftLock;

unsigned int g_customerThreadCounterLock;
unsigned int g_salesmanThreadCounterLock;
unsigned int g_goodsLoaderThreadCounterLock;
unsigned int g_cashierThreadCounterLock;

unsigned int g_customerTrolleyLock;

unsigned int g_managerCashierLock;
unsigned int g_managerCashierInteractionLock;
unsigned int g_managerCustomerInteractionLock;

unsigned int g_cashierLineLock[NO_OF_CASHIERS];
unsigned int g_cashierPrivilegedLineLock[NO_OF_CASHIERS];
unsigned int g_customerCashierLock[NO_OF_CASHIERS];
unsigned int g_managerCashierCashLock[NO_OF_CASHIERS];

unsigned int g_customerSalesmanLock[NO_OF_SALESMAN];

unsigned int g_customerDepartmentLock[NO_OF_DEPARTMENT];
unsigned int g_customerDepartmentComplainLock[NO_OF_DEPARTMENT];

unsigned int g_shelfAccessLock[NO_OF_SHELFS];

unsigned int g_salesmanGoodsLoaderLock[NO_OF_GOOD_LOADERS];
unsigned int g_goodLoaderWaitLock[NO_OF_GOODLOADER_WAIT_QUEUE];

/*****************************************LOCKS**************************************************************/

/*****************************************CVS**************************************************************/

unsigned int g_customerTrolleyCV;

unsigned int g_managerCashierCV;
unsigned int g_managerCashierInteractionCV;
unsigned int g_managerCustomerInteractionCV;

unsigned int g_cashierLineCV[NO_OF_CASHIERS];
unsigned int g_cashierPrivilegedLineCV[NO_OF_CASHIERS];
unsigned int g_customerCashierCV[NO_OF_CASHIERS];

unsigned int g_customerSalesmanCV[NO_OF_SALESMAN];

unsigned int g_customerDepartmentCV[NO_OF_DEPARTMENT];
unsigned int g_customerDepartmentComplainCV[NO_OF_DEPARTMENT];

unsigned int g_salesmanGoodsLoaderCV[NO_OF_GOOD_LOADERS];

unsigned int g_goodLoaderWaitCV[NO_OF_GOODLOADER_WAIT_QUEUE];

/*****************************************CVS**************************************************************/

/*****************************************WAIT Q's**************************************************************/


int	g_cashierWaitQueue[NO_OF_CASHIERS];
int	g_cashierPrivilegedWaitQueue[NO_OF_CASHIERS];

int	g_departmentWaitQueue[NO_OF_DEPARTMENT];
int	g_departmentComplainWaitQueue[NO_OF_DEPARTMENT];

int	g_goodLoaderWaitQueue[NO_OF_GOODLOADER_WAIT_QUEUE];

/*****************************************WAIT Q's**************************************************************/
int g_usedTrolleyCount = 0;
int g_waitForTrolleyCount = 0;
int	g_managerWaitQueueLength = 0;

/*************************************GLOBAL DATA STRUCTS DECLARATION**********************************************/

/********************************************PROJ1 PROBLEM SIMULATION***********************************************/

void printItemInfo()
{
	int i;
	for (i = 0;i<NO_OF_ITEM_TYPES;i++)
	{
		Print1("Item id is = %d \n",i);
		Print2("Item %d Price is = %d \n",i,g_itemInfo[i].Price);
		Print2("Item %d is in Department = %d \n",i,g_itemInfo[i].departmentNo);
		Print2("Item %d is in Shelf = %d \n",i,g_itemInfo[i].shelfNo);
		Print2("Item %d Total Stock no = %d \n",i,g_itemInfo[i].noOfItems);
	}
}

void printCustomerShoppingList(int customerId)
{
	int j;
	Print1("Customer %d shopping list is as follows : \n",customerId);
	for(j=0;j<g_customerInfo[customerId].noOfItems;j++)
	{
		Print1("Item Type: %d\n",
				g_customerInfo[customerId].pCustomerShoppingList[j].itemNo);
		Print2("No of Items of Item Type:%d ===== %d\n",
				g_customerInfo[customerId].pCustomerShoppingList[j].itemNo,
				g_customerInfo[customerId].pCustomerShoppingList[j].noOfItems);
	}
}

void printCustomerInfo()
{
	int customerId;
	for(customerId=0;customerId<NO_OF_CUSTOMERS;customerId++)
	{
		Print1("Customer ID is %d\n",
				customerId);
		Print2("Customer %d is of type %d \n",
				customerId,g_customerInfo[customerId].type);
		Print2("Customer %d can spend %d amount on shopping \n",
				customerId,g_customerInfo[customerId].money);
		Print2("Customer %d will purchase %d items today for shopping \n",
				customerId,g_customerInfo[customerId].noOfItems);
		printCustomerShoppingList(customerId);
	}
}

void printConfiguration()
{
	printItemInfo();
	printCustomerInfo();
}

void initItemInfo()
{
    int i;
	for (i = 0;i<NO_OF_ITEM_TYPES;i++)
	{
		g_itemInfo[i].Price = GetRand()%MAX_PRICE_PER_ITEM + 1;
		g_itemInfo[i].departmentNo = (int)(i/NO_OF_ITEMS_PER_DEPARTMENT);
		g_itemInfo[i].shelfNo = i;
		g_itemInfo[i].noOfItems = GetRand()%MAX_NO_ITEMS_PER_SHELF + 1;
	}
}

void initCustomerInfo()
{
    int i,j;
	for(i = 0;i<NO_OF_CUSTOMERS;i++)
	{
		g_customerInfo[i].money = GetRand()%MAX_AMT_PER_CUSTOMER + 1;
		if(GetRand()%2 == 0)
		{
			g_customerInfo[i].type = PRIVILEGED;
		}
		else
		{
			g_customerInfo[i].type = NON_PRIVILEGED;
		}
		g_customerInfo[i].noOfItems = NO_OF_ITEMS_TO_SHOP;
		for(j=0;j<g_customerInfo[i].noOfItems;j++)
		{
			g_customerInfo[i].pCustomerShoppingList[j].itemNo = 0;
			g_customerInfo[i].pCustomerShoppingList[j].noOfItems = 0;
		}

		g_customerInfo[i].hasEnoughMoneyForShopping = false;
		g_customerInfo[i].isDoneShopping = false;
	}
}

void initCustomerShoppingList()
{
    int i,j;
	for(i = 0;i<NO_OF_CUSTOMERS;i++)
	{
		for(j =0;j<g_customerInfo[i].noOfItems;j++)
		{
			g_customerInfo[i].pCustomerShoppingList[j].itemNo = GetRand()%NO_OF_ITEM_TYPES;
			g_customerInfo[i].pCustomerShoppingList[j].noOfItems =
					GetRand()%MAX_NO_ITEMS_TO_BE_PURCHASED_OF_EACH_TYPE + 1;
		}
	}

}

void initSalesManInfo()
{
	int i;
	for(i=0;i<NO_OF_SALESMAN;i++)
	{
		g_salesmanInfo[i].customerId = -1;
		g_salesmanInfo[i].status = salesmanIsBusy;
		g_salesmanInfo[i].departmentNo = (int)(i/NO_OF_SALESMAN_PER_DEPARTMENT);
		g_salesmanInfo[i].itemToRestock = -1;
	}
}

void printSalesManInfo(int salesManId)
{

}

void initGoodLoaderInfo()
{
    int i;

	for(i=0;i<NO_OF_GOOD_LOADERS;i++)
	{
		g_goodLoaderInfo[i].status = goodLoaderIsBusy;
		g_goodLoaderInfo[i].salesmanId = -1;
		g_goodLoaderInfo[i].itemToRestock = -1;
	}

}

void printGoodLoaderInfo(int goodLoaderId)
{

}

void initCashierInfo()
{
    int i;
	for(i=0;i<NO_OF_CASHIERS;i++)
	{
		g_cashierInfo[i].status = cashierIsBusy;
		g_cashierInfo[i].customerId = -1;
		g_cashierInfo[i].bill = -1;
		g_cashierInfo[i].totalSalesMoney = 0;
	}

}

void printCashierInfo()
{
	int i;
	for(i=0;i<NO_OF_CASHIERS;i++)
	{
		Print2("Cashier %d Status is %d \n",i,g_cashierInfo[i].status);
		Print2("Cashier %d Customer Id is %d \n",i,g_cashierInfo[i].customerId);
		Print2("Cashier %d bill is %d \n",i,g_cashierInfo[i].bill);
		Print2("Cashier %d total sales money is %d \n",i,g_cashierInfo[i].totalSalesMoney);
	}
}

void initManagerInfo()
{
    int i;

    	for(i=0;i<NO_OF_MANAGERS;i++)
    	{
    		g_managerInfo.cashierId = -1;
    		g_managerInfo.customerId = -1;
    		g_managerInfo.totalRevenue = 0;
    		g_managerInfo.customerBill = 0;
    	}

}

void printManagerInfo(int managerId)
{

}

void printCustomerLeftList()
{
	int i;
	Print("CUTOMER WHO HAS NOT LEFT YET ARE AS FOLLOWS: \n");
	for(i=0;i<NO_OF_CUSTOMERS;i++)
	{
		if(g_customerListLeft[i] == 0)
		{
			Print1("%d \n",g_customerListLeft[i]);
		}
	}
	Print1("NO OF CUSTOMERS LEFT SO FAR ARE %d \n",g_noOfCustomersLeft);
}

void CustomerThread()
{

    /**
     * Local Variable for Customer Thread.
     */
    int ThreadId;
    int currentItemNoFromShoppingList;
    int currentItemNoCountFromShoppingList;
    int currentDepartmentNoForItem;
    int salesManStartForDepartment;
    int salesManEndForDepartment;
    int mySalesMan = -1;
    int myBill = 0;
    int myCashier = -1;
    int minCashierLineLength = MAX_CASHIER_LINE_LENGTH;
	int currentItemNoCountToRemoveFromTrolley = 0;
	int i;
	int salesmanIndex;

	Acquire(g_customerThreadCounterLock);
	ThreadId = g_customerThreadCounter;
	g_customerThreadCounter++;
	Release(g_customerThreadCounterLock);

	Acquire(printLock);
    Print1("CUSTOMER %d enters the SuperMarket !!!!!!! \n",ThreadId);
    Release(printLock);

#if 1


	    /*************************************CUSTOMER-TROLLEY INTERACTION STARTS HERE*********************************************/

    Acquire(g_customerTrolleyLock);
	Acquire(printLock);
    Print1("CUSTOMER %d gets in line for a trolley\n",ThreadId);
    Release(printLock);
    if(g_usedTrolleyCount<MAX_NO_OF_TROLLEYS)
    {
    	g_usedTrolleyCount++;
    }
    else
    {
    	g_waitForTrolleyCount++;
    	Wait(g_customerTrolleyCV,g_customerTrolleyLock);
    	g_waitForTrolleyCount--;
    	g_usedTrolleyCount++;
    }
	Acquire(printLock);
    Print1("CUSTOMER %d has a trolley for shopping\n",ThreadId);
    Release(printLock);
    Release(g_customerTrolleyLock);

    /*************************************CUSTOMER-TROLLEY INTERACTION ENDS HERE*********************************************/

    /*************************************CUSTOMER-SALESMAN INTERACTION STARTS HERE*********************************************/

    for(i=0;i<g_customerInfo[ThreadId].noOfItems;i++)
    {
    	Acquire(printLock);
    	Print3("CUSTOMER %d wants to shop ITEM_%d in DEPARTMENT_%d \n",
    			ThreadId,
    			g_customerInfo[ThreadId].pCustomerShoppingList[i].itemNo,
    			g_itemInfo[g_customerInfo[ThreadId].pCustomerShoppingList[i].itemNo].departmentNo);
    	Release(printLock);
    	currentItemNoFromShoppingList = g_customerInfo[ThreadId].pCustomerShoppingList[i].itemNo;
      	currentItemNoCountFromShoppingList = g_customerInfo[ThreadId].pCustomerShoppingList[i].noOfItems;

    	currentDepartmentNoForItem =
    			g_itemInfo[g_customerInfo[ThreadId].pCustomerShoppingList[i].itemNo].departmentNo;


    	salesManStartForDepartment = currentDepartmentNoForItem*NO_OF_SALESMAN_PER_DEPARTMENT;
    	salesManEndForDepartment = salesManStartForDepartment + NO_OF_SALESMAN_PER_DEPARTMENT;

    	mySalesMan = -1;

    	for(salesmanIndex=salesManStartForDepartment;
    			salesmanIndex<salesManEndForDepartment;salesmanIndex++)
    	{
    		Acquire(g_customerSalesmanLock[salesmanIndex]);
    		if(g_salesmanInfo[salesmanIndex].status == salesmanIsFree)
    		{
    			mySalesMan = salesmanIndex;
    			g_salesmanInfo[salesmanIndex].status = salesmanIsBusy;
    			g_salesmanInfo[salesmanIndex].customerId = ThreadId;
    			Acquire(printLock);
    			Print3("CUSTOMER %d will enquire about ITEM_%d to SALESMAN_%d\n",ThreadId,
    					currentItemNoFromShoppingList,mySalesMan);
    			Release(printLock);
    			break;
    		}
    		Release(g_customerSalesmanLock[salesmanIndex]);
    	}

    	if(mySalesMan == -1)
    	{
    		Acquire(printLock);
    		Print3("CUSTOMER %d is waiting in Line for DEPARTMENT_%d as NO SALESMAN is free for ITEM_%d inquiry \n",
    				ThreadId,
    				currentDepartmentNoForItem,
    				currentItemNoFromShoppingList);
    		Release(printLock);
    		Acquire(g_customerDepartmentLock[currentDepartmentNoForItem]);

    		g_departmentWaitQueue[currentDepartmentNoForItem]++;
    		Acquire(printLock);
    		Print2("CUSTOMER %d gets in line for the DEPARTMENT_%d \n",ThreadId,
    				currentDepartmentNoForItem);
    		Release(printLock);
    		Wait(g_customerDepartmentCV[currentDepartmentNoForItem],g_customerDepartmentLock[currentDepartmentNoForItem]);

        	for(salesmanIndex=salesManStartForDepartment;
        			salesmanIndex<salesManEndForDepartment;salesmanIndex++)
        	{
    			Acquire(g_customerSalesmanLock[salesmanIndex]);
        		if(g_salesmanInfo[salesmanIndex].status == salesmanSignalToCustomerFromDeptWaitQueue)
        		{
        			mySalesMan = salesmanIndex;
        			Acquire(printLock);
        			Print3("CUSTOMER %d assumes that SALESMAN_%d Signaled him regarding enquiry of ITEM_%d\n",
        					ThreadId,mySalesMan,currentItemNoFromShoppingList);
        			Release(printLock);
        			g_salesmanInfo[salesmanIndex].status = salesmanIsBusy;
        			g_salesmanInfo[salesmanIndex].customerId = ThreadId;
        			break;
        		}
        		Release(g_customerSalesmanLock[salesmanIndex]);
        	}
        	Release(g_customerDepartmentLock[currentDepartmentNoForItem]);
    	}

    	Acquire(printLock);
		Print3("CUSTOMER %d is interacting with SALESMAN_%d from DEPARTMENT_%d \n",
				ThreadId,mySalesMan,currentDepartmentNoForItem);
		Release(printLock);
    	Signal(g_customerSalesmanCV[mySalesMan],g_customerSalesmanLock[mySalesMan]);

    	Wait(g_customerSalesmanCV[mySalesMan],g_customerSalesmanLock[mySalesMan]);

    	Signal(g_customerSalesmanCV[mySalesMan],g_customerSalesmanLock[mySalesMan]);

    	Release(g_customerSalesmanLock[mySalesMan]);


		Acquire(g_shelfAccessLock[currentItemNoFromShoppingList]);

		if(g_itemInfo[currentItemNoFromShoppingList].noOfItems>=currentItemNoCountFromShoppingList)
		{
			g_itemInfo[currentItemNoFromShoppingList].noOfItems -= currentItemNoCountFromShoppingList;
			Acquire(printLock);
			Print3("CUSTOMER %d has placed %d ITEM_%d's in the trolley \n",ThreadId,
					currentItemNoFromShoppingList,currentItemNoCountFromShoppingList);
			Release(printLock);
			Release(g_shelfAccessLock[currentItemNoFromShoppingList]);
		}
		else
		{
			Release(g_shelfAccessLock[currentItemNoFromShoppingList]);
			mySalesMan = -1;
			Acquire(printLock);
    		Print2("CUSTOMER %d is not able to find ITEM_%d and is searching for SALESMAN's \n",
    				ThreadId,currentItemNoFromShoppingList);
    		Release(printLock);
    		for(salesmanIndex=salesManStartForDepartment;
	    			salesmanIndex<salesManEndForDepartment;salesmanIndex++)
	    	{

	    		Acquire(g_customerSalesmanLock[salesmanIndex]);
	    		if(g_salesmanInfo[salesmanIndex].status == salesmanIsFree)
	    		{
	    			mySalesMan = salesmanIndex;
	    			Acquire(printLock);
	    			Print3("CUSTOMER %d will enquire about RESTOCK-ITEM_%d to SALESMAN_%d\n",ThreadId,
	    					currentItemNoFromShoppingList,mySalesMan);
	    			Release(printLock);
	    			g_salesmanInfo[salesmanIndex].status = salesmanIsBusy;
	    			g_salesmanInfo[salesmanIndex].customerId = ThreadId;
	    			g_salesmanInfo[salesmanIndex].itemToRestock = currentItemNoFromShoppingList;
	    			break;
	    		}
	    		Release(g_customerSalesmanLock[salesmanIndex]);
	    	}

	    	if(mySalesMan == -1)
	    	{
	    		Acquire(printLock);
	    		Print3("CUSTOMER %d is waiting in COMPLAIN LINE for DEPARTMENT_%d AS "
	    				"NO SALESMAN is free for RESTOCK-ITEM_%d inquiry so \n",
	    				ThreadId,
	    				currentDepartmentNoForItem,
	    				currentItemNoFromShoppingList);
	    		Release(printLock);
	    		Acquire(g_customerDepartmentComplainLock[currentDepartmentNoForItem]);

	    		g_departmentComplainWaitQueue[currentDepartmentNoForItem]++;

	    		Wait(g_customerDepartmentComplainCV[currentDepartmentNoForItem],
	    				g_customerDepartmentComplainLock[currentDepartmentNoForItem]);

	        	for(salesmanIndex=salesManStartForDepartment;
	        			salesmanIndex<salesManEndForDepartment;salesmanIndex++)
	        	{
	    			Acquire(g_customerSalesmanLock[salesmanIndex]);
	        		if(g_salesmanInfo[salesmanIndex].status == salesmanSignalToCustomerFromDeptComplainWaitQueue)
	        		{
		    			mySalesMan = salesmanIndex;
		    			Acquire(printLock);
	        			Print3("CUSTOMER %d assumes that SALESMAN_%d Signaled him regarding inquiry of RESTOCK ITEM_%d\n",
	        					ThreadId,mySalesMan,currentItemNoFromShoppingList);
	        			Release(printLock);
		    			g_salesmanInfo[salesmanIndex].status = salesmanIsBusy;
		    			g_salesmanInfo[salesmanIndex].customerId = ThreadId;
		    			g_salesmanInfo[salesmanIndex].itemToRestock = currentItemNoFromShoppingList;
	        			break;
	        		}
	        		Release(g_customerSalesmanLock[salesmanIndex]);
	        	}
	        	Release(g_customerDepartmentComplainLock[currentDepartmentNoForItem]);
	    	}
	    	Acquire(printLock);
	    	Print2("CUSTOMER %d is asking for assistance from SALESMAN_%d \n",
	    			ThreadId,mySalesMan);
	    	Release(printLock);
	    	Signal(g_customerSalesmanCV[mySalesMan],g_customerSalesmanLock[mySalesMan]);

	    	Wait(g_customerSalesmanCV[mySalesMan],g_customerSalesmanLock[mySalesMan]);
	    	Acquire(printLock);
	    	Print3("CUSTOMER %d has received assistance about re stocking of ITEM_%d from SALESMAN_%d \n",
	    			ThreadId,currentItemNoFromShoppingList,mySalesMan);
	    	Release(printLock);
	    	Signal(g_customerSalesmanCV[mySalesMan],g_customerSalesmanLock[mySalesMan]);

			g_itemInfo[currentItemNoFromShoppingList].noOfItems -= currentItemNoCountFromShoppingList;

	    	g_salesmanInfo[mySalesMan].itemToRestock = -1;
	    	g_salesmanInfo[mySalesMan].customerId = -1;

	    	Release(g_customerSalesmanLock[mySalesMan]);

		}
		Acquire(printLock);
		Print2("CUSTOMER %d has finished shopping in  DEPARTMENT_%d \n",
				ThreadId,currentDepartmentNoForItem);
		Release(printLock);
        /*************************************CUSTOMER-SALESMAN INTERACTION ENDS HERE*********************************************/
    }
	Acquire(printLock);
    Print1("CUSTOMER %d has finished shopping for all items \n",ThreadId);

    Print1("CUSTOMER %d is looking for the Cashier \n",ThreadId);
    Release(printLock);
    /*******************************************CUSTOMER-CASHIER INTERACTION STARTS HERE*******************************************/
    for(i=0;i<NO_OF_CASHIERS;i++)
    {
    	if(g_customerInfo[ThreadId].type == PRIVILEGED)
    	{
    		Acquire(g_cashierPrivilegedLineLock[i]);
    		if(g_cashierPrivilegedWaitQueue[i]<minCashierLineLength)
    		{
    			minCashierLineLength = g_cashierPrivilegedWaitQueue[i];
    			myCashier = i;
    		}
    		Release(g_cashierPrivilegedLineLock[i]);
    	}
    	else
    	{
    		Acquire(g_cashierLineLock[i]);
    		if(g_cashierPrivilegedWaitQueue[i]<minCashierLineLength)
    		{
    			minCashierLineLength = g_cashierWaitQueue[i];
    			myCashier = i;
    		}
    		Release(g_cashierLineLock[i]);
    	}
    }

	if(g_customerInfo[ThreadId].type == PRIVILEGED)
	{
		Acquire(printLock);
		Print3("CUSTOMER %d chose CASHIER_%d with Privileged Line of length %d \n",
				ThreadId,myCashier,minCashierLineLength);
		Release(printLock);
	}
	else
	{
		Acquire(printLock);
		Print3("CUSTOMER %d chose CASHIER_%d with Line of length %d \n",
				ThreadId,myCashier,minCashierLineLength);
		Release(printLock);
	}

    Acquire(g_customerCashierLock[myCashier]);
    if(g_cashierInfo[myCashier].status == cashierIsFree)
    {
    	g_cashierInfo[myCashier].status = cashierIsBusy;
    	g_cashierInfo[myCashier].customerId = ThreadId;
    }
    else
    {
    	if(g_customerInfo[ThreadId].type == PRIVILEGED)
    	{
    		Acquire(g_cashierPrivilegedLineLock[myCashier]);
    	    Release(g_customerCashierLock[myCashier]);
    	    g_cashierPrivilegedWaitQueue[myCashier]++;
    	    Wait(g_cashierPrivilegedLineCV[myCashier],g_cashierPrivilegedLineLock[myCashier]);
    	}
    	else
    	{
    		Acquire(g_cashierLineLock[myCashier]);
    	    Release(g_customerCashierLock[myCashier]);
    	    g_cashierWaitQueue[myCashier]++;
    	    Wait(g_cashierLineCV[myCashier],g_cashierLineLock[myCashier]);
    	}

    	Acquire(g_customerCashierLock[myCashier]);
    	g_cashierInfo[myCashier].status = cashierIsBusy;
    	g_cashierInfo[myCashier].customerId = ThreadId;

    	if(g_customerInfo[ThreadId].type == PRIVILEGED)
    	{
    		Release(g_cashierPrivilegedLineLock[myCashier]);
    	}
    	else
    	{
    		Release(g_cashierLineLock[myCashier]);
    	}
    }

    Signal(g_customerCashierCV[myCashier],g_customerCashierLock[myCashier]);

    Wait(g_customerCashierCV[myCashier],g_customerCashierLock[myCashier]);

    myBill = g_cashierInfo[myCashier].bill;

    if(myBill <= g_customerInfo[ThreadId].money)
    {
    	g_customerInfo[ThreadId].hasEnoughMoneyForShopping = true;
    	Acquire(printLock);
    	Print3("CUSTOMER %d pays %d amount to CASHIER_%d and is waiting for the receipt\n",
    			ThreadId,myBill,myCashier);
    	Release(printLock);
    }
    else
    {
    	g_customerInfo[ThreadId].hasEnoughMoneyForShopping = false;
    	Acquire(printLock);
    	Print2("CUSTOMER %d cannot pay %d\n",
    			ThreadId,myBill);
    	Release(printLock);
    }

    Signal(g_customerCashierCV[myCashier],g_customerCashierLock[myCashier]);

    Wait(g_customerCashierCV[myCashier],g_customerCashierLock[myCashier]);

    if(g_customerInfo[ThreadId].hasEnoughMoneyForShopping == true)
    {
    	Acquire(printLock);
    	Print2("CUSTOMER %d got receipt from CASHIER_%d and is now leaving\n",
    			ThreadId,myCashier);
    	Release(printLock);

    }
	Acquire(printLock);
	Print2("CUSTOMER %d signaled CASHIER_%d that I am leaving\n",
			ThreadId,myCashier);
	Release(printLock);
    Signal(g_customerCashierCV[myCashier],g_customerCashierLock[myCashier]);

    Acquire(g_managerCustomerInteractionLock);

    Release(g_customerCashierLock[myCashier]);

    /*************************************CUSTOMER - CASHIER INTERACTION ENDS HERE*********************************************/

    /*************************************CUSTOMER - MANAGER INTERACTION STARTS HERE *******************************************/

    if(g_customerInfo[ThreadId].hasEnoughMoneyForShopping == false)
    {
    	Acquire(printLock);
    	Print1("CUSTOMER %d is waiting for MANAGER for negotiations\n",ThreadId);
    	Release(printLock);
    	g_managerInfo.customerBill = myBill;
    	g_managerInfo.customerId = ThreadId;

    	Signal(g_managerCustomerInteractionCV,g_managerCustomerInteractionLock);

    	Wait(g_managerCustomerInteractionCV,g_managerCustomerInteractionLock);
    	while(myBill<=g_customerInfo[ThreadId].hasEnoughMoneyForShopping)
    	{
    		Signal(g_managerCustomerInteractionCV,g_managerCustomerInteractionLock);
    		Acquire(printLock);
    		Print2("CUSTOMER %d tells MANAGER to remove ITEM_%d from trolley\n",
    				ThreadId,
    				g_customerInfo[ThreadId].pCustomerShoppingList[currentItemNoCountToRemoveFromTrolley].itemNo);
    		Release(printLock);
        	Wait(g_managerCustomerInteractionCV,g_managerCustomerInteractionLock);
        	myBill = g_managerInfo.customerBill;
        	currentItemNoCountToRemoveFromTrolley++;
    	}

    	g_customerInfo[ThreadId].isDoneShopping = true;
    	Acquire(printLock);
    	Print2("CUSTOMER %d pays $ %d MANAGER after removing items and is waiting for receipt from MANAGER\n",
    			ThreadId,myBill);
    	Release(printLock);
		Signal(g_managerCustomerInteractionCV,g_managerCustomerInteractionLock);
    	Wait(g_managerCustomerInteractionCV,g_managerCustomerInteractionLock);

    	Acquire(printLock);
    	Print1("CUSTOMER %d got receipt from MANAGER and is now leaving\n",ThreadId);
    	Release(printLock);

    	Release(g_managerCustomerInteractionLock);
    }

    else
    {
    	Release(g_managerCustomerInteractionLock);
    }

    /*************************************CUSTOMER - MANAGER INTERACTION ENDS HERE *******************************************/

	Acquire(printLock);
    Print1("CUSTOMER %d left the shopping mall !!!!! \n",ThreadId);
    Release(printLock);

    Acquire(g_customerTrolleyLock);
    g_usedTrolleyCount--;
    if(g_waitForTrolleyCount>0)
    {
    	Signal(g_customerTrolleyCV,g_customerTrolleyLock);
    }
    Release(g_customerTrolleyLock);

    Acquire(g_customersLeftLock);
    g_noOfCustomersLeft++;
    g_customerListLeft[ThreadId] = 1;
    Release(g_customersLeftLock);

#endif

	Acquire(printLock);
    Print1("CUSTOMER %d exits !!!!!!! \n",ThreadId);
    Release(printLock);

    Exit(0);
}

void SalesmanThread()
{
	int ThreadId;
    int myGoodsLoader = -1;
	int goodLoaderIndex;
	int myDepartmentNo;

	Acquire(g_salesmanThreadCounterLock);
	ThreadId = g_salesmanThreadCounter;
	g_salesmanThreadCounter++;
	Release(g_salesmanThreadCounterLock);
	Acquire(printLock);
    Print1( "SALESMAN %d Started !!!!!!! \n",ThreadId);
    Print2("SALESMAN %d will be working for DEPARTMENT_%d \n",
    		ThreadId,g_salesmanInfo[ThreadId].departmentNo);
    Release(printLock);


#if 1




	myDepartmentNo = g_salesmanInfo[ThreadId].departmentNo;



    while(!simulationEnd)
    {
    	Acquire(g_customerDepartmentLock[myDepartmentNo]);

    	if(g_departmentWaitQueue[myDepartmentNo]>0)
    	{
    		Acquire(printLock);
    		Print2("SALESMAN %d ---- DEPARTMENT_%d WAIT QUEUE is not Empty... Someone Waiting for Assistance \n",
    				ThreadId,myDepartmentNo);
    		Release(printLock);
    		g_departmentWaitQueue[myDepartmentNo]--;

    		Acquire(g_customerSalesmanLock[ThreadId]);

    		g_salesmanInfo[ThreadId].status = salesmanSignalToCustomerFromDeptWaitQueue;

    		Signal(g_customerDepartmentCV[myDepartmentNo],g_customerDepartmentLock[myDepartmentNo]);
    		Acquire(printLock);
    		Print2("%SALESMAN %d signaling someone waiting in DEPARTMENT_%d WAIT QUEUE\n",ThreadId,
    				myDepartmentNo);
    		Release(printLock);
    		Release(g_customerDepartmentLock[myDepartmentNo]);

    		Acquire(printLock);
    		Print2("SALESMAN %d waiting for response from UNKNOWN CUSTOMER now in DEPARTMENT_%d WAIT QUEUE \n",
    				ThreadId,myDepartmentNo);
    		Release(printLock);

    		Wait(g_customerSalesmanCV[ThreadId],g_customerSalesmanLock[ThreadId]);
    		Acquire(printLock);
    		Print3("SALESMAN %d GOT INITIAL RESPONSE from CUSTOMER_%d now in DEPARTMENT_%d WAIT QUEUE\n",
    				ThreadId,g_salesmanInfo[ThreadId].customerId,myDepartmentNo);
    		Release(printLock);
    		Signal(g_customerSalesmanCV[ThreadId],g_customerSalesmanLock[ThreadId]);
    		Acquire(printLock);
    		Print3("SALESMAN %d SENDING RESPONSE to CUSTOMER_%d now in DEPARTMENT_%d WAIT QUEUE \n",
    				ThreadId,g_salesmanInfo[ThreadId].customerId,myDepartmentNo);
    		Release(printLock);
    		Acquire(printLock);
    		Print3("SALESMAN %d welcomes CUSTOMER_%d to DEPARTMENT_%d \n",
    				ThreadId,g_salesmanInfo[ThreadId].customerId,myDepartmentNo);
    		Release(printLock);
    		Wait(g_customerSalesmanCV[ThreadId],g_customerSalesmanLock[ThreadId]);

    		Print2("SALESMAN %d GOT FINAL RESPONSE from CUSTOMER_%d now \n",ThreadId,g_salesmanInfo[ThreadId].customerId);

    		Release(g_customerSalesmanLock[ThreadId]);
    	}
        else
        {
        	Release(g_customerDepartmentLock[myDepartmentNo]);
        }

    	Acquire(g_customerDepartmentComplainLock[myDepartmentNo]);

    	if(g_departmentComplainWaitQueue[myDepartmentNo]>0)
    	{
    		Acquire(printLock);
    		Print2("SALESMAN %d ---- DEPARTMENT_%d COMPLAIN WAIT QUEUE is not Empty... Someone Waiting for Assistance \n",
    				ThreadId,myDepartmentNo);
    		Release(printLock);
    		g_departmentComplainWaitQueue[myDepartmentNo]--;

    		Acquire(g_customerSalesmanLock[ThreadId]);

    		g_salesmanInfo[ThreadId].status = salesmanSignalToCustomerFromDeptComplainWaitQueue;

    		Signal(g_customerDepartmentComplainCV[myDepartmentNo],g_customerDepartmentComplainLock[myDepartmentNo]);
    		Acquire(printLock);
    		Print2("SALESMAN %d signaling someone waiting in DEPARTMENT_%d COMPLAIN WAIT QUEUE \n",ThreadId,
    				myDepartmentNo);
    		Release(printLock);
    		Release(g_customerDepartmentComplainLock[myDepartmentNo]);
    		Acquire(printLock);
    		Print2("SALESMAN %d waiting for response from UNKNOWN CUSTOMER now in DEPARTMENT_%d COMPLAIN WAIT QUEUE \n",
    				ThreadId,myDepartmentNo);
    		Release(printLock);
    		Wait(g_customerSalesmanCV[ThreadId],g_customerSalesmanLock[ThreadId]);

    		Acquire(printLock);
    		Print3("SALESMAN %d is informed by CUSTOMER_%d that ITEM_%d is out of stock \n",
    				ThreadId,
    				g_salesmanInfo[ThreadId].customerId,
    				g_salesmanInfo[ThreadId].itemToRestock);
    		Release(printLock);

    		/**************************START INTERACTING WITH GOODS LOADER NOW ***************************/

    		for(goodLoaderIndex=0;goodLoaderIndex<NO_OF_GOOD_LOADERS;goodLoaderIndex++)
    		{
    			Acquire(g_salesmanGoodsLoaderLock[goodLoaderIndex]);

    			if(g_goodLoaderInfo[goodLoaderIndex].status == goodLoaderIsFree)
    			{
    				myGoodsLoader = goodLoaderIndex;
    				Acquire(printLock);
	    			Print3("SALESMAN %d will enquire about RESTOCK-ITEM_%d to GOODLOADER_%d\n",ThreadId,
	    					g_salesmanInfo[ThreadId].itemToRestock,myGoodsLoader);
	    			Release(printLock);

    				g_goodLoaderInfo[goodLoaderIndex].status = goodLoaderIsBusy;
    				g_goodLoaderInfo[goodLoaderIndex].salesmanId = ThreadId;
    				break;
    			}
    			Release(g_salesmanGoodsLoaderLock[goodLoaderIndex]);
    		}

    		if(myGoodsLoader == -1)
    		{
    			Acquire(printLock);
	    		Print2("SALESMAN %d is waiting in SINGLE LINE for GOOD LOADERS as "
	    				" NO GOODLOADER is free for RESTOCK-ITEM_%d inquiry so "
	    				"\n",ThreadId,
	    				g_salesmanInfo[ThreadId].itemToRestock);
	    		Release(printLock);


    			Acquire(g_goodLoaderWaitLock[0]);
    			g_goodLoaderWaitQueue[0]++;

    			Wait(g_goodLoaderWaitCV[0],g_goodLoaderWaitLock[0]);
        		for(goodLoaderIndex=0;goodLoaderIndex<NO_OF_GOOD_LOADERS;goodLoaderIndex++)
        		{
        			Acquire(g_salesmanGoodsLoaderLock[goodLoaderIndex]);

        			if(g_goodLoaderInfo[goodLoaderIndex].status == goodLoaderSignalToSalesman)
        			{
        				myGoodsLoader = goodLoaderIndex;
        				Acquire(printLock);
	        			Print3("SALESMAN %d assumes that GOOD LOADER_%d Signaled him regarding inquiry of RESTOCK ITEM_%d\n",
	        					ThreadId,myGoodsLoader,g_salesmanInfo[ThreadId].itemToRestock);
	        			Release(printLock);

        				g_goodLoaderInfo[goodLoaderIndex].status = goodLoaderIsBusy;
        				g_goodLoaderInfo[goodLoaderIndex].salesmanId = ThreadId;
        				break;
        			}
        			Release(g_salesmanGoodsLoaderLock[goodLoaderIndex]);
        		}
    		}

    		Release(g_goodLoaderWaitLock[0]);

    		g_goodLoaderInfo[myGoodsLoader].itemToRestock = g_salesmanInfo[ThreadId].itemToRestock;

    		Signal(g_salesmanGoodsLoaderCV[myGoodsLoader],g_salesmanGoodsLoaderLock[myGoodsLoader]);
    		Acquire(printLock);
    		Print3("SALESMAN %d informs GOOD LOADER_%d that ITEM_%d is out of stock \n",
    				ThreadId,myGoodsLoader,g_salesmanInfo[ThreadId].itemToRestock);
    		Release(printLock);
    		Wait(g_salesmanGoodsLoaderCV[myGoodsLoader],g_salesmanGoodsLoaderLock[myGoodsLoader]);
    		Acquire(printLock);
    		Print3("SALESMAN %d is informed by the GOOD LOADER_%d that ITEM_%d is re stocked \n",
    				ThreadId,myGoodsLoader,g_salesmanInfo[ThreadId].itemToRestock);
    		Release(printLock);
    		Release(g_salesmanGoodsLoaderLock[myGoodsLoader]);

    		/**************************END INTERACTING WITH GOODS LOADER NOW ***************************/

    		Signal(g_customerSalesmanCV[ThreadId],g_customerSalesmanLock[ThreadId]);
    		Acquire(printLock);
    		Print3("SALESMAN %d SENDING RESPONSE to CUSTOMER_%d now in DEPARTMENT_%d COMPLAIN WAIT QUEUE \n",
    				ThreadId,g_salesmanInfo[ThreadId].customerId,myDepartmentNo);

    		Print3("SALESMAN %d informs the CUSTOMER_%d that ITEM_%d is re stocked \n",
    				ThreadId,g_salesmanInfo[ThreadId].customerId,
    				g_salesmanInfo[ThreadId].itemToRestock);
    		Release(printLock);

    		Wait(g_customerSalesmanCV[ThreadId],g_customerSalesmanLock[ThreadId]);
    		Acquire(printLock);
    		Print2("SALESMAN %d GOT FINAL RESPONSE from CUSTOMER_%d now \n",ThreadId,g_salesmanInfo[ThreadId].customerId);
    		Release(printLock);
    		myGoodsLoader = -1;
    		g_salesmanInfo[ThreadId].itemToRestock = -1;

    		Release(g_customerSalesmanLock[ThreadId]);
    	}
        else
        {
        	Release(g_customerDepartmentComplainLock[myDepartmentNo]);
        }

    	Acquire(g_customerDepartmentLock[myDepartmentNo]);
    	Acquire(g_customerDepartmentComplainLock[myDepartmentNo]);

    	if(g_departmentWaitQueue[myDepartmentNo] == 0 &&
    			g_departmentComplainWaitQueue[myDepartmentNo] == 0)
    	{
    		Acquire(printLock);
    		Print3("SALESMAN %d is RELAXING AS DEPARTMENT_%d WAIT QUEUE and DEPARTMENT_%d "
    				"COMPLAIN WAIT QUEUE both are empty"
    				"SET STATUS TO FREE\n ",ThreadId,myDepartmentNo,myDepartmentNo);
    		Release(printLock);
    		Acquire(g_customerSalesmanLock[ThreadId]);

    		g_salesmanInfo[ThreadId].status = salesmanIsFree;

    		Release(g_customerDepartmentComplainLock[myDepartmentNo]);

    		Release(g_customerDepartmentLock[myDepartmentNo]);

    		Wait(g_customerSalesmanCV[ThreadId],g_customerSalesmanLock[ThreadId]);
    		Acquire(printLock);
    		Print2("SALESMAN %d GOT INITIAL RESPONSE from CUSTOMER_%d in NO WAIT QUEUE\n",
    				ThreadId,g_salesmanInfo[ThreadId].customerId);
    		Release(printLock);


    		if(g_salesmanInfo[ThreadId].itemToRestock == -1)
    		{
    			Acquire(printLock);
    			Print1("SALESMAN %d is checking if INITIAL RESPONSE is not regarding RE-STOCK of Item in NO WAIT QUEUE\n",
    					ThreadId);
    			Release(printLock);
    			g_salesmanInfo[ThreadId].status = salesmanSignalToCustomerFromDeptWaitQueue;

    			Signal(g_customerSalesmanCV[ThreadId],g_customerSalesmanLock[ThreadId]);
    			Acquire(printLock);
        		Print2("SALESMAN %d SENDING RESPONSE to CUSTOMER_%d in NO WAIT QUEUE \n",
        				ThreadId,g_salesmanInfo[ThreadId].customerId);

        		Print3("SALESMAN %d welcomes CUSTOMER_%d to DEPARTMENT_%d \n",
        				ThreadId,g_salesmanInfo[ThreadId].customerId,myDepartmentNo);
        		Release(printLock);
    			Wait(g_customerSalesmanCV[ThreadId],g_customerSalesmanLock[ThreadId]);

    			Acquire(printLock);
        		Print2("SALESMAN %d GOT FINAL RESPONSE from CUSTOMER_%d in NO WAIT QUEUE \n",
        				ThreadId,g_salesmanInfo[ThreadId].customerId);
        		Release(printLock);

        		Release(g_customerSalesmanLock[ThreadId]);
    		}
    		else
    		{
    			Acquire(printLock);
    			Print1("SALESMAN %d is IF INITIAL RESPONSE is regarding RE-STOCK of Item in NO WAIT QUEUE\n",ThreadId);
    			Release(printLock);

    			g_salesmanInfo[ThreadId].status = salesmanSignalToCustomerFromDeptComplainWaitQueue;

        		for(goodLoaderIndex=0;goodLoaderIndex<NO_OF_GOOD_LOADERS;goodLoaderIndex++)
        		{
        			Acquire(g_salesmanGoodsLoaderLock[goodLoaderIndex]);

        			if(g_goodLoaderInfo[goodLoaderIndex].status == goodLoaderIsFree)
        			{
        				myGoodsLoader = goodLoaderIndex;
        				Acquire(printLock);
        	    		Print2(" SALESMAN %d is waiting in SINGLE LINE for GOOD LOADERS NO WAIT QUEUE as "
        	    				"NO GOODLOADER is free for RESTOCK-ITEM_%d inquiry so "
        	    				"\n",ThreadId,
        	    				g_salesmanInfo[ThreadId].itemToRestock
        	    				);
        	    		Release(printLock);

        				g_goodLoaderInfo[goodLoaderIndex].status = goodLoaderIsBusy;
        				g_goodLoaderInfo[goodLoaderIndex].salesmanId = ThreadId;
        				break;
        			}
        			Release(g_salesmanGoodsLoaderLock[goodLoaderIndex]);
        		}

        		if(myGoodsLoader == -1)
        		{
        			Acquire(printLock);
    	    		Print2("SALESMAN %d is waiting in SINGLE LINE for "
    	    				"GOOD LOADERS NO WAIT QUEUE AS NO GOODLOADER is free for RESTOCK-ITEM_%d inquiry so \n",
    	    				ThreadId,
    	    				g_salesmanInfo[ThreadId].itemToRestock
    	    				);
    	    		Release(printLock);

        			Acquire(g_goodLoaderWaitLock[0]);
        			g_goodLoaderWaitQueue[0]++;

        			Wait(g_goodLoaderWaitCV[0],g_goodLoaderWaitLock[0]);

            		for(goodLoaderIndex=0;goodLoaderIndex<NO_OF_GOOD_LOADERS;goodLoaderIndex++)
            		{
            			Acquire(g_salesmanGoodsLoaderLock[goodLoaderIndex]);

            			if(g_goodLoaderInfo[goodLoaderIndex].status == goodLoaderSignalToSalesman)
            			{
            				myGoodsLoader = goodLoaderIndex;
            				Acquire(printLock);
    	        			Print3("SALESMAN %d assumes that GOOD LOADER_%d Signaled him regarding inquiry of RESTOCK ITEM_%d\n",
    	        					ThreadId,myGoodsLoader,g_salesmanInfo[ThreadId].itemToRestock);
    	        			Release(printLock);

            				g_goodLoaderInfo[goodLoaderIndex].status = goodLoaderIsBusy;
            				g_goodLoaderInfo[goodLoaderIndex].salesmanId = ThreadId;
            				break;
            			}
            			Release(g_salesmanGoodsLoaderLock[goodLoaderIndex]);
            		}
        		}

        		Release(g_goodLoaderWaitLock[0]);

        		g_goodLoaderInfo[myGoodsLoader].itemToRestock = g_salesmanInfo[ThreadId].itemToRestock;

        		Signal(g_salesmanGoodsLoaderCV[myGoodsLoader],g_salesmanGoodsLoaderLock[myGoodsLoader]);
        		Acquire(printLock);
        		Print2("SALESMAN %d SENDING RESPONSE to CUSTOMER_%d now in NO WAIT QUEUE \n",
        				ThreadId,g_salesmanInfo[ThreadId].customerId);

        		Print3("SALESMAN %d informs GOOD LOADER_%d that ITEM_%d is out of stock \n",
        				ThreadId,myGoodsLoader,g_salesmanInfo[ThreadId].itemToRestock);
        		Release(printLock);
        		Wait(g_salesmanGoodsLoaderCV[myGoodsLoader],g_salesmanGoodsLoaderLock[myGoodsLoader]);
        		Acquire(printLock);
        		Print3("SALESMAN %d is informed by the GOOD LOADER_%d that ITEM_%d is re stocked \n",
        				ThreadId,myGoodsLoader,g_salesmanInfo[ThreadId].itemToRestock);
        		Release(printLock);
        		Release(g_salesmanGoodsLoaderLock[myGoodsLoader]);

        		Signal(g_customerSalesmanCV[ThreadId],g_customerSalesmanLock[ThreadId]);
        		Acquire(printLock);
        		Print3("SALESMAN %d informs the CUSTOMER_%d that ITEM_%d is re stocked \n",
        				ThreadId,g_salesmanInfo[ThreadId].customerId,
        				g_salesmanInfo[ThreadId].itemToRestock);
        		Release(printLock);
        		Wait(g_customerSalesmanCV[ThreadId],g_customerSalesmanLock[ThreadId]);
        		Acquire(printLock);
        		Print2("SALESMAN %d GOT FINAL RESPONSE from CUSTOMER_%d now \n",ThreadId,g_salesmanInfo[ThreadId].customerId);
        		Release(printLock);
        		myGoodsLoader = -1;
        		g_salesmanInfo[ThreadId].itemToRestock = -1;

        		Release(g_customerSalesmanLock[ThreadId]);
    		}
    	}
    	else
    	{
    		Release(g_customerDepartmentLock[myDepartmentNo]);
    		Release(g_customerDepartmentComplainLock[myDepartmentNo]);
    	}
    }
#endif
	Acquire(printLock);
    Print1("SALESMAN %d exits !!!!!!! \n",ThreadId);
    Release(printLock);
	Exit(0);
}

void GoodLoaderThread()
{

    int ThreadId;
    int mySalesman = -1;
    int i;


	Acquire(g_goodsLoaderThreadCounterLock);
	ThreadId = g_goodsLoaderThreadCounter;
	g_goodsLoaderThreadCounter++;
	Release(g_goodsLoaderThreadCounterLock);
	Acquire(printLock);
	Print1("GOODSLOADER %d Started !!!!!!!\n",ThreadId);
	Release(printLock);

#if 1


    while(!simulationEnd)
    {
    	int goodsLoaderWalkingTime;
    	goodsLoaderWalkingTime = GetRand()%(MAX_GOODS_LOADER_WALKINGTIME-MIN_GOODS_LOADER_WALKINGTIME) +
    			MIN_GOODS_LOADER_WALKINGTIME;

    	Acquire(g_goodLoaderWaitLock[0]);

    	if(g_goodLoaderWaitQueue[0]>0)
    	{
    		Acquire(printLock);
    		Print1("GOODSLOADER %d GOOD LOADER WAIT QUEUE IS NOT EMPTY...RE-STOCK OF SOME ITEM \n",ThreadId);
    		Release(printLock);
    		g_goodLoaderWaitQueue[0]--;

    		Acquire(g_salesmanGoodsLoaderLock[ThreadId]);

    		g_goodLoaderInfo[ThreadId].status = goodLoaderSignalToSalesman;

    		Signal(g_goodLoaderWaitCV[0],g_goodLoaderWaitLock[0]);
    		Acquire(printLock);
    		Print1("GOODSLOADER %d signaling someone waiting in GOOD LOADERS WAIT QUEUE\n",ThreadId);
    		Release(printLock);
    		Release(g_goodLoaderWaitLock[0]);

    		Wait(g_salesmanGoodsLoaderCV[ThreadId],g_salesmanGoodsLoaderLock[ThreadId]);
    		Acquire(printLock);
    		Print3("GOODSLOADER %d is informed by SALESMAN_%d of DEPARTMENT_%d ",
    				ThreadId,g_goodLoaderInfo[ThreadId].salesmanId,
    				g_salesmanInfo[g_goodLoaderInfo[ThreadId].salesmanId].departmentNo);
    				Print1("to re stock ITEM_%d \n",g_goodLoaderInfo[ThreadId].itemToRestock);
    		Release(printLock);

    		Acquire(g_shelfAccessLock[g_goodLoaderInfo[ThreadId].itemToRestock]);

    		g_itemInfo[g_goodLoaderInfo[ThreadId].itemToRestock].noOfItems = MAX_NO_ITEMS_PER_SHELF;

    		Release(g_shelfAccessLock[g_goodLoaderInfo[ThreadId].itemToRestock]);

    		for(i=0;i<goodsLoaderWalkingTime;i++)
    		{
    			Yield();
    			Acquire(printLock);
    			Print2("GOODSLOADER %d is walking from RE-STOCK room to shelf to RE-STOCK ITEM_%d\n",ThreadId,
    					g_goodLoaderInfo[ThreadId].itemToRestock);
    			Release(printLock);
    		}

    		Signal(g_salesmanGoodsLoaderCV[ThreadId],g_salesmanGoodsLoaderLock[ThreadId]);
    		Acquire(printLock);
    		Print3("GOODSLOADER %d has re stocked ITEM_%d in DEPARTMENT_%d \n",
    				ThreadId,
    				g_goodLoaderInfo[ThreadId].itemToRestock,
    				g_salesmanInfo[g_goodLoaderInfo[ThreadId].salesmanId].departmentNo);
    		Release(printLock);

    		Release(g_salesmanGoodsLoaderLock[ThreadId]);
    	}
    	else
    	{
    		Release(g_goodLoaderWaitLock[0]);
    	}

    	Acquire(g_goodLoaderWaitLock[0]);

    	if(g_goodLoaderWaitQueue[0] == 0)
    	{
    		Acquire(printLock);
    		Print1("GOODSLOADER %d GOOD LOADER WAIT QUEUE IS EMPTY...RELAX \n",ThreadId);

    		Print1("GOODSLOADER %d is waiting for orders to Re stock\n",ThreadId);
    		Release(printLock);

    		Acquire(g_salesmanGoodsLoaderLock[ThreadId]);
    		g_goodLoaderInfo[ThreadId].status = goodLoaderIsFree;

    		Release(g_goodLoaderWaitLock[0]);
    		Wait(g_salesmanGoodsLoaderCV[ThreadId],g_salesmanGoodsLoaderLock[ThreadId]);
    		Acquire(printLock);
    		Print3("GOODSLOADER %d is informed by SALESMAN_%d of DEPARTMENT_%d ",
    				ThreadId,g_goodLoaderInfo[ThreadId].salesmanId,
    				g_salesmanInfo[g_goodLoaderInfo[ThreadId].salesmanId].departmentNo);
    				Print1("to re stock ITEM_%d \n",g_goodLoaderInfo[ThreadId].itemToRestock);
    	    Release(printLock);


    		Acquire(g_shelfAccessLock[g_goodLoaderInfo[ThreadId].itemToRestock]);

    		g_itemInfo[g_goodLoaderInfo[ThreadId].itemToRestock].noOfItems = MAX_NO_ITEMS_PER_SHELF;

    		Release(g_shelfAccessLock[g_goodLoaderInfo[ThreadId].itemToRestock]);


    		for(i=0;i<goodsLoaderWalkingTime;i++)
    		{
    			Yield();
        		Acquire(printLock);
    			Print2("GOODSLOADER %d is walking from RE-STOCK room to shelf to RE-STOCK ITEM_%d\n",ThreadId,
    					g_goodLoaderInfo[ThreadId].itemToRestock);
        		Release(printLock);

    		}

    		Signal(g_salesmanGoodsLoaderCV[ThreadId],g_salesmanGoodsLoaderLock[ThreadId]);
    		Acquire(printLock);
    		Print3("GOODSLOADER %d has re stocked ITEM_%d in DEPARTMENT_%d \n",
    				ThreadId,
    				g_goodLoaderInfo[ThreadId].itemToRestock,
    				g_salesmanInfo[g_goodLoaderInfo[ThreadId].salesmanId].departmentNo);
    		Release(printLock);
    		Release(g_salesmanGoodsLoaderLock[ThreadId]);
    	}

    	else
    	{
    		Release(g_goodLoaderWaitLock[0]);
    	}
    }

#endif

	Acquire(printLock);
    Print1("GOODS LOADER %d exits !!!!!!! \n",ThreadId);
    Release(printLock);

	Exit(0);

}


void CashierThread()
{

	int ThreadId;
	int myCustomer = -1;
	int totalBill = 0;
    int currentItemNoFromShoppingList;
    int currentItemNoCountFromShoppingList;
    int currentItemNoPriceFromShoppingList;
	int i;


	Acquire(g_cashierThreadCounterLock);
	ThreadId = g_cashierThreadCounter;
	g_cashierThreadCounter++;
	Release(g_cashierThreadCounterLock);

	Acquire(printLock);
    Print1( "CASHIER %d Started \n",ThreadId);
    Release(printLock);

#if 1




    while(!simulationEnd)
    {
    	myCustomer = -1;
    	totalBill = 0;
        currentItemNoFromShoppingList = 0;
        currentItemNoCountFromShoppingList = 0;
        currentItemNoPriceFromShoppingList = 0;

        Acquire(g_cashierPrivilegedLineLock[ThreadId]);
    	if(g_cashierPrivilegedWaitQueue[ThreadId]>0)
    	{
    		g_cashierPrivilegedWaitQueue[ThreadId]--;
    		g_cashierInfo[ThreadId].status = cashierSignalToCustomer;
    		Signal(g_cashierPrivilegedLineCV[ThreadId],g_cashierPrivilegedLineLock[ThreadId]);
    		Acquire(g_customerCashierLock[ThreadId]);
    		Release(g_cashierPrivilegedLineLock[ThreadId]);
    	}
    	else
    	{
    		Release(g_cashierPrivilegedLineLock[ThreadId]);

    		Acquire(g_cashierLineLock[ThreadId]);

    		if(g_cashierWaitQueue[ThreadId]>0)
    		{
    			g_cashierWaitQueue[ThreadId]--;
        		g_cashierInfo[ThreadId].status = cashierSignalToCustomer;
        		Signal(g_cashierLineCV[ThreadId],g_cashierLineLock[ThreadId]);
        		Acquire(g_customerCashierLock[ThreadId]);
    		}
    		else
    		{
    			Acquire(g_customerCashierLock[ThreadId]);
    			g_cashierInfo[ThreadId].status = cashierIsFree;
    		}
    		Release(g_cashierLineLock[ThreadId]);
    	}
    	Acquire(printLock);
		Print2("Cashier %d total sales money is %d \n",ThreadId,g_cashierInfo[ThreadId].totalSalesMoney);
		Release(printLock);

    	Wait(g_customerCashierCV[ThreadId],g_customerCashierLock[ThreadId]);
    	myCustomer = g_cashierInfo[ThreadId].customerId;

    	for(i=0;i<g_customerInfo[myCustomer].noOfItems;i++)
    	{
        	currentItemNoFromShoppingList = g_customerInfo[myCustomer].pCustomerShoppingList[i].itemNo;
          	currentItemNoCountFromShoppingList = g_customerInfo[myCustomer].pCustomerShoppingList[i].noOfItems;
          	currentItemNoPriceFromShoppingList = g_itemInfo[currentItemNoFromShoppingList].Price;
          	totalBill +=  currentItemNoPriceFromShoppingList * currentItemNoCountFromShoppingList ;
        	Acquire(printLock);
          	Print2("CASHIER %d got ITEM_%d from trolley\n",ThreadId,currentItemNoFromShoppingList);
          	Release(printLock);
    	}

    	g_cashierInfo[ThreadId].bill = totalBill;

    	Signal(g_customerCashierCV[ThreadId],g_customerCashierLock[ThreadId]);
    	Acquire(printLock);
    	Print3("CASHIER %d tells CUSTOMER_%d total cost is $ %d\n",ThreadId,myCustomer,totalBill);
    	Release(printLock);

    	Wait(g_customerCashierCV[ThreadId],g_customerCashierLock[ThreadId]);

    	if(g_customerInfo[myCustomer].hasEnoughMoneyForShopping == false)
    	{
    		Acquire(g_managerCashierLock);

    		g_managerWaitQueueLength++;

    		Wait(g_managerCashierCV,g_managerCashierLock);

    		g_managerInfo.cashierId = ThreadId;

    		Acquire(g_managerCashierInteractionLock);

    		Release(g_managerCashierLock);

    		Signal(g_managerCashierInteractionCV,g_managerCashierInteractionLock);
        	Acquire(printLock);
    		Print2("CASHIER %d informs Manager that CUSTOMER_%d does not have enough money\n",
    				ThreadId,myCustomer);
        	Release(printLock);

    		Wait(g_managerCashierInteractionCV,g_managerCashierInteractionLock);

    		Release(g_managerCashierInteractionLock);

        	Signal(g_customerCashierCV[ThreadId],g_customerCashierLock[ThreadId]);
        	Acquire(printLock);
        	Print2("CASHIER %d asks CUSTOMER_%d to wait for Manager\n",ThreadId,myCustomer);
        	Release(printLock);
        	Wait(g_customerCashierCV[ThreadId],g_customerCashierLock[ThreadId]);

    	}
    	else
    	{
    		Acquire(g_managerCashierCashLock[ThreadId]);
    		g_cashierInfo[ThreadId].totalSalesMoney += totalBill;
    		Release(g_managerCashierCashLock[ThreadId]);
        	Acquire(printLock);
    		Print3("CASHIER %d got money $ %d from CUSTOMER_%d\n",ThreadId,totalBill,myCustomer);
        	Release(printLock);
        	Signal(g_customerCashierCV[ThreadId],g_customerCashierLock[ThreadId]);
        	Acquire(printLock);
        	Print2("CASHIER %d gave the receipt to CUSTOMER_%d and tells him to leave\n",
        			ThreadId,myCustomer);
        	Release(printLock);
        	Wait(g_customerCashierCV[ThreadId],g_customerCashierLock[ThreadId]);
    	}

    	Release(g_customerCashierLock[ThreadId]);

    }
#endif


	Acquire(printLock);
    Print1("CASHIER %d exits !!!!!!! \n",ThreadId);
    Release(printLock);

	Exit(0);

}

void ManagerThread()
{
    int ThreadId = 0;
    int managerSales = 0;
    int itemRemoveCounter = 0;
	int currentItemNoToRemove = 0;
	int currentItemNoToRemoveCount = 0;
	int currentItemNoToRemovePrice = 0;
	int i;


	Acquire(printLock);
	Print1( "MANAGER %d Started  \n",ThreadId);
	Release(printLock);

#if 1




    while(!simulationEnd)
    {
    	itemRemoveCounter = 0;

    	for(i=0;i<NO_OF_CASHIERS;i++)
    	{
    		Acquire(g_managerCashierCashLock[i]);
    		Acquire(printLock);
    		Print2("Total Sales Money From CASHIER %d is %d\n",i,g_cashierInfo[i].totalSalesMoney);
    		Release(printLock);
    		g_managerInfo.totalRevenue += g_cashierInfo[i].totalSalesMoney;
    		g_cashierInfo[i].totalSalesMoney = 0;
    		Release(g_managerCashierCashLock[i]);
    	}

    	g_managerInfo.totalRevenue += managerSales;
    	managerSales = 0;

    	Acquire(printLock);
    	Print1("Total Sale of the entire store until now is $ %d \n",g_managerInfo.totalRevenue);

        Acquire(g_customersLeftLock);
        printCustomerLeftList();
        Release(g_customersLeftLock);

    	Release(printLock);

    	Acquire(g_managerCashierLock);
    	if(g_managerWaitQueueLength == 0)
    	{
            Acquire(g_customersLeftLock);
    		Release(g_managerCashierLock);
    	    if(g_noOfCustomersLeft == NO_OF_CUSTOMERS)
    	    {
    	    	simulationEnd = 1;
    	    	Print("MANAGER HAS THE LAST SAY... END OF SIMULATION \n");
    	    }
            Release(g_customersLeftLock);
    	}
    	else
    	{
    		Signal(g_managerCashierCV,g_managerCashierLock);

    		g_managerWaitQueueLength--;

    		Acquire(g_managerCashierInteractionLock);

    		Release(g_managerCashierLock);

    		Wait(g_managerCashierInteractionCV,g_managerCashierInteractionLock);

    		Acquire(printLock);
    		Print2("MANAGER %d got a call from CASHIER_%d\n",ThreadId,g_managerInfo.cashierId);
    		Release(printLock);

    		Signal(g_managerCashierInteractionCV,g_managerCashierInteractionLock);

    		Acquire(g_managerCustomerInteractionLock);

    		Release(g_managerCashierInteractionLock);

    		Wait(g_managerCustomerInteractionCV,g_managerCustomerInteractionLock);

    		Signal(g_managerCustomerInteractionCV,g_managerCustomerInteractionLock);

    		Wait(g_managerCustomerInteractionCV,g_managerCustomerInteractionLock);

    		while(g_customerInfo[g_managerInfo.customerId].isDoneShopping != true)
    		{
    			currentItemNoToRemove =
    					g_customerInfo[g_managerInfo.customerId].pCustomerShoppingList[itemRemoveCounter].itemNo;

    			currentItemNoToRemovePrice = g_itemInfo[currentItemNoToRemove].Price;

    			currentItemNoToRemoveCount =
    					g_customerInfo[g_managerInfo.customerId].pCustomerShoppingList[itemRemoveCounter].noOfItems;

    			Acquire(printLock);
    			Print3("MANAGER %d removes ITEM_%d from trolley of CUSTOMER_%d\n",
    					ThreadId,currentItemNoToRemove,g_managerInfo.customerId);
    			Release(printLock);

    			g_managerInfo.customerBill -= currentItemNoToRemovePrice * currentItemNoToRemoveCount;

        		Signal(g_managerCustomerInteractionCV,g_managerCustomerInteractionLock);

        		Wait(g_managerCustomerInteractionCV,g_managerCustomerInteractionLock);
    		}

    		managerSales += g_managerInfo.customerBill;

    		Signal(g_managerCustomerInteractionCV,g_managerCustomerInteractionLock);

    		Acquire(printLock);
    		Print2("MANAGER %d gives receipt to CUSTOMER_%d \n",ThreadId,g_managerInfo.customerId);
    		Release(printLock);

    		Release(g_managerCustomerInteractionLock);

    		Acquire(printLock);
    		Print2("MANAGER %d has total sale of $ %d\n",ThreadId,managerSales);
    		Release(printLock);
    	}

    	for(i=0;i<MANAGER_RANDOM_SLEEP_TIME;i++)
    	{
    		Yield();
    	}
    }

#endif

	Acquire(printLock);
    Print1("MANAGER %d exits !!!!!!! \n",ThreadId);
    Release(printLock);

    Exit(0);
}

void initLockForSimulation()
{
	int i;

	g_customersLeftLock = CreateLock("CustomersLeftLock",sizeof("CustomersLeftLock"));
	g_customerThreadCounterLock = CreateLock("CustomerThreadCounterLock",sizeof("CustomerThreadCounterLock"));
	g_salesmanThreadCounterLock = CreateLock("SalesmanThreadCounterLock",sizeof("SalesmanThreadCounterLock"));
	g_goodsLoaderThreadCounterLock = CreateLock("GoodsLoaderThreadCounterLock",sizeof("GoodsLoaderThreadCounterLock"));
	g_cashierThreadCounterLock = CreateLock("CustomerThreadCounterLock",sizeof("CustomerThreadCounterLock"));

	g_customerTrolleyLock = CreateLock("CustomerTrolleyLock",sizeof("CustomerTrolleyLock"));

    g_managerCashierLock = CreateLock("managerCashierLock",sizeof("managerCashierLock"));
    g_managerCashierInteractionLock = CreateLock("managerCashierInteractionLock",sizeof("managerCashierInteractionLock"));

    g_managerCustomerInteractionLock = CreateLock("managerCustomerInteractionLock",sizeof("managerCustomerInteractionLock"));

	for(i=0;i<NO_OF_CASHIERS;i++)
	{

        g_cashierLineLock[i] = CreateLock("cashierLineLock",sizeof("cashierLineLock"));
        g_cashierPrivilegedLineLock[i] = CreateLock("cashierPrivilegedLineLock",
        		sizeof("cashierPrivilegedLineLock"));
        g_customerCashierLock[i] = CreateLock("customerCashierLock",sizeof("customerCashierLock"));
        g_managerCashierCashLock[i] = CreateLock("managerCashierCashLock",sizeof("managerCashierCashLock"));
	}

	for(i=0;i<NO_OF_SALESMAN;i++)
	{

        g_customerSalesmanLock[i] = CreateLock("customerSalesmanLock",sizeof("customerSalesmanLock"));
	}

	for(i=0;i<NO_OF_DEPARTMENT;i++)
	{
        g_customerDepartmentLock[i] = CreateLock("customerDepartmentLock",sizeof("customerDepartmentLock"));
        g_customerDepartmentComplainLock[i] = CreateLock("customerDepartmentComplainLock",
        		sizeof("customerDepartmentComplainLock"));
	}

	for(i =0;i<NO_OF_SHELFS;i++)
	{

        g_shelfAccessLock[i] = CreateLock("shelfAccessLock",sizeof("shelfAccessLock"));
	}

	for(i=0;i<NO_OF_GOOD_LOADERS;i++)
	{
        g_salesmanGoodsLoaderLock[i] = CreateLock("salesmanGoodsLoaderLock",sizeof("salesmanGoodsLoaderLock"));
	}

	for(i=0;i<NO_OF_GOODLOADER_WAIT_QUEUE;i++)
	{
		g_goodLoaderWaitLock[i] = CreateLock("goodLoaderWaitLock",sizeof("goodLoaderWaitLock"));
	}
}

void initCvForSimulation()
{
	int i;

	g_customerTrolleyCV = CreateCondition((char*)"CustomerTrolleyCV",sizeof("CustomerTrolleyCV"));
    g_managerCashierCV = CreateCondition("managerCashierCV",sizeof("managerCashierCV"));
    g_managerCashierInteractionCV = CreateCondition("managerCashierInteractionCV",
        		sizeof("managerCashierInteractionCV"));
    g_managerCustomerInteractionCV = CreateCondition("managerCustomerInteractionCV",
        		sizeof("managerCustomerInteractionCV"));


	for(i=0;i<NO_OF_CASHIERS;i++)
	{

        g_cashierLineCV[i] = CreateCondition("cashierLineCV",sizeof("cashierLineCV"));
        g_cashierPrivilegedLineCV[i] = CreateCondition("cashierPrivilegedLineCV",
        		sizeof("cashierPrivilegedLineCV"));
        g_customerCashierCV[i] = CreateCondition("customerCashierCV",sizeof("customerCashierCV"));
	}


	for(i=0;i<NO_OF_SALESMAN;i++)
	{

        g_customerSalesmanCV[i] = CreateCondition("customerSalesmanCV",sizeof("customerSalesmanCV"));
	}

	for(i=0;i<NO_OF_DEPARTMENT;i++)
	{

        g_customerDepartmentCV[i] =  CreateCondition("customerDepartmentCV",sizeof("customerDepartmentCV"));
        g_customerDepartmentComplainCV[i] = CreateCondition("customerDepartmentComplainCV",
        		sizeof("customerDepartmentComplainCV"));
	}


	for(i=0;i<NO_OF_GOOD_LOADERS;i++)
	{

        g_salesmanGoodsLoaderCV[i] = CreateCondition("salesmanGoodsLoaderCV",sizeof("salesmanGoodsLoaderCV"));
	}

	for(i=0;i<NO_OF_GOODLOADER_WAIT_QUEUE;i++)
	{

        g_goodLoaderWaitCV[i] = CreateCondition("goodLoaderWaitCV",sizeof("goodLoaderWaitCV"));
	}

}

void initWaitQueueForSimulation()
{
	int i;

	for(i=0;i<NO_OF_CASHIERS;i++)
	{

        g_cashierWaitQueue[i] = 0;
        g_cashierPrivilegedWaitQueue[i] = 0;
	}

	for(i=0;i<NO_OF_DEPARTMENT;i++)
	{
        g_departmentWaitQueue[i] = 0;
        g_departmentComplainWaitQueue[i] = 0;
	}

	for(i=0;i<NO_OF_GOODLOADER_WAIT_QUEUE;i++)
	{
        g_goodLoaderWaitQueue[i] = 0;
	}

	for(i=0;i<NO_OF_CUSTOMERS;i++)
	{
		g_customerListLeft[i] = 0;
	}
}

void printLockForSimulation()
{
	int i;

	Acquire(printLock);
	Print1("customerThreadCounterLock = %u \n",g_customerThreadCounterLock);
	Print1("salesmanThreadCounterLock = %u \n",g_salesmanThreadCounterLock);
	Print1("goodsLoaderThreadCounterLock = %u \n",g_goodsLoaderThreadCounterLock);
	Print1("cashierThreadCounterLock = %u \n",g_cashierThreadCounterLock);
	Print1("customerTrolleyLock = %u \n",g_customerTrolleyLock);
	Print1("managerCashierLock = %u \n",g_managerCashierLock);
	Print1("managerCashierInteractionLock = %u \n",g_managerCashierInteractionLock);
	Print1("managerCustomerInteractionLock = %u \n",g_managerCustomerInteractionLock);

	for(i=0;i<NO_OF_CASHIERS;i++)
	{
    	Print2("cashierLineLock %d = %u \n",i,g_cashierLineLock[i]);
    	Print2("cashierPrivilegedLineLock %d = %u \n",i,g_cashierPrivilegedLineLock[i]);
    	Print2("customerCashierLock %d = %u \n",i,g_customerCashierLock[i]);
    	Print2("managerCashierCashLock %d = %u \n",i,g_managerCashierCashLock[i]);
	}
	for(i=0;i<NO_OF_SALESMAN;i++)
	{
    	Print2("customerSalesmanLock %d = %u \n",i,g_customerSalesmanLock[i]);
	}
	for(i=0;i<NO_OF_DEPARTMENT;i++)
	{
    	Print2("customerDepartmentLock %d = %u \n",i,g_customerDepartmentLock[i]);
    	Print2("customerDepartmentComplainLock %d = %u \n",i,g_customerDepartmentComplainLock[i]);
	}
	for(i =0;i<NO_OF_SHELFS;i++)
	{
		Print2("shelfAccessLock %d = %u \n",i,g_shelfAccessLock[i]);
	}
	for(i=0;i<NO_OF_GOOD_LOADERS;i++)
	{
    	Print2("salesmanGoodsLoaderLock %d = %u \n",i,g_salesmanGoodsLoaderLock[i]);
	}
	for(i=0;i<NO_OF_GOODLOADER_WAIT_QUEUE;i++)
	{
		Print2("goodLoaderWaitLock %d = %u \n",i,g_goodLoaderWaitLock[i]);
	}

	Release(printLock);

}

void printCvForSimulation()
{
	int i;
	Acquire(printLock);
	Print1("customerTrolleyCV = %u \n",g_customerTrolleyCV);

	for(i=0;i<NO_OF_MANAGERS;i++)
	{
    	Print1("managerCashierCV = %u \n",g_managerCashierCV);
    	Print1("managerCashierInteractionCV = %u \n",g_managerCashierInteractionCV);
    	Print1("managerCustomerInteractionCV = %u \n",g_managerCustomerInteractionCV);
	}

	for(i=0;i<NO_OF_CASHIERS;i++)
	{
    	Print2("cashierLineCV %d = %u \n",i,g_cashierLineCV[i]);
    	Print2("cashierPrivilegedLineCV %d = %u \n",i,g_cashierPrivilegedLineCV[i]);
    	Print2("customerCashierCV %d = %u \n",i,g_customerCashierCV[i]);
	}

	for(i=0;i<NO_OF_SALESMAN;i++)
	{
    	Print2("customerSalesmanCV %d = %u \n",i,g_customerSalesmanCV[i]);
	}

	for(i=0;i<NO_OF_DEPARTMENT;i++)
	{
    	Print2("customerDepartmentCV %d = %u \n",i,g_customerDepartmentCV[i]);
    	Print2("customerDepartmentComplainCV %d = %u \n",i,g_customerDepartmentComplainCV[i]);
	}

	for(i=0;i<NO_OF_GOOD_LOADERS;i++)
	{
    	Print2("salesmanGoodsLoaderCV %d = %u \n",i,g_salesmanGoodsLoaderCV[i]);
	}

	for(i=0;i<NO_OF_GOODLOADER_WAIT_QUEUE;i++)
	{
    	Print2("goodLoaderWaitCV %d = %u \n",i, g_goodLoaderWaitCV[i]);
	}
	Release(printLock);
}

int main()
{

#if 1
	int configRetVal;
	int i;

	printLock = CreateLock("printLock",sizeof("printLock"));

	/**
	 *	Acquire(printLock);
    Print("Entering Assign 1 Problem 2 !!!!!!! \n");
    Print("Starting to initialize all the Threads for Problem 2 !!!!!!! \n");

    Print1("Number of Cashiers = %d \n",NO_OF_CASHIERS);
    Print1("Number of Good Loaders = %d \n",NO_OF_GOOD_LOADERS);
    Print1("Number of PrivilegedCustomers = %d \n",NO_OF_CUSTOMERS);
    Print1("Number of Customers = %d \n",NO_OF_CUSTOMERS);
    Print1("Number of Managers = %d \n",NO_OF_MANAGERS);
    Print1("Number of DepartmentSalesmen = %d \n",NO_OF_SALESMAN);
    Release(printLock);
	 */


	initItemInfo();
	initCustomerInfo();
	initCustomerShoppingList();

	Acquire(printLock);
	/**
	 *	printConfiguration();
	 */
	Release(printLock);

    initLockForSimulation();
	initCvForSimulation();
	initWaitQueueForSimulation();

	/**
	 *	printLockForSimulation();
		printCvForSimulation();
	 */


    initSalesManInfo();
    initGoodLoaderInfo();
    initCashierInfo();
    initManagerInfo();

    for(i = 0;i<NO_OF_GOOD_LOADERS;i++)
    {
    	Fork(GoodLoaderThread);
    }

    for(i = 0;i<NO_OF_MANAGERS;i++)
    {
       Fork(ManagerThread);
    }

    for(i = 0;i<NO_OF_CASHIERS;i++)
    {
    	Fork(CashierThread);
    }
    for(i = 0;i<NO_OF_SALESMAN;i++)
    {
    	Fork(SalesmanThread);
    }

    for(i = 0;i<NO_OF_CUSTOMERS;i++)
    {
    	Fork(CustomerThread);
    }


#endif

    Exit(0);

}

/********************************************PROJ1 PROBLEM SIMULATION***********************************************/



