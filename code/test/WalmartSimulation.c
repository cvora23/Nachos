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
int simulationEnd = 0;

int g_customerThreadCounter = 0;
int g_salesmanThreadCounter = 0;
int g_goodsLoaderThreadCounter = 0;
int g_cashierThreadCounter = 0;

unsigned int printLock;

unsigned int g_customerThreadCounterLock;
unsigned int g_salesmanThreadCounterLock;
unsigned int g_goodsLoaderThreadCounterLock;
unsigned int g_cashierThreadCounterLock;

int g_usedTrolleyCount = 0;
int g_waitForTrolleyCount = 0;
unsigned int g_customerTrolleyLock;
unsigned int g_customerTrolleyCV;

unsigned int g_customerSalesmanLock[NO_OF_SALESMAN];
unsigned int g_customerSalesmanCV[NO_OF_SALESMAN];


unsigned int g_customerDepartmentLock[NO_OF_DEPARTMENT];
unsigned int g_customerDepartmentCV[NO_OF_DEPARTMENT];
int	g_departmentWaitQueue[NO_OF_DEPARTMENT];


unsigned int g_shelfAccessLock[NO_OF_SHELFS];

unsigned int g_customerDepartmentComplainLock[NO_OF_DEPARTMENT];
unsigned int g_customerDepartmentComplainCV[NO_OF_DEPARTMENT];
int	g_departmentComplainWaitQueue[NO_OF_DEPARTMENT];

unsigned int g_salesmanGoodsLoaderLock[NO_OF_GOOD_LOADERS];
unsigned int g_salesmanGoodsLoaderCV[NO_OF_GOOD_LOADERS];

unsigned int g_goodLoaderWaitLock[NO_OF_GOODLOADER_WAIT_QUEUE];
unsigned int g_goodLoaderWaitCV[NO_OF_GOODLOADER_WAIT_QUEUE];
int	g_goodLoaderWaitQueue[NO_OF_GOODLOADER_WAIT_QUEUE];

unsigned int g_cashierLineLock[NO_OF_CASHIERS];
unsigned int g_cashierLineCV[NO_OF_CASHIERS];
int	g_cashierWaitQueue[NO_OF_CASHIERS];

unsigned int g_cashierPrivilegedLineLock[NO_OF_CASHIERS];
unsigned int g_cashierPrivilegedLineCV[NO_OF_CASHIERS];
int	g_cashierPrivilegedWaitQueue[NO_OF_CASHIERS];

unsigned int g_customerCashierLock[NO_OF_CASHIERS];
unsigned int g_customerCashierCV[NO_OF_CASHIERS];

unsigned int g_managerCashierCashLock[NO_OF_CASHIERS];

unsigned int g_managerCashierLock;
unsigned int g_managerCashierCV;
int	g_managerWaitQueueLength = 0;

unsigned int g_managerCashierInteractionLock;
unsigned int g_managerCashierInteractionCV;

unsigned int g_managerCustomerInteractionLock;
unsigned int g_managerCustomerInteractionCV;

/*************************************GLOBAL DATA STRUCTS DECLARATION**********************************************/

/********************************************PROJ1 PROBLEM SIMULATION***********************************************/

void initItemInfo()
{
    int i;
	for (i = 0;i<NO_OF_ITEM_TYPES;i++)
	{
		g_itemInfo[i].Price = Random()%MAX_PRICE_PER_ITEM + 1;
		g_itemInfo[i].departmentNo = (int)(i/NO_OF_ITEMS_PER_DEPARTMENT);
		g_itemInfo[i].shelfNo = i;
		g_itemInfo[i].noOfItems = Random()%MAX_NO_ITEMS_PER_SHELF + 1;
	}
}

