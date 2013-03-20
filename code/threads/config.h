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
void printConfiguration();

#endif /* CONFIG_H_ */