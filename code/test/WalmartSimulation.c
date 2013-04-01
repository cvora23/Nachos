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


/**
 * STORE STOCK INFO
 */
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

#define MANAGER_STRING									(const char*)"MANAGER"
#define CASHIER_STRING									(const char*)"CASHIER"
#define DEPARTMENT_STRING								(const char*)"DEPARTMENT"
#define SALESMAN_STRING									(const char*)"SALESMAN"
#define GOOD_LOADERS_STRING								(const char*)"GOODLOADER"
#define CUSTOMER_STRING									(const char*)"CUSTOMER"

#define CUSTOMERTHREADCOUNTER_STRING					(const char*)"CustomerThreadCounterLock"
#define SALESMANTHREADCOUNTER_STRING					(const char*)"SalesmanThreadCounterLock"
#define GOODSLOADERTHREADCOUNTER_STRING					(const char*)"GoodsLoaderThreadCounterLock"
#define CASHIERTHREADCOUNTER_STRING						(const char*)"CustomerThreadCounterLock"

#define PRINTLOCK_STRING								(const char*)"PrintLock"
#define CUSTOMERTROLLEYLOCK_STRING						(const char*)"CustomerTrolleyLock"
#define CUSTOMERTROLLEYCV_STRING						(const char*)"CustomerTrolleyCV"
#define CUSTOMERSALESMANLOCK_STRING						(const char*)"CustomerSalesmanLock"
#define CUSTOMERSALESMANCV_STRING						(const char*)"CustomerSalesmanCV"
#define CUSTOMERDEPARTMENTLOCK_STRING					(const char*)"CustomerDepartmentLock"
#define CUSTOMERDEPARTMENTCV_STRING						(const char*)"CustomerDepartmentCV"
#define SHELFACCESSLOCK_STRING							(const char*)"ShelfAccessLock"
#define CUSTOMERDEPARTMENTCOMPLAINLOCK_STRING			(const char*)"CustomerDepartmentComplainLock"
#define CUSTOMERDEPARTMENTCOMPLAINCV_STRING				(const char*)"CustomerDepartmentComplainCV"
#define SALESMANGOODLOADERLOCK_STRING					(const char*)"SalesmanGoodLoaderLock"
#define SALESMANGOODLOADERCV_STRING						(const char*)"SalesmanGoodLoderCV"
#define GOODLOADERWAITQUEUELOCK_STRING					(const char*)"GoodLoaderWaitQueueLock"
#define GOODLOADERWAITQUEUECV_STRING					(const char*)"GoodLoaderWaitQueueCV"
#define	CASHIERLINELOCK_STRING							(const char*)"CashierLineLock"
#define CASHIERLINECV_STRING							(const char*)"CashierLineCV"
#define CASHIERPRIVILEGEDLINELOCK_STRING				(const char*)"CashierPrivilegedLineLock"
#define CASHIERPRIVILEGEDLINECV_STRING					(const char*)"CashierPrivilegedLineCV"
#define CUSTOMERCASHIERLOCK_STRING						(const char*)"CustomerCashierLock"
#define CUSTOMERCASHIERCV_STRING						(const char*)"CustomerCashierCV"
#define MANAGERCASHIERCASHLOCK_STRING					(const char*)"ManagerCashierCashLock"
#define MANAGERCASHIERLOCK_STRING						(const char*)"ManagerCashierLock"
#define MANAGERCASHIERCV_STRING							(const char*)"ManagerCashierCV"
#define MANAGERCASHIERINTERACTIONLOCK_STRING			(const char*)"ManagerCashierInteractionLock"
#define MANAGERCASHIERINTERACTIONCV_STRING				(const char*)"ManagerCashierInteractionCV"
#define MANAGERCUSTOMERINTERACTIONLOCK_STRING			(const char*)"ManagerCustomerLock"
#define MANAGERCUSTOMERINTERACTIONCV_STRING				(const char*)"ManagerCustomerCV"

/*************************************DEFINES DECLARATION**********************************************/

/*************************************GLOBAL DATA STRUCTS DEFINITION**********************************************/


/**
 * Define all the global data structure here
 */

typedef enum _bool
{
	false = 0,
	true
}bool;

typedef struct _ItemInfo
{
	/**
	 * Price of the item
	 */
	int 	Price;
	/**
	 * Shelf where the item is placed
	 */
	int 	shelfNo;
	/**
	 * Department No where item is from
	 */
	int 	departmentNo;
	/**
	 * Total number of item Type in stock
	 */
	int		noOfItems;
}ItemInfo;


typedef struct _CustomerShoppingList
{
	/**
	 * Item No to shop
	 */
	int 	 itemNo;
	/**
	 * No (Quantity) of items of that Item No above to shop
	 */
	int		 noOfItems;
}CustomerShoppingList;

typedef enum _CustomerType
{
	PRIVILEGED = 0,
	NON_PRIVILEGED
}CustomerType;

typedef struct _CustomerInfo
{
	/**
	 * Type of Customer - Privileged or Non Privileged
	 */
	CustomerType 			type;
	/**
	 * Money Customer can spend for shopping
	 */
	int 					money;
	/**
	 * Shopping List Item Count
	 */
	int						noOfItems;
	/**
	 * has enough money for shopping or no
	 */
	bool					hasEnoughMoneyForShopping;
	/**
	 * is done shopping or not
	 */
	bool					isDoneShopping;
	/**
	 * Shopping List for Customer
	 */
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
	/**
	 * If the Salesman is free or busy handling customer or just signaled a Customer
	 */
	SalesManStatus status;
	/**
	 *
	 */
	/**
	 * Department Salesman is working for
	 */
	int	 departmentNo;
	/**
	 * Customer Salesman is handling or will be handling in future
	 */
	int customerId;
	/**
	 * item to be re stocked
	 */
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
	/**
	 * If the Good Loader is free or busy handling salesman
	 */
	GoodLoaderStatus status;
	/**
	 * Salesman Good Loader is handling or will be handling in future.
	 */
	int salesmanId;
	/**
	 * item to be re stocked
	 */
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
	/*
	 * Status of cashier
	 */
	CashierStatus	status;
	/**
	 * Customer Cashier is handling or will be handling in future
	 */
	int 			customerId;
	/**
	 * bill of current Customer
	 */
	int 			bill;
	/**
	 * total sales collection
	 */
	int 			totalSalesMoney;
}CashierInfo;

typedef struct _ManagerInfo
{
	/**
	 * Cashier Manager is handling or will be handling in future
	 */
	int 			cashierId;
	/**
	 * Customer Manager is handling or will be handling in future
	 */
	int 			customerId;
	/**
	 * Customer's bill to negotiate
	 */
	int 			customerBill;
	/**
	 * Total revenue generated by store as of now
	 */
	int				totalRevenue;
}ManagerInfo;

/*************************************GLOBAL DATA STRUCTS DEFINITION**********************************************/


/*************************************GLOBAL DATA STRUCTS DECLARATION**********************************************/

/**
 * GLOBAL DATA STRUCTURES
 */
ItemInfo 		g_itemInfo[NO_OF_ITEM_TYPES];

CustomerInfo 	g_customerInfo[NO_OF_CUSTOMERS];

SalesManInfo 	g_salesmanInfo[NO_OF_SALESMAN];

GoodLoaderInfo 	g_goodLoaderInfo[NO_OF_GOOD_LOADERS];

CashierInfo		g_cashierInfo[NO_OF_CASHIERS];

ManagerInfo		g_managerInfo;

/**
 * Number of customer served and left
 */
int g_noOfCustomersLeft = 0;
int simulationEnd = 0;

/**
 * Threads Counter for Customer, Department Salesman, Goods Loader and Cashier
 */
int g_customerThreadCounter = 0;
int g_salesmanThreadCounter = 0;
int g_goodsLoaderThreadCounter = 0;
int g_cashierThreadCounter = 0;

/**
 * Locks for Customer, Department Salesman, Goods Loader and Cashier Thread Counter
 */
unsigned int g_customerThreadCounterLock;
unsigned int g_salesmanThreadCounterLock;
unsigned int g_goodsLoaderThreadCounterLock;
unsigned int g_cashierThreadCounterLock;

/**
 * Locks, Condition Variables, Queue Wait Count for Customer-Trolley interaction
 */
int 				g_usedTrolleyCount;
int 				g_waitForTrolleyCount;
unsigned int 		g_customerTrolleyLock;
unsigned int 		g_customerTrolleyCV;

/**
 * Locks, Condition Variables for Customer-Salesman interaction
 */
unsigned int 	g_customerSalesmanLock[NO_OF_SALESMAN];
unsigned int	g_customerSalesmanCV[NO_OF_SALESMAN];

/**
 * Locks,Condition Variables and Queue Length for Customer-Department Interaction
 */
unsigned int			g_customerDepartmentLock[NO_OF_DEPARTMENT];
unsigned int			g_customerDepartmentCV[NO_OF_DEPARTMENT];
int						g_departmentWaitQueue[NO_OF_DEPARTMENT];

/**
 * Locks for accessing items on the shelf
 */
unsigned int			g_shelfAccessLock[NO_OF_SHELFS];

/**
 * Locks,Condition Variables and Queue Length for Customer-Department Interaction
 */
unsigned int			g_customerDepartmentComplainLock[NO_OF_DEPARTMENT];
unsigned int			g_customerDepartmentComplainCV[NO_OF_DEPARTMENT];
int						g_departmentComplainWaitQueue[NO_OF_DEPARTMENT];

/**
 * Locks,Condition Variables and Queue Length for Salesman - GoodsLoader Interaction
 */
unsigned int			g_salesmanGoodsLoaderLock[NO_OF_GOOD_LOADERS];
unsigned int			g_salesmanGoodsLoaderCV[NO_OF_GOOD_LOADERS];

unsigned int			g_goodLoaderWaitLock[NO_OF_GOODLOADER_WAIT_QUEUE];
unsigned int			g_goodLoaderWaitCV[NO_OF_GOODLOADER_WAIT_QUEUE];
int						g_goodLoaderWaitQueue[NO_OF_GOODLOADER_WAIT_QUEUE];