void initCustomerInfo()
{
    int i,j;
	for(i = 0;i<NO_OF_CUSTOMERS;i++)
	{
		g_customerInfo[i].money = Random()%MAX_AMT_PER_CUSTOMER + 1;
		if(Random()%2 == 0)
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
			g_customerInfo[i].pCustomerShoppingList[j].itemNo = Random()%NO_OF_ITEM_TYPES;
			g_customerInfo[i].pCustomerShoppingList[j].noOfItems =
					Random()%MAX_NO_ITEMS_TO_BE_PURCHASED_OF_EACH_TYPE + 1;
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
		print2("Cashier %d Status is %d \n",i,g_cashierInfo[i].status);
		print2("Cashier %d Customer Id is %d \n",i,g_cashierInfo[i].customerId);
		print2("Cashier %d bill is %d \n",i,g_cashierInfo[i].bill);
		print2("Cashier %d total sales money is %d \n",i,g_cashierInfo[i].totalSalesMoney);
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

	acquireLock(g_customerThreadCounterLock);
	ThreadId = g_customerThreadCounter;
	g_customerThreadCounter++;
	releaseLock(g_customerThreadCounterLock);

    print1("CUSTOMER %d enters the SuperMarket !!!!!!! \n",ThreadId);

    /*************************************CUSTOMER-TROLLEY INTERACTION STARTS HERE*********************************************/

    acquireLock(g_customerTrolleyLock);
    print1("CUSTOMER %d gets in line for a trolley\n",ThreadId);
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
    print1("CUSTOMER %d has a trolley for shopping\n",ThreadId);
    releaseLock(g_customerTrolleyLock);

    /*************************************CUSTOMER-TROLLEY INTERACTION ENDS HERE*********************************************/

    /*************************************CUSTOMER-SALESMAN INTERACTION STARTS HERE*********************************************/

    for(i=0;i<g_customerInfo[ThreadId].noOfItems;i++)
    {

    	print3("CUSTOMER %d wants to shop ITEM_%d in DEPARTMENT_%d \n",
    			ThreadId,
    			g_customerInfo[ThreadId].pCustomerShoppingList[i].itemNo,
    			g_itemInfo[g_customerInfo[ThreadId].pCustomerShoppingList[i].itemNo].departmentNo);

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
    		acquireLock(g_customerSalesmanLock[salesmanIndex]);
    		if(g_salesmanInfo[salesmanIndex].status == salesmanIsFree)
    		{
    			mySalesMan = salesmanIndex;
    			g_salesmanInfo[salesmanIndex].status = salesmanIsBusy;
    			g_salesmanInfo[salesmanIndex].customerId = ThreadId;
    			print3("CUSTOMER %d will enquire about ITEM_%d to SALESMAN_%d\n",ThreadId,
    					currentItemNoFromShoppingList,mySalesMan);
    			break;
    		}
    		releaseLock(g_customerSalesmanLock[salesmanIndex]);
    	}

    	if(mySalesMan == -1)
    	{

    		print3("CUSTOMER %d is waiting in Line for DEPARTMENT_%d as NO SALESMAN is free for ITEM_%d inquiry \n",
    				ThreadId,
    				currentDepartmentNoForItem,
    				currentItemNoFromShoppingList);

    		acquireLock(g_customerDepartmentLock[currentDepartmentNoForItem]);

    		g_departmentWaitQueue[currentDepartmentNoForItem]++;

    		print2("CUSTOMER %d gets in line for the DEPARTMENT_%d \n",ThreadId,
    				currentDepartmentNoForItem);

    		Wait(g_customerDepartmentCV[currentDepartmentNoForItem],g_customerDepartmentLock[currentDepartmentNoForItem]);

        	for(salesmanIndex=salesManStartForDepartment;
        			salesmanIndex<salesManEndForDepartment;salesmanIndex++)
        	{
    			acquireLock(g_customerSalesmanLock[salesmanIndex]);
        		if(g_salesmanInfo[salesmanIndex].status == salesmanSignalToCustomerFromDeptWaitQueue)
        		{
        			mySalesMan = salesmanIndex;
        			print3("CUSTOMER %d assumes that SALESMAN_%d Signaled him regarding enquiry of ITEM_%d\n",
        					ThreadId,mySalesMan,currentItemNoFromShoppingList);
        			g_salesmanInfo[salesmanIndex].status = salesmanIsBusy;
        			g_salesmanInfo[salesmanIndex].customerId = ThreadId;
        			break;
        		}
        		releaseLock(g_customerSalesmanLock[salesmanIndex]);
        	}
        	releaseLock(g_customerDepartmentLock[currentDepartmentNoForItem]);
    	}


		print3("CUSTOMER %d is interacting with SALESMAN_%d from DEPARTMENT_%d \n",
				ThreadId,mySalesMan,currentDepartmentNoForItem);

    	Signal(g_customerSalesmanCV[mySalesMan],g_customerSalesmanLock[mySalesMan]);

    	Wait(g_customerSalesmanCV[mySalesMan],g_customerSalesmanLock[mySalesMan]);

    	Signal(g_customerSalesmanCV[mySalesMan],g_customerSalesmanLock[mySalesMan]);

    	releaseLock(g_customerSalesmanLock[mySalesMan]);


		acquireLock(g_shelfAccessLock[currentItemNoFromShoppingList]);

		if(g_itemInfo[currentItemNoFromShoppingList].noOfItems>=currentItemNoCountFromShoppingList)
		{
			g_itemInfo[currentItemNoFromShoppingList].noOfItems -= currentItemNoCountFromShoppingList;

			print3("CUSTOMER %d has found ITEM_%d and placed ITEM_%d in the trolley \n",ThreadId,
					currentItemNoFromShoppingList,currentItemNoCountFromShoppingList);
			releaseLock(g_shelfAccessLock[currentItemNoFromShoppingList]);
		}
		else
		{
			releaseLock(g_shelfAccessLock[currentItemNoFromShoppingList]);
			mySalesMan = -1;

    		print2("CUSTOMER %d is not able to find ITEM_%d and is searching for SALESMAN's \n",
    				ThreadId,currentItemNoFromShoppingList);

    		for(salesmanIndex=salesManStartForDepartment;
	    			salesmanIndex<salesManEndForDepartment;salesmanIndex++)
	    	{

	    		acquireLock(g_customerSalesmanLock[salesmanIndex]);
	    		if(g_salesmanInfo[salesmanIndex].status == salesmanIsFree)
	    		{
	    			mySalesMan = salesmanIndex;
	    			print3("CUSTOMER %d will enquire about RESTOCK-ITEM_%d to SALESMAN_%d\n",ThreadId,
	    					currentItemNoFromShoppingList,mySalesMan);
	    			g_salesmanInfo[salesmanIndex].status = salesmanIsBusy;
	    			g_salesmanInfo[salesmanIndex].customerId = ThreadId;
	    			g_salesmanInfo[salesmanIndex].itemToRestock = currentItemNoFromShoppingList;
	    			break;
	    		}
	    		releaseLock(g_customerSalesmanLock[salesmanIndex]);
	    	}

	    	if(mySalesMan == -1)
	    	{

	    		print3("CUSTOMER %d is waiting in COMPLAIN LINE for DEPARTMENT_%d AS "
	    				"NO SALESMAN is free for RESTOCK-ITEM_%d inquiry so \n",
	    				ThreadId,
	    				currentDepartmentNoForItem,
	    				currentItemNoFromShoppingList);

	    		acquireLock(g_customerDepartmentComplainLock[currentDepartmentNoForItem]);

	    		g_departmentComplainWaitQueue[currentDepartmentNoForItem]++;

	    		Wait(g_customerDepartmentComplainCV[currentDepartmentNoForItem],
	    				g_customerDepartmentComplainLock[currentDepartmentNoForItem]);

	        	for(salesmanIndex=salesManStartForDepartment;
	        			salesmanIndex<salesManEndForDepartment;salesmanIndex++)
	        	{
	    			acquireLock(g_customerSalesmanLock[salesmanIndex]);
	        		if(g_salesmanInfo[salesmanIndex].status == salesmanSignalToCustomerFromDeptComplainWaitQueue)
	        		{
		    			mySalesMan = salesmanIndex;
	        			print3("CUSTOMER %d assumes that SALESMAN_%d Signaled him regarding inquiry of RESTOCK ITEM_%d\n",
	        					ThreadId,mySalesMan,currentItemNoFromShoppingList);
		    			g_salesmanInfo[salesmanIndex].status = salesmanIsBusy;
		    			g_salesmanInfo[salesmanIndex].customerId = ThreadId;
		    			g_salesmanInfo[salesmanIndex].itemToRestock = currentItemNoFromShoppingList;
	        			break;
	        		}
	        		releaseLock(g_customerSalesmanLock[salesmanIndex]);
	        	}
	        	releaseLock(g_customerDepartmentComplainLock[currentDepartmentNoForItem]);
	    	}

	    	print2("CUSTOMER %d is asking for assistance from SALESMAN_%d \n",
	    			ThreadId,mySalesMan);

	    	Signal(g_customerSalesmanCV[mySalesMan],g_customerSalesmanLock[mySalesMan]);

	    	Wait(g_customerSalesmanCV[mySalesMan],g_customerSalesmanLock[mySalesMan]);

	    	print3("CUSTOMER %d has received assistance about re stocking of ITEM_%d from SALESMAN_%d \n",
	    			ThreadId,currentItemNoFromShoppingList,mySalesMan);

	    	Signal(g_customerSalesmanCV[mySalesMan],g_customerSalesmanLock[mySalesMan]);

			g_itemInfo[currentItemNoFromShoppingList].noOfItems -= currentItemNoCountFromShoppingList;

	    	g_salesmanInfo[mySalesMan].itemToRestock = -1;
	    	g_salesmanInfo[mySalesMan].customerId = -1;

	    	releaseLock(g_customerSalesmanLock[mySalesMan]);

		}

		print2("CUSTOMER %d has finished shopping in  DEPARTMENT_%d \n",
				ThreadId,currentDepartmentNoForItem);
        /*************************************CUSTOMER-SALESMAN INTERACTION ENDS HERE*********************************************/
    }

    print1("CUSTOMER %d has finished shopping for all items \n",ThreadId);

    print1("CUSTOMER %d is looking for the Cashier \n",ThreadId);

    /*******************************************CUSTOMER-CASHIER INTERACTION STARTS HERE*******************************************/
    for(i=0;i<NO_OF_CASHIERS;i++)
    {
    	if(g_customerInfo[ThreadId].type == PRIVILEGED)
    	{
    		acquireLock(g_cashierPrivilegedLineLock[i]);
    		if(g_cashierPrivilegedWaitQueue[i]<minCashierLineLength)
    		{
    			minCashierLineLength = g_cashierPrivilegedWaitQueue[i];
    			myCashier = i;
    		}
    		releaseLock(g_cashierPrivilegedLineLock[i]);
    	}
    	else
    	{
    		acquireLock(g_cashierLineLock[i]);
    		if(g_cashierPrivilegedWaitQueue[i]<minCashierLineLength)
    		{
    			minCashierLineLength = g_cashierWaitQueue[i];
    			myCashier = i;
    		}
    		releaseLock(g_cashierLineLock[i]);
    	}
    }

	if(g_customerInfo[ThreadId].type == PRIVILEGED)
	{
		print3("CUSTOMER %d chose CASHIER_%d with Privileged Line of length %d \n",
				ThreadId,myCashier,minCashierLineLength);
	}
	else
	{
		print3("CUSTOMER %d chose CASHIER_%d with Line of length %d \n",
				ThreadId,myCashier,minCashierLineLength);
	}

    acquireLock(g_customerCashierLock[myCashier]);
    if(g_cashierInfo[myCashier].status == cashierIsFree)
    {
    	g_cashierInfo[myCashier].status = cashierIsBusy;
    	g_cashierInfo[myCashier].customerId = ThreadId;
    }
    else
    {
    	if(g_customerInfo[ThreadId].type == PRIVILEGED)
    	{
    		acquireLock(g_cashierPrivilegedLineLock[myCashier]);
    	    releaseLock(g_customerCashierLock[myCashier]);
    	    g_cashierPrivilegedWaitQueue[myCashier]++;
    	    Wait(g_cashierPrivilegedLineCV[myCashier],g_cashierPrivilegedLineLock[myCashier]);
    	}
    	else
    	{
    		acquireLock(g_cashierLineLock[myCashier]);
    	    releaseLock(g_customerCashierLock[myCashier]);
    	    g_cashierWaitQueue[myCashier]++;
    	    Wait(g_cashierLineCV[myCashier],g_cashierLineLock[myCashier]);
    	}

    	acquireLock(g_customerCashierLock[myCashier]);
    	g_cashierInfo[myCashier].status = cashierIsBusy;
    	g_cashierInfo[myCashier].customerId = ThreadId;

    	if(g_customerInfo[ThreadId].type == PRIVILEGED)
    	{
    		releaseLock(g_cashierPrivilegedLineLock[myCashier]);
    	}
    	else
    	{
    		releaseLock(g_cashierLineLock[myCashier]);
    	}
    }

    Signal(g_customerCashierCV[myCashier],g_customerCashierLock[myCashier]);

    Wait(g_customerCashierCV[myCashier],g_customerCashierLock[myCashier]);

    myBill = g_cashierInfo[myCashier].bill;

    if(myBill <= g_customerInfo[ThreadId].money)
    {
    	g_customerInfo[ThreadId].hasEnoughMoneyForShopping = true;
    	print3("CUSTOMER %d pays %d amount to CASHIER_%d and is waiting for the receipt\n",
    			ThreadId,myBill,myCashier);
    }
    else
    {
    	g_customerInfo[ThreadId].hasEnoughMoneyForShopping = false;
    	print2("CUSTOMER %d cannot pay %d\n",
    			ThreadId,myBill);
    }

    Signal(g_customerCashierCV[myCashier],g_customerCashierLock[myCashier]);

    Wait(g_customerCashierCV[myCashier],g_customerCashierLock[myCashier]);

    if(g_customerInfo[ThreadId].hasEnoughMoneyForShopping == true)
    {
    	print2("CUSTOMER %d got receipt from CASHIER_%d and is now leaving\n",
    			ThreadId,myCashier);
    }

	print2("CUSTOMER %d signaled CASHIER_%d that I am leaving\n",
			ThreadId,myCashier);

    Signal(g_customerCashierCV[myCashier],g_customerCashierLock[myCashier]);

    acquireLock(g_managerCustomerInteractionLock);

    releaseLock(g_customerCashierLock[myCashier]);

    /*************************************CUSTOMER - CASHIER INTERACTION ENDS HERE*********************************************/

    /*************************************CUSTOMER - MANAGER INTERACTION STARTS HERE *******************************************/

    if(g_customerInfo[ThreadId].hasEnoughMoneyForShopping == false)
    {
    	print1("CUSTOMER %d is waiting for MANAGER for negotiations\n",ThreadId);
    	g_managerInfo.customerBill = myBill;
    	g_managerInfo.customerId = ThreadId;

    	Signal(g_managerCustomerInteractionCV,g_managerCustomerInteractionLock);

    	Wait(g_managerCustomerInteractionCV,g_managerCustomerInteractionLock);
    	while(myBill<=g_customerInfo[ThreadId].hasEnoughMoneyForShopping)
    	{
    		Signal(g_managerCustomerInteractionCV,g_managerCustomerInteractionLock);
    		print2("CUSTOMER %d tells MANAGER to remove ITEM_%d from trolley\n",
    				ThreadId,
    				g_customerInfo[ThreadId].pCustomerShoppingList[currentItemNoCountToRemoveFromTrolley].itemNo);
        	Wait(g_managerCustomerInteractionCV,g_managerCustomerInteractionLock);
        	myBill = g_managerInfo.customerBill;
        	currentItemNoCountToRemoveFromTrolley++;
    	}

    	g_customerInfo[ThreadId].isDoneShopping = true;

    	print2("CUSTOMER %d pays $ %d MANAGER after removing items and is waiting for receipt from MANAGER\n",
    			ThreadId,myBill);

		Signal(g_managerCustomerInteractionCV,g_managerCustomerInteractionLock);
    	Wait(g_managerCustomerInteractionCV,g_managerCustomerInteractionLock);

    	print1("CUSTOMER %d got receipt from MANAGER and is now leaving\n",ThreadId);

    	releaseLock(g_managerCustomerInteractionLock);
    }

    else
    {
    	releaseLock(g_managerCustomerInteractionLock);
    }

    /*************************************CUSTOMER - MANAGER INTERACTION ENDS HERE *******************************************/

    print1("CUSTOMER %d left the shopping mall !!!!! \n",ThreadId);

    acquireLock(g_customerTrolleyLock);
    g_usedTrolleyCount--;
    if(g_waitForTrolleyCount>0)
    {
    	Signal(g_customerTrolleyCV,g_customerTrolleyLock);
    }
    g_noOfCustomersLeft++;
    releaseLock(g_customerTrolleyLock);

    Exit(0);
}

