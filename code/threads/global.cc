/*
 * global.cpp
 *
 *  Created on: Mar 20, 2013
 *      Author: cvora
 */

#include "defines.h"
#include "Assign1Prob2.h"

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

/*************************************GLOBAL DATA STRUCTS DECLARATION**********************************************/