/**
 * Locks,Condition Variables and Queue Length for Customer - Cashier Interaction
 */
unsigned int			g_cashierLineLock[NO_OF_CASHIERS];
unsigned int			g_cashierLineCV[NO_OF_CASHIERS];
int						g_cashierWaitQueue[NO_OF_CASHIERS];

unsigned int			g_cashierPrivilegedLineLock[NO_OF_CASHIERS];
unsigned int			g_cashierPrivilegedLineCV[NO_OF_CASHIERS];
int						g_cashierPrivilegedWaitQueue[NO_OF_CASHIERS];

unsigned int			g_customerCashierLock[NO_OF_CASHIERS];
unsigned int			g_customerCashierCV[NO_OF_CASHIERS];

/**
 * Locks, Condition Variables and Queue Length for Manager - Cashier  Cash Collection Interaction
 */
unsigned int			g_managerCashierCashLock[NO_OF_CASHIERS];

/**
 * Locks, Condition Variables and Queue Length for Manager Interaction
 */

unsigned int			g_managerCashierLock;
unsigned int			g_managerCashierCV;
int						g_managerWaitQueueLength;

unsigned int			g_managerCashierInteractionLock;
unsigned int			g_managerCashierInteractionCV;

/**
 * Locks, Condition Variables for Manager - Customer Interaction
 */
unsigned int			g_managerCustomerInteractionLock;
unsigned int			g_managerCustomerInteractionCV;

/*************************************GLOBAL DATA STRUCTS DECLARATION**********************************************/

/**
 *	DYNAMIC CONFIGURATION OF ITEMS
 *	\todo: Remember to do this....
 */

/********************************************PROJ1 PROBLEM SIMULATION***********************************************/

int xstrlen(const char* s)
{
	int length = 0;
	while(*s!='\0')
	{
		length++;
		s++;
	}

	return length;
}

void clearCharBuf(char* s)
{
	while(*s!='\0')
	{
		*s = '\0';
		s++;
	}
}

/**
 * GLOBAL DATA STRUCTURES INIT FUNCTIONS
 */
static void initItemInfo()
{
    static bool firstCall = true;
    int i;
    if(firstCall)
    {
		for (i = 0;i<NO_OF_ITEM_TYPES;i++)
		{
			g_itemInfo[i].Price = Random()%MAX_PRICE_PER_ITEM + 1;
			g_itemInfo[i].departmentNo = (int)(i/NO_OF_ITEMS_PER_DEPARTMENT);
			g_itemInfo[i].shelfNo = i;
			g_itemInfo[i].noOfItems = Random()%MAX_NO_ITEMS_PER_SHELF + 1;
		}
		firstCall = false;
    }
}

static void initCustomerInfo()
{
    static bool firstCall = true;
    int i,j;
    if(firstCall)
    {
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
#if 0
			g_customerInfo[i].noOfItems = Random()%MAX_NO_ITEMS_TO_BE_PURCHASED + 1;
			g_customerInfo[i].pCustomerShoppingList = new CustomerShoppingList[g_customerInfo[i].noOfItems];
#endif
			g_customerInfo[i].noOfItems = NO_OF_ITEMS_TO_SHOP;

			for(j=0;j<g_customerInfo[i].noOfItems;j++)
			{
				g_customerInfo[i].pCustomerShoppingList[j].itemNo = 0;
				g_customerInfo[i].pCustomerShoppingList[j].noOfItems = 0;
			}

			g_customerInfo[i].hasEnoughMoneyForShopping = false;
			g_customerInfo[i].isDoneShopping = false;
		}
		firstCall = false;
    }
}

static void initCustomerShoppingList()
{
    static bool firstCall = true;
    int i,j;
    if(firstCall)
    {
    	for(i = 0;i<NO_OF_CUSTOMERS;i++)
    	{
			for(j =0;j<g_customerInfo[i].noOfItems;j++)
			{
				g_customerInfo[i].pCustomerShoppingList[j].itemNo = Random()%NO_OF_ITEM_TYPES;
				g_customerInfo[i].pCustomerShoppingList[j].noOfItems =
						Random()%MAX_NO_ITEMS_TO_BE_PURCHASED_OF_EACH_TYPE + 1;
			}
    	}
		firstCall = false;
	}
}

static void initSalesManInfo()
{
    static bool firstCall = true;
    int i;
    if(firstCall)
    {
    	for(i=0;i<NO_OF_SALESMAN;i++)
    	{
    		g_salesmanInfo[i].customerId = -1;
    		g_salesmanInfo[i].status = salesmanIsBusy;
    		g_salesmanInfo[i].departmentNo = (int)(i/NO_OF_SALESMAN_PER_DEPARTMENT);
    		g_salesmanInfo[i].itemToRestock = -1;
    	}
		firstCall = false;
	}
}

void printSalesManInfo(int salesManId)
{

}

static void initGoodLoaderInfo()
{
    static bool firstCall = true;
    int i;
    if(firstCall)
    {
    	for(i=0;i<NO_OF_GOOD_LOADERS;i++)
    	{
    		g_goodLoaderInfo[i].status = goodLoaderIsBusy;
    		g_goodLoaderInfo[i].salesmanId = -1;
    		g_goodLoaderInfo[i].itemToRestock = -1;
    	}
		firstCall = false;
	}
}

void printGoodLoaderInfo(int goodLoaderId)
{

}