void SalesmanThread()
{
	int ThreadId;
    int myDepartmentNo = g_salesmanInfo[ThreadId].departmentNo;
    int myGoodsLoader = -1;
	int goodLoaderIndex;


	acquireLock(g_salesmanThreadCounterLock);
	ThreadId = g_salesmanThreadCounter;
	g_salesmanThreadCounter++;
	releaseLock(g_salesmanThreadCounterLock);

    print1( "SALESMAN %d Started !!!!!!! \n",ThreadId);
    print2("SALESMAN %d will be working for DEPARTMENT_%d \n",
    		ThreadId,g_salesmanInfo[ThreadId].departmentNo);

    while(1)
    {
    	acquireLock(g_customerDepartmentLock[myDepartmentNo]);

    	if(g_departmentWaitQueue[myDepartmentNo]>0)
    	{

    		print2("SALESMAN %d ---- DEPARTMENT_%d WAIT QUEUE is not Empty... Someone Waiting for Assistance \n",
    				ThreadId,myDepartmentNo);

    		g_departmentWaitQueue[myDepartmentNo]--;

    		acquireLock(g_customerSalesmanLock[ThreadId]);

    		g_salesmanInfo[ThreadId].status = salesmanSignalToCustomerFromDeptWaitQueue;

    		Signal(g_customerDepartmentCV[myDepartmentNo],g_customerDepartmentLock[myDepartmentNo]);

    		print2("%SALESMAN %d signaling someone waiting in DEPARTMENT_%d WAIT QUEUE\n",ThreadId,
    				myDepartmentNo);

    		releaseLock(g_customerDepartmentLock[myDepartmentNo]);

    		print2("SALESMAN %d waiting for response from UNKNOWN CUSTOMER now in DEPARTMENT_%d WAIT QUEUE \n",
    				ThreadId,myDepartmentNo);

    		Wait(g_customerSalesmanCV[ThreadId],g_customerSalesmanLock[ThreadId]);

    		print3("SALESMAN %d GOT INITIAL RESPONSE from CUSTOMER_%d now in DEPARTMENT_%d WAIT QUEUE\n",
    				ThreadId,g_salesmanInfo[ThreadId].customerId,myDepartmentNo);

    		Signal(g_customerSalesmanCV[ThreadId],g_customerSalesmanLock[ThreadId]);

    		print3("SALESMAN %d SENDING RESPONSE to CUSTOMER_%d now in DEPARTMENT_%d WAIT QUEUE \n",
    				ThreadId,g_salesmanInfo[ThreadId].customerId,myDepartmentNo);


    		print3("SALESMAN %d welcomes CUSTOMER_%d to DEPARTMENT_%d \n",
    				ThreadId,g_salesmanInfo[ThreadId].customerId,myDepartmentNo);

    		Wait(g_customerSalesmanCV[ThreadId],g_customerSalesmanLock[ThreadId]);

    		print2("SALESMAN %d GOT FINAL RESPONSE from CUSTOMER_%d now \n",ThreadId,g_salesmanInfo[ThreadId].customerId);

    		releaseLock(g_customerSalesmanLock[ThreadId]);
    	}
        else
        {
        	releaseLock(g_customerDepartmentLock[myDepartmentNo]);
        }

    	acquireLock(g_customerDepartmentComplainLock[myDepartmentNo]);

    	if(g_departmentComplainWaitQueue[myDepartmentNo]>0)
    	{

    		print2("SALESMAN %d ---- DEPARTMENT_%d COMPLAIN WAIT QUEUE is not Empty... Someone Waiting for Assistance \n",
    				ThreadId,myDepartmentNo);

    		g_departmentComplainWaitQueue[myDepartmentNo]--;

    		acquireLock(g_customerSalesmanLock[ThreadId]);

    		g_salesmanInfo[ThreadId].status = salesmanSignalToCustomerFromDeptComplainWaitQueue;

    		Signal(g_customerDepartmentComplainCV[myDepartmentNo],g_customerDepartmentComplainLock[myDepartmentNo]);

    		print2("SALESMAN %d signaling someone waiting in DEPARTMENT_%d COMPLAIN WAIT QUEUE \n",ThreadId,
    				myDepartmentNo);

    		releaseLock(g_customerDepartmentComplainLock[myDepartmentNo]);

    		print2("SALESMAN %d waiting for response from UNKNOWN CUSTOMER now in DEPARTMENT_%d COMPLAIN WAIT QUEUE \n",
    				ThreadId,myDepartmentNo);

    		Wait(g_customerSalesmanCV[ThreadId],g_customerSalesmanLock[ThreadId]);

    		print3("SALESMAN %d is informed by CUSTOMER_%d that ITEM_%d is out of stock \n",
    				ThreadId,
    				g_salesmanInfo[ThreadId].customerId,
    				g_salesmanInfo[ThreadId].itemToRestock);


    		/**************************START INTERACTING WITH GOODS LOADER NOW ***************************/

    		for(goodLoaderIndex=0;goodLoaderIndex<NO_OF_GOOD_LOADERS;goodLoaderIndex++)
    		{
    			acquireLock(g_salesmanGoodsLoaderLock[goodLoaderIndex]);

    			if(g_goodLoaderInfo[goodLoaderIndex].status == goodLoaderIsFree)
    			{
    				myGoodsLoader = goodLoaderIndex;
	    			print3("SALESMAN %d will enquire about RESTOCK-ITEM_%d to GOODLOADER_%d\n",ThreadId,
	    					g_salesmanInfo[ThreadId].itemToRestock,myGoodsLoader);
    				g_goodLoaderInfo[goodLoaderIndex].status = goodLoaderIsBusy;
    				g_goodLoaderInfo[goodLoaderIndex].salesmanId = ThreadId;
    				break;
    			}
    			releaseLock(g_salesmanGoodsLoaderLock[goodLoaderIndex]);
    		}

    		if(myGoodsLoader == -1)
    		{
	    		print2("SALESMAN %d is waiting in SINGLE LINE for GOOD LOADERS as "
	    				" NO GOODLOADER is free for RESTOCK-ITEM_%d inquiry so "
	    				"\n",ThreadId,
	    				g_salesmanInfo[ThreadId].itemToRestock);

    			acquireLock(g_goodLoaderWaitLock[0]);
    			g_goodLoaderWaitQueue[0]++;

    			Wait(g_goodLoaderWaitCV[0],g_goodLoaderWaitLock[0]);
        		for(goodLoaderIndex=0;goodLoaderIndex<NO_OF_GOOD_LOADERS;goodLoaderIndex++)
        		{
        			acquireLock(g_salesmanGoodsLoaderLock[goodLoaderIndex]);

        			if(g_goodLoaderInfo[goodLoaderIndex].status == goodLoaderSignalToSalesman)
        			{
        				myGoodsLoader = goodLoaderIndex;
	        			print3("SALESMAN %d assumes that GOOD LOADER_%d Signaled him regarding inquiry of RESTOCK ITEM_%d\n",
	        					ThreadId,myGoodsLoader,g_salesmanInfo[ThreadId].itemToRestock);
        				g_goodLoaderInfo[goodLoaderIndex].status = goodLoaderIsBusy;
        				g_goodLoaderInfo[goodLoaderIndex].salesmanId = ThreadId;
        				break;
        			}
        			releaseLock(g_salesmanGoodsLoaderLock[goodLoaderIndex]);
        		}
    		}

    		releaseLock(g_goodLoaderWaitLock[0]);

    		g_goodLoaderInfo[myGoodsLoader].itemToRestock = g_salesmanInfo[ThreadId].itemToRestock;

    		Signal(g_salesmanGoodsLoaderCV[myGoodsLoader],g_salesmanGoodsLoaderLock[myGoodsLoader]);

    		print3("SALESMAN %d informs GOOD LOADER_%d that ITEM_%d is out of stock \n",
    				ThreadId,myGoodsLoader,g_salesmanInfo[ThreadId].itemToRestock);

    		Wait(g_salesmanGoodsLoaderCV[myGoodsLoader],g_salesmanGoodsLoaderLock[myGoodsLoader]);

    		print3("SALESMAN %d is informed by the GOOD LOADER_%d that ITEM_%d is re stocked \n",
    				ThreadId,myGoodsLoader,g_salesmanInfo[ThreadId].itemToRestock);

    		releaseLock(g_salesmanGoodsLoaderLock[myGoodsLoader]);

    		/**************************END INTERACTING WITH GOODS LOADER NOW ***************************/

    		Signal(g_customerSalesmanCV[ThreadId],g_customerSalesmanLock[ThreadId]);

    		print3("SALESMAN %d SENDING RESPONSE to CUSTOMER_%d now in DEPARTMENT_%d COMPLAIN WAIT QUEUE \n",
    				ThreadId,g_salesmanInfo[ThreadId].customerId,myDepartmentNo);

    		print3("SALESMAN %d informs the CUSTOMER_%d that ITEM_%d is re stocked \n",
    				ThreadId,g_salesmanInfo[ThreadId].customerId,
    				g_salesmanInfo[ThreadId].itemToRestock);

    		Wait(g_customerSalesmanCV[ThreadId],g_customerSalesmanLock[ThreadId]);

    		print2("SALESMAN %d GOT FINAL RESPONSE from CUSTOMER_%d now \n",ThreadId,g_salesmanInfo[ThreadId].customerId);

    		myGoodsLoader = -1;
    		g_salesmanInfo[ThreadId].itemToRestock = -1;

    		releaseLock(g_customerSalesmanLock[ThreadId]);
    	}
        else
        {
        	releaseLock(g_customerDepartmentComplainLock[myDepartmentNo]);
        }

    	acquireLock(g_customerDepartmentLock[myDepartmentNo]);
    	acquireLock(g_customerDepartmentComplainLock[myDepartmentNo]);

    	if(g_departmentWaitQueue[myDepartmentNo] == 0 &&
    			g_departmentComplainWaitQueue[myDepartmentNo] == 0)
    	{

    		print3("SALESMAN %d is RELAXING AS DEPARTMENT_%d WAIT QUEUE and DEPARTMENT_%d "
    				"COMPLAIN WAIT QUEUE both are empty"
    				"SET STATUS TO FREE\n ",ThreadId,myDepartmentNo,myDepartmentNo);

    		acquireLock(g_customerSalesmanLock[ThreadId]);

    		g_salesmanInfo[ThreadId].status = salesmanIsFree;

    		releaseLock(g_customerDepartmentComplainLock[myDepartmentNo]);

    		releaseLock(g_customerDepartmentLock[myDepartmentNo]);

    		Wait(g_customerSalesmanCV[ThreadId],g_customerSalesmanLock[ThreadId]);

    		print2("SALESMAN %d GOT INITIAL RESPONSE from CUSTOMER_%d in NO WAIT QUEUE\n",
    				ThreadId,g_salesmanInfo[ThreadId].customerId);



    		if(g_salesmanInfo[ThreadId].itemToRestock == -1)
    		{
    			print1("SALESMAN %d is checking if INITIAL RESPONSE is not regarding RE-STOCK of Item in NO WAIT QUEUE\n",
    					ThreadId);

    			g_salesmanInfo[ThreadId].status = salesmanSignalToCustomerFromDeptWaitQueue;

    			Signal(g_customerSalesmanCV[ThreadId],g_customerSalesmanLock[ThreadId]);

        		print2("SALESMAN %d SENDING RESPONSE to CUSTOMER_%d in NO WAIT QUEUE \n",
        				ThreadId,g_salesmanInfo[ThreadId].customerId);

        		print3("SALESMAN %d welcomes CUSTOMER_%d to DEPARTMENT_%d \n",
        				ThreadId,g_salesmanInfo[ThreadId].customerId,myDepartmentNo);

    			Wait(g_customerSalesmanCV[ThreadId],g_customerSalesmanLock[ThreadId]);

        		print2("SALESMAN %d GOT FINAL RESPONSE from CUSTOMER_%d in NO WAIT QUEUE \n",
        				ThreadId,g_salesmanInfo[ThreadId].customerId);

        		releaseLock(g_customerSalesmanLock[ThreadId]);
    		}
    		else
    		{
    			print1("SALESMAN %d is IF INITIAL RESPONSE is regarding RE-STOCK of Item in NO WAIT QUEUE\n",ThreadId);

    			g_salesmanInfo[ThreadId].status = salesmanSignalToCustomerFromDeptComplainWaitQueue;

        		for(goodLoaderIndex=0;goodLoaderIndex<NO_OF_GOOD_LOADERS;goodLoaderIndex++)
        		{
        			acquireLock(g_salesmanGoodsLoaderLock[goodLoaderIndex]);

        			if(g_goodLoaderInfo[goodLoaderIndex].status == goodLoaderIsFree)
        			{
        				myGoodsLoader = goodLoaderIndex;
        	    		print2(" SALESMAN %d is waiting in SINGLE LINE for GOOD LOADERS NO WAIT QUEUE as "
        	    				"NO GOODLOADER is free for RESTOCK-ITEM_%d inquiry so "
        	    				"\n",ThreadId,
        	    				g_salesmanInfo[ThreadId].itemToRestock
        	    				);
        				g_goodLoaderInfo[goodLoaderIndex].status = goodLoaderIsBusy;
        				g_goodLoaderInfo[goodLoaderIndex].salesmanId = ThreadId;
        				break;
        			}
        			releaseLock(g_salesmanGoodsLoaderLock[goodLoaderIndex]);
        		}

        		if(myGoodsLoader == -1)
        		{
    	    		print2("SALESMAN %d is waiting in SINGLE LINE for "
    	    				"GOOD LOADERS NO WAIT QUEUE AS NO GOODLOADER is free for RESTOCK-ITEM_%d inquiry so \n",
    	    				ThreadId,
    	    				g_salesmanInfo[ThreadId].itemToRestock
    	    				);

        			acquireLock(g_goodLoaderWaitLock[0]);
        			g_goodLoaderWaitQueue[0]++;

        			Wait(g_goodLoaderWaitCV[0],g_goodLoaderWaitLock[0]);

            		for(goodLoaderIndex=0;goodLoaderIndex<NO_OF_GOOD_LOADERS;goodLoaderIndex++)
            		{
            			acquireLock(g_salesmanGoodsLoaderLock[goodLoaderIndex]);

            			if(g_goodLoaderInfo[goodLoaderIndex].status == goodLoaderSignalToSalesman)
            			{
            				myGoodsLoader = goodLoaderIndex;
    	        			print3("SALESMAN %d assumes that GOOD LOADER_%d Signaled him regarding inquiry of RESTOCK ITEM_%d\n",
    	        					ThreadId,myGoodsLoader,g_salesmanInfo[ThreadId].itemToRestock);
            				g_goodLoaderInfo[goodLoaderIndex].status = goodLoaderIsBusy;
            				g_goodLoaderInfo[goodLoaderIndex].salesmanId = ThreadId;
            				break;
            			}
            			releaseLock(g_salesmanGoodsLoaderLock[goodLoaderIndex]);
            		}
        		}

        		releaseLock(g_goodLoaderWaitLock[0]);

        		g_goodLoaderInfo[myGoodsLoader].itemToRestock = g_salesmanInfo[ThreadId].itemToRestock;

        		Signal(g_salesmanGoodsLoaderCV[myGoodsLoader],g_salesmanGoodsLoaderLock[myGoodsLoader]);

        		print2("SALESMAN %d SENDING RESPONSE to CUSTOMER_%d now in NO WAIT QUEUE \n",
        				ThreadId,g_salesmanInfo[ThreadId].customerId);

        		print3("SALESMAN %d informs GOOD LOADER_%d that ITEM_%d is out of stock \n",
        				ThreadId,myGoodsLoader,g_salesmanInfo[ThreadId].itemToRestock);

        		Wait(g_salesmanGoodsLoaderCV[myGoodsLoader],g_salesmanGoodsLoaderLock[myGoodsLoader]);

        		print3("SALESMAN %d is informed by the GOOD LOADER_%d that ITEM_%d is re stocked \n",
        				ThreadId,myGoodsLoader,g_salesmanInfo[ThreadId].itemToRestock);

        		releaseLock(g_salesmanGoodsLoaderLock[myGoodsLoader]);

        		Signal(g_customerSalesmanCV[ThreadId],g_customerSalesmanLock[ThreadId]);

        		print3("SALESMAN %d informs the CUSTOMER_%d that ITEM_%d is re stocked \n",
        				ThreadId,g_salesmanInfo[ThreadId].customerId,
        				g_salesmanInfo[ThreadId].itemToRestock);

        		Wait(g_customerSalesmanCV[ThreadId],g_customerSalesmanLock[ThreadId]);

        		print2("SALESMAN %d GOT FINAL RESPONSE from CUSTOMER_%d now \n",ThreadId,g_salesmanInfo[ThreadId].customerId);

        		myGoodsLoader = -1;
        		g_salesmanInfo[ThreadId].itemToRestock = -1;

        		releaseLock(g_customerSalesmanLock[ThreadId]);
    		}
    	}
    	else
    	{
    		releaseLock(g_customerDepartmentLock[myDepartmentNo]);
    		releaseLock(g_customerDepartmentComplainLock[myDepartmentNo]);
    	}
    }
}

