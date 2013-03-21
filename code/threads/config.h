/*
 * config.h
 *
 *  Created on: Mar 20, 2013
 *      Author: cvora
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "defines.h"
#include "global.h"
#include "ini.h"

void createConfigFileForItem();
void createConfigFileForCustomer();

int startConfiguration(const char* configFileName);

int startItemConfiguration(const char* configFileName);
int startCustomerConfiguration(const char* configFileName);

void printItemInfo();
void printCustomerInfo();
void printCustomerShoppingList(int customerId);
void printConfiguration();

#endif /* CONFIG_H_ */