static void initCashierInfo()
{
    static bool firstCall = true;
    int i;
    if(firstCall)
    {
    	for(i=0;i<NO_OF_CASHIERS;i++)
    	{
    		g_cashierInfo[i].status = cashierIsBusy;
    		g_cashierInfo[i].customerId = -1;
    		g_cashierInfo[i].bill = -1;
    		g_cashierInfo[i].totalSalesMoney = 0;
    	}
		firstCall = false;
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

static void initManagerInfo()
{
    static bool firstCall = true;
    int i;
    if(firstCall)
    {
    	for(i=0;i<NO_OF_MANAGERS;i++)
    	{
    		g_managerInfo.cashierId = -1;
    		g_managerInfo.customerId = -1;
    		g_managerInfo.totalRevenue = 0;
    		g_managerInfo.customerBill = 0;
    	}
		firstCall = false;
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
	char threadName[256];
	int i;
	int salesmanIndex;

	acquireLock(g_customerThreadCounterLock);
	ThreadId = g_customerThreadCounter;
	g_customerThreadCounter++;
	releaseLock(g_customerThreadCounterLock);

    sprintf(threadName, CUSTOMER_STRING,xstrlen(CUSTOMER_STRING), ThreadId);
    printString("%s enters the SuperMarket !!!!!!! \n",threadName);

    /*************************************CUSTOMER-TROLLEY INTERACTION STARTS HERE*********************************************/

    /**
     * Starting to get in line to get a shopping trolley
     */
    acquireLock(g_customerTrolleyLock);
    printString("%s gets in line for a trolley\n",threadName);
    if(g_usedTrolleyCount<MAX_NO_OF_TROLLEYS)
    {
    	g_usedTrolleyCount++;
    }
    else
    {
    	g_waitForTrolleyCount++;
    	wait(g_customerTrolleyCV,g_customerTrolleyLock);
    	g_waitForTrolleyCount--;
    	g_usedTrolleyCount++;
    }
    printString("%s has a trolley for shopping\n",threadName);
    releaseLock(g_customerTrolleyLock);

    /*************************************CUSTOMER-TROLLEY INTERACTION ENDS HERE*********************************************/

    /*************************************CUSTOMER-SALESMAN INTERACTION STARTS HERE*********************************************/


    /**
     * Customer will start the shopping now.
     */
    for(i=0;i<g_customerInfo[ThreadId].noOfItems;i++)
    {
    	/**
    	 * Customer will now find Department for particular item no
    	 */
    	printStringIntInt("%s wants to shop ITEM_%d in DEPARTMENT_%d \n",
    			threadName,
    			g_customerInfo[ThreadId].pCustomerShoppingList[i].itemNo,
    			g_itemInfo[g_customerInfo[ThreadId].pCustomerShoppingList[i].itemNo].departmentNo);

    	currentItemNoFromShoppingList = g_customerInfo[ThreadId].pCustomerShoppingList[i].itemNo;
      	currentItemNoCountFromShoppingList = g_customerInfo[ThreadId].pCustomerShoppingList[i].noOfItems;

    	currentDepartmentNoForItem =
    			g_itemInfo[g_customerInfo[ThreadId].pCustomerShoppingList[i].itemNo].departmentNo;


    	salesManStartForDepartment = currentDepartmentNoForItem*NO_OF_SALESMAN_PER_DEPARTMENT;
    	salesManEndForDepartment = salesManStartForDepartment + NO_OF_SALESMAN_PER_DEPARTMENT;
    	/**
    	 * Customer will now start interacting with Department Salesman for the item
    	 */
    	mySalesMan = -1;

    	for(salesmanIndex=salesManStartForDepartment;
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

    		acquireLock(g_customerSalesmanLock[salesmanIndex]);
    		if(g_salesmanInfo[salesmanIndex].status == salesmanIsFree)
    		{
    			mySalesMan = salesmanIndex;
    			g_salesmanInfo[salesmanIndex].status = salesmanIsBusy;
    			g_salesmanInfo[salesmanIndex].customerId = ThreadId;
    			printStringIntInt("%s will enquire about ITEM_%d to SALESMAN_%d\n",threadName,
    					currentItemNoFromShoppingList,mySalesMan);
    			break;
    		}
    		releaseLock(g_customerSalesmanLock[salesmanIndex]);
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

    		printStringIntInt("%s is waiting in Line for DEPARTMENT_%d as NO SALESMAN is free for ITEM_%d inquiry \n",
    				threadName,
    				currentDepartmentNoForItem,
    				currentItemNoFromShoppingList);

    		acquireLock(g_customerDepartmentLock[currentDepartmentNoForItem]);

    		g_departmentWaitQueue[currentDepartmentNoForItem]++;

    		printStringInt("%s gets in line for the DEPARTMENT_%d \n",threadName,
    				currentDepartmentNoForItem);

    		wait(g_customerDepartmentCV[currentDepartmentNoForItem],g_customerDepartmentLock[currentDepartmentNoForItem]);

        	for(salesmanIndex=salesManStartForDepartment;
        			salesmanIndex<salesManEndForDepartment;salesmanIndex++)
        	{
        		/**
        		 * \todo: ASSUMPTION HERE THAT FIRST SALESMAN WHO SIGNALED WILL BE OUR
        		 * SALESMAN (Will Work For Now)
        		 */
    			acquireLock(g_customerSalesmanLock[salesmanIndex]);
        		if(g_salesmanInfo[salesmanIndex].status == salesmanSignalToCustomerFromDeptWaitQueue)
        		{
        			mySalesMan = salesmanIndex;
        			printStringIntInt("%s assumes that SALESMAN_%d Signaled him regarding enquiry of ITEM_%d\n",
        					threadName,mySalesMan,currentItemNoFromShoppingList);
        			g_salesmanInfo[salesmanIndex].status = salesmanIsBusy;
        			g_salesmanInfo[salesmanIndex].customerId = ThreadId;
        			break;
        		}
        		releaseLock(g_customerSalesmanLock[salesmanIndex]);
        	}
        	releaseLock(g_customerDepartmentLock[currentDepartmentNoForItem]);
    	}


		printStringIntInt("%s is interacting with SALESMAN_%d from DEPARTMENT_%d \n",
				threadName,mySalesMan,currentDepartmentNoForItem);

    	signal(g_customerSalesmanCV[mySalesMan],g_customerSalesmanLock[mySalesMan]);

    	wait(g_customerSalesmanCV[mySalesMan],g_customerSalesmanLock[mySalesMan]);

    	signal(g_customerSalesmanCV[mySalesMan],g_customerSalesmanLock[mySalesMan]);

    	releaseLock(g_customerSalesmanLock[mySalesMan]);


		/**
		 * Customer started interaction with Department Salesman and will now
		 * start shopping from his shopping list.
		 */

		acquireLock(g_shelfAccessLock[currentItemNoFromShoppingList]);

		if(g_itemInfo[currentItemNoFromShoppingList].noOfItems>=currentItemNoCountFromShoppingList)
		{
			g_itemInfo[currentItemNoFromShoppingList].noOfItems -= currentItemNoCountFromShoppingList;

			printStringIntInt("%s has found ITEM_%d and placed ITEM_%d in the trolley \n",threadName,
					currentItemNoFromShoppingList,currentItemNoCountFromShoppingList);
			releaseLock(g_shelfAccessLock[currentItemNoFromShoppingList]);
		}
		else
		{
			releaseLock(g_shelfAccessLock[currentItemNoFromShoppingList]);
			mySalesMan = -1;

    		printStringInt("%s is not able to find ITEM_%d and is searching for SALESMAN's \n",
    				threadName,currentItemNoFromShoppingList);

    		for(salesmanIndex=salesManStartForDepartment;
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

	    		acquireLock(g_customerSalesmanLock[salesmanIndex]);
	    		if(g_salesmanInfo[salesmanIndex].status == salesmanIsFree)
	    		{
	    			mySalesMan = salesmanIndex;
	    			printStringIntInt("%s will enquire about RESTOCK-ITEM_%d to SALESMAN_%d\n",threadName,
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

	    		printStringIntInt("%s is waiting in COMPLAIN LINE for DEPARTMENT_%d AS "
	    				"NO SALESMAN is free for RESTOCK-ITEM_%d inquiry so \n",
	    				threadName,
	    				currentDepartmentNoForItem,
	    				currentItemNoFromShoppingList);

	    		acquireLock(g_customerDepartmentComplainLock[currentDepartmentNoForItem]);

	    		g_departmentComplainWaitQueue[currentDepartmentNoForItem]++;

	    		wait(g_customerDepartmentComplainCV[currentDepartmentNoForItem],
	    				g_customerDepartmentComplainLock[currentDepartmentNoForItem]);

	        	for(salesmanIndex=salesManStartForDepartment;
	        			salesmanIndex<salesManEndForDepartment;salesmanIndex++)
	        	{
	        		/**
	        		 * \todo: ASSUMPTION HERE THAT FIRST SALESMAN WHO SIGNALED WILL BE OUR
	        		 * SALESMAN (Will Work For Now)
	        		 */
	    			acquireLock(g_customerSalesmanLock[salesmanIndex]);
	        		if(g_salesmanInfo[salesmanIndex].status == salesmanSignalToCustomerFromDeptComplainWaitQueue)
	        		{
		    			mySalesMan = salesmanIndex;
	        			printStringIntInt("%s assumes that SALESMAN_%d Signaled him regarding inquiry of RESTOCK ITEM_%d\n",
	        					threadName,mySalesMan,currentItemNoFromShoppingList);
		    			g_salesmanInfo[salesmanIndex].status = salesmanIsBusy;
		    			g_salesmanInfo[salesmanIndex].customerId = ThreadId;
		    			g_salesmanInfo[salesmanIndex].itemToRestock = currentItemNoFromShoppingList;
	        			break;
	        		}
	        		releaseLock(g_customerSalesmanLock[salesmanIndex]);
	        	}
	        	releaseLock(g_customerDepartmentComplainLock[currentDepartmentNoForItem]);
	    	}

	    	printStringInt("%s is asking for assistance from SALESMAN_%d \n",
	    			threadName,mySalesMan);

	    	signal(g_customerSalesmanCV[mySalesMan],g_customerSalesmanLock[mySalesMan]);

	    	wait(g_customerSalesmanCV[mySalesMan],g_customerSalesmanLock[mySalesMan]);

	    	printStringIntInt("%s has received assistance about re stocking of ITEM_%d from SALESMAN_%d \n",
	    			threadName,currentItemNoFromShoppingList,mySalesMan);

	    	signal(g_customerSalesmanCV[mySalesMan],g_customerSalesmanLock[mySalesMan]);

			g_itemInfo[currentItemNoFromShoppingList].noOfItems -= currentItemNoCountFromShoppingList;

	    	g_salesmanInfo[mySalesMan].itemToRestock = -1;
	    	g_salesmanInfo[mySalesMan].customerId = -1;

	    	releaseLock(g_customerSalesmanLock[mySalesMan]);

		}

		printStringInt("%s has finished shopping in  DEPARTMENT_%d \n",
				threadName,currentDepartmentNoForItem);
        /*************************************CUSTOMER-SALESMAN INTERACTION ENDS HERE*********************************************/
    }

    printString("%s has finished shopping for all items \n",threadName);

    printString("%s is looking for the Cashier \n",threadName);

    /*******************************************CUSTOMER-CASHIER INTERACTION STARTS HERE*******************************************/

    /**
     * Finding the Cashier which is least busy.
     */
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
		printStringIntInt("%s chose CASHIER_%d with Privileged Line of length %d \n",
				threadName,myCashier,minCashierLineLength);
	}
	else
	{
		printStringIntInt("%s chose CASHIER_%d with Line of length %d \n",
				threadName,myCashier,minCashierLineLength);
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
    	    wait(g_cashierPrivilegedLineCV[myCashier],g_cashierPrivilegedLineLock[myCashier]);
    	}
    	else
    	{
    		acquireLock(g_cashierLineLock[myCashier]);
    	    releaseLock(g_customerCashierLock[myCashier]);
    	    g_cashierWaitQueue[myCashier]++;
    	    wait(g_cashierLineCV[myCashier],g_cashierLineLock[myCashier]);
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

    /**
     * Item Processing between Customer and Cashier.
     * Waits for the bill from Cashier
     */
    signal(g_customerCashierCV[myCashier],g_customerCashierLock[myCashier]);

    wait(g_customerCashierCV[myCashier],g_customerCashierLock[myCashier]);

    myBill = g_cashierInfo[myCashier].bill;

    if(myBill <= g_customerInfo[ThreadId].money)
    {
    	g_customerInfo[ThreadId].hasEnoughMoneyForShopping = true;
    	printStringIntInt("%s pays %d amount to CASHIER_%d and is waiting for the receipt\n",
    			threadName,myBill,myCashier);
    }
    else
    {
    	g_customerInfo[ThreadId].hasEnoughMoneyForShopping = false;
    	printStringInt("%s cannot pay %d\n",
    			threadName,myBill);
    }

    signal(g_customerCashierCV[myCashier],g_customerCashierLock[myCashier]);

    wait(g_customerCashierCV[myCashier],g_customerCashierLock[myCashier]);

    if(g_customerInfo[ThreadId].hasEnoughMoneyForShopping == true)
    {
    	printStringInt("%s got receipt from CASHIER_%d and is now leaving\n",
    			threadName,myCashier);
    }

	printStringInt("%s signaled CASHIER_%d that I am leaving\n",
			threadName,myCashier);

    signal(g_customerCashierCV[myCashier],g_customerCashierLock[myCashier]);

    acquireLock(g_managerCustomerInteractionLock);

    releaseLock(g_customerCashierLock[myCashier]);

    /*************************************CUSTOMER - CASHIER INTERACTION ENDS HERE*********************************************/

    /*************************************CUSTOMER - MANAGER INTERACTION STARTS HERE *******************************************/

    if(g_customerInfo[ThreadId].hasEnoughMoneyForShopping == false)
    {
    	printString("%s is waiting for MANAGER for negotiations\n",threadName);
    	g_managerInfo.customerBill = myBill;
    	g_managerInfo.customerId = ThreadId;

    	signal(g_managerCustomerInteractionCV,g_managerCustomerInteractionLock);

    	wait(g_managerCustomerInteractionCV,g_managerCustomerInteractionLock);

    	/**
    	 * Start Removing Items from the Trolley
    	 */
    	while(myBill<=g_customerInfo[ThreadId].hasEnoughMoneyForShopping)
    	{
    		signal(g_managerCustomerInteractionCV,g_managerCustomerInteractionLock);
    		printStringInt("%s tells MANAGER to remove ITEM_%d from trolley\n",
    				threadName,
    				g_customerInfo[ThreadId].pCustomerShoppingList[currentItemNoCountToRemoveFromTrolley].itemNo);
        	wait(g_managerCustomerInteractionCV,g_managerCustomerInteractionLock);
        	myBill = g_managerInfo.customerBill;
        	currentItemNoCountToRemoveFromTrolley++;
    	}

    	g_customerInfo[ThreadId].isDoneShopping = true;

    	printStringInt("%s pays $ %d MANAGER after removing items and is waiting for receipt from MANAGER\n",
    			threadName,myBill);

		signal(g_managerCustomerInteractionCV,g_managerCustomerInteractionLock);
    	wait(g_managerCustomerInteractionCV,g_managerCustomerInteractionLock);

    	printString("%s got receipt from MANAGER and is now leaving\n",threadName);

    	releaseLock(g_managerCustomerInteractionLock);
    }

    else
    {
    	releaseLock(g_managerCustomerInteractionLock);
    }

    /*************************************CUSTOMER - MANAGER INTERACTION ENDS HERE *******************************************/

    printString("%s left the shopping mall !!!!! \n",threadName);

    acquireLock(g_customerTrolleyLock);
    g_usedTrolleyCount--;
    if(g_waitForTrolleyCount>0)
    {
    	signal(g_customerTrolleyCV,g_customerTrolleyLock);
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
	char threadName[256];
	int goodLoaderIndex;


	acquireLock(g_salesmanThreadCounterLock);
	ThreadId = g_salesmanThreadCounter;
	g_salesmanThreadCounter++;
	releaseLock(g_salesmanThreadCounterLock);

    sprintf(threadName, SALESMAN_STRING,xstrlen(SALESMAN_STRING), ThreadId);
    printString( "%s Started !!!!!!! \n",threadName);
    printStringInt("%s will be working for DEPARTMENT_%d \n",
    		threadName,g_salesmanInfo[ThreadId].departmentNo);

    while(1)
    {
    	/**
    	 * Check to see if there is some one in department line
    	 * If YES:
    	 * 1:	Signal to that customer
    	 */
    	acquireLock(g_customerDepartmentLock[myDepartmentNo]);

    	if(g_departmentWaitQueue[myDepartmentNo]>0)
    	{

    		printStringInt("%s ---- DEPARTMENT_%d WAIT QUEUE is not Empty... Someone Waiting for Assistance \n",
    				threadName,myDepartmentNo);

    		g_departmentWaitQueue[myDepartmentNo]--;
    		/**
    		 * Very Important to acquire the g_customerSalesmanLock before signaling the
    		 * customer waiting on  g_customerDepartmentCV with g_customerDepartmentLock because:
    		 * In case we signal on  g_customerDepartmentCV with g_customerDepartmentLock and release
    		 * the g_customerDepartmentLock there is a chance of CS and customer acquiring the
    		 * g_customerSalesmanLock and then salesman and customer waiting for signals from each other
    		 * in other words LiveLock Situation
    		 */
    		acquireLock(g_customerSalesmanLock[ThreadId]);

    		g_salesmanInfo[ThreadId].status = salesmanSignalToCustomerFromDeptWaitQueue;

    		signal(g_customerDepartmentCV[myDepartmentNo],g_customerDepartmentLock[myDepartmentNo]);

    		printStringInt("%s signaling someone waiting in DEPARTMENT_%d WAIT QUEUE\n",threadName,
    				myDepartmentNo);

    		releaseLock(g_customerDepartmentLock[myDepartmentNo]);

    		printStringInt("%s waiting for response from UNKNOWN CUSTOMER now in DEPARTMENT_%d WAIT QUEUE \n",
    				threadName,myDepartmentNo);

    		wait(g_customerSalesmanCV[ThreadId],g_customerSalesmanLock[ThreadId]);

    		printStringIntInt("%s GOT INITIAL RESPONSE from CUSTOMER_%d now in DEPARTMENT_%d WAIT QUEUE\n",
    				threadName,g_salesmanInfo[ThreadId].customerId,myDepartmentNo);

    		signal(g_customerSalesmanCV[ThreadId],g_customerSalesmanLock[ThreadId]);

    		printStringIntInt("%s SENDING RESPONSE to CUSTOMER_%d now in DEPARTMENT_%d WAIT QUEUE \n",
    				threadName,g_salesmanInfo[ThreadId].customerId,myDepartmentNo);


    		printStringIntInt("%s welcomes CUSTOMER_%d to DEPARTMENT_%d \n",
    				threadName,g_salesmanInfo[ThreadId].customerId,myDepartmentNo);

    		wait(g_customerSalesmanCV[ThreadId],g_customerSalesmanLock[ThreadId]);

    		printStringInt("%s GOT FINAL RESPONSE from CUSTOMER_%d now \n",threadName,g_salesmanInfo[ThreadId].customerId);

    		releaseLock(g_customerSalesmanLock[ThreadId]);
    	}
        else
        {
        	releaseLock(g_customerDepartmentLock[myDepartmentNo]);
        }

    	acquireLock(g_customerDepartmentComplainLock[myDepartmentNo]);

    	if(g_departmentComplainWaitQueue[myDepartmentNo]>0)
    	{

    		printStringInt("%s ---- DEPARTMENT_%d COMPLAIN WAIT QUEUE is not Empty... Someone Waiting for Assistance \n",
    				threadName,myDepartmentNo);

    		g_departmentComplainWaitQueue[myDepartmentNo]--;
    		/**
    		 * Very Important to acquire the g_customerSalesmanLock before signaling the
    		 * customer waiting on  g_customerDepartmentCV with g_customerDepartmentLock because:
    		 * In case we signal on  g_customerDepartmentCV with g_customerDepartmentLock and release
    		 * the g_customerDepartmentLock there is a chance of CS and customer acquiring the
    		 * g_customerSalesmanLock and then salesman and customer waiting for signals from each other
    		 * in other words LiveLock Situation
    		 */
    		acquireLock(g_customerSalesmanLock[ThreadId]);

    		g_salesmanInfo[ThreadId].status = salesmanSignalToCustomerFromDeptComplainWaitQueue;

    		signal(g_customerDepartmentComplainCV[myDepartmentNo],g_customerDepartmentComplainLock[myDepartmentNo]);

    		printStringInt("%s signaling someone waiting in DEPARTMENT_%d COMPLAIN WAIT QUEUE \n",threadName,
    				myDepartmentNo);

    		releaseLock(g_customerDepartmentComplainLock[myDepartmentNo]);

    		printStringInt("%s waiting for response from UNKNOWN CUSTOMER now in DEPARTMENT_%d COMPLAIN WAIT QUEUE \n",
    				threadName,myDepartmentNo);

    		wait(g_customerSalesmanCV[ThreadId],g_customerSalesmanLock[ThreadId]);

    		printStringIntInt("%s is informed by CUSTOMER_%d that ITEM_%d is out of stock \n",
    				threadName,
    				g_salesmanInfo[ThreadId].customerId,
    				g_salesmanInfo[ThreadId].itemToRestock);


    		/**************************START INTERACTING WITH GOODS LOADER NOW ***************************/

    		for(goodLoaderIndex=0;goodLoaderIndex<NO_OF_GOOD_LOADERS;goodLoaderIndex++)
    		{
    			acquireLock(g_salesmanGoodsLoaderLock[goodLoaderIndex]);

    			if(g_goodLoaderInfo[goodLoaderIndex].status == goodLoaderIsFree)
    			{
    				myGoodsLoader = goodLoaderIndex;
	    			printStringIntInt("%s will enquire about RESTOCK-ITEM_%d to GOODLOADER_%d\n",threadName,
	    					g_salesmanInfo[ThreadId].itemToRestock,myGoodsLoader);
    				g_goodLoaderInfo[goodLoaderIndex].status = goodLoaderIsBusy;
    				g_goodLoaderInfo[goodLoaderIndex].salesmanId = ThreadId;
    				break;
    			}
    			releaseLock(g_salesmanGoodsLoaderLock[goodLoaderIndex]);
    		}

    		if(myGoodsLoader == -1)
    		{
	    		printStringInt("%s is waiting in SINGLE LINE for GOOD LOADERS as "
	    				" NO GOODLOADER is free for RESTOCK-ITEM_%d inquiry so "
	    				"\n",threadName,
	    				g_salesmanInfo[ThreadId].itemToRestock
	    				);

    			acquireLock(g_goodLoaderWaitLock[0]);
    			g_goodLoaderWaitQueue[0]++;

    			wait(g_goodLoaderWaitCV[0],g_goodLoaderWaitLock[0]);
        		for(goodLoaderIndex=0;goodLoaderIndex<NO_OF_GOOD_LOADERS;goodLoaderIndex++)
        		{
        			acquireLock(g_salesmanGoodsLoaderLock[goodLoaderIndex]);

        			if(g_goodLoaderInfo[goodLoaderIndex].status == goodLoaderSignalToSalesman)
        			{
        				myGoodsLoader = goodLoaderIndex;
	        			printStringIntInt("%s assumes that GOOD LOADER_%d Signaled him regarding inquiry of RESTOCK ITEM_%d\n",
	        					threadName,myGoodsLoader,g_salesmanInfo[ThreadId].itemToRestock);
        				g_goodLoaderInfo[goodLoaderIndex].status = goodLoaderIsBusy;
        				g_goodLoaderInfo[goodLoaderIndex].salesmanId = ThreadId;
        				break;
        			}
        			releaseLock(g_salesmanGoodsLoaderLock[goodLoaderIndex]);
        		}
    		}

    		releaseLock(g_goodLoaderWaitLock[0]);

    		g_goodLoaderInfo[myGoodsLoader].itemToRestock = g_salesmanInfo[ThreadId].itemToRestock;

    		signal(g_salesmanGoodsLoaderCV[myGoodsLoader],g_salesmanGoodsLoaderLock[myGoodsLoader]);

    		printStringIntInt("%s informs GOOD LOADER_%d that ITEM_%d is out of stock \n",
    				threadName,myGoodsLoader,g_salesmanInfo[ThreadId].itemToRestock);

    		wait(g_salesmanGoodsLoaderCV[myGoodsLoader],g_salesmanGoodsLoaderLock[myGoodsLoader]);

    		printStringIntInt("%s is informed by the GOOD LOADER_%d that ITEM_%d is re stocked \n",
    				threadName,myGoodsLoader,g_salesmanInfo[ThreadId].itemToRestock);

    		releaseLock(g_salesmanGoodsLoaderLock[myGoodsLoader]);

    		/**************************END INTERACTING WITH GOODS LOADER NOW ***************************/

    		signal(g_customerSalesmanCV[ThreadId],g_customerSalesmanLock[ThreadId]);

    		printStringIntInt("%s SENDING RESPONSE to CUSTOMER_%d now in DEPARTMENT_%d COMPLAIN WAIT QUEUE \n",
    				threadName,g_salesmanInfo[ThreadId].customerId,myDepartmentNo);

    		printStringIntInt("%s informs the CUSTOMER_%d that ITEM_%d is re stocked \n",
    				threadName,g_salesmanInfo[ThreadId].customerId,
    				g_salesmanInfo[ThreadId].itemToRestock);

    		wait(g_customerSalesmanCV[ThreadId],g_customerSalesmanLock[ThreadId]);

    		printStringInt("%s GOT FINAL RESPONSE from CUSTOMER_%d now \n",threadName,g_salesmanInfo[ThreadId].customerId);

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

    		printStringIntInt("%s is RELAXING AS DEPARTMENT_%d WAIT QUEUE and DEPARTMENT_%d COMPLAIN WAIT QUEUE both are empty"
    				"SET STATUS TO FREE\n ",threadName,myDepartmentNo,myDepartmentNo);

    		acquireLock(g_customerSalesmanLock[ThreadId]);

    		g_salesmanInfo[ThreadId].status = salesmanIsFree;

    		releaseLock(g_customerDepartmentComplainLock[myDepartmentNo]);

    		releaseLock(g_customerDepartmentLock[myDepartmentNo]);

    		wait(g_customerSalesmanCV[ThreadId],g_customerSalesmanLock[ThreadId]);

    		printStringInt("%s GOT INITIAL RESPONSE from CUSTOMER_%d in NO WAIT QUEUE\n",
    				threadName,g_salesmanInfo[ThreadId].customerId);



    		if(g_salesmanInfo[ThreadId].itemToRestock == -1)
    		{
    			printString("%s is checking if INITIAL RESPONSE is not regarding RE-STOCK of Item in NO WAIT QUEUE\n",
    					threadName);

    			g_salesmanInfo[ThreadId].status = salesmanSignalToCustomerFromDeptWaitQueue;

    			signal(g_customerSalesmanCV[ThreadId],g_customerSalesmanLock[ThreadId]);

        		printStringInt("%s SENDING RESPONSE to CUSTOMER_%d in NO WAIT QUEUE \n",
        				threadName,g_salesmanInfo[ThreadId].customerId);

        		printStringIntInt("%s welcomes CUSTOMER_%d to DEPARTMENT_%d \n",
        				threadName,g_salesmanInfo[ThreadId].customerId,myDepartmentNo);

    			wait(g_customerSalesmanCV[ThreadId],g_customerSalesmanLock[ThreadId]);

        		printStringInt("%s GOT FINAL RESPONSE from CUSTOMER_%d in NO WAIT QUEUE \n",
        				threadName,g_salesmanInfo[ThreadId].customerId);

        		releaseLock(g_customerSalesmanLock[ThreadId]);
    		}
    		else
    		{
    			/**
    			 * Nothing for now
    			 */
    			printString("%s is IF INITIAL RESPONSE is regarding RE-STOCK of Item in NO WAIT QUEUE\n",threadName);

    			g_salesmanInfo[ThreadId].status = salesmanSignalToCustomerFromDeptComplainWaitQueue;

        		for(goodLoaderIndex=0;goodLoaderIndex<NO_OF_GOOD_LOADERS;goodLoaderIndex++)
        		{
        			acquireLock(g_salesmanGoodsLoaderLock[goodLoaderIndex]);

        			if(g_goodLoaderInfo[goodLoaderIndex].status == goodLoaderIsFree)
        			{
        				myGoodsLoader = goodLoaderIndex;
        	    		printStringInt(" %s is waiting in SINGLE LINE for GOOD LOADERS NO WAIT QUEUE as "
        	    				"NO GOODLOADER is free for RESTOCK-ITEM_%d inquiry so "
        	    				"\n",threadName,
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
    	    		printStringInt("%s is waiting in SINGLE LINE for "
    	    				"GOOD LOADERS NO WAIT QUEUE AS NO GOODLOADER is free for RESTOCK-ITEM_%d inquiry so \n",
    	    				threadName,
    	    				g_salesmanInfo[ThreadId].itemToRestock
    	    				);

        			acquireLock(g_goodLoaderWaitLock[0]);
        			g_goodLoaderWaitQueue[0]++;

        			wait(g_goodLoaderWaitCV[0],g_goodLoaderWaitLock[0]);

            		for(goodLoaderIndex=0;goodLoaderIndex<NO_OF_GOOD_LOADERS;goodLoaderIndex++)
            		{
            			acquireLock(g_salesmanGoodsLoaderLock[goodLoaderIndex]);

            			if(g_goodLoaderInfo[goodLoaderIndex].status == goodLoaderSignalToSalesman)
            			{
            				myGoodsLoader = goodLoaderIndex;
    	        			printStringIntInt("%s assumes that GOOD LOADER_%d Signaled him regarding inquiry of RESTOCK ITEM_%d\n",
    	        					threadName,myGoodsLoader,g_salesmanInfo[ThreadId].itemToRestock);
            				g_goodLoaderInfo[goodLoaderIndex].status = goodLoaderIsBusy;
            				g_goodLoaderInfo[goodLoaderIndex].salesmanId = ThreadId;
            				break;
            			}
            			releaseLock(g_salesmanGoodsLoaderLock[goodLoaderIndex]);
            		}
        		}

        		releaseLock(g_goodLoaderWaitLock[0]);

        		g_goodLoaderInfo[myGoodsLoader].itemToRestock = g_salesmanInfo[ThreadId].itemToRestock;

        		signal(g_salesmanGoodsLoaderCV[myGoodsLoader],g_salesmanGoodsLoaderLock[myGoodsLoader]);

        		printStringInt("%s SENDING RESPONSE to CUSTOMER_%d now in NO WAIT QUEUE \n",
        				threadName,g_salesmanInfo[ThreadId].customerId);

        		printStringIntInt("%s informs GOOD LOADER_%d that ITEM_%d is out of stock \n",
        				threadName,myGoodsLoader,g_salesmanInfo[ThreadId].itemToRestock);

        		wait(g_salesmanGoodsLoaderCV[myGoodsLoader],g_salesmanGoodsLoaderLock[myGoodsLoader]);

        		printStringIntInt("%s is informed by the GOOD LOADER_%d that ITEM_%d is re stocked \n",
        				threadName,myGoodsLoader,g_salesmanInfo[ThreadId].itemToRestock);

        		releaseLock(g_salesmanGoodsLoaderLock[myGoodsLoader]);

        		signal(g_customerSalesmanCV[ThreadId],g_customerSalesmanLock[ThreadId]);

        		printStringIntInt("%s informs the CUSTOMER_%d that ITEM_%d is re stocked \n",
        				threadName,g_salesmanInfo[ThreadId].customerId,
        				g_salesmanInfo[ThreadId].itemToRestock);

        		wait(g_customerSalesmanCV[ThreadId],g_customerSalesmanLock[ThreadId]);

        		printStringInt("%s GOT FINAL RESPONSE from CUSTOMER_%d now \n",threadName,g_salesmanInfo[ThreadId].customerId);

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
	char threadName[256];
	int i;

	acquireLock(g_goodsLoaderThreadCounterLock);
	ThreadId = g_goodsLoaderThreadCounter;
	g_goodsLoaderThreadCounter++;
	releaseLock(g_goodsLoaderThreadCounterLock);

    sprintf(threadName, GOOD_LOADERS_STRING,xstrlen(GOOD_LOADERS_STRING), ThreadId);
    printString( "%s Started !!!!!!! \n",threadName);

    while(1)
    {
    	int goodsLoaderWalkingTime;
    	goodsLoaderWalkingTime = Random()%(MAX_GOODS_LOADER_WALKINGTIME-MIN_GOODS_LOADER_WALKINGTIME) +
    			MIN_GOODS_LOADER_WALKINGTIME;

    	acquireLock(g_goodLoaderWaitLock[0]);

    	if(g_goodLoaderWaitQueue[0]>0)
    	{
    		printString("%s GOOD LOADER WAIT QUEUE IS NOT EMPTY...RE-STOCK OF SOME ITEM \n",threadName);

    		g_goodLoaderWaitQueue[0]--;

    		acquireLock(g_salesmanGoodsLoaderLock[ThreadId]);

    		g_goodLoaderInfo[ThreadId].status = goodLoaderSignalToSalesman;

    		signal(g_goodLoaderWaitCV[0],g_goodLoaderWaitLock[0]);

    		printString("%s signaling someone waiting in GOOD LOADERS WAIT QUEUE\n",threadName);

    		releaseLock(g_goodLoaderWaitLock[0]);

    		wait(g_salesmanGoodsLoaderCV[ThreadId],g_salesmanGoodsLoaderLock[ThreadId]);

    		printStringIntInt("%s is informed by SALESMAN_%d of DEPARTMENT_%d ",
    				threadName,g_goodLoaderInfo[ThreadId].salesmanId,
    				g_salesmanInfo[g_goodLoaderInfo[ThreadId].salesmanId].departmentNo);
    				print1("to re stock ITEM_%d \n",g_goodLoaderInfo[ThreadId].itemToRestock);


    		/**
    		 * BEFORE SIGNALING BACK TO SALESMAN RESTOCK THE ITEM
    		 */

    		acquireLock(g_shelfAccessLock[g_goodLoaderInfo[ThreadId].itemToRestock]);

    		g_itemInfo[g_goodLoaderInfo[ThreadId].itemToRestock].noOfItems = MAX_NO_ITEMS_PER_SHELF;

    		releaseLock(g_shelfAccessLock[g_goodLoaderInfo[ThreadId].itemToRestock]);

    		/**
    		 * Walking time from re stocking room to shelf
    		 */

    		for(i=0;i<goodsLoaderWalkingTime;i++)
    		{
    			Yield();
    			printStringInt("%s is walking from RE-STOCK room to shelf to RE-STOCK ITEM_%d\n",threadName,
    					g_goodLoaderInfo[ThreadId].itemToRestock);
    		}

    		signal(g_salesmanGoodsLoaderCV[ThreadId],g_salesmanGoodsLoaderLock[ThreadId]);

    		printStringIntInt("%s has re stocked ITEM_%d in DEPARTMENT_%d \n",
    				threadName,
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
    		printString("%s GOOD LOADER WAIT QUEUE IS EMPTY...RELAX \n",threadName);

    		printString("%s is waiting for orders to Re stock\n",threadName);

    		acquireLock(g_salesmanGoodsLoaderLock[ThreadId]);
    		g_goodLoaderInfo[ThreadId].status = goodLoaderIsFree;

    		releaseLock(g_goodLoaderWaitLock[0]);

    		print1("ThreadId is %d \n",ThreadId);
    		print1("g_salesmanGoodsLoaderCV = %d \n",g_salesmanGoodsLoaderCV[ThreadId]);
    		wait(g_salesmanGoodsLoaderCV[ThreadId],g_salesmanGoodsLoaderLock[ThreadId]);

    		printStringIntInt("%s is informed by SALESMAN_%d of DEPARTMENT_%d ",
    				threadName,g_goodLoaderInfo[ThreadId].salesmanId,
    				g_salesmanInfo[g_goodLoaderInfo[ThreadId].salesmanId].departmentNo);
    				print1("to re stock ITEM_%d \n",g_goodLoaderInfo[ThreadId].itemToRestock);

    		/**
    		 * BEFORE SIGNALING BACK TO SALESMAN RESTOCK THE ITEM
    		 */

    		acquireLock(g_shelfAccessLock[g_goodLoaderInfo[ThreadId].itemToRestock]);

    		g_itemInfo[g_goodLoaderInfo[ThreadId].itemToRestock].noOfItems = MAX_NO_ITEMS_PER_SHELF;

    		releaseLock(g_shelfAccessLock[g_goodLoaderInfo[ThreadId].itemToRestock]);

    		/**
    		 * Walking time from re stocking room to shelf
    		 */

    		for(i=0;i<goodsLoaderWalkingTime;i++)
    		{
    			Yield();
    			printStringInt("%s is walking from RE-STOCK room to shelf to RE-STOCK ITEM_%d\n",threadName,
    					g_goodLoaderInfo[ThreadId].itemToRestock);
    		}

    		signal(g_salesmanGoodsLoaderCV[ThreadId],g_salesmanGoodsLoaderLock[ThreadId]);

    		printStringIntInt("%s has re stocked ITEM_%d in DEPARTMENT_%d \n",
    				threadName,
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
	char threadName[256];
	int i;

	acquireLock(g_cashierThreadCounterLock);
	ThreadId = g_cashierThreadCounter;
	g_cashierThreadCounter++;
	releaseLock(g_cashierThreadCounterLock);

    sprintf(threadName, CASHIER_STRING,xstrlen(CASHIER_STRING), ThreadId);
    printString( "%s Started !!!!!!! \n",threadName);

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
    		signal(g_cashierPrivilegedLineCV[ThreadId],g_cashierPrivilegedLineLock[ThreadId]);
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
        		signal(g_cashierLineCV[ThreadId],g_cashierLineLock[ThreadId]);
        		acquireLock(g_customerCashierLock[ThreadId]);
    		}
    		else
    		{
    			acquireLock(g_customerCashierLock[ThreadId]);
    			g_cashierInfo[ThreadId].status = cashierIsFree;
    		}
    		releaseLock(g_cashierLineLock[ThreadId]);
    	}
		print2("Cashier %d total sales money is %d \n",ThreadId,g_cashierInfo[ThreadId].totalSalesMoney);
    	wait(g_customerCashierCV[ThreadId],g_customerCashierLock[ThreadId]);
    	myCustomer = g_cashierInfo[ThreadId].customerId;

    	for(i=0;i<g_customerInfo[myCustomer].noOfItems;i++)
    	{
        	currentItemNoFromShoppingList = g_customerInfo[myCustomer].pCustomerShoppingList[i].itemNo;
          	currentItemNoCountFromShoppingList = g_customerInfo[myCustomer].pCustomerShoppingList[i].noOfItems;
          	currentItemNoPriceFromShoppingList = g_itemInfo[currentItemNoFromShoppingList].Price;
          	totalBill +=  currentItemNoPriceFromShoppingList * currentItemNoCountFromShoppingList ;
          	printStringInt("%s got ITEM_%d from trolley\n",threadName,currentItemNoFromShoppingList);
    	}

    	g_cashierInfo[ThreadId].bill = totalBill;

    	signal(g_customerCashierCV[ThreadId],g_customerCashierLock[ThreadId]);

    	printStringIntInt("%s tells CUSTOMER_%d total cost is $ %d\n",threadName,myCustomer,totalBill);

    	wait(g_customerCashierCV[ThreadId],g_customerCashierLock[ThreadId]);

    	if(g_customerInfo[myCustomer].hasEnoughMoneyForShopping == false)
    	{
    		/**
    		 * REPORT CUSTOMER DOES NOT HAVE ENOUGH MONEY TO MANAGER
    		 */
    		acquireLock(g_managerCashierLock);

    		g_managerWaitQueueLength++;

    		wait(g_managerCashierCV,g_managerCashierLock);

    		g_managerInfo.cashierId = ThreadId;

    		acquireLock(g_managerCashierInteractionLock);

    		releaseLock(g_managerCashierLock);

    		signal(g_managerCashierInteractionCV,g_managerCashierInteractionLock);

    		printStringInt("%s informs Manager that CUSTOMER_%d does not have enough money\n",
    				threadName,myCustomer);

    		wait(g_managerCashierInteractionCV,g_managerCashierInteractionLock);

    		releaseLock(g_managerCashierInteractionLock);

        	signal(g_customerCashierCV[ThreadId],g_customerCashierLock[ThreadId]);

        	printStringInt("%s asks CUSTOMER_%d to wait for Manager\n",threadName,myCustomer);

        	wait(g_customerCashierCV[ThreadId],g_customerCashierLock[ThreadId]);

    	}
    	else
    	{
    		acquireLock(g_managerCashierCashLock[ThreadId]);
    		g_cashierInfo[ThreadId].totalSalesMoney += totalBill;
    		releaseLock(g_managerCashierCashLock[ThreadId]);

    		printStringIntInt("%s got money $ %d from CUSTOMER_%d\n",threadName,totalBill,myCustomer);

        	signal(g_customerCashierCV[ThreadId],g_customerCashierLock[ThreadId]);

        	printStringInt("%s gave the receipt to CUSTOMER_%d and tells him to leave\n",
        			threadName,myCustomer);

        	wait(g_customerCashierCV[ThreadId],g_customerCashierLock[ThreadId]);
    	}

    	releaseLock(g_customerCashierLock[ThreadId]);

    }
}

void ManagerThread()
{
    int ThreadId;
    int managerSales = 0;
    int itemRemoveCounter = 0;
	int currentItemNoToRemove = 0;
	int currentItemNoToRemoveCount = 0;
	int currentItemNoToRemovePrice = 0;
	int i;
	char threadName[256];


    sprintf(threadName, MANAGER_STRING,xstrlen(MANAGER_STRING), 0);
    printString( "%s Started !!!!!!! \n",threadName);

    while(!simulationEnd)
    {
    	itemRemoveCounter = 0;

    	for(i=0;i<NO_OF_CASHIERS;i++)
    	{
    		acquireLock(g_managerCashierCashLock[i]);
    		print2("Total Sales Money From Cashier %d is %d\n",i,g_cashierInfo[i].totalSalesMoney);
    		g_managerInfo.totalRevenue += g_cashierInfo[i].totalSalesMoney;
    		g_cashierInfo[i].totalSalesMoney = 0;
    		releaseLock(g_managerCashierCashLock[i]);
    	}

    	g_managerInfo.totalRevenue += managerSales;
    	managerSales = 0;
    	print1("Total Sale of the entire store until now is $ %d \n",g_managerInfo.totalRevenue);

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
    		signal(g_managerCashierCV,g_managerCashierLock);

    		g_managerWaitQueueLength--;

    		acquireLock(g_managerCashierInteractionLock);

    		releaseLock(g_managerCashierLock);

    		wait(g_managerCashierInteractionCV,g_managerCashierInteractionLock);

    		printStringInt("%s got a call from CASHIER_%d\n",threadName,g_managerInfo.cashierId);

    		signal(g_managerCashierInteractionCV,g_managerCashierInteractionLock);

    		acquireLock(g_managerCustomerInteractionLock);

    		releaseLock(g_managerCashierInteractionLock);

    		wait(g_managerCustomerInteractionCV,g_managerCustomerInteractionLock);

    		signal(g_managerCustomerInteractionCV,g_managerCustomerInteractionLock);

    		wait(g_managerCustomerInteractionCV,g_managerCustomerInteractionLock);

    		while(g_customerInfo[g_managerInfo.customerId].isDoneShopping != true)
    		{
    			currentItemNoToRemove =
    					g_customerInfo[g_managerInfo.customerId].pCustomerShoppingList[itemRemoveCounter].itemNo;

    			currentItemNoToRemovePrice = g_itemInfo[currentItemNoToRemove].Price;

    			currentItemNoToRemoveCount =
    					g_customerInfo[g_managerInfo.customerId].pCustomerShoppingList[itemRemoveCounter].noOfItems;

    			printStringIntInt("%s removes ITEM_%d from trolley of CUSTOMER_%d\n",
    					threadName,currentItemNoToRemove,g_managerInfo.customerId);

    			g_managerInfo.customerBill -= currentItemNoToRemovePrice * currentItemNoToRemoveCount;

        		signal(g_managerCustomerInteractionCV,g_managerCustomerInteractionLock);

        		wait(g_managerCustomerInteractionCV,g_managerCustomerInteractionLock);
    		}

    		managerSales += g_managerInfo.customerBill;

    		signal(g_managerCustomerInteractionCV,g_managerCustomerInteractionLock);

    		printStringInt("%s gives receipt to CUSTOMER_%d \n",threadName,g_managerInfo.customerId);

    		releaseLock(g_managerCustomerInteractionLock);

    		printStringInt("%s has total sale of $ %d\n",threadName,managerSales);
    	}

    	for(i=0;i<MANAGER_RANDOM_SLEEP_TIME;i++)
    	{
    		Yield();
    	}
    }

    Exit(0);
}

void initLockCvForSimulation()
{
	char lockName[256];
	char cvName[256];
	int i;


#if 1
	/**
	 * Creating Locks for Thread Counters
	 */
	g_customerThreadCounterLock = createLock("CustomerThreadCounterLock",
			sizeof("CustomerThreadCounterLock"));
	print1("g_customerThreadCounterLock = %d \n",g_customerThreadCounterLock);

	g_salesmanThreadCounterLock = createLock("SalesmanThreadCounterLock",
			sizeof("SalesmanThreadCounterLock"));
	print1("g_salesmanThreadCounterLock = %d \n",g_salesmanThreadCounterLock);

	g_goodsLoaderThreadCounterLock = createLock("GoodsLoaderThreadCounterLock",
			sizeof("GoodsLoaderThreadCounterLock"));
	print1("g_goodsLoaderThreadCounterLock = %d \n",g_goodsLoaderThreadCounterLock);

	g_cashierThreadCounterLock = createLock("CustomerThreadCounterLock",
			sizeof("CustomerThreadCounterLock"));
	print1("g_cashierThreadCounterLock = %d \n",g_cashierThreadCounterLock);
#endif

	/**
	 * Creating Locks for Thread Counters
	 */
	g_customerThreadCounterLock = createLock((char*)CUSTOMERTHREADCOUNTER_STRING,
			xstrlen(CUSTOMERTHREADCOUNTER_STRING));
	print1(" \n g_customerThreadCounterLock = %d \n",g_customerThreadCounterLock);

	g_salesmanThreadCounterLock = createLock((char*)SALESMANTHREADCOUNTER_STRING,
			xstrlen(SALESMANTHREADCOUNTER_STRING));
	print1("g_salesmanThreadCounterLock = %d \n",g_salesmanThreadCounterLock);

	g_goodsLoaderThreadCounterLock = createLock((char*)GOODSLOADERTHREADCOUNTER_STRING,
			xstrlen(GOODSLOADERTHREADCOUNTER_STRING));
	print1("g_goodsLoaderThreadCounterLock = %d \n",g_goodsLoaderThreadCounterLock);

	g_cashierThreadCounterLock = createLock((char*)CASHIERTHREADCOUNTER_STRING,
			xstrlen(CASHIERTHREADCOUNTER_STRING));
	print1("g_cashierThreadCounterLock = %d \n",g_cashierThreadCounterLock);


#if 0
	/**
	 * Locks, CV Customer-Trolley
	 */
	g_usedTrolleyCount = 0;
	g_waitForTrolleyCount = 0;

	g_customerTrolleyLock = createLock((char*)CUSTOMERTROLLEYLOCK_STRING,xstrlen(CUSTOMERTROLLEYLOCK_STRING));
	print1("g_customerTrolleyLock = %d \n",g_customerTrolleyLock);


	g_customerTrolleyCV = createCondition((char*)CUSTOMERTROLLEYCV_STRING,xstrlen(CUSTOMERTROLLEYCV_STRING));
	print1("g_customerTrolleyCV = %d \n",g_customerTrolleyCV);


	for(i=0;i<NO_OF_MANAGERS;i++)
	{
        sprintf (lockName,MANAGERCASHIERLOCK_STRING,
        		xstrlen(MANAGERCASHIERLOCK_STRING),i);
        g_managerCashierLock = createLock(lockName,xstrlen(lockName));
    	print1("g_managerCashierLock = %d \n",g_managerCashierLock);
        clearCharBuf(lockName);

        sprintf (cvName,MANAGERCASHIERCV_STRING,
        		xstrlen(MANAGERCASHIERCV_STRING),i);
        g_managerCashierCV = createCondition(cvName,xstrlen(cvName));
    	print1("g_managerCashierCV = %d \n",g_managerCashierCV);
        clearCharBuf(cvName);

        sprintf (lockName,MANAGERCASHIERINTERACTIONLOCK_STRING,
        		xstrlen(MANAGERCASHIERINTERACTIONLOCK_STRING),i);
        g_managerCashierInteractionLock = createLock(lockName,xstrlen(lockName));
    	print1("g_managerCashierInteractionLock = %d \n",g_managerCashierInteractionLock);
        clearCharBuf(lockName);

        sprintf (cvName,MANAGERCASHIERINTERACTIONCV_STRING,
        		xstrlen(MANAGERCASHIERINTERACTIONCV_STRING), i);
        g_managerCashierInteractionCV = createCondition(cvName,xstrlen(cvName));
    	print1("g_managerCashierInteractionCV = %d \n",g_managerCashierInteractionCV);
        clearCharBuf(cvName);
	}

	for(i=0;i<NO_OF_MANAGERS;i++)
	{
        sprintf (lockName,MANAGERCUSTOMERINTERACTIONLOCK_STRING,
        		xstrlen(MANAGERCUSTOMERINTERACTIONLOCK_STRING), i);
        g_managerCustomerInteractionLock = createLock(lockName,xstrlen(lockName));
    	print1("g_managerCustomerInteractionLock = %d \n",g_managerCustomerInteractionLock);
        clearCharBuf(lockName);

        sprintf (cvName,MANAGERCUSTOMERINTERACTIONCV_STRING,
        		xstrlen(MANAGERCUSTOMERINTERACTIONCV_STRING), i);
        g_managerCustomerInteractionCV = createCondition(cvName,xstrlen(cvName));
    	print1("g_managerCustomerInteractionCV = %d \n",g_managerCustomerInteractionCV);
        clearCharBuf(cvName);
	}

	for(i=0;i<NO_OF_CASHIERS;i++)
	{
        sprintf (lockName,CASHIERLINELOCK_STRING,
        		xstrlen(CASHIERLINELOCK_STRING),i);
        g_cashierLineLock[i] = createLock(lockName,xstrlen(lockName));
    	print2("g_cashierLineLock[%d] = %d \n",i,g_cashierLineLock[i]);
        clearCharBuf(lockName);

        sprintf (cvName,CASHIERLINECV_STRING,
        		xstrlen(CASHIERLINECV_STRING),i);
        g_cashierLineCV[i] = createCondition(cvName,xstrlen(cvName));
    	print2("g_cashierLineCV[%d] = %d \n",i,g_cashierLineCV[i]);
        clearCharBuf(cvName);

        g_cashierWaitQueue[i] = 0;

        sprintf (lockName,CASHIERPRIVILEGEDLINELOCK_STRING,
        		xstrlen(CASHIERPRIVILEGEDLINELOCK_STRING),i);
        g_cashierPrivilegedLineLock[i] = createLock(lockName,xstrlen(lockName));
    	print2("g_cashierPrivilegedLineLock[%d] = %d \n",i,g_cashierPrivilegedLineLock[i]);
        clearCharBuf(lockName);

        sprintf (cvName,CASHIERPRIVILEGEDLINECV_STRING,
        		xstrlen(CASHIERPRIVILEGEDLINECV_STRING),i);
        g_cashierPrivilegedLineCV[i] = createCondition(cvName,xstrlen(cvName));
    	print2("g_cashierPrivilegedLineCV[%d] = %d \n",i,g_cashierPrivilegedLineCV[i]);
        clearCharBuf(cvName);

        g_cashierPrivilegedWaitQueue[i] = 0;

        sprintf (lockName,CUSTOMERCASHIERLOCK_STRING,
        		xstrlen(CUSTOMERCASHIERLOCK_STRING),i);
        g_customerCashierLock[i] = createLock(lockName,xstrlen(lockName));
    	print2("g_customerCashierLock[%d] = %d \n",i,g_customerCashierLock[i]);
        clearCharBuf(lockName);

        sprintf (cvName,CUSTOMERCASHIERCV_STRING,
        		xstrlen(CUSTOMERCASHIERCV_STRING),i);
        g_customerCashierCV[i] = createCondition(cvName,xstrlen(cvName));
    	print2("g_customerCashierCV[%d] = %d \n",i,g_customerCashierCV[i]);
        clearCharBuf(cvName);
	}

	for(i=0;i<NO_OF_CASHIERS;i++)
	{
        sprintf (lockName, MANAGERCASHIERCASHLOCK_STRING,
        		xstrlen(MANAGERCASHIERCASHLOCK_STRING),i);
        g_managerCashierCashLock[i] = createLock(lockName,xstrlen(lockName));
    	print2("g_managerCashierCashLock[%d] = %d \n",i,g_managerCashierCashLock[i]);
        clearCharBuf(lockName);
	}


	for(i=0;i<NO_OF_SALESMAN;i++)
	{
		sprintf(lockName,CUSTOMERSALESMANLOCK_STRING,xstrlen(CUSTOMERSALESMANLOCK_STRING), i);
        g_customerSalesmanLock[i] = createLock(lockName,xstrlen(lockName));
    	print2("g_customerSalesmanLock[%d] = %d \n",i,g_customerSalesmanLock[i]);
        clearCharBuf(lockName);

        sprintf (cvName,CUSTOMERSALESMANCV_STRING,xstrlen(CUSTOMERSALESMANCV_STRING), i);
        g_customerSalesmanCV[i] = createCondition(cvName,xstrlen(cvName));
    	print2("g_customerSalesmanCV[%d] = %d \n",i,g_customerSalesmanCV[i]);
        clearCharBuf(cvName);
	}

	for(i=0;i<NO_OF_DEPARTMENT;i++)
	{
        sprintf (lockName,CUSTOMERDEPARTMENTLOCK_STRING,xstrlen(CUSTOMERDEPARTMENTLOCK_STRING), i);
        g_customerDepartmentLock[i] = createLock(lockName,xstrlen(lockName));
    	print2("g_customerDepartmentLock[%d] = %d \n",i,g_customerDepartmentLock[i]);
        clearCharBuf(lockName);

        sprintf (cvName,CUSTOMERDEPARTMENTCV_STRING,xstrlen(CUSTOMERDEPARTMENTCV_STRING), i);
        g_customerDepartmentCV[i] =  createCondition(cvName,xstrlen(cvName));
    	print2("g_customerDepartmentCV[%d] = %d \n",i,g_customerDepartmentCV[i]);
        clearCharBuf(cvName);

        g_departmentWaitQueue[i] = 0;
	}

	for(i =0;i<NO_OF_SHELFS;i++)
	{
        sprintf (lockName,SHELFACCESSLOCK_STRING,xstrlen(SHELFACCESSLOCK_STRING), i);
        g_shelfAccessLock[i] = createLock(lockName,xstrlen(lockName));
    	print2("g_shelfAccessLock[%d] = %d \n",i,g_shelfAccessLock[i]);
        clearCharBuf(lockName);
	}

	for(i=0;i<NO_OF_DEPARTMENT;i++)
	{
        sprintf (lockName,CUSTOMERDEPARTMENTCOMPLAINLOCK_STRING,
        		xstrlen(CUSTOMERDEPARTMENTCOMPLAINLOCK_STRING), i);
        g_customerDepartmentComplainLock[i] = createLock(lockName,xstrlen(lockName));
    	print2("g_customerDepartmentComplainLock[%d] = %d \n",i,g_customerDepartmentComplainLock[i]);
        clearCharBuf(lockName);

        sprintf (cvName,CUSTOMERDEPARTMENTCOMPLAINCV_STRING,
        		xstrlen(CUSTOMERDEPARTMENTCOMPLAINCV_STRING), i);
        g_customerDepartmentComplainCV[i] = createCondition(cvName,xstrlen(cvName));
    	print2("g_customerDepartmentComplainCV[%d] = %d \n",i,g_customerDepartmentComplainCV[i]);
        clearCharBuf(cvName);

        g_departmentComplainWaitQueue[i] = 0;
	}

	for(i=0;i<NO_OF_GOOD_LOADERS;i++)
	{
        sprintf (lockName,SALESMANGOODLOADERLOCK_STRING,xstrlen(SALESMANGOODLOADERLOCK_STRING), i);
        g_salesmanGoodsLoaderLock[i] = createLock(lockName,xstrlen(lockName));
    	print2("g_salesmanGoodsLoaderLock[%d] = %d \n",i,g_salesmanGoodsLoaderLock[i]);
        clearCharBuf(lockName);

        sprintf (cvName,SALESMANGOODLOADERCV_STRING,xstrlen(SALESMANGOODLOADERCV_STRING), i);
        g_salesmanGoodsLoaderCV[i] = createCondition(cvName,xstrlen(cvName));
    	print2("g_salesmanGoodsLoaderCV[%d] = %d \n",i,g_salesmanGoodsLoaderCV[i]);
        clearCharBuf(cvName);
	}

	for(i=0;i<NO_OF_GOODLOADER_WAIT_QUEUE;i++)
	{
        sprintf (lockName,GOODLOADERWAITQUEUELOCK_STRING,
        		xstrlen(GOODLOADERWAITQUEUELOCK_STRING),i);
        g_goodLoaderWaitLock[i] = createLock(lockName,xstrlen(lockName));
    	print2("g_goodLoaderWaitLock[%d] = %d \n",i,g_goodLoaderWaitLock[i]);
        clearCharBuf(lockName);

        sprintf (cvName,GOODLOADERWAITQUEUECV_STRING,
        		xstrlen(GOODLOADERWAITQUEUECV_STRING),i);
        g_goodLoaderWaitCV[i] = createCondition(cvName,xstrlen(cvName));
    	print2("g_goodLoaderWaitCV[%d] = %d \n",i, g_goodLoaderWaitCV[i]);

        clearCharBuf(cvName);

        g_goodLoaderWaitQueue[i] = 0;
	}
#endif

}

void printLockCvForSimulation()
{
	int i;

	print1("g_customerThreadCounterLock = %d \n",g_customerThreadCounterLock);
	print1("g_salesmanThreadCounterLock = %d \n",g_salesmanThreadCounterLock);
	print1("g_goodsLoaderThreadCounterLock = %d \n",g_goodsLoaderThreadCounterLock);
	print1("g_cashierThreadCounterLock = %d \n",g_cashierThreadCounterLock);

	print1("g_customerTrolleyLock = %d \n",g_customerTrolleyLock);
	print1("g_customerTrolleyCV = %d \n",g_customerTrolleyCV);


	for(i=0;i<NO_OF_MANAGERS;i++)
	{
    	print1("g_managerCashierLock = %d \n",g_managerCashierLock);

    	print1("g_managerCashierCV = %d \n",g_managerCashierCV);

    	print1("g_managerCashierInteractionLock = %d \n",g_managerCashierInteractionLock);

    	print1("g_managerCashierInteractionCV = %d \n",g_managerCashierInteractionCV);
	}

	for(i=0;i<NO_OF_MANAGERS;i++)
	{
    	print1("g_managerCustomerInteractionLock = %d \n",g_managerCustomerInteractionLock);
    	print1("g_managerCustomerInteractionCV = %d \n",g_managerCustomerInteractionCV);
	}

	for(i=0;i<NO_OF_CASHIERS;i++)
	{
    	print2("g_cashierLineLock[%d] = %d \n",i,g_cashierLineLock[i]);

    	print2("g_cashierLineCV[%d] = %d \n",i,g_cashierLineCV[i]);

    	print2("g_cashierPrivilegedLineLock[%d] = %d \n",i,g_cashierPrivilegedLineLock[i]);

    	print2("g_cashierPrivilegedLineCV[%d] = %d \n",i,g_cashierPrivilegedLineCV[i]);

    	print2("g_customerCashierLock[%d] = %d \n",i,g_customerCashierLock[i]);

    	print2("g_customerCashierCV[%d] = %d \n",i,g_customerCashierCV[i]);
	}

	for(i=0;i<NO_OF_CASHIERS;i++)
	{

    	print2("g_managerCashierCashLock[%d] = %d \n",i,g_managerCashierCashLock[i]);
	}


	for(i=0;i<NO_OF_SALESMAN;i++)
	{

    	print2("g_customerSalesmanLock[%d] = %d \n",i,g_customerSalesmanLock[i]);

    	print2("g_customerSalesmanCV[%d] = %d \n",i,g_customerSalesmanCV[i]);
	}

	for(i=0;i<NO_OF_DEPARTMENT;i++)
	{

    	print2("g_customerDepartmentLock[%d] = %d \n",i,g_customerDepartmentLock[i]);

    	print2("g_customerDepartmentCV[%d] = %d \n",i,g_customerDepartmentCV[i]);
	}

	for(i =0;i<NO_OF_SHELFS;i++)
	{

    	print2("g_shelfAccessLock[%d] = %d \n",i,g_shelfAccessLock[i]);
	}

	for(i=0;i<NO_OF_DEPARTMENT;i++)
	{

    	print2("g_customerDepartmentComplainLock[%d] = %d \n",i,g_customerDepartmentComplainLock[i]);

    	print2("g_customerDepartmentComplainCV[%d] = %d \n",i,g_customerDepartmentComplainCV[i]);

	}

	for(i=0;i<NO_OF_GOOD_LOADERS;i++)
	{

    	print2("g_salesmanGoodsLoaderLock[%d] = %d \n",i,g_salesmanGoodsLoaderLock[i]);

    	print2("g_salesmanGoodsLoaderCV[%d] = %d \n",i,g_salesmanGoodsLoaderCV[i]);
	}

	for(i=0;i<NO_OF_GOODLOADER_WAIT_QUEUE;i++)
	{

    	print2("g_goodLoaderWaitLock[%d] = %d \n",i,g_goodLoaderWaitLock[i]);

    	print2("g_goodLoaderWaitCV[%d] = %d \n",i, g_goodLoaderWaitCV[i]);

	}

}

void main(const char* testOption)
{
	int configRetVal;
	int i;

    print( "Entering Assign 1 Problem 2 !!!!!!! \n");
    print( "Starting to initialize all the Threads for Problem 2 !!!!!!! \n");

    print1("Number of Cashiers = %d \n",NO_OF_CASHIERS);
    print1("Number of Good Loaders = %d \n",NO_OF_GOOD_LOADERS);
    print1("Number of PrivilegedCustomers = %d \n",NO_OF_CUSTOMERS);
    print1("Number of Customers = %d \n",NO_OF_CUSTOMERS);
    print1("Number of Managers = %d \n",NO_OF_MANAGERS);
    print1("Number of DepartmentSalesmen = %d \n",NO_OF_SALESMAN);

    initLockCvForSimulation();


#if 0
    /**
     * Check if client wants to repeat the test with same configuration
     */

    /**
     * IF YES - DO NOTHING USE THE SAME OLD CONFIG FILE
     */
    if(strcmp(testOption,"N"))
    {
    	print("Repeat the same Previous test again\n");
    }
    /**
     * ELSE NO - CREATE A LATEST CONFIG FILE
     */
    if (!strcmp(testOption, "N"))
    {

    	print("Run the new test\n");

		/**
		 * Create configuration file for ITEM
		 */
		createConfigFileForItem();

		/**
		 * Create configuration file for CUSTOMER
		 */
		createConfigFileForCustomer();
    }

    /**
     * Read the values from config file into global data structures
     */
    startItemConfiguration("itemConfigFile.ini");
    startCustomerConfiguration("customerConfigFile.ini");

#endif

	initItemInfo();
	initCustomerInfo();
	initCustomerShoppingList();

	/**
	 *     printConfiguration();
	 */


    initSalesManInfo();

    initGoodLoaderInfo();

    initCashierInfo();

    initManagerInfo();



#if 0

    for(i = 0;i<NO_OF_MANAGERS;i++)
    {
       Fork(ManagerThread);
    }
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