void GoodLoaderThread()
{
    int mySalesman = -1;
    int ThreadId;
    int i;

	acquireLock(g_goodsLoaderThreadCounterLock);
	ThreadId = g_goodsLoaderThreadCounter;
	g_goodsLoaderThreadCounter++;
	releaseLock(g_goodsLoaderThreadCounterLock);

	print1( "GOODSLOADER %d Started !!!!!!! \n",ThreadId);

    while(1)
    {
    	int goodsLoaderWalkingTime;
    	goodsLoaderWalkingTime = Random()%(MAX_GOODS_LOADER_WALKINGTIME-MIN_GOODS_LOADER_WALKINGTIME) +
    			MIN_GOODS_LOADER_WALKINGTIME;

    	acquireLock(g_goodLoaderWaitLock[0]);

    	if(g_goodLoaderWaitQueue[0]>0)
    	{
    		print1("GOODSLOADER %d GOOD LOADER WAIT QUEUE IS NOT EMPTY...RE-STOCK OF SOME ITEM \n",ThreadId);

    		g_goodLoaderWaitQueue[0]--;

    		acquireLock(g_salesmanGoodsLoaderLock[ThreadId]);

    		g_goodLoaderInfo[ThreadId].status = goodLoaderSignalToSalesman;

    		Signal(g_goodLoaderWaitCV[0],g_goodLoaderWaitLock[0]);

    		print1("GOODSLOADER %d signaling someone waiting in GOOD LOADERS WAIT QUEUE\n",ThreadId);

    		releaseLock(g_goodLoaderWaitLock[0]);

    		Wait(g_salesmanGoodsLoaderCV[ThreadId],g_salesmanGoodsLoaderLock[ThreadId]);

    		print3("GOODSLOADER %d is informed by SALESMAN_%d of DEPARTMENT_%d ",
    				ThreadId,g_goodLoaderInfo[ThreadId].salesmanId,
    				g_salesmanInfo[g_goodLoaderInfo[ThreadId].salesmanId].departmentNo);
    				print1("to re stock ITEM_%d \n",g_goodLoaderInfo[ThreadId].itemToRestock);

    		acquireLock(g_shelfAccessLock[g_goodLoaderInfo[ThreadId].itemToRestock]);

    		g_itemInfo[g_goodLoaderInfo[ThreadId].itemToRestock].noOfItems = MAX_NO_ITEMS_PER_SHELF;

    		releaseLock(g_shelfAccessLock[g_goodLoaderInfo[ThreadId].itemToRestock]);

    		for(i=0;i<goodsLoaderWalkingTime;i++)
    		{
    			Yield();
    			print2("GOODSLOADER %d is walking from RE-STOCK room to shelf to RE-STOCK ITEM_%d\n",ThreadId,
    					g_goodLoaderInfo[ThreadId].itemToRestock);
    		}

    		Signal(g_salesmanGoodsLoaderCV[ThreadId],g_salesmanGoodsLoaderLock[ThreadId]);

    		print3("GOODSLOADER %d has re stocked ITEM_%d in DEPARTMENT_%d \n",
    				ThreadId,
    				g_goodLoaderInfo[ThreadId].itemToRestock,
    				g_salesmanInfo[g_goodLoaderInfo[ThreadId].salesmanId].departmentNo);

    		releaseLock(g_salesmanGoodsLoaderLock[ThreadId]);
    	}
    	else
    	{
    		releaseLock(g_goodLoaderWaitLock[0]);
    	}

    	acquireLock(g_goodLoaderWaitLock[0]);

    	if(g_goodLoaderWaitQueue[0] == 0)
    	{
    		print1("GOODSLOADER %d GOOD LOADER WAIT QUEUE IS EMPTY...RELAX \n",ThreadId);

    		print1("GOODSLOADER %d is waiting for orders to Re stock\n",ThreadId);

    		acquireLock(g_salesmanGoodsLoaderLock[ThreadId]);
    		g_goodLoaderInfo[ThreadId].status = goodLoaderIsFree;

    		releaseLock(g_goodLoaderWaitLock[0]);
    		Wait(g_salesmanGoodsLoaderCV[ThreadId],g_salesmanGoodsLoaderLock[ThreadId]);

    		print3("GOODSLOADER %d is informed by SALESMAN_%d of DEPARTMENT_%d ",
    				ThreadId,g_goodLoaderInfo[ThreadId].salesmanId,
    				g_salesmanInfo[g_goodLoaderInfo[ThreadId].salesmanId].departmentNo);
    				print1("to re stock ITEM_%d \n",g_goodLoaderInfo[ThreadId].itemToRestock);

    		acquireLock(g_shelfAccessLock[g_goodLoaderInfo[ThreadId].itemToRestock]);

    		g_itemInfo[g_goodLoaderInfo[ThreadId].itemToRestock].noOfItems = MAX_NO_ITEMS_PER_SHELF;

    		releaseLock(g_shelfAccessLock[g_goodLoaderInfo[ThreadId].itemToRestock]);


    		for(i=0;i<goodsLoaderWalkingTime;i++)
    		{
    			Yield();
    			print2("GOODSLOADER %d is walking from RE-STOCK room to shelf to RE-STOCK ITEM_%d\n",ThreadId,
    					g_goodLoaderInfo[ThreadId].itemToRestock);
    		}

    		Signal(g_salesmanGoodsLoaderCV[ThreadId],g_salesmanGoodsLoaderLock[ThreadId]);

    		print3("GOODSLOADER %d has re stocked ITEM_%d in DEPARTMENT_%d \n",
    				ThreadId,
    				g_goodLoaderInfo[ThreadId].itemToRestock,
    				g_salesmanInfo[g_goodLoaderInfo[ThreadId].salesmanId].departmentNo);

    		releaseLock(g_salesmanGoodsLoaderLock[ThreadId]);
    	}

    	else
    	{
    		releaseLock(g_goodLoaderWaitLock[0]);
    	}
    }
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

	acquireLock(g_cashierThreadCounterLock);
	ThreadId = g_cashierThreadCounter;
	g_cashierThreadCounter++;
	releaseLock(g_cashierThreadCounterLock);

	acquireLock(printLock);
    print1( "CASHIER %d Started \n",ThreadId);
    releaseLock(printLock);

    while(1)
    {
    	myCustomer = -1;
    	totalBill = 0;
        currentItemNoFromShoppingList = 0;
        currentItemNoCountFromShoppingList = 0;
        currentItemNoPriceFromShoppingList = 0;

        acquireLock(g_cashierPrivilegedLineLock[ThreadId]);
    	if(g_cashierPrivilegedWaitQueue[ThreadId]>0)
    	{
    		g_cashierPrivilegedWaitQueue[ThreadId]--;
    		g_cashierInfo[ThreadId].status = cashierSignalToCustomer;
    		Signal(g_cashierPrivilegedLineCV[ThreadId],g_cashierPrivilegedLineLock[ThreadId]);
    		acquireLock(g_customerCashierLock[ThreadId]);
    		releaseLock(g_cashierPrivilegedLineLock[ThreadId]);
    	}
    	else
    	{
    		releaseLock(g_cashierPrivilegedLineLock[ThreadId]);

    		acquireLock(g_cashierLineLock[ThreadId]);

    		if(g_cashierWaitQueue[ThreadId]>0)
    		{
    			g_cashierWaitQueue[ThreadId]--;
        		g_cashierInfo[ThreadId].status = cashierSignalToCustomer;
        		Signal(g_cashierLineCV[ThreadId],g_cashierLineLock[ThreadId]);
        		acquireLock(g_customerCashierLock[ThreadId]);
    		}
    		else
    		{
    			acquireLock(g_customerCashierLock[ThreadId]);
    			g_cashierInfo[ThreadId].status = cashierIsFree;
    		}
    		releaseLock(g_cashierLineLock[ThreadId]);
    	}
    	acquireLock(printLock);
		print2("Cashier %d total sales money is %d \n",ThreadId,g_cashierInfo[ThreadId].totalSalesMoney);
		releaseLock(printLock);

    	Wait(g_customerCashierCV[ThreadId],g_customerCashierLock[ThreadId]);
    	myCustomer = g_cashierInfo[ThreadId].customerId;

    	for(i=0;i<g_customerInfo[myCustomer].noOfItems;i++)
    	{
        	currentItemNoFromShoppingList = g_customerInfo[myCustomer].pCustomerShoppingList[i].itemNo;
          	currentItemNoCountFromShoppingList = g_customerInfo[myCustomer].pCustomerShoppingList[i].noOfItems;
          	currentItemNoPriceFromShoppingList = g_itemInfo[currentItemNoFromShoppingList].Price;
          	totalBill +=  currentItemNoPriceFromShoppingList * currentItemNoCountFromShoppingList ;
        	acquireLock(printLock);
          	print2("CASHIER %d got ITEM_%d from trolley\n",ThreadId,currentItemNoFromShoppingList);
          	releaseLock(printLock);
    	}

    	g_cashierInfo[ThreadId].bill = totalBill;

    	Signal(g_customerCashierCV[ThreadId],g_customerCashierLock[ThreadId]);
    	acquireLock(printLock);
    	print3("CASHIER %d tells CUSTOMER_%d total cost is $ %d\n",ThreadId,myCustomer,totalBill);
    	releaseLock(printLock);

    	Wait(g_customerCashierCV[ThreadId],g_customerCashierLock[ThreadId]);

    	if(g_customerInfo[myCustomer].hasEnoughMoneyForShopping == false)
    	{
    		acquireLock(g_managerCashierLock);

    		g_managerWaitQueueLength++;

    		Wait(g_managerCashierCV,g_managerCashierLock);

    		g_managerInfo.cashierId = ThreadId;

    		acquireLock(g_managerCashierInteractionLock);

    		releaseLock(g_managerCashierLock);

    		Signal(g_managerCashierInteractionCV,g_managerCashierInteractionLock);
        	acquireLock(printLock);
    		print2("CASHIER %d informs Manager that CUSTOMER_%d does not have enough money\n",
    				ThreadId,myCustomer);
        	releaseLock(printLock);

    		Wait(g_managerCashierInteractionCV,g_managerCashierInteractionLock);

    		releaseLock(g_managerCashierInteractionLock);

        	Signal(g_customerCashierCV[ThreadId],g_customerCashierLock[ThreadId]);
        	acquireLock(printLock);
        	print2("CASHIER %d asks CUSTOMER_%d to wait for Manager\n",ThreadId,myCustomer);
        	releaseLock(printLock);
        	Wait(g_customerCashierCV[ThreadId],g_customerCashierLock[ThreadId]);

    	}
    	else
    	{
    		acquireLock(g_managerCashierCashLock[ThreadId]);
    		g_cashierInfo[ThreadId].totalSalesMoney += totalBill;
    		releaseLock(g_managerCashierCashLock[ThreadId]);
        	acquireLock(printLock);
    		print3("CASHIER %d got money $ %d from CUSTOMER_%d\n",ThreadId,totalBill,myCustomer);
        	releaseLock(printLock);
        	Signal(g_customerCashierCV[ThreadId],g_customerCashierLock[ThreadId]);
        	acquireLock(printLock);
        	print2("CASHIER %d gave the receipt to CUSTOMER_%d and tells him to leave\n",
        			ThreadId,myCustomer);
        	releaseLock(printLock);
        	Wait(g_customerCashierCV[ThreadId],g_customerCashierLock[ThreadId]);
    	}

    	releaseLock(g_customerCashierLock[ThreadId]);

    }
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

	acquireLock(printLock);
	print1( "MANAGER %d Started  \n",ThreadId);
	releaseLock(printLock);

    while(!simulationEnd)
    {
    	itemRemoveCounter = 0;

    	for(i=0;i<NO_OF_CASHIERS;i++)
    	{
    		acquireLock(g_managerCashierCashLock[i]);
    		acquireLock(printLock);
    		print2("Total Sales Money From CASHIER %d is %d\n",i,g_cashierInfo[i].totalSalesMoney);
    		releaseLock(printLock);
    		g_managerInfo.totalRevenue += g_cashierInfo[i].totalSalesMoney;
    		g_cashierInfo[i].totalSalesMoney = 0;
    		releaseLock(g_managerCashierCashLock[i]);
    	}

    	g_managerInfo.totalRevenue += managerSales;
    	managerSales = 0;

    	acquireLock(printLock);
    	print1("Total Sale of the entire store until now is $ %d \n",g_managerInfo.totalRevenue);
    	releaseLock(printLock);

    	acquireLock(g_managerCashierLock);
    	if(g_managerWaitQueueLength == 0)
    	{
    		releaseLock(g_managerCashierLock);
    	    if(g_noOfCustomersLeft == NO_OF_CUSTOMERS)
    	    {
    	    	simulationEnd = 1;
    	    }
    	}
    	else
    	{
    		Signal(g_managerCashierCV,g_managerCashierLock);

    		g_managerWaitQueueLength--;

    		acquireLock(g_managerCashierInteractionLock);

    		releaseLock(g_managerCashierLock);

    		Wait(g_managerCashierInteractionCV,g_managerCashierInteractionLock);

    		acquireLock(printLock);
    		print2("MANAGER %d got a call from CASHIER_%d\n",ThreadId,g_managerInfo.cashierId);
    		releaseLock(printLock);

    		Signal(g_managerCashierInteractionCV,g_managerCashierInteractionLock);

    		acquireLock(g_managerCustomerInteractionLock);

    		releaseLock(g_managerCashierInteractionLock);

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

    			acquireLock(printLock);
    			print3("MANAGER %d removes ITEM_%d from trolley of CUSTOMER_%d\n",
    					ThreadId,currentItemNoToRemove,g_managerInfo.customerId);
    			releaseLock(printLock);

    			g_managerInfo.customerBill -= currentItemNoToRemovePrice * currentItemNoToRemoveCount;

        		Signal(g_managerCustomerInteractionCV,g_managerCustomerInteractionLock);

        		Wait(g_managerCustomerInteractionCV,g_managerCustomerInteractionLock);
    		}

    		managerSales += g_managerInfo.customerBill;

    		Signal(g_managerCustomerInteractionCV,g_managerCustomerInteractionLock);

    		acquireLock(printLock);
    		print2("MANAGER %d gives receipt to CUSTOMER_%d \n",ThreadId,g_managerInfo.customerId);
    		releaseLock(printLock);

    		releaseLock(g_managerCustomerInteractionLock);

    		acquireLock(printLock);
    		print2("MANAGER %d has total sale of $ %d\n",ThreadId,managerSales);
    		releaseLock(printLock);
    	}

    	for(i=0;i<MANAGER_RANDOM_SLEEP_TIME;i++)
    	{
    		Yield();
    	}
    }

    Exit(0);
}

