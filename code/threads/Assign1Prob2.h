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
#include "config.h"
#include "ini.h"
#include "defines.h"

static void initItemInfo();

static void initCustomerShoppingList();

static void initCustomerInfo();

static void initSalesManInfo();
void printSalesManInfo(int salesManId);

static void initGoodLoaderInfo();
void printGoodLoaderInfo(int goodLoaderId);

static void initCashierInfo();
void printCashierInfo(int cashierId);

static void initManagerInfo();
void printManagerInfo(int managerId);

void initLockCvForSimulation();

void startSimulation(const char* testOption);

#endif /* ASSIGN1PROB2_H_ */
