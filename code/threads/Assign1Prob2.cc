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
ItemInfo 		g_itemInfo[NO_OF_ITEM_TYPES];

CustomerInfo 	g_customerInfo[NO_OF_CUSTOMERS];

SalesManInfo 	g_salesmanInfo[NO_OF_SALESMAN];

GoodLoaderInfo 	g_goodLoaderInfo[NO_OF_GOOD_LOADERS];

CashierInfo		g_cashierInfo[NO_OF_CASHIERS];

ManagerInfo		g_managerInfo;

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
 * Locks for accessing items on the shelf
 */
Lock*		g_shelfAccessLock[NO_OF_SHELFS];

/**
 * Locks,Condition Variables and Queue Length for Customer-Department Interaction
 */
Lock*		g_customerDepartmentComplainLock[NO_OF_DEPARTMENT];
Condition*	g_customerDepartmentComplainCV[NO_OF_DEPARTMENT];
int			g_departmentComplainWaitQueue[NO_OF_DEPARTMENT];

/**
 * Locks,Condition Variables and Queue Length for Salesman - GoodsLoader Interaction
 */
Lock*		g_salesmanGoodsLoaderLock[NO_OF_GOOD_LOADERS];
Condition*  g_salesmanGoodsLoaderCV[NO_OF_GOOD_LOADERS];

Lock*		g_goodLoaderWaitLock[NO_OF_GOODLOADER_WAIT_QUEUE];
Condition*	g_goodLoaderWaitCV[NO_OF_GOODLOADER_WAIT_QUEUE];
int			g_goodLoaderWaitQueue[NO_OF_GOODLOADER_WAIT_QUEUE];

/**
 * Locks,Condition Variables and Queue Length for Customer - Cashier Interaction
 */
Lock*		g_cashierLineLock[NO_OF_CASHIERS];
Condition*	g_cashierLineCV[NO_OF_CASHIERS];
int			g_cashierWaitQueue[NO_OF_CASHIERS];

Lock*		g_cashierPrivilegedLineLock[NO_OF_CASHIERS];
Condition*  g_cashierPrivilegedLineCV[NO_OF_CASHIERS];
int			g_cashierPrivilegedWaitQueue[NO_OF_CASHIERS];

Lock*		g_customerCashierLock[NO_OF_CASHIERS];
Condition*	g_customerCashierCV[NO_OF_CASHIERS];

/**
 * Locks, Condition Variables and Queue Length for Manager - Cashier  Cash Collection Interaction
 */
Lock*		g_managerCashierCashLock[NO_OF_CASHIERS];

/**
 * Locks, Condition Variables and Queue Length for Manager Interaction
 */

Lock*		g_managerCashierLock;
Condition*	g_managerCashierCV;
int			g_managerWaitQueueLength;

Lock*		g_managerCashierInteractionLock;
Condition*	g_managerCashierInteractionCV;

/**
 * Locks, Condition Variables for Manager - Customer Interaction
 */