void initLockForSimulation()
{
	int i;

	g_customerThreadCounterLock = createLock("CustomerThreadCounterLock",sizeof("CustomerThreadCounterLock"));
	g_salesmanThreadCounterLock = createLock("SalesmanThreadCounterLock",sizeof("SalesmanThreadCounterLock"));
	g_goodsLoaderThreadCounterLock = createLock("GoodsLoaderThreadCounterLock",sizeof("GoodsLoaderThreadCounterLock"));
	g_cashierThreadCounterLock = createLock("CustomerThreadCounterLock",sizeof("CustomerThreadCounterLock"));
	g_customerTrolleyLock = createLock("CustomerTrolleyLock",sizeof("CustomerTrolleyLock"));
    g_managerCashierLock = createLock("managerCashierLock",sizeof("managerCashierLock"));
    g_managerCashierInteractionLock = createLock("managerCashierInteractionLock",sizeof("managerCashierInteractionLock"));
    g_managerCustomerInteractionLock = createLock("managerCustomerInteractionLock",sizeof("managerCustomerInteractionLock"));

	for(i=0;i<NO_OF_CASHIERS;i++)
	{

        g_cashierLineLock[i] = createLock("cashierLineLock",sizeof("cashierLineLock"));
        g_cashierPrivilegedLineLock[i] = createLock("cashierPrivilegedLineLock",
        		sizeof("cashierPrivilegedLineLock"));
        g_customerCashierLock[i] = createLock("customerCashierLock",sizeof("customerCashierLock"));
        g_managerCashierCashLock[i] = createLock("managerCashierCashLock",sizeof("managerCashierCashLock"));
	}

	for(i=0;i<NO_OF_SALESMAN;i++)
	{

        g_customerSalesmanLock[i] = createLock("customerSalesmanLock",sizeof("customerSalesmanLock"));
	}

	for(i=0;i<NO_OF_DEPARTMENT;i++)
	{
        g_customerDepartmentLock[i] = createLock("customerDepartmentLock",sizeof("customerDepartmentLock"));
        g_customerDepartmentComplainLock[i] = createLock("customerDepartmentComplainLock",
        		sizeof("customerDepartmentComplainLock"));
	}

	for(i =0;i<NO_OF_SHELFS;i++)
	{

        g_shelfAccessLock[i] = createLock("shelfAccessLock",sizeof("shelfAccessLock"));
	}

	for(i=0;i<NO_OF_GOOD_LOADERS;i++)
	{
        g_salesmanGoodsLoaderLock[i] = createLock("salesmanGoodsLoaderLock",sizeof("salesmanGoodsLoaderLock"));
	}
}

