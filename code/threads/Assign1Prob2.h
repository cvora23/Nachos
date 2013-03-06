/*
 * Assign1Prob2.h
 *
 *  Created on: Mar 3, 2013
 *      Author: cvora
 */

#ifndef ASSIGN1PROB2_H_
#define ASSIGN1PROB2_H_

#include "system.h"
#include "synch.h"

/**
 * \todo: Add Config File reader for changing the default configuration parameters.
 * \todo: Also put proper comments as it will help in future.
 */

/**
 * STORE STOCK INFO
 */
#define NO_OF_DEPARTMENT								5
#define NO_OF_SHELFS									(NO_OF_DEPARTMENT*3)
#define NO_OF_ITEM_TYPES								NO_OF_SHELFS
#define NO_OF_ITEMS_PER_DEPARTMENT						(NO_OF_ITEM_TYPES/NO_OF_DEPARTMENT)
#define MAX_NO_ITEMS_TO_BE_PURCHASED					10
#define MAX_NO_ITEMS_TO_BE_PURCHASED_OF_EACH_TYPE 		5
#define MAX_AMT_PER_CUSTOMER							1000
#define MAX_PRICE_PER_ITEM								200
#define MAX_NO_ITEMS_PER_SHELF							100
#define MAX_NO_OF_TROLLEYS								100
#define NO_OF_GOODLOADER_WAIT_QUEUE						1
#define MIN_GOODS_LOADER_WALKINGTIME					20
#define MAX_GOODS_LOADER_WALKINGTIME					40

/**
 * STORE MANAGEMENT INFO
 */
#define NO_OF_MANAGERS									1
#define NO_OF_CASHIERS									5
#define NO_OF_SALESMAN									(NO_OF_DEPARTMENT*3)
#define NO_OF_SALESMAN_PER_DEPARTMENT					(NO_OF_SALESMAN/NO_OF_DEPARTMENT)
#define NO_OF_GOOD_LOADERS								5
#define NO_OF_CUSTOMERS									30

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

static void initItemInfo();
void printItemInfo();

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

static void initCustomerShoppingList();
void printCustomerShoppingList(int customerId);

typedef enum _CustomerType{PRIVILEGED = 0,NON_PRIVILEGED}CustomerType;

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
	 * Shopping List for Customer
	 */
	CustomerShoppingList	*pCustomerShoppingList;
}CustomerInfo;

static void initCustomerInfo();
void printCustomerInfo();

typedef enum _SalesManStatus
{
	salesmanIsFree = 0,
	salesmanIsBusy,
	salesmanSignalToCustomer
}SalesManStatus;

typedef struct _SalesManInfo
{
	/**
	 * If the Salesman is free or busy handling customer or just signaled a Customer
	 */
	SalesManStatus status;
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

static void initSalesManInfo();
void printSalesManInfo(int salesManId);

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

static void initGoodLoaderInfo();
void printGoodLoaderInfo(int goodLoaderInfo);

void initLockCvForSimulation();
void startSimulation();

#endif /* ASSIGN1PROB2_H_ */
