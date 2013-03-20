/*
 * Assign1Prob2.h
 *
 *  Created on: Mar 3, 2013
 *      Author: cvora
 */

#ifndef ASSIGN1PROB2_H_
#define ASSIGN1PROB2_H_

#include <stdio.h>
#include "system.h"
#include "synch.h"
#include "config.h"
#include "ini.h"
#include "defines.h"

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
	CustomerShoppingList	*pCustomerShoppingList;
}CustomerInfo;

static void initCustomerInfo();
void printCustomerInfo();

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
void printGoodLoaderInfo(int goodLoaderId);

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

static void initCashierInfo();
void printCashierInfo(int cashierId);

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

static void initManagerInfo();
void printManagerInfo(int managerId);

void initLockCvForSimulation();

void startSimulation(const char* configFileName);

#endif /* ASSIGN1PROB2_H_ */