void initCvForSimulation()
{
	int i;

	g_customerTrolleyCV = createCondition((char*)"CustomerTrolleyCV",sizeof("CustomerTrolleyCV"));
    g_managerCashierCV = createCondition("managerCashierCV",sizeof("managerCashierCV"));
    g_managerCashierInteractionCV = createCondition("managerCashierInteractionCV",
        		sizeof("managerCashierInteractionCV"));
    g_managerCustomerInteractionCV = createCondition("managerCustomerInteractionCV",
        		sizeof("managerCustomerInteractionCV"));


	for(i=0;i<NO_OF_CASHIERS;i++)
	{

        g_cashierLineCV[i] = createCondition("cashierLineCV",sizeof("cashierLineCV"));
        g_cashierPrivilegedLineCV[i] = createCondition("cashierPrivilegedLineCV",
        		sizeof("cashierPrivilegedLineCV"));
        g_customerCashierCV[i] = createCondition("customerCashierCV",sizeof("customerCashierCV"));
	}

	for(i=0;i<NO_OF_CASHIERS;i++)
	{

        g_cashierWaitQueue[i] = 0;
        g_cashierPrivilegedWaitQueue[i] = 0;
	}

	for(i=0;i<NO_OF_SALESMAN;i++)
	{

        g_customerSalesmanCV[i] = createCondition("customerSalesmanCV",sizeof("customerSalesmanCV"));
	}

	for(i=0;i<NO_OF_DEPARTMENT;i++)
	{

        g_customerDepartmentCV[i] =  createCondition("customerDepartmentCV",sizeof("customerDepartmentCV"));
        g_customerDepartmentComplainCV[i] = createCondition("customerDepartmentComplainCV",
        		sizeof("customerDepartmentComplainCV"));
	}

	for(i=0;i<NO_OF_DEPARTMENT;i++)
	{
        g_departmentWaitQueue[i] = 0;
        g_departmentComplainWaitQueue[i] = 0;
	}

	for(i=0;i<NO_OF_GOOD_LOADERS;i++)
	{

        g_salesmanGoodsLoaderCV[i] = createCondition("salesmanGoodsLoaderCV",sizeof("salesmanGoodsLoaderCV"));
	}

	for(i=0;i<NO_OF_GOODLOADER_WAIT_QUEUE;i++)
	{

        g_goodLoaderWaitCV[i] = createCondition("goodLoaderWaitCV",sizeof("goodLoaderWaitCV"));
	}

	for(i=0;i<NO_OF_GOODLOADER_WAIT_QUEUE;i++)
	{
        g_goodLoaderWaitQueue[i] = 0;
	}
}

