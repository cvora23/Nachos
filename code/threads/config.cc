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
			fprintf(itemConfigFile,"departmentNo = %d\n",(i/NO_OF_ITEMS_PER_DEPARTMENT));
			fprintf(itemConfigFile,"noOfItems = %d\n",Random()%MAX_NO_ITEMS_PER_SHELF + 1);
			fprintf(itemConfigFile,"\n");
		}
		fprintf(itemConfigFile,"; ****************************ITEM CONFIGURATION END******************"
				"*******\n");
	}
	fclose(itemConfigFile);
}

void createConfigFileForCustomer()
{
	int noOfItemsToPurchase;
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

			noOfItemsToPurchase = Random()%MAX_NO_ITEMS_TO_BE_PURCHASED + 1;

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

void printItemInfo()
{
	for (int i = 0;i<NO_OF_ITEM_TYPES;i++)
	{
		DEBUG('p',"Item id is = %d \n",i);
		DEBUG('p',"Item %d Price is = %d \n",i,g_itemInfo[i].Price);
		DEBUG('p',"Item %d is in Department = %d \n",i,g_itemInfo[i].departmentNo);
		DEBUG('p',"Item %d is in Shelf = %d \n",i,g_itemInfo[i].shelfNo);
		DEBUG('p',"Item %d Total Stock no = %d \n",i,g_itemInfo[i].noOfItems);
	}
}

void printCustomerInfo()
{
	for(int customerId=0;customerId<NO_OF_CUSTOMERS;customerId++)
	{
		DEBUG('p',"Customer ID is %d\n",
				customerId);
		DEBUG('p',"Customer %d is of type %d \n",
				customerId,g_customerInfo[customerId].type);
		DEBUG('p',"Customer %d can spend %d amount on shopping \n",
				customerId,g_customerInfo[customerId].money);
		DEBUG('p',"Customer %d will purchase %d items today for shopping \n",
				customerId,g_customerInfo[customerId].noOfItems);
		printCustomerShoppingList(customerId);
	}
}

void printCustomerShoppingList(int customerId)
{
	DEBUG('p',"Customer %d shopping list is as follows : \n",customerId);
	for(int j =0;j<g_customerInfo[customerId].noOfItems;j++)
	{
		DEBUG('p',"Item Type: %d\n",
				g_customerInfo[customerId].pCustomerShoppingList[j].itemNo);
		DEBUG('p',"No of Items of Item Type:%d ===== %d\n",
				g_customerInfo[customerId].pCustomerShoppingList[j].itemNo,
				g_customerInfo[customerId].pCustomerShoppingList[j].noOfItems);
	}
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
