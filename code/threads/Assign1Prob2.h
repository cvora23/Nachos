/*
 * Assign1Prob2.h
 *
 *  Created on: Mar 3, 2013
 *      Author: cvora
 */

#ifndef ASSIGN1PROB2_H_
#define ASSIGN1PROB2_H_

#include "system.h"

/**
 * STORE STOCK INFO
 */
#define NO_OF_DEPARTMENT								5
#define NO_OF_SHELFS									(NO_OF_DEPARTMENT*3)
#define NO_OF_ITEM_TYPES								NO_OF_SHELFS
#define MAX_NO_ITEMS_TO_BE_PURCHASED					10
#define MAX_NO_ITEMS_TO_BE_PURCHASED_OF_EACH_TYPE 		5
#define MAX_AMT_PER_CUSTOMER							1000
#define MAX_PRICE_PER_ITEM								200
#define MAX_NO_ITEMS_PER_SHELF							100
#define MAX_NO_OF_ITEMS									5000
#define MAX_NO_OF_TROLLEYS								100

/**
 * STORE MANAGEMENT INFO
 */
#define NO_OF_MANAGERS									1
#define NO_OF_CASHIERS									5
#define NO_OF_SALESMAN									(NO_OF_DEPARTMENT*3)
#define NO_OF_GOOD_LOADERS								5
#define NO_OF_CUSTOMERS									30

#define MANAGER_STRING									(const char*)"MANAGER"
#define CASHIER_STRING									(const char*)"CASHIER"
#define DEPARTMENT_STRING								(const char*)"DEPARTMENT"
#define SALESMAN_STRING									(const char*)"SALESMAN"
#define GOOD_LOADERS_STRING								(const char*)"GOOD LOADER"
#define CUSTOMER_STRING									(const char*)"CUSTOMER"

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
	int 	 itemNo;
	int		 noOfItems;
}CustomerShoppingList;

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
static void initCustomerShoppingList();
void printCustomerInfo(int customerId);
void printCustomerShoppingInfo(int customerId);

void initLockCvForSimulation();
void startSimulation();

#endif /* ASSIGN1PROB2_H_ */