void printLockForSimulation()
{
	int i;

	acquireLock(printLock);
	print1("customerThreadCounterLock = %u \n",g_customerThreadCounterLock);
	print1("salesmanThreadCounterLock = %u \n",g_salesmanThreadCounterLock);
	print1("goodsLoaderThreadCounterLock = %u \n",g_goodsLoaderThreadCounterLock);
	print1("cashierThreadCounterLock = %u \n",g_cashierThreadCounterLock);
	print1("customerTrolleyLock = %u \n",g_customerTrolleyLock);
	print1("managerCashierLock = %u \n",g_managerCashierLock);
	print1("managerCashierInteractionLock = %u \n",g_managerCashierInteractionLock);
	print1("managerCustomerInteractionLock = %u \n",g_managerCustomerInteractionLock);

	for(i=0;i<NO_OF_CASHIERS;i++)
	{
    	print2("cashierLineLock %d = %u \n",i,g_cashierLineLock[i]);
    	print2("cashierPrivilegedLineLock %d = %u \n",i,g_cashierPrivilegedLineLock[i]);
    	print2("customerCashierLock %d = %u \n",i,g_customerCashierLock[i]);
    	print2("managerCashierCashLock %d = %u \n",i,g_managerCashierCashLock[i]);
	}
	for(i=0;i<NO_OF_SALESMAN;i++)
	{
    	print2("customerSalesmanLock %d = %u \n",i,g_customerSalesmanLock[i]);
	}
	for(i=0;i<NO_OF_DEPARTMENT;i++)
	{
    	print2("customerDepartmentLock %d = %u \n",i,g_customerDepartmentLock[i]);
    	print2("customerDepartmentComplainLock %d = %u \n",i,g_customerDepartmentComplainLock[i]);
	}
	for(i =0;i<NO_OF_SHELFS;i++)
	{
		print2("shelfAccessLock %d = %u \n",i,g_shelfAccessLock[i]);
	}
	for(i=0;i<NO_OF_GOOD_LOADERS;i++)
	{
    	print2("salesmanGoodsLoaderLock %d = %u \n",i,g_salesmanGoodsLoaderLock[i]);
	}
	releaseLock(printLock);

}

