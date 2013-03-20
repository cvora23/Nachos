/*
 * defines.h
 *
 *  Created on: Mar 20, 2013
 *      Author: cvora
 */

#ifndef DEFINES_H_
#define DEFINES_H_


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
#define GOOD_LOADERS_STRING								(const char*)"GOOD LOADER"
#define CUSTOMER_STRING									(const char*)"CUSTOMER"

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
#define MANAGERCASHIERINTERACTIONLOCK_STRING			(const char*)"ManagerCashierLock"
#define MANAGERCASHIERINTERACTIONCV_STRING				(const char*)"ManagerCashierCV"
#define MANAGERCUSTOMERINTERACTIONLOCK_STRING			(const char*)"ManagerCustomerLock"
#define MANAGERCUSTOMERINTERACTIONCV_STRING				(const char*)"ManagerCustomerCV"


#endif /* DEFINES_H_ */
