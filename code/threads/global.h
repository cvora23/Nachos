/*
 * global.h
 *
 *  Created on: Mar 20, 2013
 *      Author: cvora
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

/**
 * extern tells the compiler this variable is declared elsewhere
 */

/**
 * GLOBAL DATA STRUCTURES
 */
extern ItemInfo 		g_itemInfo[NO_OF_ITEM_TYPES];

extern CustomerInfo 	g_customerInfo[NO_OF_CUSTOMERS];

extern SalesManInfo 	g_salesmanInfo[NO_OF_SALESMAN];

extern GoodLoaderInfo 	g_goodLoaderInfo[NO_OF_GOOD_LOADERS];

extern CashierInfo		g_cashierInfo[NO_OF_CASHIERS];

extern ManagerInfo		g_managerInfo;

/**
 * Locks, Condition Variables, Queue Wait Count for Customer-Trolley interaction
 */
extern int 			g_usedTrolleyCount;
extern int 			g_waitForTrolleyCount;
extern Lock* 		g_customerTrolleyLock;
extern Condition* 	g_customerTrolleyCV;

/**
 * Locks, Condition Variables for Customer-Salesman interaction
 */
extern Lock*		g_customerSalesmanLock[NO_OF_SALESMAN];
extern Condition*  	g_customerSalesmanCV[NO_OF_SALESMAN];

/**
 * Locks,Condition Variables and Queue Length for Customer-Department Interaction
 */
extern Lock*		g_customerDepartmentLock[NO_OF_DEPARTMENT];
extern Condition*	g_customerDepartmentCV[NO_OF_DEPARTMENT];
extern int			g_departmentWaitQueue[NO_OF_DEPARTMENT];

/**
 * Locks for accessing items on the shelf
 */
extern Lock*		g_shelfAccessLock[NO_OF_SHELFS];

/**
 * Locks,Condition Variables and Queue Length for Customer-Department Interaction
 */
extern Lock*		g_customerDepartmentComplainLock[NO_OF_DEPARTMENT];
extern Condition*	g_customerDepartmentComplainCV[NO_OF_DEPARTMENT];
extern int			g_departmentComplainWaitQueue[NO_OF_DEPARTMENT];

/**
 * Locks,Condition Variables and Queue Length for Salesman - GoodsLoader Interaction
 */
extern Lock*		g_salesmanGoodsLoaderLock[NO_OF_GOOD_LOADERS];
extern Condition*  	g_salesmanGoodsLoaderCV[NO_OF_GOOD_LOADERS];

extern Lock*		g_goodLoaderWaitLock[NO_OF_GOODLOADER_WAIT_QUEUE];
extern Condition*	g_goodLoaderWaitCV[NO_OF_GOODLOADER_WAIT_QUEUE];
extern int			g_goodLoaderWaitQueue[NO_OF_GOODLOADER_WAIT_QUEUE];

/**
 * Locks,Condition Variables and Queue Length for Customer - Cashier Interaction
 */
extern Lock*		g_cashierLineLock[NO_OF_CASHIERS];
extern Condition*	g_cashierLineCV[NO_OF_CASHIERS];
extern int			g_cashierWaitQueue[NO_OF_CASHIERS];

extern Lock*		g_cashierPrivilegedLineLock[NO_OF_CASHIERS];
extern Condition*  	g_cashierPrivilegedLineCV[NO_OF_CASHIERS];
extern int			g_cashierPrivilegedWaitQueue[NO_OF_CASHIERS];

extern Lock*		g_customerCashierLock[NO_OF_CASHIERS];
extern Condition*	g_customerCashierCV[NO_OF_CASHIERS];

/**
 * Locks, Condition Variables and Queue Length for Manager - Cashier  Cash Collection Interaction
 */
extern Lock*		g_managerCashierCashLock[NO_OF_CASHIERS];

/**
 * Locks, Condition Variables and Queue Length for Manager Interaction
 */

extern Lock*		g_managerCashierLock;
extern Condition*	g_managerCashierCV;
extern int			g_managerWaitQueueLength;

extern Lock*		g_managerCashierInteractionLock;
extern Condition*	g_managerCashierInteractionCV;

/**
 * Locks, Condition Variables for Manager - Customer Interaction
 */
extern Lock*		g_managerCustomerInteractionLock;
extern Condition*	g_managerCustomerInteractionCV;

/*************************************GLOBAL DATA STRUCTS DECLARATION**********************************************/


#endif /* GLOBAL_H_ */