void printCvForSimulation()
{
	int i;
	acquireLock(printLock);
	print1("customerTrolleyCV = %u \n",g_customerTrolleyCV);

	for(i=0;i<NO_OF_MANAGERS;i++)
	{
    	print1("managerCashierCV = %u \n",g_managerCashierCV);
    	print1("managerCashierInteractionCV = %u \n",g_managerCashierInteractionCV);
    	print1("managerCustomerInteractionCV = %u \n",g_managerCustomerInteractionCV);
	}

	for(i=0;i<NO_OF_CASHIERS;i++)
	{
    	print2("cashierLineCV %d = %u \n",i,g_cashierLineCV[i]);
    	print2("cashierPrivilegedLineCV %d = %u \n",i,g_cashierPrivilegedLineCV[i]);
    	print2("customerCashierCV %d = %u \n",i,g_customerCashierCV[i]);
	}

	for(i=0;i<NO_OF_SALESMAN;i++)
	{
    	print2("customerSalesmanCV %d = %u \n",i,g_customerSalesmanCV[i]);
	}

	for(i=0;i<NO_OF_DEPARTMENT;i++)
	{
    	print2("customerDepartmentCV %d = %u \n",i,g_customerDepartmentCV[i]);
    	print2("customerDepartmentComplainCV %d = %u \n",i,g_customerDepartmentComplainCV[i]);
	}

	for(i=0;i<NO_OF_GOOD_LOADERS;i++)
	{
    	print2("salesmanGoodsLoaderCV %d = %u \n",i,g_salesmanGoodsLoaderCV[i]);
	}

	for(i=0;i<NO_OF_GOODLOADER_WAIT_QUEUE;i++)
	{
    	print2("goodLoaderWaitCV %d = %u \n",i, g_goodLoaderWaitCV[i]);
	}
	releaseLock(printLock);
}

void main(const char* testOption)
{
	int configRetVal;
	int i;

	printLock = createLock("printLock",sizeof("printLock"));

	acquireLock(printLock);
    print( "Entering Assign 1 Problem 2 !!!!!!! \n");
    print( "Starting to initialize all the Threads for Problem 2 !!!!!!! \n");

    print1("Number of Cashiers = %d \n",NO_OF_CASHIERS);
    print1("Number of Good Loaders = %d \n",NO_OF_GOOD_LOADERS);
    print1("Number of PrivilegedCustomers = %d \n",NO_OF_CUSTOMERS);
    print1("Number of Customers = %d \n",NO_OF_CUSTOMERS);
    print1("Number of Managers = %d \n",NO_OF_MANAGERS);
    print1("Number of DepartmentSalesmen = %d \n",NO_OF_SALESMAN);
    releaseLock(printLock);

    initLockForSimulation();
	initCvForSimulation();

	/* printLockForSimulation(); */
	/* printCvForSimulation(); */

	initItemInfo();
	initCustomerInfo();
	initCustomerShoppingList();

    initSalesManInfo();

    initGoodLoaderInfo();

    initCashierInfo();

    initManagerInfo();



    for(i = 0;i<NO_OF_MANAGERS;i++)
    {
       Fork(ManagerThread);
    }


#if 0
    for(i = 0;i<NO_OF_CASHIERS;i++)
    {
    	Fork(CashierThread);
    }

    for(i = 0;i<NO_OF_GOOD_LOADERS;i++)
    {
    	Fork(GoodLoaderThread);
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

}

/********************************************PROJ1 PROBLEM SIMULATION***********************************************/