Lock*		g_managerCustomerInteractionLock;
Condition*	g_managerCustomerInteractionCV;


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
		for(int i = 0;i<NO_OF_CUSTOMERS;i++)
		{
			g_customerInfo[i].money = Random()%MAX_AMT_PER_CUSTOMER + 1;
			g_customerInfo[i].type = CustomerType(Random()%2);
			g_customerInfo[i].noOfItems = Random()%MAX_NO_ITEMS_TO_BE_PURCHASED + 1;
			g_customerInfo[i].pCustomerShoppingList = new CustomerShoppingList[g_customerInfo[i].noOfItems];
			g_customerInfo[i].hasEnoughMoneyForShopping = false;
			g_customerInfo[i].isDoneShopping = false;
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

void printCustomerInfo()
{
	for(int customerId=0;customerId<NO_OF_CUSTOMERS;customerId++)
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

static void initSalesManInfo()
{
    static bool firstCall = true;
    if(firstCall)
    {
    	for(int i=0;i<NO_OF_SALESMAN;i++)
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
    if(firstCall)
    {
    	for(int i=0;i<NO_OF_GOOD_LOADERS;i++)
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
    if(firstCall)
    {
    	for(int i=0;i<NO_OF_CASHIERS;i++)
    	{
    		g_cashierInfo[i].status = cashierIsBusy;
    		g_cashierInfo[i].customerId = -1;
    		g_cashierInfo[i].bill = -1;
    		g_cashierInfo[i].totalSalesMoney = 0;
    	}
		firstCall = false;
	}
}

void printCashierInfo(int cashierId)
{

}

static void initManagerInfo()
{
    static bool firstCall = true;
    if(firstCall)
    {
    	for(int i=0;i<NO_OF_MANAGERS;i++)
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

void CustomerThread(int ThreadId)
{
    DEBUG('p', "%s enters the SuperMarket !!!!!!! \n",currentThread->getName());

    /**
     * Local Variable for Customer Thread.
     */
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
    	DEBUG('p',"%s wants to shop ITEM_%d in DEPARTMENT_%d \n",
    			currentThread->getName(),
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
    		if(g_salesmanInfo[salesmanIndex].status == salesmanIsFree)
    		{
    			mySalesMan = salesmanIndex;
    			g_salesmanInfo[salesmanIndex].status = salesmanIsBusy;
    			g_salesmanInfo[salesmanIndex].customerId = ThreadId;
    			DEBUG('p',"%s will enquire about ITEM_%d to SALESMAN_%d\n",currentThread->getName(),
    					currentItemNoFromShoppingList,mySalesMan);
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

    		DEBUG('p',"NO SALESMAN is free for ITEM_%d inquiry so %s is waiting in Line for DEPARTMENT_%d\n",
    				currentItemNoFromShoppingList,
    				currentThread->getName(),
    				currentDepartmentNoForItem);

    		g_customerDepartmentLock[currentDepartmentNoForItem]->Acquire();

    		g_departmentWaitQueue[currentDepartmentNoForItem]++;

    		DEBUG('p',"%s gets in line for the DEPARTMENT_%d \n",currentThread->getName(),
    				currentDepartmentNoForItem);

    		g_customerDepartmentCV[currentDepartmentNoForItem]->Wait(g_customerDepartmentLock[currentDepartmentNoForItem]);

        	for(int salesmanIndex=salesManStartForDepartment;
        			salesmanIndex<salesManEndForDepartment;salesmanIndex++)
        	{
        		/**
        		 * \todo: ASSUMPTION HERE THAT FIRST SALESMAN WHO SIGNALED WILL BE OUR
        		 * SALESMAN (Will Work For Now)
        		 */
    			g_customerSalesmanLock[salesmanIndex]->Acquire();
        		if(g_salesmanInfo[salesmanIndex].status == salesmanSignalToCustomerFromDeptWaitQueue)
        		{
        			mySalesMan = salesmanIndex;
        			DEBUG('p',"%s assumes that SALESMAN_%d Signaled him regarding enquiry of ITEM_%d\n",
        					currentThread->getName(),mySalesMan,currentItemNoFromShoppingList);
        			g_salesmanInfo[salesmanIndex].status = salesmanIsBusy;
        			g_salesmanInfo[salesmanIndex].customerId = ThreadId;
        			break;
        		}
        		g_customerSalesmanLock[salesmanIndex]->Release();
        	}
        	g_customerDepartmentLock[currentDepartmentNoForItem]->Release();
    	}


		DEBUG('p',"%s is interacting with SALESMAN_%d from DEPARTMENT_%d \n",
				currentThread->getName(),mySalesMan,currentDepartmentNoForItem);

    	g_customerSalesmanCV[mySalesMan]->Signal(g_customerSalesmanLock[mySalesMan]);

    	g_customerSalesmanCV[mySalesMan]->Wait(g_customerSalesmanLock[mySalesMan]);

    	g_customerSalesmanCV[mySalesMan]->Signal(g_customerSalesmanLock[mySalesMan]);

    	g_customerSalesmanLock[mySalesMan]->Release();


		/**
		 * Customer started interaction with Department Salesman and will now
		 * start shopping from his shopping list.
		 */

		g_shelfAccessLock[currentItemNoFromShoppingList]->Acquire();

		if(g_itemInfo[currentItemNoFromShoppingList].noOfItems>=currentItemNoCountFromShoppingList)
		{
			g_itemInfo[currentItemNoFromShoppingList].noOfItems -= currentItemNoCountFromShoppingList;

			DEBUG('p',"%s has found ITEM_%d and placed ITEM_%d in the trolley \n",currentThread->getName(),
					currentItemNoFromShoppingList,currentItemNoCountFromShoppingList);
			g_shelfAccessLock[currentItemNoFromShoppingList]->Release();
		}
		else
		{
			g_shelfAccessLock[currentItemNoFromShoppingList]->Release();
			mySalesMan = -1;

    		DEBUG('p',"%s is not able to find ITEM_%d and is searching for SALESMAN's \n",
    				currentThread->getName(),currentItemNoFromShoppingList);

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
	    		if(g_salesmanInfo[salesmanIndex].status == salesmanIsFree)
	    		{
	    			mySalesMan = salesmanIndex;
	    			DEBUG('p',"%s will enquire about RESTOCK-ITEM_%d to SALESMAN_%d\n",currentThread->getName(),
	    					currentItemNoFromShoppingList,mySalesMan);
	    			g_salesmanInfo[salesmanIndex].status = salesmanIsBusy;
	    			g_salesmanInfo[salesmanIndex].customerId = ThreadId;
	    			g_salesmanInfo[salesmanIndex].itemToRestock = currentItemNoFromShoppingList;
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

	    		DEBUG('p',"NO SALESMAN is free for RESTOCK-ITEM_%d inquiry so %s is waiting in COMPLAIN LINE for DEPARTMENT_%d\n",
	    				currentItemNoFromShoppingList,
	    				currentThread->getName(),
	    				currentDepartmentNoForItem);

	    		g_customerDepartmentComplainLock[currentDepartmentNoForItem]->Acquire();

	    		g_departmentComplainWaitQueue[currentDepartmentNoForItem]++;

	    		g_customerDepartmentComplainCV[currentDepartmentNoForItem]->Wait(g_customerDepartmentComplainLock[currentDepartmentNoForItem]);

	        	for(int salesmanIndex=salesManStartForDepartment;
	        			salesmanIndex<salesManEndForDepartment;salesmanIndex++)
	        	{
	        		/**
	        		 * \todo: ASSUMPTION HERE THAT FIRST SALESMAN WHO SIGNALED WILL BE OUR
	        		 * SALESMAN (Will Work For Now)
	        		 */
	    			g_customerSalesmanLock[salesmanIndex]->Acquire();
	        		if(g_salesmanInfo[salesmanIndex].status == salesmanSignalToCustomerFromDeptComplainWaitQueue)
	        		{
		    			mySalesMan = salesmanIndex;
	        			DEBUG('p',"%s assumes that SALESMAN_%d Signaled him regarding inquiry of RESTOCK ITEM_%d\n",
	        					currentThread->getName(),mySalesMan,currentItemNoFromShoppingList);
		    			g_salesmanInfo[salesmanIndex].status = salesmanIsBusy;
		    			g_salesmanInfo[salesmanIndex].customerId = ThreadId;
		    			g_salesmanInfo[salesmanIndex].itemToRestock = currentItemNoFromShoppingList;
	        			break;
	        		}
	        		g_customerSalesmanLock[salesmanIndex]->Release();
	        	}
	        	g_customerDepartmentComplainLock[currentDepartmentNoForItem]->Release();
	    	}

	    	DEBUG('p',"%s is asking for assistance from SALESMAN_%d \n",
	    			currentThread->getName(),mySalesMan);

	    	g_customerSalesmanCV[mySalesMan]->Signal(g_customerSalesmanLock[mySalesMan]);

	    	g_customerSalesmanCV[mySalesMan]->Wait(g_customerSalesmanLock[mySalesMan]);

	    	DEBUG('p',"%s has received assistance about re stocking of ITEM_%d from SALESMAN_%d \n",
	    			currentThread->getName(),currentItemNoFromShoppingList,mySalesMan);

	    	g_customerSalesmanCV[mySalesMan]->Signal(g_customerSalesmanLock[mySalesMan]);

			g_itemInfo[currentItemNoFromShoppingList].noOfItems -= currentItemNoCountFromShoppingList;

	    	g_salesmanInfo[mySalesMan].itemToRestock = -1;
	    	g_salesmanInfo[mySalesMan].customerId = -1;

	    	g_customerSalesmanLock[mySalesMan]->Release();

		}

		DEBUG('p',"%s has finished shopping in  DEPARTMENT_%d \n",
				currentThread->getName(),currentDepartmentNoForItem);
        /*************************************CUSTOMER-SALESMAN INTERACTION ENDS HERE*********************************************/
    }

    DEBUG('p',"%s has finished shopping for all items \n",currentThread->getName());

    DEBUG('p',"%s is looking for the Cashier \n",currentThread->getName());

    /*******************************************CUSTOMER-CASHIER INTERACTION STARTS HERE*******************************************/

    /**
     * Finding the Cashier which is least busy.
     */
    for(int i=0;i<NO_OF_CASHIERS;i++)
    {
    	if(g_customerInfo[ThreadId].type == PRIVILEGED)
    	{
    		g_cashierPrivilegedLineLock[i]->Acquire();
    		if(g_cashierPrivilegedWaitQueue[i]<minCashierLineLength)
    		{
    			minCashierLineLength = g_cashierPrivilegedWaitQueue[i];
    			myCashier = i;
    		}
    		g_cashierPrivilegedLineLock[i]->Release();
    	}
    	else
    	{
    		g_cashierLineLock[i]->Acquire();
    		if(g_cashierPrivilegedWaitQueue[i]<minCashierLineLength)
    		{
    			minCashierLineLength = g_cashierWaitQueue[i];
    			myCashier = i;
    		}
    		g_cashierLineLock[i]->Release();
    	}
    }

	if(g_customerInfo[ThreadId].type == PRIVILEGED)
	{
		DEBUG('p',"%s chose CASHIER_%d with Privileged Line of length %d \n",
				currentThread->getName(),myCashier,minCashierLineLength);
	}
	else
	{
		DEBUG('p',"%s chose CASHIER_%d with Line of length %d \n",
				currentThread->getName(),myCashier,minCashierLineLength);
	}

    g_customerCashierLock[myCashier]->Acquire();
    if(g_cashierInfo[myCashier].status == cashierIsFree)
    {
    	g_cashierInfo[myCashier].status = cashierIsBusy;
    	g_cashierInfo[myCashier].customerId = ThreadId;
    }
    else
    {
    	if(g_customerInfo[ThreadId].type == PRIVILEGED)
    	{
    		g_cashierPrivilegedLineLock[myCashier]->Acquire();
    	    g_customerCashierLock[myCashier]->Release();
    	    g_cashierPrivilegedWaitQueue[myCashier]++;
    	    g_cashierPrivilegedLineCV[myCashier]->Wait(g_cashierPrivilegedLineLock[myCashier]);
    	}
    	else
    	{
    		g_cashierLineLock[myCashier]->Acquire();
    	    g_customerCashierLock[myCashier]->Release();
    	    g_cashierWaitQueue[myCashier]++;
    	    g_cashierLineCV[myCashier]->Wait(g_cashierLineLock[myCashier]);
    	}

    	g_customerCashierLock[myCashier]->Acquire();
    	g_cashierInfo[myCashier].status = cashierIsBusy;
    	g_cashierInfo[myCashier].customerId = ThreadId;

    	if(g_customerInfo[ThreadId].type == PRIVILEGED)
    	{
    		g_cashierPrivilegedLineLock[myCashier]->Release();
    	}
    	else
    	{
    		g_cashierLineLock[myCashier]->Release();
    	}
    }

    /**
     * Item Processing between Customer and Cashier.
     * Waits for the bill from Cashier
     */
    g_customerCashierCV[myCashier]->Signal(g_customerCashierLock[myCashier]);

    g_customerCashierCV[myCashier]->Wait(g_customerCashierLock[myCashier]);

    myBill = g_cashierInfo[myCashier].bill;

    if(myBill <= g_customerInfo[ThreadId].money)
    {
    	g_customerInfo[ThreadId].hasEnoughMoneyForShopping = true;
    	DEBUG('p',"%s pays %d amount to CASHIER_%d and is waiting for the receipt\n",
    			currentThread->getName(),myBill,myCashier);
    }
    else
    {
    	g_customerInfo[ThreadId].hasEnoughMoneyForShopping = false;
    	DEBUG('p',"%s cannot pay %d\n",
    			currentThread->getName(),myBill);
    }

    g_customerCashierCV[myCashier]->Signal(g_customerCashierLock[myCashier]);

    g_customerCashierCV[myCashier]->Wait(g_customerCashierLock[myCashier]);

    if(g_customerInfo[ThreadId].hasEnoughMoneyForShopping == true)
    {
    	DEBUG('p',"%s got receipt from CASHIER_%d and is now leaving\n",
    			currentThread->getName(),myCashier);
    }

	DEBUG('p',"%s signaled CASHIER_%d that I am leaving\n",
			currentThread->getName(),myCashier);

    g_customerCashierCV[myCashier]->Signal(g_customerCashierLock[myCashier]);

    g_managerCustomerInteractionLock->Acquire();

    g_customerCashierLock[myCashier]->Release();

    /*************************************CUSTOMER - CASHIER INTERACTION ENDS HERE*********************************************/

    /*************************************CUSTOMER - MANAGER INTERACTION STARTS HERE *******************************************/

    if(g_customerInfo[ThreadId].hasEnoughMoneyForShopping == false)
    {
    	DEBUG('p',"%s is waiting for MANAGER for negotiations\n",currentThread->getName());
    	g_managerInfo.customerBill = myBill;
    	g_managerInfo.customerId = ThreadId;

    	g_managerCustomerInteractionCV->Signal(g_managerCustomerInteractionLock);

    	g_managerCustomerInteractionCV->Wait(g_managerCustomerInteractionLock);

    	/**
    	 * Start Removing Items from the Trolley
    	 */
    	while(myBill<=g_customerInfo[ThreadId].hasEnoughMoneyForShopping)
    	{
    		g_managerCustomerInteractionCV->Signal(g_managerCustomerInteractionLock);
    		DEBUG('p',"%s tells MANAGER to remove ITEM_%d from trolley\n",
    				currentThread->getName(),
    				g_customerInfo[ThreadId].pCustomerShoppingList[currentItemNoCountToRemoveFromTrolley].itemNo);
        	g_managerCustomerInteractionCV->Wait(g_managerCustomerInteractionLock);
        	myBill = g_managerInfo.customerBill;
        	currentItemNoCountToRemoveFromTrolley++;
    	}

    	g_customerInfo[ThreadId].isDoneShopping = true;

    	DEBUG('p',"%s pays $ %d MANAGER after removing items and is waiting for receipt from MANAGER\n",
    			currentThread->getName(),myBill);

		g_managerCustomerInteractionCV->Signal(g_managerCustomerInteractionLock);
    	g_managerCustomerInteractionCV->Wait(g_managerCustomerInteractionLock);

    	DEBUG('p',"%s got receipt from MANAGER and is now leaving\n",currentThread->getName());

    	g_managerCustomerInteractionLock->Release();
    }

    else
    {
    	g_managerCustomerInteractionLock->Release();
    }

    /*************************************CUSTOMER - MANAGER INTERACTION ENDS HERE *******************************************/

    DEBUG('p',"%s left the shopping mall !!!!! \n",currentThread->getName());

    g_customerTrolleyLock->Acquire();
    g_usedTrolleyCount--;
    if(g_waitForTrolleyCount>0)
    {
    	g_customerTrolleyCV->Signal(g_customerTrolleyLock);
    }
    g_customerTrolleyLock->Release();

}

void SalesmanThread(int ThreadId)
{
    DEBUG('p', "%s Started !!!!!!! \n",currentThread->getName());
    DEBUG('p',"%s will be working for DEPARTMENT_%d \n",
    		currentThread->getName(),g_salesmanInfo[ThreadId].departmentNo);

    int myDepartmentNo = g_salesmanInfo[ThreadId].departmentNo;
    int myGoodsLoader = -1;

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

    		DEBUG('p',"%s ---- DEPARTMENT_%d WAIT QUEUE is not Empty... Someone Waiting for Assistance \n",
    				currentThread->getName(),myDepartmentNo);

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

    		g_salesmanInfo[ThreadId].status = salesmanSignalToCustomerFromDeptWaitQueue;

    		g_customerDepartmentCV[myDepartmentNo]->Signal(g_customerDepartmentLock[myDepartmentNo]);

    		DEBUG('p',"%s signaling someone waiting in DEPARTMENT_%d WAIT QUEUE\n",currentThread->getName(),
    				myDepartmentNo);

    		g_customerDepartmentLock[myDepartmentNo]->Release();

    		DEBUG('p',"%s waiting for response from UNKNOWN CUSTOMER now in DEPARTMENT_%d WAIT QUEUE \n",
    				currentThread->getName(),myDepartmentNo);

    		g_customerSalesmanCV[ThreadId]->Wait(g_customerSalesmanLock[ThreadId]);

    		DEBUG('p',"%s GOT INITIAL RESPONSE from CUSTOMER_%d now in DEPARTMENT_%d WAIT QUEUE\n",
    				currentThread->getName(),g_salesmanInfo[ThreadId].customerId,myDepartmentNo);

    		g_customerSalesmanCV[ThreadId]->Signal(g_customerSalesmanLock[ThreadId]);

    		DEBUG('p',"%s SENDING RESPONSE to CUSTOMER_%d now in DEPARTMENT_%d WAIT QUEUE \n",
    				currentThread->getName(),g_salesmanInfo[ThreadId].customerId,myDepartmentNo);


    		DEBUG('p',"%s welcomes CUSTOMER_%d to DEPARTMENT_%d \n",
    				currentThread->getName(),g_salesmanInfo[ThreadId].customerId,myDepartmentNo);

    		g_customerSalesmanCV[ThreadId]->Wait(g_customerSalesmanLock[ThreadId]);

    		DEBUG('p',"%s GOT FINAL RESPONSE from CUSTOMER_%d now \n",currentThread->getName(),g_salesmanInfo[ThreadId].customerId);

    		g_customerSalesmanLock[ThreadId]->Release();
    	}
        else
        {
        	g_customerDepartmentLock[myDepartmentNo]->Release();
        }

    	g_customerDepartmentComplainLock[myDepartmentNo]->Acquire();

    	if(g_departmentComplainWaitQueue[myDepartmentNo]>0)
    	{

    		DEBUG('p',"%s ---- DEPARTMENT_%d COMPLAIN WAIT QUEUE is not Empty... Someone Waiting for Assistance \n",
    				currentThread->getName(),myDepartmentNo);

    		g_departmentComplainWaitQueue[myDepartmentNo]--;
    		/**
    		 * Very Important to acquire the g_customerSalesmanLock before signaling the
    		 * customer waiting on  g_customerDepartmentCV with g_customerDepartmentLock because:
    		 * In case we signal on  g_customerDepartmentCV with g_customerDepartmentLock and release
    		 * the g_customerDepartmentLock there is a chance of CS and customer acquiring the
    		 * g_customerSalesmanLock and then salesman and customer waiting for signals from each other
    		 * in other words LiveLock Situation
    		 */
    		g_customerSalesmanLock[ThreadId]->Acquire();

    		g_salesmanInfo[ThreadId].status = salesmanSignalToCustomerFromDeptComplainWaitQueue;

    		g_customerDepartmentComplainCV[myDepartmentNo]->Signal(g_customerDepartmentComplainLock[myDepartmentNo]);

    		DEBUG('p',"%s signaling someone waiting in DEPARTMENT_%d COMPLAIN WAIT QUEUE \n",currentThread->getName(),
    				myDepartmentNo);

    		g_customerDepartmentComplainLock[myDepartmentNo]->Release();

    		DEBUG('p',"%s waiting for response from UNKNOWN CUSTOMER now in DEPARTMENT_%d COMPLAIN WAIT QUEUE \n",
    				currentThread->getName(),myDepartmentNo);

    		g_customerSalesmanCV[ThreadId]->Wait(g_customerSalesmanLock[ThreadId]);

    		DEBUG('p',"%s is informed by CUSTOMER_%d that ITEM_%d is out of stock \n",
    				currentThread->getName(),
    				g_salesmanInfo[ThreadId].customerId,
    				g_salesmanInfo[ThreadId].itemToRestock);


    		/**************************START INTERACTING WITH GOODS LOADER NOW ***************************/

    		for(int goodLoaderIndex=0;goodLoaderIndex<NO_OF_GOOD_LOADERS;goodLoaderIndex++)
    		{
    			g_salesmanGoodsLoaderLock[goodLoaderIndex]->Acquire();

    			if(g_goodLoaderInfo[goodLoaderIndex].status == goodLoaderIsFree)
    			{
    				myGoodsLoader = goodLoaderIndex;
	    			DEBUG('p',"%s will enquire about RESTOCK-ITEM_%d to GOODLOADER_%d\n",currentThread->getName(),
	    					g_salesmanInfo[ThreadId].itemToRestock,myGoodsLoader);
    				g_goodLoaderInfo[goodLoaderIndex].status = goodLoaderIsBusy;
    				g_goodLoaderInfo[goodLoaderIndex].salesmanId = ThreadId;
    				break;
    			}
    			g_salesmanGoodsLoaderLock[goodLoaderIndex]->Release();
    		}

    		if(myGoodsLoader == -1)
    		{
	    		DEBUG('p',"NO GOODLOADER is free for RESTOCK-ITEM_%d inquiry so %s is waiting in SINGLE LINE for "
	    				"GOOD LOADERS\n",
	    				g_salesmanInfo[ThreadId].itemToRestock,
	    				currentThread->getName());

    			g_goodLoaderWaitLock[0]->Acquire();
    			g_goodLoaderWaitQueue[0]++;

    			g_goodLoaderWaitCV[0]->Wait(g_goodLoaderWaitLock[0]);
        		for(int goodLoaderIndex=0;goodLoaderIndex<NO_OF_GOOD_LOADERS;goodLoaderIndex++)
        		{
        			g_salesmanGoodsLoaderLock[goodLoaderIndex]->Acquire();

        			if(g_goodLoaderInfo[goodLoaderIndex].status == goodLoaderSignalToSalesman)
        			{
        				myGoodsLoader = goodLoaderIndex;
	        			DEBUG('p',"%s assumes that GOOD LOADER_%d Signaled him regarding inquiry of RESTOCK ITEM_%d\n",
	        					currentThread->getName(),myGoodsLoader,g_salesmanInfo[ThreadId].itemToRestock);
        				g_goodLoaderInfo[goodLoaderIndex].status = goodLoaderIsBusy;
        				g_goodLoaderInfo[goodLoaderIndex].salesmanId = ThreadId;
        				break;
        			}
        			g_salesmanGoodsLoaderLock[goodLoaderIndex]->Release();
        		}
    		}

    		g_goodLoaderWaitLock[0]->Release();

    		g_goodLoaderInfo[myGoodsLoader].itemToRestock = g_salesmanInfo[ThreadId].itemToRestock;

    		g_salesmanGoodsLoaderCV[myGoodsLoader]->Signal(g_salesmanGoodsLoaderLock[myGoodsLoader]);

    		DEBUG('p',"%s informs GOOD LOADER_%d that ITEM_%d is out of stock \n",
    				currentThread->getName(),myGoodsLoader,g_salesmanInfo[ThreadId].itemToRestock);

    		g_salesmanGoodsLoaderCV[myGoodsLoader]->Wait(g_salesmanGoodsLoaderLock[myGoodsLoader]);

    		DEBUG('p',"%s is informed by the GOOD LOADER_%d that ITEM_%d is re stocked \n",
    				currentThread->getName(),myGoodsLoader,g_salesmanInfo[ThreadId].itemToRestock);

    		g_salesmanGoodsLoaderLock[myGoodsLoader]->Release();

    		/**************************END INTERACTING WITH GOODS LOADER NOW ***************************/

    		g_customerSalesmanCV[ThreadId]->Signal(g_customerSalesmanLock[ThreadId]);

    		DEBUG('p',"%s SENDING RESPONSE to CUSTOMER_%d now in DEPARTMENT_%d COMPLAIN WAIT QUEUE \n",
    				currentThread->getName(),g_salesmanInfo[ThreadId].customerId,myDepartmentNo);

    		DEBUG('p',"%s informs the CUSTOMER_%d that ITEM_%d is re stocked \n",
    				currentThread->getName(),g_salesmanInfo[ThreadId].customerId,
    				g_salesmanInfo[ThreadId].itemToRestock);

    		g_customerSalesmanCV[ThreadId]->Wait(g_customerSalesmanLock[ThreadId]);

    		DEBUG('p',"%s GOT FINAL RESPONSE from CUSTOMER_%d now \n",currentThread->getName(),g_salesmanInfo[ThreadId].customerId);

    		myGoodsLoader = -1;
    		g_salesmanInfo[ThreadId].itemToRestock = -1;

    		g_customerSalesmanLock[ThreadId]->Release();
    	}
        else
        {
        	g_customerDepartmentComplainLock[myDepartmentNo]->Release();
        }

    	g_customerDepartmentLock[myDepartmentNo]->Acquire();
    	g_customerDepartmentComplainLock[myDepartmentNo]->Acquire();

    	if(g_departmentWaitQueue[myDepartmentNo] == 0 &&
    			g_departmentComplainWaitQueue[myDepartmentNo] == 0)
    	{

    		DEBUG('p',"%s is RELAXING AS DEPARTMENT_%d WAIT QUEUE and DEPARTMENT_%d COMPLAIN WAIT QUEUE both are empty"
    				"SET STATUS TO FREE\n ",currentThread->getName(),myDepartmentNo,myDepartmentNo);

    		g_customerSalesmanLock[ThreadId]->Acquire();

    		g_salesmanInfo[ThreadId].status = salesmanIsFree;

    		g_customerDepartmentComplainLock[myDepartmentNo]->Release();

    		g_customerDepartmentLock[myDepartmentNo]->Release();

    		g_customerSalesmanCV[ThreadId]->Wait(g_customerSalesmanLock[ThreadId]);

    		DEBUG('p',"%s GOT INITIAL RESPONSE from CUSTOMER_%d in NO WAIT QUEUE\n",
    				currentThread->getName(),g_salesmanInfo[ThreadId].customerId);



    		if(g_salesmanInfo[ThreadId].itemToRestock == -1)
    		{
    			DEBUG('p',"%s is checking if INITIAL RESPONSE is not regarding RE-STOCK of Item in NO WAIT QUEUE\n",
    					currentThread->getName());

    			g_salesmanInfo[ThreadId].status = salesmanSignalToCustomerFromDeptWaitQueue;

    			g_customerSalesmanCV[ThreadId]->Signal(g_customerSalesmanLock[ThreadId]);

        		DEBUG('p',"%s SENDING RESPONSE to CUSTOMER_%d in NO WAIT QUEUE \n",
        				currentThread->getName(),g_salesmanInfo[ThreadId].customerId);

        		DEBUG('p',"%s welcomes CUSTOMER_%d to DEPARTMENT_%d \n",
        				currentThread->getName(),g_salesmanInfo[ThreadId].customerId,myDepartmentNo);

    			g_customerSalesmanCV[ThreadId]->Wait(g_customerSalesmanLock[ThreadId]);

        		DEBUG('p',"%s GOT FINAL RESPONSE from CUSTOMER_%d in NO WAIT QUEUE \n",
        				currentThread->getName(),g_salesmanInfo[ThreadId].customerId);

        		g_customerSalesmanLock[ThreadId]->Release();
    		}
    		else
    		{
    			/**
    			 * Nothing for now
    			 */
    			DEBUG('p',"%s is IF INITIAL RESPONSE is regarding RE-STOCK of Item in NO WAIT QUEUE\n",currentThread->getName());

    			g_salesmanInfo[ThreadId].status = salesmanSignalToCustomerFromDeptComplainWaitQueue;

        		for(int goodLoaderIndex=0;goodLoaderIndex<NO_OF_GOOD_LOADERS;goodLoaderIndex++)
        		{
        			g_salesmanGoodsLoaderLock[goodLoaderIndex]->Acquire();

        			if(g_goodLoaderInfo[goodLoaderIndex].status == goodLoaderIsFree)
        			{
        				myGoodsLoader = goodLoaderIndex;
        	    		DEBUG('p',"NO GOODLOADER is free for RESTOCK-ITEM_%d inquiry so %s is waiting in SINGLE LINE for "
        	    				"GOOD LOADERS NO WAIT QUEUE\n",
        	    				g_salesmanInfo[ThreadId].itemToRestock,
        	    				currentThread->getName());
        				g_goodLoaderInfo[goodLoaderIndex].status = goodLoaderIsBusy;
        				g_goodLoaderInfo[goodLoaderIndex].salesmanId = ThreadId;
        				break;
        			}
        			g_salesmanGoodsLoaderLock[goodLoaderIndex]->Release();
        		}

        		if(myGoodsLoader == -1)
        		{
    	    		DEBUG('p',"NO GOODLOADER is free for RESTOCK-ITEM_%d inquiry so %s is waiting in SINGLE LINE for "
    	    				"GOOD LOADERS NO WAIT QUEUE\n",
    	    				g_salesmanInfo[ThreadId].itemToRestock,
    	    				currentThread->getName());

        			g_goodLoaderWaitLock[0]->Acquire();
        			g_goodLoaderWaitQueue[0]++;

        			g_goodLoaderWaitCV[0]->Wait(g_goodLoaderWaitLock[0]);

            		for(int goodLoaderIndex=0;goodLoaderIndex<NO_OF_GOOD_LOADERS;goodLoaderIndex++)
            		{
            			g_salesmanGoodsLoaderLock[goodLoaderIndex]->Acquire();

            			if(g_goodLoaderInfo[goodLoaderIndex].status == goodLoaderSignalToSalesman)
            			{
            				myGoodsLoader = goodLoaderIndex;
    	        			DEBUG('p',"%s assumes that GOOD LOADER_%d Signaled him regarding inquiry of RESTOCK ITEM_%d\n",
    	        					currentThread->getName(),myGoodsLoader,g_salesmanInfo[ThreadId].itemToRestock);
            				g_goodLoaderInfo[goodLoaderIndex].status = goodLoaderIsBusy;
            				g_goodLoaderInfo[goodLoaderIndex].salesmanId = ThreadId;
            				break;
            			}
            			g_salesmanGoodsLoaderLock[goodLoaderIndex]->Release();
            		}
        		}

        		g_goodLoaderWaitLock[0]->Release();

        		g_goodLoaderInfo[myGoodsLoader].itemToRestock = g_salesmanInfo[ThreadId].itemToRestock;

        		g_salesmanGoodsLoaderCV[myGoodsLoader]->Signal(g_salesmanGoodsLoaderLock[myGoodsLoader]);

        		DEBUG('p',"%s SENDING RESPONSE to CUSTOMER_%d now in NO WAIT QUEUE \n",
        				currentThread->getName(),g_salesmanInfo[ThreadId].customerId);

        		DEBUG('p',"%s informs GOOD LOADER_%d that ITEM_%d is out of stock \n",
        				currentThread->getName(),myGoodsLoader,g_salesmanInfo[ThreadId].itemToRestock);

        		g_salesmanGoodsLoaderCV[myGoodsLoader]->Wait(g_salesmanGoodsLoaderLock[myGoodsLoader]);

        		DEBUG('p',"%s is informed by the GOOD LOADER_%d that ITEM_%d is re stocked \n",
        				currentThread->getName(),myGoodsLoader,g_salesmanInfo[ThreadId].itemToRestock);

        		g_salesmanGoodsLoaderLock[myGoodsLoader]->Release();

        		g_customerSalesmanCV[ThreadId]->Signal(g_customerSalesmanLock[ThreadId]);

        		DEBUG('p',"%s informs the CUSTOMER_%d that ITEM_%d is re stocked \n",
        				currentThread->getName(),g_salesmanInfo[ThreadId].customerId,
        				g_salesmanInfo[ThreadId].itemToRestock);

        		g_customerSalesmanCV[ThreadId]->Wait(g_customerSalesmanLock[ThreadId]);

        		DEBUG('p',"%s GOT FINAL RESPONSE from CUSTOMER_%d now \n",currentThread->getName(),g_salesmanInfo[ThreadId].customerId);

        		myGoodsLoader = -1;
        		g_salesmanInfo[ThreadId].itemToRestock = -1;

        		g_customerSalesmanLock[ThreadId]->Release();
    		}
    	}
    	else
    	{
    		g_customerDepartmentLock[myDepartmentNo]->Release();
    		g_customerDepartmentComplainLock[myDepartmentNo]->Release();
    	}
    }
}

void GoodLoaderThread(int ThreadId)
{
    DEBUG('p', "%s Started !!!!!!! \n",currentThread->getName());

    int mySalesman = -1;

    while(1)
    {
    	int goodsLoaderWalkingTime;
    	goodsLoaderWalkingTime = Random()%(MAX_GOODS_LOADER_WALKINGTIME-MIN_GOODS_LOADER_WALKINGTIME) +
    			MIN_GOODS_LOADER_WALKINGTIME;

    	g_goodLoaderWaitLock[0]->Acquire();

    	if(g_goodLoaderWaitQueue[0]>0)
    	{
    		DEBUG('p',"%s GOOD LOADER WAIT QUEUE IS NOT EMPTY...RE-STOCK OF SOME ITEM \n",currentThread->getName());

    		g_goodLoaderWaitQueue[0]--;

    		g_salesmanGoodsLoaderLock[ThreadId]->Acquire();

    		g_goodLoaderInfo[ThreadId].status = goodLoaderSignalToSalesman;

    		g_goodLoaderWaitCV[0]->Signal(g_goodLoaderWaitLock[0]);

    		DEBUG('p',"%s signaling someone waiting in GOOD LOADERS WAIT QUEUE\n",currentThread->getName());

    		g_goodLoaderWaitLock[0]->Release();

    		g_salesmanGoodsLoaderCV[ThreadId]->Wait(g_salesmanGoodsLoaderLock[ThreadId]);

    		DEBUG('p',"%s is informed by SALESMAN_%d of DEPARTMENT_%d to re stock ITEM_%d \n",
    				currentThread->getName(),g_goodLoaderInfo[ThreadId].salesmanId,
    				g_salesmanInfo[g_goodLoaderInfo[ThreadId].salesmanId].departmentNo,
    				g_goodLoaderInfo[ThreadId].itemToRestock);

    		/**
    		 * BEFORE SIGNALING BACK TO SALESMAN RESTOCK THE ITEM
    		 */

    		g_shelfAccessLock[g_goodLoaderInfo[ThreadId].itemToRestock]->Acquire();

    		g_itemInfo[g_goodLoaderInfo[ThreadId].itemToRestock].noOfItems = MAX_NO_ITEMS_PER_SHELF;

    		g_shelfAccessLock[g_goodLoaderInfo[ThreadId].itemToRestock]->Release();

    		/**
    		 * Walking time from re stocking room to shelf
    		 */

    		for(int i=0;i<goodsLoaderWalkingTime;i++)
    		{
    			currentThread->Yield();
    			DEBUG('p',"%s is walking from RE-STOCK room to shelf to RE-STOCK ITEM_%d\n",currentThread->getName(),
    					g_goodLoaderInfo[ThreadId].itemToRestock);
    		}

    		g_salesmanGoodsLoaderCV[ThreadId]->Signal(g_salesmanGoodsLoaderLock[ThreadId]);

    		DEBUG('p',"%s has re stocked ITEM_%d in DEPARTMENT_%d \n",
    				currentThread->getName(),
    				g_goodLoaderInfo[ThreadId].itemToRestock,
    				g_salesmanInfo[g_goodLoaderInfo[ThreadId].salesmanId].departmentNo);

    		g_salesmanGoodsLoaderLock[ThreadId]->Release();
    	}
    	else
    	{
    		g_goodLoaderWaitLock[0]->Release();
    	}

    	g_goodLoaderWaitLock[0]->Acquire();

    	if(g_goodLoaderWaitQueue[0] == 0)
    	{
    		DEBUG('p',"%s GOOD LOADER WAIT QUEUE IS EMPTY...RELAX \n",currentThread->getName());

    		DEBUG('p',"%s is waiting for orders to Re stock\n",currentThread->getName());

    		g_salesmanGoodsLoaderLock[ThreadId]->Acquire();
    		g_goodLoaderInfo[ThreadId].status = goodLoaderIsFree;

    		g_goodLoaderWaitLock[0]->Release();

    		g_salesmanGoodsLoaderCV[ThreadId]->Wait(g_salesmanGoodsLoaderLock[ThreadId]);

    		DEBUG('p',"%s is informed by SALESMAN_%d of DEPARTMENT_%d to re stock ITEM_%d \n",
    				currentThread->getName(),g_goodLoaderInfo[ThreadId].salesmanId,
    				g_salesmanInfo[g_goodLoaderInfo[ThreadId].salesmanId].departmentNo,
    				g_goodLoaderInfo[ThreadId].itemToRestock);

    		/**
    		 * BEFORE SIGNALING BACK TO SALESMAN RESTOCK THE ITEM
    		 */

    		g_shelfAccessLock[g_goodLoaderInfo[ThreadId].itemToRestock]->Acquire();

    		g_itemInfo[g_goodLoaderInfo[ThreadId].itemToRestock].noOfItems = MAX_NO_ITEMS_PER_SHELF;

    		g_shelfAccessLock[g_goodLoaderInfo[ThreadId].itemToRestock]->Release();

    		/**
    		 * Walking time from re stocking room to shelf
    		 */

    		for(int i=0;i<goodsLoaderWalkingTime;i++)
    		{
    			currentThread->Yield();
    			DEBUG('p',"%s is walking from RE-STOCK room to shelf to RE-STOCK ITEM_%d\n",currentThread->getName(),
    					g_goodLoaderInfo[ThreadId].itemToRestock);
    		}

    		g_salesmanGoodsLoaderCV[ThreadId]->Signal(g_salesmanGoodsLoaderLock[ThreadId]);

    		DEBUG('p',"%s has re stocked ITEM_%d in DEPARTMENT_%d \n",
    				currentThread->getName(),
    				g_goodLoaderInfo[ThreadId].itemToRestock,
    				g_salesmanInfo[g_goodLoaderInfo[ThreadId].salesmanId].departmentNo);

    		g_salesmanGoodsLoaderLock[ThreadId]->Release();
    	}

    	else
    	{
    		g_goodLoaderWaitLock[0]->Release();
    	}
    }
}


void CashierThread(int ThreadId)
{
    DEBUG('p', "%s Started !!!!!!! \n",currentThread->getName());
	int myCustomer = -1;
	int totalBill = 0;
    int currentItemNoFromShoppingList;
    int currentItemNoCountFromShoppingList;
    int currentItemNoPriceFromShoppingList;
    while(1)
    {
    	myCustomer = -1;
    	totalBill = 0;
        currentItemNoFromShoppingList = 0;
        currentItemNoCountFromShoppingList = 0;
        currentItemNoPriceFromShoppingList = 0;

    	g_cashierPrivilegedLineLock[ThreadId]->Acquire();
    	if(g_cashierPrivilegedWaitQueue[ThreadId]>0)
    	{
    		g_cashierPrivilegedWaitQueue[ThreadId]--;
    		g_cashierInfo[ThreadId].status = cashierSignalToCustomer;
    		g_cashierPrivilegedLineCV[ThreadId]->Signal(g_cashierPrivilegedLineLock[ThreadId]);
    		g_customerCashierLock[ThreadId]->Acquire();
    		g_cashierPrivilegedLineLock[ThreadId]->Release();
    	}
    	else
    	{
    		g_cashierPrivilegedLineLock[ThreadId]->Release();

    		g_cashierLineLock[ThreadId]->Acquire();

    		if(g_cashierWaitQueue[ThreadId]>0)
    		{
    			g_cashierWaitQueue[ThreadId]--;
        		g_cashierInfo[ThreadId].status = cashierSignalToCustomer;
        		g_cashierLineCV[ThreadId]->Signal(g_cashierLineLock[ThreadId]);
        		g_customerCashierLock[ThreadId]->Acquire();
    		}
    		else
    		{
    			g_customerCashierLock[ThreadId]->Acquire();
    			g_cashierInfo[ThreadId].status = cashierIsFree;
    		}
    		g_cashierLineLock[ThreadId]->Release();
    	}

    	g_customerCashierCV[ThreadId]->Wait(g_customerCashierLock[ThreadId]);
    	myCustomer = g_cashierInfo[ThreadId].customerId;

    	for(int i=0;i<g_customerInfo[myCustomer].noOfItems;i++)
    	{
        	currentItemNoFromShoppingList = g_customerInfo[myCustomer].pCustomerShoppingList[i].itemNo;
          	currentItemNoCountFromShoppingList = g_customerInfo[myCustomer].pCustomerShoppingList[i].noOfItems;
          	currentItemNoPriceFromShoppingList = g_itemInfo[currentItemNoFromShoppingList].Price;
          	totalBill +=  currentItemNoPriceFromShoppingList * currentItemNoCountFromShoppingList ;
          	DEBUG('p',"%s got ITEM_%d from trolley\n",currentThread->getName(),currentItemNoFromShoppingList);
    	}

    	g_cashierInfo[ThreadId].bill = totalBill;

    	g_customerCashierCV[ThreadId]->Signal(g_customerCashierLock[ThreadId]);

    	DEBUG('p',"%s tells CUSTOMER_%d total cost is $ %d\n",currentThread->getName(),myCustomer,totalBill);

    	g_customerCashierCV[ThreadId]->Wait(g_customerCashierLock[ThreadId]);

    	if(g_customerInfo[myCustomer].hasEnoughMoneyForShopping == false)
    	{
    		/**
    		 * REPORT CUSTOMER DOES NOT HAVE ENOUGH MONEY TO MANAGER
    		 */
    		g_managerCashierLock->Acquire();

    		g_managerWaitQueueLength++;

    		g_managerCashierCV->Wait(g_managerCashierLock);

    		g_managerInfo.cashierId = ThreadId;

    		g_managerCashierInteractionLock->Acquire();

    		g_managerCashierLock->Release();

    		g_managerCashierInteractionCV->Signal(g_managerCashierInteractionLock);

    		DEBUG('p',"%s informs Manager that CUSTOMER_%d does not have enough money\n",
    				currentThread->getName(),myCustomer);

    		g_managerCashierInteractionCV->Wait(g_managerCashierInteractionLock);

    		g_managerCashierInteractionLock->Release();

        	g_customerCashierCV[ThreadId]->Signal(g_customerCashierLock[ThreadId]);

        	DEBUG('p',"%s asks CUSTOMER_%d to wait for Manager\n",currentThread->getName(),myCustomer);

        	g_customerCashierCV[ThreadId]->Wait(g_customerCashierLock[ThreadId]);

    	}
    	else
    	{
    		g_managerCashierCashLock[ThreadId]->Acquire();
    		g_cashierInfo[ThreadId].totalSalesMoney += totalBill;
    		g_managerCashierCashLock[ThreadId]->Release();

    		DEBUG('p',"%s got money $ %d from CUSTOMER_%d\n",currentThread->getName(),totalBill,myCustomer);

        	g_customerCashierCV[ThreadId]->Signal(g_customerCashierLock[ThreadId]);

        	DEBUG('p',"%s gave the receipt to CUSTOMER_%d and tells him to leave\n",
        			currentThread->getName(),myCustomer);

        	g_customerCashierCV[ThreadId]->Wait(g_customerCashierLock[ThreadId]);
    	}

    	g_customerCashierLock[ThreadId]->Release();

    }
}

void ManagerThread(int ThreadId)
{
    DEBUG('p', "%s Started !!!!!!! \n",currentThread->getName());
    int managerSales = 0;

    int itemRemoveCounter = 0;
	int currentItemNoToRemove = 0;
	int currentItemNoToRemoveCount = 0;
	int currentItemNoToRemovePrice = 0;

    while(1)
    {

    	itemRemoveCounter = 0;

    	for(int i=0;i<NO_OF_CASHIERS;i++)
    	{
    		g_managerCashierCashLock[i]->Acquire();
    		g_managerInfo.totalRevenue += g_cashierInfo[i].totalSalesMoney;
    		g_cashierInfo[i].totalSalesMoney = 0;
    		g_managerCashierCashLock[i]->Release();
    	}

    	g_managerInfo.totalRevenue += managerSales;
    	managerSales = 0;
    	DEBUG('p',"Total Sale of the entire store until now is $ %d \n",g_managerInfo.totalRevenue);

    	g_managerCashierLock->Acquire();
    	if(g_managerWaitQueueLength == 0)
    	{
    		g_managerCashierLock->Release();
    	}
    	else
    	{
    		g_managerCashierCV->Signal(g_managerCashierLock);

    		g_managerWaitQueueLength--;

    		g_managerCashierInteractionLock->Acquire();

    		g_managerCashierLock->Release();

    		g_managerCashierInteractionCV->Wait(g_managerCashierInteractionLock);

    		DEBUG('p',"%s got a call from CASHIER_%d\n",currentThread->getName(),g_managerInfo.cashierId);

    		g_managerCashierInteractionCV->Signal(g_managerCashierInteractionLock);

    		g_managerCustomerInteractionLock->Acquire();

    		g_managerCashierInteractionLock->Release();

    		g_managerCustomerInteractionCV->Wait(g_managerCustomerInteractionLock);

    		g_managerCustomerInteractionCV->Signal(g_managerCustomerInteractionLock);

    		g_managerCustomerInteractionCV->Wait(g_managerCustomerInteractionLock);

    		while(g_customerInfo[g_managerInfo.customerId].isDoneShopping != true)
    		{
    			currentItemNoToRemove =
    					g_customerInfo[g_managerInfo.customerId].pCustomerShoppingList[itemRemoveCounter].itemNo;

    			currentItemNoToRemovePrice = g_itemInfo[currentItemNoToRemove].Price;

    			currentItemNoToRemoveCount =
    					g_customerInfo[g_managerInfo.customerId].pCustomerShoppingList[itemRemoveCounter].noOfItems;

    			DEBUG('p',"%s removes ITEM_%d from trolley of CUSTOMER_%d\n",
    					currentThread->getName(),currentItemNoToRemove);

    			g_managerInfo.customerBill -= currentItemNoToRemovePrice * currentItemNoToRemoveCount;

        		g_managerCustomerInteractionCV->Signal(g_managerCustomerInteractionLock);

        		g_managerCustomerInteractionCV->Wait(g_managerCustomerInteractionLock);
    		}

    		managerSales += g_managerInfo.customerBill;

    		g_managerCustomerInteractionCV->Signal(g_managerCustomerInteractionLock);

    		DEBUG('p',"%s gives receipt to CUSTOMER_%d \n",currentThread->getName(),g_managerInfo.customerId);

    		g_managerCustomerInteractionLock->Release();

    		DEBUG('p',"%s has total sale of $ %d\n",currentThread->getName(),managerSales);
    	}

    	for(int i=0;i<MANAGER_RANDOM_SLEEP_TIME;i++)
    	{
    		currentThread->Yield();
    	}
    }
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

	for(int i =0;i<NO_OF_SHELFS;i++)
	{
		lockName = new char[50];
        sprintf (lockName, "%s_%d", SHELFACCESSLOCK_STRING, i);
        g_shelfAccessLock[i] = new Lock(lockName);
	}

	for(int i=0;i<NO_OF_DEPARTMENT;i++)
	{
		lockName = new char[50];
        sprintf (lockName, "%s_%d", CUSTOMERDEPARTMENTCOMPLAINLOCK_STRING, i);
        g_customerDepartmentComplainLock[i] = new Lock(lockName);

        cvName = new char[50];
        sprintf (cvName, "%s_%d", CUSTOMERDEPARTMENTCOMPLAINCV_STRING, i);
        g_customerDepartmentComplainCV[i] = new Condition(cvName);

        g_departmentComplainWaitQueue[i] = 0;
	}

	for(int i=0;i<NO_OF_GOOD_LOADERS;i++)
	{
		lockName = new char[50];
        sprintf (lockName, "%s_%d", SALESMANGOODLOADERLOCK_STRING, i);
        g_salesmanGoodsLoaderLock[i] = new Lock(lockName);

        cvName = new char[50];
        sprintf (cvName, "%s_%d", SALESMANGOODLOADERCV_STRING, i);
        g_salesmanGoodsLoaderCV[i] = new Condition(cvName);
	}

	for(int i=0;i<NO_OF_GOODLOADER_WAIT_QUEUE;i++)
	{
		lockName = new char[50];
        sprintf (lockName, "%s_%d", GOODLOADERWAITQUEUELOCK_STRING, i);
        g_goodLoaderWaitLock[i] = new Lock(lockName);

        cvName = new char[50];
        sprintf (cvName, "%s_%d", GOODLOADERWAITQUEUECV_STRING, i);
        g_goodLoaderWaitCV[i] = new Condition(cvName);

        g_goodLoaderWaitQueue[i] = 0;
	}

	for(int i=0;i<NO_OF_CASHIERS;i++)
	{
		lockName = new char[50];
        sprintf (lockName, "%s_%d", CASHIERLINELOCK_STRING, i);
        g_cashierLineLock[i] = new Lock(lockName);

        cvName = new char[50];
        sprintf (cvName, "%s_%d", CASHIERLINECV_STRING, i);
        g_cashierLineCV[i] = new Condition(cvName);

        g_cashierWaitQueue[i] = 0;

		lockName = new char[50];
        sprintf (lockName, "%s_%d", CASHIERPRIVILEGEDLINELOCK_STRING, i);
        g_cashierPrivilegedLineLock[i] = new Lock(lockName);

        cvName = new char[50];
        sprintf (cvName, "%s_%d", CASHIERPRIVILEGEDLINECV_STRING, i);
        g_cashierPrivilegedLineCV[i] = new Condition(cvName);

        g_cashierPrivilegedWaitQueue[i] = 0;

		lockName = new char[50];
        sprintf (lockName, "%s_%d", CUSTOMERCASHIERLOCK_STRING, i);
        g_customerCashierLock[i] = new Lock(lockName);

        cvName = new char[50];
        sprintf (cvName, "%s_%d", CUSTOMERCASHIERCV_STRING, i);
        g_customerCashierCV[i] = new Condition(cvName);
	}

	for(int i=0;i<NO_OF_CASHIERS;i++)
	{
		lockName = new char[50];
        sprintf (lockName, "%s_%d", MANAGERCASHIERCASHLOCK_STRING, i);
        g_managerCashierCashLock[i] = new Lock(lockName);
	}

	for(int i=0;i<NO_OF_MANAGERS;i++)
	{
		lockName = new char[50];
        sprintf (lockName, "%s_%d", MANAGERCASHIERLOCK_STRING, i);
        g_managerCashierLock = new Lock(lockName);

        cvName = new char[50];
        sprintf (cvName, "%s_%d", MANAGERCASHIERCV_STRING, i);
        g_managerCashierCV = new Condition(cvName);

		lockName = new char[50];
        sprintf (lockName, "%s_%d", MANAGERCASHIERINTERACTIONLOCK_STRING, i);
        g_managerCashierInteractionLock = new Lock(lockName);

        cvName = new char[50];
        sprintf (cvName, "%s_%d", MANAGERCASHIERINTERACTIONCV_STRING, i);
        g_managerCashierInteractionCV = new Condition(cvName);
	}

	for(int i=0;i<NO_OF_MANAGERS;i++)
	{
		lockName = new char[50];
        sprintf (lockName, "%s_%d", MANAGERCUSTOMERINTERACTIONLOCK_STRING, i);
        g_managerCustomerInteractionLock = new Lock(lockName);

        cvName = new char[50];
        sprintf (cvName, "%s_%d", MANAGERCUSTOMERINTERACTIONCV_STRING, i);
        g_managerCustomerInteractionCV = new Condition(cvName);
	}
}

static int handler(void* user, const char* section, const char* name,
                   const char* value)
{
	/**
	 * Handling Item Configuration
	 */

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
	int currentItemNo;

    if (MATCH("ITEM", "itemNo"))
    {
    	currentItemNo = atoi(value);
    }
    else if (MATCH("ITEM", "price"))
    {
        g_itemInfo[currentItemNo].Price = atoi(value);
    }
    else if (MATCH("ITEM", "shelfNo"))
    {
        g_itemInfo[currentItemNo].shelfNo = atoi(value);
    }
    else if(MATCH("ITEM","departmentNo"))
    {
    	g_itemInfo[currentItemNo].departmentNo = atoi(value);
    }
    else if(MATCH("ITEM","noOfItems"))
    {
    	g_itemInfo[currentItemNo].noOfItems = atoi(value);
    }
    else
    {
        return 0;  /* unknown section/name, error */
    }
    return 1;
}

void printConfiguration()
{
	printItemInfo();
}

int startConfiguration(const char* configFileName)
{
	int retVal = 1;
    if (ini_parse(configFileName, handler, NULL) < 0)
    {
    	DEBUG('p',"Can't load 'test.ini'\n");
        printf("Can't load 'test.ini'\n");
    }
    return retVal;
}

void startSimulation(const char* configFileName)
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

    /**
     * Start the configuration Process
     */
    startConfiguration(configFileName);
    printConfiguration();

#if 0
    initItemInfo();
    printItemInfo();

    initCustomerInfo();
    initCustomerShoppingList();

    printCustomerInfo();

    initSalesManInfo();

    initGoodLoaderInfo();

    initCashierInfo();

    initManagerInfo();

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
#endif

}

void Problem2(const char* configFileName)
{
	startSimulation(configFileName);
}
