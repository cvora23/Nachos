/*
 * config.cc
 *
 *  Created on: Mar 20, 2013
 *      Author: cvora
 */

#include "config.h"


void createConfigFileForItem()
{
	FILE* itemConfigFile;
	itemConfigFile = fopen("itemConfigFile.ini","w");
	if(itemConfigFile != NULL)
	{
		fprintf(itemConfigFile,"; ********************************ITEM CONFIGURATION START******************"
				"*******\n");
		for(int i=0;i<NO_OF_ITEM_TYPES;i++)
		{
			fprintf(itemConfigFile,"[ITEM]\n");
			fprintf(itemConfigFile,"itemNo = %d\n",i);
			fprintf(itemConfigFile,"price = %d\n",Random()%MAX_PRICE_PER_ITEM + 1);
			fprintf(itemConfigFile,"shelfNo = %d\n",i);
			fprintf(itemConfigFile,"departmentNo = %d\n",Random()%MAX_PRICE_PER_ITEM + 1);
			fprintf(itemConfigFile,"noOfItems = %d\n",Random()%MAX_PRICE_PER_ITEM + 1);
			fprintf(itemConfigFile,"\n");
		}
		fprintf(itemConfigFile,"; ****************************ITEM CONFIGURATION END******************"
				"*******\n");
	}
	fclose(itemConfigFile);
}

void createConfigFileForCustomer()
{
	int noOfItemsToPurchase = Random()%MAX_NO_ITEMS_TO_BE_PURCHASED + 1;
	FILE* customerConfigFile;
	customerConfigFile = fopen("customerConfigFile.ini","w");
	if(customerConfigFile != NULL)
	{
		fprintf(customerConfigFile,"; ********************************CUSTOMER CONFIGURATION START******************"
				"*******\n");
		fprintf(customerConfigFile,"\n");
		for(int i=0;i<NO_OF_CUSTOMERS;i++)
		{
			fprintf(customerConfigFile,"[CUSTOMER]\n");
			fprintf(customerConfigFile,"id = %d\n",i);
			fprintf(customerConfigFile,"type = %d\n",CustomerType(Random()%2));
			fprintf(customerConfigFile,"money = %d\n",Random()%MAX_AMT_PER_CUSTOMER + 1);
			fprintf(customerConfigFile,"noOfItemsToShop = %d\n",noOfItemsToPurchase);
			fprintf(customerConfigFile,"hasEnoughMoneyForShopping = %d\n",0);
			fprintf(customerConfigFile,"isDoneShopping = %d\n",0);
			fprintf(customerConfigFile,"[SHOPPING LIST]\n");
			for(int j=0;j<noOfItemsToPurchase;j++)
			{
				fprintf(customerConfigFile,"itemToShop = %d\n",Random()%NO_OF_ITEM_TYPES);
				fprintf(customerConfigFile,"Quantity = %d\n",Random()%MAX_NO_ITEMS_TO_BE_PURCHASED_OF_EACH_TYPE + 1);
			}
			fprintf(customerConfigFile,"END = now\n");
			fprintf(customerConfigFile,"\n");
		}
		fprintf(customerConfigFile,"; ****************************CUSTOMER CONFIGURATION END******************"
				"*******\n");
	}
	fclose(customerConfigFile);
}

static int handler(void* user, const char* section, const char* name,
                   const char* value)
{
	/**
	 * Handling Item Configuration
	 */

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
	static int currentItemNo;
	static int customerId;
	static int currentCustomerShoppingListCount = 0;

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
    else if(MATCH("CUSTOMER","id"))
    {
    	customerId = atoi(value);
    }
    else if(MATCH("CUSTOMER","type"))
    {
    	g_customerInfo[customerId].type = (CustomerType)atoi(value);
    }
    else if(MATCH("CUSTOMER","money"))
    {
    	g_customerInfo[customerId].money = atoi(value);
    }
    else if(MATCH("CUSTOMER","hasEnoughMoneyForShopping"))
    {
    	g_customerInfo[customerId].hasEnoughMoneyForShopping = atoi(value);
    }
    else if(MATCH("CUSTOMER","isDoneShopping"))
    {
    	g_customerInfo[customerId].isDoneShopping = atoi(value);
    }
    else if(MATCH("CUSTOMER","noOfItemsToShop"))
    {
    	g_customerInfo[customerId].noOfItems = atoi(value);
		g_customerInfo[customerId].pCustomerShoppingList =
				new CustomerShoppingList[g_customerInfo[customerId].noOfItems];

    }
    else if(MATCH("SHOPPING LIST","itemToShop"))
    {
    	g_customerInfo[customerId].pCustomerShoppingList[currentCustomerShoppingListCount].itemNo = atoi(value);
    }
    else if(MATCH("SHOPPING LIST","Quantity"))
    {
    	g_customerInfo[customerId].pCustomerShoppingList[currentCustomerShoppingListCount].noOfItems = atoi(value);
    	currentCustomerShoppingListCount++;
    }
    else if(MATCH("SHOPPING LIST","END"))
    {
    	currentCustomerShoppingListCount = 0;
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
	printCustomerInfo();
}

int startConfiguration(const char* configFileName)
{
	int retVal = 1;
    if (ini_parse(configFileName, handler, NULL) < 0)
    {
    	DEBUG('p',"Can't load 'test.ini'\n");
        retVal = 0;
    }
    return retVal;
}



