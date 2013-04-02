/*-----------------------------------------
 Declaration of the Header files required.
------------------------------------------*/

#include "syscall.h"


/*--------------------------------------------------------------
 Declaration of global variables and Data Structures
--------------------------------------------------------------*/

#define MAX_NO_OF_MANAGER 1
#define MAX_NO_OF_CASHIER 5
#define MAX_NO_OF_DEPARTMENTS 5
#define MAX_NO_OF_SALESMAN 3
#define MAX_NO_OF_CUSTOMERS 1000
#define MAX_NO_OF_AISLE 25
#define MAX_NO_OF_GOODSLOADER 5
#define MAX_TROLLY_COUNT 750
#define MAX_NO_OF_ITEMS 5
#define MAX_QUANTITY 9999
#define MAX_NO_OF_ITEMS_PER_CUSTOMER 10
#define TOTAL_MAX_MONEY = 1000
#define FALSE 0
#define TRUE 1

typedef int bool;


enum cashierStatusEnum{CASHIER_BUSY, CASHIER_FREE, CASHIER_NOT_STARTED, CASHIER_SIGNAL_TO_CUSTOMER};
enum cashierStatusEnum cashierStatus[MAX_NO_OF_CASHIER] = {CASHIER_NOT_STARTED, CASHIER_NOT_STARTED, CASHIER_NOT_STARTED, CASHIER_NOT_STARTED, CASHIER_NOT_STARTED};

enum goodsLoaderStatusEnum{GLOADER_BUSY, GLOADER_FREE, GLOADER_ONBREAK, GLOADER_SIGNAL_TO_SALESMAN};
enum goodsLoaderStatusEnum goodsLoaderStatus[MAX_NO_OF_GOODSLOADER] = {GLOADER_ONBREAK, GLOADER_ONBREAK, GLOADER_ONBREAK, GLOADER_ONBREAK, GLOADER_ONBREAK};


enum salesmanStatusEnum{SALESMAN_BUSY, SALESMAN_FREE, SALESMAN_ONBREAK, SALESMAN_SIGNAL_TO_CUSTOMER};
enum salesmanStatusEnum salesmanStatus[MAX_NO_OF_SALESMAN*MAX_NO_OF_DEPARTMENTS] = {SALESMAN_ONBREAK,SALESMAN_ONBREAK,SALESMAN_ONBREAK,
																				SALESMAN_ONBREAK,SALESMAN_ONBREAK,SALESMAN_ONBREAK,
																				SALESMAN_ONBREAK,SALESMAN_ONBREAK,SALESMAN_ONBREAK,
																				SALESMAN_ONBREAK,SALESMAN_ONBREAK,SALESMAN_ONBREAK,
																				SALESMAN_ONBREAK,SALESMAN_ONBREAK,SALESMAN_ONBREAK};



int noOfCustomers;
int noOfPrivilageCustomers = 0;
int itemQuantity;
int maxMoney;
int noOfCashiers;
int noOfSalesman;
int noOfDepartments;
int noOfGoodsLoaders;
int noOfCustomersExited = 0;
int managersCashier;
int managersCustomer;
int salesmansGL;

int aisleQuantityArray[MAX_NO_OF_DEPARTMENTS*MAX_NO_OF_ITEMS];

typedef struct productInfo
{
	int departmentNo;
	int aisleId;
	int price;
}productDataTemplate;



/*int productID[25]


List* restockingList;

enum salesmanStatusEnum{BUSY, FREE, ONBREAK};
salesmanStatusEnum salesmanStatus[MAX_NO_OF_SALESMAN] = {ONBREAK, ONBREAK, ONBREAK};
*/

/* CUSTOMER DATA STRUCTURE */
typedef struct customer
{
	bool isPrivilageCustomer;
	bool isComplaining;
	
	int noOfItemsToPurchase;
	
	
	
	int queryItem;
	int moneyWithMe;
	int itemsToPurchase[10];
	
}customerDataTemplate;
customerDataTemplate customerData[MAX_NO_OF_CUSTOMERS];
productDataTemplate productData[MAX_NO_OF_DEPARTMENTS*5];




int custThreadCntr = 0;
int cashierThreadCounter = 0;
int salesmanThreadCounter = 0;
int glThreadCntr = 0;


/*----------------------------------------------------------------------------------------
 Declaration of  Locks, CV and Variables for Customer-Trolly Interaction
----------------------------------------------------------------------------------------*/

unsigned int printLock;

unsigned int custThreadCntrLock;
unsigned int cashierThreadCounterLock;
unsigned int salesmanThreadCounterLock;
unsigned int glThreadCntrLock;

/*Lock for mutual exclusion on getting a .*/
unsigned int trollyLock;
int totalTrollyCount;
int trollyWaitCount = 0;

/*Condition for customer waiting for a trolly. */
unsigned int customerTrollyWaitingCV;


/*----------------------------------------------------------------------------------------
 Declaration of  Locks, CV and Variables for Customer-Salesman Interaction
----------------------------------------------------------------------------------------*/

/* int a3[100]; */

/* Lock for mutual exclusion for exiting customer. */
unsigned int customerNumberLock;

/* int a4[100]; */

/* Lock for setting which customer is served by which salesman */
unsigned int salesmanServing;

/* Lock for mutual exclusion on entering department line. */
unsigned int departmentLineLock[MAX_NO_OF_DEPARTMENTS];

/* Lock for mutual exclusion on entering complaining line. */
unsigned int complainLineLock[MAX_NO_OF_DEPARTMENTS];

/* Locks for each salesman interaction. */
unsigned int salesmanLock[MAX_NO_OF_SALESMAN*MAX_NO_OF_DEPARTMENTS];


/* Condition for interaction btw one customer and a salesman at the complain Queue. */
unsigned int custSalesTalkCV[MAX_NO_OF_SALESMAN*MAX_NO_OF_DEPARTMENTS];

/* Condition for signaling a customer in departmentLine. */
unsigned int deptLineCV[MAX_NO_OF_DEPARTMENTS];

/* Condition for signaling a customer in complainLine. */
unsigned int complainLineCV[MAX_NO_OF_DEPARTMENTS];



/* maintains the status of each salesman. */

/* integer indicating the number of customers waiting for the salesman for welcome */
int departmentLineLength[MAX_NO_OF_DEPARTMENTS];

/* integer array indicating which item is to be restocked, updated by salesman and customer */
int itemToRestockArray[MAX_NO_OF_SALESMAN*MAX_NO_OF_DEPARTMENTS];

/* integer indicating the number of customers waiting for the salesman for complaining */
int complainLineLength[MAX_NO_OF_DEPARTMENTS];

/* integer array indicating which salesman serves which customer */
int salesmanServingCustomer[MAX_NO_OF_SALESMAN*MAX_NO_OF_DEPARTMENTS];
/* int iiii[1000]; */

/*------------------------------------------------------------------------------------------
 Declaration of  Locks, CV and Variables for Salesman-GoodsLoader Interaction
------------------------------------------------------------------------------------------ */
/* int a1[100]; */

/* Lock for mutual exclusion on getting a goodsLoaderLineLock. */
unsigned int goodsLoaderLineLock; 

/* Lock for mutual exclusion for setting salesman value in goods Loader */
unsigned int goodsSalesServing; 
/* int yyy[100]; */
/* Lock for talk between GoodsLoader and Salesman. */
unsigned int goodsLoaderSalesmanLock[MAX_NO_OF_GOODSLOADER]; 
/* int zzz[100]; */

/* int a12[100]; */

unsigned int goodsLoaderLineCV;

/* Condition for interaction btw a salesman and a Goods Loader at a time. */
unsigned int goodsLoaderSalesmanCV[MAX_NO_OF_GOODSLOADER];


/* integer array indicating which item is to be restocked, updated by salesman and goods loader */
int goodsSalesRestocking[MAX_NO_OF_GOODSLOADER];

/* integer indicating the number of salesman waiting for the goodsLoader */
int goodsLoaderLineLength;

/* enum for indicating the state of the goodsLoader */

/*------------------------------------------------------------------------------------------
 Declaration of  Locks, CV and Variables for GoodsLoader Restocking Action
------------------------------------------------------------------------------------------ */

/* Lock for mutual exclusion on entering the room. */
unsigned int stockRoomLineLock;

/* Lock for ensuring that only one thread access a perticular aisle */
unsigned int aisleAccessLock[MAX_NO_OF_AISLE];



/* ------------------------------------------------------------------------------------------
 Declaration of  Locks, CV and Variables for Cashier - Customer Interaction
------------------------------------------------------------------------------------------ */


/* Lock for setting which customer is served by which cashier */
unsigned int cashierServing;


/* integer array indicating which salesman serves which customer */
int cashierServingCustomer[MAX_NO_OF_CASHIER];

/*------------------------------------------------------------------------------------------
 Declaration of  Locks, CV and Variables for Salesman-Cashier
------------------------------------------------------------------------------------------ */

bool hasCustomerMoney[MAX_NO_OF_CUSTOMERS];
bool isCashier;
int cashierLineLength[MAX_NO_OF_CASHIER];
int privilageCashierLineLength[MAX_NO_OF_CASHIER];
int custCashItemArray[MAX_NO_OF_CASHIER];

/* Lock for mutual exclusion for checking shortets line. */
unsigned int shortestLineLock;

/* Lock for mutual exclusion for getting into customer-cashier interaction. */
unsigned int cashierLineLock[MAX_NO_OF_CASHIER];

/* Lock for mutual exclusion for getting into PrivilageCustomer-cashier interaction. */
unsigned int privilageCashierLineLock[MAX_NO_OF_CASHIER];

/* Lock for mutual exclusion for customer-cashier interaction. */
unsigned int custCashierTalkLock[MAX_NO_OF_CASHIER];

/* Condition variable for the customer to talk to cashier while in line. */
unsigned int cashierLineCV[MAX_NO_OF_CASHIER];

/* Condition variable for the Privilagecustomer to talk to cashier while in line. */
unsigned int privilageCashierLineCV[MAX_NO_OF_CASHIER];

/* Condition variable for customer - cashier interaction. */
unsigned int custCashierTalkCV[MAX_NO_OF_CASHIER];

/*------------------------------------------------------------------------------------------
 Declaration of  Locks, CV and Variables for GoodsLoader - Manager Interaction
------------------------------------------------------------------------------------------ */

/* Lock for mutual exclusion for checking items held by manager. */
/* int b1[100]; */
unsigned int cancelledItemLock;
/* int b2[100]; */
int cancelledItemArray[MAX_NO_OF_AISLE];



/*------------------------------------------------------------------------------------------
 Declaration of  Locks, CV and Variables for Manager-Cashier-Customer Interaction
------------------------------------------------------------------------------------------ */

int totalRevenue = 0;
int customerManagerItemExchange;
int managerLineLength = 0;
bool isCashierOnBreak[MAX_NO_OF_CASHIER];
bool isSalesmanOnBreak[MAX_NO_OF_SALESMAN];
int cashierSalesArray[MAX_NO_OF_CASHIER];
int totalDrawerCollection[MAX_NO_OF_CASHIER];

/* Lock for mutual exclusion for updating the total Revenue. */
unsigned int managerCashierLock[MAX_NO_OF_CASHIER];
unsigned int managerCustomerInteractionLock;
/* Lock for mutual exclusion for sending a salesman on break. */
unsigned int managerSalesmanBreakLock[MAX_NO_OF_SALESMAN];
unsigned int managerCashierInteractionLock;
/* Lock for mutual exclusion for sending a cashier on break. */
unsigned int managerCashierBreakLock[MAX_NO_OF_CASHIER];

/* Lock for mutual exclusion for checking items held by manager. */
unsigned int managerLock;

/* int c[100];

int c1[100];

int c2[100]; */

/* Locks for giving Cashier/Customer Id to manager */
unsigned int managerCashierServing;
unsigned int managerCustomerServnig;

/* Condition variable for interaction between manager-Cashier */
unsigned int managerCashierCV;


/* Condition variable for 1st Signal between manager-Cashier */
unsigned int managerCustomerCV;

/* Condition variable for interaction between manager-Customer */
unsigned int managerCustomerInteractionCV;

/* Condition variable for interaction between manager-Cashier */
unsigned int managerCashierInteractionCV;

/*----------------------------------------
 All Thread Functions
---------------------------------------- */



/* ============ Customer Thread Code STARTS HERE ================== */

void Customer()
{
	int threadId;
	int temp;
	bool isSignaled = FALSE;
	int itemDepartmentId;
	int mySalesman;
	int salesmanIndex;
	int itemAisleId;
	int prevDept = -1;		
	int item=0;
	int i;
	int j;
	int currentItem;
	int myBill = 0;
	int myCashier = -1;
	int minLineLength = 9999;
	
	Acquire(custThreadCntrLock);
	threadId=custThreadCntr;
	custThreadCntr++;
	Release(custThreadCntrLock);

	/* ===================================Customer's Trolly Getting code below=================================== */
	Print("WTF");
	/*Print1("Total Items: %d\n", noOfDepartments*MAX_NO_OF_ITEMS);*/
	
	customerData[threadId].noOfItemsToPurchase=(int)((GetRand()%10)+1);						/* generating number of items to be purchased by customer  */
	temp=customerData[threadId].noOfItemsToPurchase;
	Print1("Total Items to purchase= %d\n",customerData[threadId].noOfItemsToPurchase);
	customerData[threadId].moneyWithMe=(int)((GetRand()% maxMoney)+1);							/* generating money with customer */
	/*Print1("money= %d\n",customerData[threadId].moneyWithMe);*/
	 Print1("Temp %d\n",temp);
	Print1("Total Items: %d\n", noOfDepartments*MAX_NO_OF_ITEMS);
	
	for(item=0; item < customerData[threadId].noOfItemsToPurchase; item++)
	{
		Print1("Total Items: %d\n", noOfDepartments*MAX_NO_OF_ITEMS);
		/*temp = (GetRand() % (noOfDepartments*MAX_NO_OF_ITEMS) );
		Print1("temp: %d",temp);*/
		Print1("1. Product id %d\n",customerData[threadId].itemsToPurchase[0]);
		customerData[threadId].itemsToPurchase[item] = 5;
		/*customerData[threadId].itemsToPurchase[item]=((GetRand() % (noOfDepartments*MAX_NO_OF_ITEMS) ));*/	/* specifies productID that are to be purchased */
		Print1("Product id %d\n",customerData[threadId].itemsToPurchase[item]);
	}
	
	Print("WTF@\n");																						/* Sorting The items to BUY */
	for(i=0; i<customerData[threadId].noOfItemsToPurchase-1; i++)
	{
		for(j=i; j<customerData[threadId].noOfItemsToPurchase - 1; j++)
		{
			if( customerData[threadId].itemsToPurchase[j] > customerData[threadId].itemsToPurchase[j+1] )
			{
				int tmp = customerData[threadId].itemsToPurchase[j];
				customerData[threadId].itemsToPurchase[j] = customerData[threadId].itemsToPurchase[j+1];
				customerData[threadId].itemsToPurchase[j+1] = tmp;
			}
		}
	}
Print("WTF@\n");
	if(customerData[threadId].isPrivilageCustomer){								/* To Check if the customer is Privilage or Not */
		Acquire(printLock);
		Print1("PrivilegedCustomer [%d] enters the SuperMarket.\n", threadId);
		Release(printLock);
		Acquire(printLock);
		Print2("PrivilegedCustomer [%d] wants to buy [%d] no.of items.\n",threadId, customerData[threadId].noOfItemsToPurchase);
		Release(printLock);
		for(i=0; i<customerData[threadId].noOfItemsToPurchase; i++)
		{
			Acquire(printLock);
			Print3("PrivilagedCustomer [%d] wants to buy [%d]-[%d].\n",threadId, i+1, customerData[threadId].itemsToPurchase[i]);
			Release(printLock);
		}
		Acquire(printLock);
		Print1("PrivilegedCustomer [%d] gets in line for a trolly\n", threadId);
		Release(printLock);
	}
	else {
		Acquire(printLock);
		Print1("Customer [%d] enter the Super Market.\n", threadId);
		Release(printLock);
		Acquire(printLock);
		Print2("Customer [%d] wants to buy [%d] no.of items.\n",threadId, customerData[threadId].noOfItemsToPurchase);
		Release(printLock);
		for(i=0; i<customerData[threadId].noOfItemsToPurchase; i++)
		{
			Acquire(printLock);
			Print3("Customer [%d] wants to buy [%d]-[%d].\n",threadId, i+1, customerData[threadId].itemsToPurchase[i]);
			Release(printLock);
		}
		Acquire(printLock);
		Print1("Customer [%d] gets in line for a trolly\n", threadId);
		Release(printLock);
	}

	Acquire(trollyLock);												/* Acquiring trolly lock */
	if(totalTrollyCount>0)												                                                              
		totalTrollyCount--;												/* if there is trolly in the line, reduce the length of trolly */
	else																                                                              
	{
		trollyWaitCount++;												/* if no Trolly Available, increment the waitCount */
		Wait(customerTrollyWaitingCV,trollyLock);						/* and wait till anyone puts back the trolly and signals */
		trollyWaitCount--;
		totalTrollyCount--;
	}	
	Release(trollyLock);											/* Release Trolly Lock */

	if(customerData[threadId].isPrivilageCustomer){
		Acquire(printLock);
		Print1("PrivilegedCustomer [%d] has a trolly for shopping\n", threadId);
		Release(printLock);
	}
	else {
		Acquire(printLock);
		Print1("Customer [%d] has a trolly for shopping\n", threadId);
		Release(printLock);
	}
	

	/* ========================For Each item to purchase, Customer does the followig=============================== */

	for(currentItem=0; currentItem<customerData[threadId].noOfItemsToPurchase; currentItem++)
	{
		/* printf("Customer [%d] will now buy CurrentItem = %d\n",threadId,currentItem);	*/
		itemDepartmentId = productData[customerData[threadId].itemsToPurchase[currentItem]].departmentNo;		/* get the departmentID of the item to be purchased */
		itemAisleId = productData[customerData[threadId].itemsToPurchase[currentItem]].aisleId;				/* get the aisleID of the product to be obtained */
		
		/* ==========================Finding and moving to the Department Line where the next item is============================== */
		if(prevDept != itemDepartmentId)													/* if this item is not in the same department as the previous one */
		{																					/* move to the department which has the item */
			
			if(currentItem>0)
			{
				if(customerData[threadId].isPrivilageCustomer){									
					Acquire(printLock);
					Print2("PrivilegedCustomer [%d] has finished shopping in Department[%d]\n", threadId, prevDept);
					Release(printLock);
				}
				else {
					Acquire(printLock);
					Print2("Customer [%d] has finished shopping in Department[%d]\n", threadId, prevDept);
					Release(printLock);
				}
			}
			
			if(customerData[threadId].isPrivilageCustomer){									
				Acquire(printLock);
				Print2("PrivilegedCustomer [%d] wants to shop in Department[%d]\n", threadId, itemDepartmentId);
				Release(printLock);
			}
			else {
				Acquire(printLock);
				Print2("Customer [%d] wants to shop in Department[%d]\n", threadId, itemDepartmentId);
				Release(printLock);
			}




			/* ==========================Customer - Salesman Interaction Starts Here============================== */
			isSignaled = FALSE;

			/* check if any salesman is free */
			for(salesmanIndex=(itemDepartmentId*noOfSalesman); salesmanIndex< ((itemDepartmentId*noOfSalesman) + noOfSalesman); salesmanIndex++)
			{																				/* if anyone is free, directly signal them */
				Acquire(salesmanLock[salesmanIndex]);
				
				/* printf("Customer [%d] acquiredSalesmanLock [%d]\n", threadId, salesmanIndex);
				
				I AM HERE WHILE DEBUGGING

				printf("Salesman Status of Salesman[%d] -- %d\n", salesmanIndex, salesmanStatus[salesmanIndex]); */
					
				if(salesmanStatus[salesmanIndex] == SALESMAN_FREE)									
				{
					/* printf("Customer [%d] found free salesman\n", threadId); */
					isSignaled = TRUE;
					mySalesman = salesmanIndex;
					/* itemToRestockArray[mySalesman] = -1;	*/					/* indicating that I am a new customer */
					salesmanStatus[mySalesman] = SALESMAN_BUSY;
					
					Acquire(salesmanServing);
					salesmanServingCustomer[mySalesman] = threadId;								/* indicating that I am a new customer  */
					Release(salesmanServing);													/* make the status of the salesman BUSY */
					
					Signal(custSalesTalkCV[mySalesman],salesmanLock[mySalesman]); 
					break;
				}
				Release(salesmanLock[salesmanIndex]);
				/* printf("Customer [%d] releasedSalesmanLock [%d]\n", threadId, salesmanIndex); */
			}

			if(isSignaled == FALSE)															/* if no one is free, get in departmentLine and wait for turn */
			{
				Acquire(departmentLineLock[itemDepartmentId]);
				/* printf("Customer[%d] acquired departmentLineLock[%d]", threadId, itemDepartmentId);	*/			/* acquire the department line lock  */
				departmentLineLength[itemDepartmentId]++;														/* and increment the department line length */
				/* printf("Customer[%d] incremented LineLength of department[%d]", threadId, itemDepartmentId); */
		
				if(customerData[threadId].isPrivilageCustomer){									
					Acquire(printLock);
					Print2("PrivilegedCustomer [%d] gets in line for the Department [%d]\n", threadId, mySalesman/noOfSalesman);
					Release(printLock);
				}
				else {
					Acquire(printLock);
					Print2("Customer [%d] gets in line for the Department [%d]\n", threadId, mySalesman/noOfSalesman);
					Release(printLock);
				}
				/* printf("Customer[%d] waiting for salesman Signal", threadId); */
				Wait(deptLineCV[itemDepartmentId],departmentLineLock[itemDepartmentId]);				/* customer waits for the signal from salesman */
				Print("In for loop ");
				for(salesmanIndex=(itemDepartmentId*noOfSalesman); salesmanIndex< ((itemDepartmentId*noOfSalesman) + noOfSalesman); salesmanIndex++)
				{
					
					Acquire(salesmanLock[salesmanIndex]);
					if(salesmanStatus[salesmanIndex] == SALESMAN_SIGNAL_TO_CUSTOMER)
					{
						isSignaled = TRUE;												/* salesman is ready to serve the customer */ 
						mySalesman = salesmanIndex;										                                           
						itemToRestockArray[mySalesman] = -1;							/* indicating that I am a new customer */     
						salesmanStatus[mySalesman] = SALESMAN_BUSY;						/* change status of salesman as BUSY  */      
																						/* and signal salesman   */                   
						Acquire(salesmanServing);
						salesmanServingCustomer[mySalesman] = threadId;
						Release(salesmanServing);
						
						Signal(custSalesTalkCV[mySalesman],salesmanLock[mySalesman]); 
						break;
					}
					Release(salesmanLock[salesmanIndex]);
				}
				Release(departmentLineLock[itemDepartmentId]);
			}
			else
			{
				isSignaled == FALSE;
			}
			
			
			if(customerData[threadId].isPrivilageCustomer){									
				Acquire(printLock);
				Print3("PrivilegedCustomer [%d] is interacting with DepartmentSalesman[%d] of Department[%d]\n", threadId, mySalesman, mySalesman/noOfSalesman);
				Release(printLock);
			}
			else {
				Acquire(printLock);
				Print3("Customer [%d] is interacting with DepartmentSalesman[%d] of Department[%d]\n", threadId, mySalesman, mySalesman/noOfSalesman);
				Release(printLock);
			}
			Signal(custSalesTalkCV[mySalesman],salesmanLock[mySalesman]);		/* signaling that I have come */
			Wait(custSalesTalkCV[mySalesman],salesmanLock[mySalesman]);		/* waiting for the salesman to greet me */
			Signal(custSalesTalkCV[mySalesman],salesmanLock[mySalesman]);		/* signaling the salesman that I am going in department */
			
			/* salesmanServing->Acquire();
			salesmanServingCustomer[mySalesman] = -1;
			salesmanServing->Release();
			*/
			
			Release(salesmanLock[mySalesman]);
			
			prevDept = itemDepartmentId;								/* set variable to check if next item is in same department */
			/* printf("Customer [%d] has been greeted\n", threadId); */
		}


		/* ==========================Customer has been greeted and is now inside the department============================== */
		
		
		Acquire(aisleAccessLock[customerData[threadId].itemsToPurchase[currentItem]]);

		/* printf("Customer [%d] acquired aisleAccessLock [%d]\n", threadId, customerData[threadId].itemsToPurchase[currentItem]); */
	
		if( aisleQuantityArray[customerData[threadId].itemsToPurchase[currentItem]] > 0 )	/* if the item to purchase is present in the aisle */
		{																				/* put it in the trolly and decrement its counter */
			aisleQuantityArray[customerData[threadId].itemsToPurchase[currentItem]]--;
			
			if(customerData[threadId].isPrivilageCustomer){									
				Acquire(printLock);
				Print3("PrivilegedCustomer [%d] has found [%d] and placed [%d] in the trolly\n", threadId, customerData[threadId].itemsToPurchase[currentItem], customerData[threadId].itemsToPurchase[currentItem]);
				Release(printLock);
			}
			else {
				Acquire(printLock);
				Print3("Customer [%d] has found [%d] and placed [%d] in the trolly\n", threadId, customerData[threadId].itemsToPurchase[currentItem], customerData[threadId].itemsToPurchase[currentItem]);
				Release(printLock);
			}
																							/* ITEM PLACED IN TROLLY */
			Release(aisleAccessLock[customerData[threadId].itemsToPurchase[currentItem]]);
		}
		else																			/* if item is not present in the aisle */
		{																				/* release aisle lock and complain to the salesman */
			Release(aisleAccessLock[customerData[threadId].itemsToPurchase[currentItem]]);
			
			isSignaled=FALSE;
			
																					/* if any salesman is free, directly go to him and signal for restock */
			for(salesmanIndex=(itemDepartmentId*noOfSalesman); salesmanIndex< ((itemDepartmentId*noOfSalesman) + noOfSalesman); salesmanIndex++)
			{
				if(customerData[threadId].isPrivilageCustomer){									
					Acquire(printLock);
					Print3("PrivilegedCustomer [%d] is not able to find [%d] and is searching for DepartmentSalesman [%d]\n", threadId, customerData[threadId].itemsToPurchase[currentItem], salesmanIndex);
					Release(printLock);
				}
				else {
					Acquire(printLock);
					Print3("Customer [%d] is not able to find [%d] and is searching for DepartmentSalesman [%d]\n", threadId, customerData[threadId].itemsToPurchase[currentItem], salesmanIndex);
					Release(printLock);
				}
				
				Acquire(salesmanLock[salesmanIndex]);
				
				Acquire(printLock);
				Print2("Customer [%d] acquired salesmanLock[%d]\n",threadId, salesmanIndex);
				Release(printLock);
				
				if(salesmanStatus[salesmanIndex] == SALESMAN_FREE)		/* Check if there is any free salesman	*/							
				{
					isSignaled = TRUE;
					mySalesman = salesmanIndex;
					salesmanStatus[mySalesman]=SALESMAN_BUSY;
					itemToRestockArray[mySalesman] = customerData[threadId].itemsToPurchase[currentItem];	/* setting which item to restock */
					
					Acquire(salesmanServing);
					salesmanServingCustomer[mySalesman] = threadId;		/* letting the salesman know which customer wants to interact */
					Release(salesmanServing);
					
					Signal(custSalesTalkCV[mySalesman],salesmanLock[mySalesman]);
					break;
				}
				Release(salesmanLock[salesmanIndex]);
				Acquire(printLock);
				Print2("Customer [%d] released salesmanLock[%d]\n",threadId, salesmanIndex);
				Release(printLock);
			}

			if(isSignaled == FALSE)										/* if no one is free, get in departmentLine and wait for turn */
			{
				Release(complainLineLock[itemDepartmentId]);
				
				Acquire(printLock);
				Print2("Customer [%d] acquired complainLineLock[%d]\n",threadId, itemDepartmentId);
				Release(printLock);
				
				complainLineLength[itemDepartmentId]++;					/* increment the length of the line and wait for the turn */
				
				Acquire(printLock);
				Print2("complainLineLength[%d] = %d\n",itemDepartmentId, complainLineLength[itemDepartmentId]);
				Release(printLock);
				
				Wait(complainLineCV[itemDepartmentId],complainLineLock[itemDepartmentId]);
				
				Acquire(printLock);
				Print1("Customer [%d] got a call in complainLine\n",threadId);
				Release(printLock);
				
				for(salesmanIndex=(itemDepartmentId*noOfSalesman); salesmanIndex< ((itemDepartmentId*noOfSalesman) + noOfSalesman); salesmanIndex++)
				{
					Acquire(salesmanLock[salesmanIndex]);
					
					Acquire(printLock);
					Print2("Customer [%d] acquired salesmanLock[%d]\n",threadId, salesmanIndex);
					Release(printLock);
					
					if(salesmanStatus[salesmanIndex] == SALESMAN_SIGNAL_TO_CUSTOMER)
					{
						Release(complainLineLock[itemDepartmentId]);
						isSignaled = TRUE;
						mySalesman = salesmanIndex;
						salesmanStatus[mySalesman]=SALESMAN_BUSY;
						
						Acquire(salesmanServing);
						salesmanServingCustomer[mySalesman] = threadId;		/* let the saleman know which customer is interacting */
						Release(salesmanServing);
						
						Signal(custSalesTalkCV[mySalesman],salesmanLock[mySalesman]); 		/* signal the salesman */
						itemToRestockArray[mySalesman] = customerData[threadId].itemsToPurchase[currentItem];	/* setting which item to restock */
						break;
					}
					Release(salesmanLock[salesmanIndex]);
					
					Acquire(printLock);
					Print2("Customer [%d] released salesmanLock[%d]\n",threadId, salesmanIndex);
					Release(printLock);
				}
			}
			else	
			{
				/* isSignaled == FALSE; */
			}
			
			if(customerData[threadId].isPrivilageCustomer){									
				Acquire(printLock);
				Print2("PrivilegedCustomer [%d] is asking for assistance from DepartmentSalesman [%d]\n", threadId, mySalesman);
				Release(printLock);
			}
			else {
				Acquire(printLock);
				Print2("Customer [%d] is asking for assistance from DepartmentSalesman [%d]\n", threadId, mySalesman);
				Release(printLock);
			}
			
			Acquire(printLock);
			Print2("Customer [%d] will wait on salesmanLock[%d] for restock signal\n",threadId, mySalesman);
			Release(printLock);
			
			Wait(custSalesTalkCV[mySalesman],salesmanLock[mySalesman]);
			
			Acquire(printLock);
			Print2("Customer [%d] got restock signal from salesman [%d]", threadId, mySalesman);
			Release(printLock);
			
			
			if(customerData[threadId].isPrivilageCustomer){									
				Acquire(printLock);
				Print3("PrivilegedCustomer [%d] has received assistance about restocking of [%d] from DepartmentSalesman [%d].\n", threadId, customerData[threadId].itemsToPurchase[currentItem], mySalesman);
				Release(printLock);
			}
			else {
				Acquire(printLock);
				Print3("Customer [%d] has received assistance about restocking of [%d] from DepartmentSalesman [%d].\n", threadId, customerData[threadId].itemsToPurchase[currentItem], mySalesman);
				Release(printLock);
			}
			

			Signal(custSalesTalkCV[mySalesman],salesmanLock[mySalesman]);	/* after getting signal that restocking complete, send a signal that -- leaving */
			
			
			itemToRestockArray[mySalesman] = -1;							/* restocking interation over between customer and saleman */
			
			/* salesmanServing->Acquire();
			salesmanServingCustomer[mySalesman] = -1;
			salesmanServing->Release();
			*/
			
			Release(salesmanLock[mySalesman]);
			
			currentItem--;
		}
		

		/* ==========================Customer - Salesman Interaction Ends Here=============================== */
	}

	
	if(customerData[threadId].isPrivilageCustomer){									
		Acquire(printLock);
		Print1("PrivilegedCustomer [%d] is looking for the Cashier.\n", threadId);
		Release(printLock);
	}
	else {
		Acquire(printLock);
		Print1("Customer [%d] is looking for the Cashier.\n", threadId);
		Release(printLock);
	}
	
	/* ==========================Customer - Cashier Interaction Starts Here=============================== */
	
	
	Acquire(shortestLineLock);
	/* printf("Customer [%d] Acquired shortestLineLock\n", threadId); */
	
	for(i=0; i<noOfCashiers; i++)						/* Acquiring all the cashier line lock to check for the shortest line */
	{
		if(customerData[threadId].isPrivilageCustomer)
		{
			Acquire(privilageCashierLineLock[i]);
			if( privilageCashierLineLength[i]< minLineLength )			/* computing minimum line length */
			{
				if(cashierStatus[i] != CASHIER_NOT_STARTED);
				{
					minLineLength = privilageCashierLineLength[i];
					myCashier = i;
				}
			}
			Release(privilageCashierLineLock[i]);
		}
		else
		{
			Acquire(cashierLineLock[i]);
			if( cashierLineLength[i]< minLineLength )
			{
				if(cashierStatus[i] != CASHIER_NOT_STARTED);
				{
					minLineLength = cashierLineLength[i];
					myCashier = i;
				}
			}
			Release(cashierLineLock[i]);
		}
		
	}
	
	
	
	/* cashierLineLock[myCashier]->Acquire(); */
	if(customerData[threadId].isPrivilageCustomer){									
		Acquire(printLock);
		Print3("PrivilegedCustomer [%d] chose Cashier [%d] with line of length [%d].\n", threadId, myCashier, privilageCashierLineLength[myCashier]);
		Release(printLock);
	}
	else {
		Acquire(printLock);
		Print3("Customer [%d] chose Cashier [%d] with line of length [%d].\n", threadId, myCashier, cashierLineLength[myCashier]);
		Release(printLock);
	}
	/* cashierLineLock[myCashier]->Release(); */
		
	Acquire(custCashierTalkLock[myCashier]);
	if(cashierStatus[myCashier] == CASHIER_FREE)
	{																		/* checking if the cashier whose shortest line the customer has computed is free */
		Release(shortestLineLock);										                                                                              
		cashierStatus[myCashier] = CASHIER_BUSY;							                                                                              
																			/* if he is free, change his status to busy  */                                  
		Acquire(cashierServing);											                                                                              
		cashierServingCustomer[myCashier] = threadId;						 
		Release(cashierServing);
		
	}
	else
	{	
																			/* if he is not free, acquire cashierline lock and increment the value of line */
		if(customerData[threadId].isPrivilageCustomer){						 
			Acquire(privilageCashierLineLock[myCashier]);					 
			Release(custCashierTalkLock[myCashier]);						 
			privilageCashierLineLength[myCashier]++;
			Release(shortestLineLock);
			Wait(privilageCashierLineCV[myCashier],privilageCashierLineLock[myCashier]);
			
		}
		else {
			Acquire(cashierLineLock[myCashier]);						
			Release(custCashierTalkLock[myCashier]);					
			cashierLineLength[myCashier]++;								
			Release(shortestLineLock);
			Wait(cashierLineCV[myCashier],cashierLineLock[myCashier]);
		}
		
		Acquire(custCashierTalkLock[myCashier]);				                 
		cashierStatus[myCashier] = CASHIER_BUSY;				/* on recieving signal from cashier, acquire the cashier talk lock */
																/* and make his status to busy   */                                  
		Acquire(cashierServing);
		cashierServingCustomer[myCashier] = threadId;
		Release(cashierServing);
		
		
		if(customerData[threadId].isPrivilageCustomer){	
			Release(privilageCashierLineLock[myCashier]);
		}
		else {
			Release(cashierLineLock[myCashier]);
		}
		
	}
	
	for(i=0; i<customerData[threadId].noOfItemsToPurchase; i++)					/* ping pong between customer and cashier of the items processed */
	{
		custCashItemArray[myCashier] = customerData[threadId].itemsToPurchase[i];
		Signal(custCashierTalkCV[myCashier],custCashierTalkLock[myCashier]);
		Wait(custCashierTalkCV[myCashier],custCashierTalkLock[myCashier]);
		/* printf("Customer [%d] processed item[%d]\n", threadId, i); */
	}
	
	custCashItemArray[myCashier] = -1;											/* -1 signifies that all items have been processed and  */                   
	Signal(custCashierTalkCV[myCashier],custCashierTalkLock[myCashier]);		/* now signals */                                                            
	Wait(custCashierTalkCV[myCashier],custCashierTalkLock[myCashier]);			/* and waits for cashier */                                                  
																				                                                                          
	myBill = custCashItemArray[myCashier];										/* reads the total bill of items purchased from the custCashItemArray array */
	/* printf("Customer [%d] BILL = [%d]\n", threadId, myBill);	*/				/* where the customer reads from the index myCashier */                      
	if(myBill <= customerData[threadId].moneyWithMe)							/* if the customer has money  */                                             
	{																			/* he pays to the cashier */                                            
		hasCustomerMoney[threadId] = TRUE;										                                                                          
																				                                                                          
		if(customerData[threadId].isPrivilageCustomer){										                     		
			Acquire(printLock);
			Print3("PrivilegedCustomer [%d] pays [%d] to Cashier [%d] and is now waiting for receipt.\n", threadId, myBill, myCashier);
			Release(printLock);
		}
		else {
			Acquire(printLock);
			Print3("Customer [%d] pays [%d] to Cashier [%d] and is now waiting for receipt.\n", threadId, myBill, myCashier);
			Release(printLock);
		}
		
	}
	else																			/* else if the customer does not have money  */
	{
		hasCustomerMoney[threadId] = FALSE;
		if(customerData[threadId].isPrivilageCustomer){									
			Acquire(printLock);
			Print2("PrivilegedCustomer [%d] cannot pay [%d]\n", threadId, myBill);
			Release(printLock);
		}
		else {
			Acquire(printLock);
			Print2("Customer [%d] cannot pay [%d]\n", threadId, myBill);
			Release(printLock);
		}
	}
	/* printf("Customer [%d] signaled Cashier[%d] if money or not.. hasCustomerMoney[%d]\n", threadId, myCashier, hasCustomerMoney); */

	Signal(custCashierTalkCV[myCashier],custCashierTalkLock[myCashier]);						/* signal if I have enough money or not */

	/* printf("Customer [%d] will wait inform the Cashier[%d] if he has enough money\n", threadId, myCashier); */

	Wait(custCashierTalkCV[myCashier],custCashierTalkLock[myCashier]);							/* wait for cashier reply/receipt */
	
	/* printf("Customer [%d] signaled Cashier[%d] that I am leaving\n", threadId, myCashier); */
	
	if(hasCustomerMoney[threadId] == TRUE)
	{
		if(customerData[threadId].isPrivilageCustomer){									
			Acquire(printLock);
			Print2("PrivilagedCustomer [%d] got receipt from Cashier [%d] and is now leaving.\n", threadId, myCashier);
			Release(printLock);
		}
		else {
			Acquire(printLock);
			Print2("Customer [%d] got receipt from Cashier [%d] and is now leaving.\n", threadId, myCashier);
			Release(printLock);
		}
	}
	
	Acquire(printLock);
	Print2("Customer[%d] signaled Cashier [%d] that I am leaving\n", threadId, myCashier);
	Release(printLock);
	
	Signal(custCashierTalkCV[myCashier],custCashierTalkLock[myCashier]);					/* signal - I am going */
	
	/*cashierServing->Acquire();
	cashierServingCustomer[myCashier] = -1;
	cashierServing->Release();
	*/
	
	/* managerLock->Acquire(); */
	
	/* printf("Customer [%d] acquired ManagerCustomerInteractionLock\n", threadId); */
	
	
	
	
	/* printf("Customer [%d] released custCashierTalkLock[%d]\n", threadId, myCashier); */
	if(hasCustomerMoney[threadId] == FALSE)
	{
		Release(custCashierTalkLock[myCashier]);
		Acquire(managerCustomerInteractionLock);
		if(customerData[threadId].isPrivilageCustomer){									
			Acquire(printLock);
			Print1("PrivilegedCustomer [%d] is waiting for Manager for negotiations.\n", threadId);
			Release(printLock);
		}
		else {
			Acquire(printLock);
			Print1("Customer [%d] is waiting for Manager for negotiations.\n", threadId);
			Release(printLock);
		}
		
		/* managerLock->Acquire(); */
		
		/* printf("Customer [%d] acquired Manager Lock\n", threadId); */
		/* managerLineLength++; */
		
		/* managerCustomerCV->Wait(managerLock); */	/*Waits for Manager to say Hello */
		customerManagerItemExchange = myBill;
		managersCustomer = threadId;
		
		Acquire(printLock);
		Print2("ManagersCustomer: %d\n Bill: %d\n", managersCustomer, customerManagerItemExchange);
		Release(printLock);
		
		Acquire(printLock);
		Print1("Customer [%d] signalled Manager for Arrival\n", threadId);
		Release(printLock);
		
		Signal(managerCustomerInteractionCV,managerCustomerInteractionLock);	/* signals Manager to Take the Bill */
		/* printf("Customer[%d] waiting for signal for 1st item", threadId); */
		Wait(managerCustomerInteractionCV,managerCustomerInteractionLock);	/* Waits for manager to ask for 1st item */
		/* printf("Customer [%d] got Signal for 1st Item from Manager\n", threadId); */
		
		i=0;

		/* while(myBill>=customerManagerItemExchange) */
		do
		{
			Acquire(printLock);																		/* signal manager to remove 1 item at a time */
			Print1("Next Item: %d\n", customerData[threadId].itemsToPurchase[i]);	
			Release(printLock);
			
			Signal(managerCustomerInteractionCV,managerCustomerInteractionLock);								                             
			
			customerManagerItemExchange = customerData[threadId].itemsToPurchase[i];		 
			
			Acquire(printLock);
			Print1("Customer [%d] Waiting for manager to give new Bill\n", threadId);
			Release(printLock);
			
			Wait(managerCustomerInteractionCV,managerCustomerInteractionLock);								                             
			/* myBill = customerManagerItemExchange; */
			i++;
		}while(customerManagerItemExchange>=customerData[threadId].moneyWithMe);
		
		myBill = customerManagerItemExchange;												/* save the final bill amount */
		/* printf("Customer [%d] processed all items with Manager and Bill = %d\n", threadId, myBill); */
		customerManagerItemExchange = -1;
		
		if(customerData[threadId].isPrivilageCustomer){									
			Acquire(printLock);
			Print2("PrivilegedCustomer [%d] pays [%d] to Manager after removing items and is waiting for receipt from Manager.\n",threadId, myBill);
			Release(printLock);
		}
		else {
			Acquire(printLock);
			Print2("Customer [%d] pays [%d] to Manager after removing items and is waiting for receipt from Manager.\n", threadId, myBill);
			Release(printLock);
		}
		
		Signal(managerCustomerInteractionCV,managerCustomerInteractionLock);									/* Signal that Item Over and Send Payment */
		/* printf("BBN\n"); */
		Wait(managerCustomerInteractionCV,managerCustomerInteractionLock);
		/* printf("BBK\n"); */
		
		if(customerData[threadId].isPrivilageCustomer){									
			Acquire(printLock);
			Print1("PrivilegedCustomer [%d] got receipt from Manager and is now leaving.\n", threadId);
			Release(printLock);
		}
		else {
			Acquire(printLock);
			Print1("PrivilegedCustomer [%d] got receipt from Manager and is now leaving.\n", threadId);
			Release(printLock);
		}
		
		Release(managerCustomerInteractionLock);
		/* printf("Customer [%d] released Manager Lock\n", threadId); */
		
	}
	else
	{
		Release(custCashierTalkLock[myCashier]);
		Release(managerCustomerInteractionLock);
	}
	
	Acquire(customerNumberLock);
	noOfCustomersExited++;
	Release(customerNumberLock);
	
	/* printf("Customer [%d] left Supermarket.!!!!\n", threadId); */
	
	Acquire(trollyLock);												
		totalTrollyCount++;												/* after sompleting Shopping, put back the trolly */
		if(trollyWaitCount>0)											/* increment trollyCount */    
		{																/* signal to costumer if anyone is waiting */
			Signal(customerTrollyWaitingCV,trollyLock);				                                                 
		}
	Release(trollyLock);
	
	Exit(0);
}



/* ============End of Customer Thread Code================== */




/*============ Salesman Thread Code STARTS HERE ==================*/

void Salesman()
{	
	
	int i;
	int threadId;
	int myGoodsLoader =-1;
	/*printf("Salesman %d starts\n",threadId); */
	/*TODO : initialize salesman thread counter and lock*/
	Print("In salesman function \n");
	Acquire(salesmanThreadCounterLock);
	threadId=salesmanThreadCounter;
	salesmanThreadCounter++;
	Release(salesmanThreadCounterLock);
	while(noOfCustomers!=noOfCustomersExited)
	{
		int myDepartment = threadId/noOfSalesman;
		
		Acquire(departmentLineLock[myDepartment]);						/* Acquires departmentLineLock to serve a customer*/

		/* printf("Salesman[%d] acquired departmentLineLock[%d]", threadId, myDepartment); */

		if(departmentLineLength[myDepartment] > 0)
		{
			
			departmentLineLength[myDepartment]--;											/*if the department line has any customers  */
																							                                            
			Acquire(salesmanLock[threadId]);												/*decrement the line and signal to customer */
			/* printf("Salesman[%d] Acquired SalesmanLock[%d]\n", threadId, threadId); */
			salesmanStatus[threadId]=SALESMAN_SIGNAL_TO_CUSTOMER;
			
			/* signal customer deptLineCV */
			Signal(deptLineCV[myDepartment],departmentLineLock[myDepartment]);
			
			/*printf("Salesman[%d] Signaled to customer on departmentLineLock[%d]\n",threadId, myDepartment);
			//lineLock->release */
			
			Release(departmentLineLock[myDepartment]);
			/*printf("Salesman[%d] released departmentLineLock[%d]\n", threadId, myDepartment);
			wait for customer coming
			printf("Salesman[%d] will wait on salesmanLock[%d]\n", threadId, threadId); */
			Wait(custSalesTalkCV[threadId],salesmanLock[threadId]);
			
			/*signal to welcome */
			Signal(custSalesTalkCV[threadId],salesmanLock[threadId]);				/*signal the customer for welcoming*/
			
			Acquire(salesmanServing);
			if(customerData[salesmanServingCustomer[threadId]].isPrivilageCustomer){									
				Acquire(printLock);
				Print3("DepartmentSalesman [%d] welcomes PrivilegeCustomer [%d] to Department [%d].\n", threadId, salesmanServingCustomer[threadId], myDepartment);
				Release(printLock);
			}
			else {
				Acquire(printLock);
				Print3("DepartmentSalesman [%d] welcomes Customer [%d] to Department [%d].\n", threadId, salesmanServingCustomer[threadId], myDepartment);
			    Release(printLock);
			}
			Release(salesmanServing);
			
			/*wait for customer leaving
			printf("Salesman[%d] will wait on salesmanLock[%d]\n", threadId, threadId);*/
			Wait(custSalesTalkCV[threadId],salesmanLock[threadId]);
			
			/*release salesmanLock*/
			Release(salesmanLock[threadId]);
			/*printf("Salesman[%d] released SalesmanLock[%d]\n", threadId, threadId);*/
		}

		else
		{
			/*printf("5. Salesman[%d]: myDepartment: %d\n", threadId, myDepartment);*/
			Release(departmentLineLock[myDepartment]);
			/*printf("Salesman[%d] released departmentLineLock[%d]\n", threadId, myDepartment);*/
		}

		
		
		
		/*------ COMPLAINING CUSTOMER CODE STARTS------------*/
		
		/*printf("6. Salesman[%d]: myDepartment: %d\n", threadId, myDepartment);*/
		Acquire(complainLineLock[myDepartment]);
		Acquire(printLock);
		Print2("Salesman[%d] acquired complainLineLock[%d]\n", threadId, myDepartment);				                                                
		Release (printLock);
		
		if(complainLineLength[myDepartment] > 0)
		{
			/*set status = signal to customer
			myGoodsLoader = -1;
			length--
			printf("7. Salesman[%d]: myDepartment: %d\n", threadId, myDepartment);*/
			complainLineLength[myDepartment]--;	
												/*if there is any customer in the complain Line */
			Print2("complainLineLength[%d] = %d\n",myDepartment, complainLineLength[myDepartment]);
			/*salesmanLock[]->acq	*/															                                                
			Acquire(salesmanLock[threadId]);													                                                
			Print2("Salesman[%d] acquired SalesmanLock[%d]\n", threadId, threadId);				                                                
																								/*set the status to signal to customer    */      
			salesmanStatus[threadId]=SALESMAN_SIGNAL_TO_CUSTOMER;						                                                
			/*signal customer complainLineCV
			printf("8. Salesman[%d]: myDepartment: %d\n", threadId, myDepartment); */
			
			Signal(complainLineCV[myDepartment],complainLineLock[myDepartment]);
			Acquire(printLock);
			Print2("Salesman[%d] signaled customer on complainLineLock[%d]\n",threadId, myDepartment);
			Release(printLock);
			/*lineLock->release
			printf("9. Salesman[%d]: myDepartment: %d\n", threadId, myDepartment);*/
			Release(complainLineLock[myDepartment]);
			Acquire(printLock);
			Print2("Salesman[%d] released complainLineLock[%d]", threadId, myDepartment);
			Release(printLock);
			/*wait for customer setting value*/
			Acquire(printLock);
			Print2("Salesman[%d] will wait on salesmanLock[%d] for Complain Signal\n", threadId, threadId);
			Release(printLock);
			
			Wait(custSalesTalkCV[threadId],salesmanLock[threadId]);
			Acquire(printLock);
			Print2("Salesman[%d] got complain signal on salesmanLock[%d]\n", threadId, threadId);
			Release(printLock);
			
			Acquire(salesmanServing);
			if(customerData[salesmanServingCustomer[threadId]].isPrivilageCustomer){									
				Acquire(printLock);
				Print3("DepartmentSalesman [%d] is informed by PrivilegeCustomer [%d] that [%d] is out of stock.\n", threadId, salesmanServingCustomer[threadId], itemToRestockArray[threadId]);
				Release(printLock);
			}
			else {
				Acquire(printLock);
				Print3("DepartmentSalesman [%d] is informed by Customer [%d] that [%d] is out of stock.\n", threadId, salesmanServingCustomer[threadId], itemToRestockArray[threadId]);
				Release(printLock);
			}
			Release(salesmanServing);
			
			for(i=0; i< noOfGoodsLoaders; i++)
			{
				Acquire(goodsLoaderSalesmanLock[i]);
				/*printf("Salesman[%d] acquired goodsLoaderSalesmanLock[%d]", threadId, i);*/

				if(goodsLoaderStatus[i]==GLOADER_FREE)						/*check if a particular goodsLoader is free*/
				{
					myGoodsLoader=i;
					goodsLoaderStatus[i]=GLOADER_BUSY;
					break;													/*if free then break so that salesman can talk to goodsLoader*/
				}
				Release(goodsLoaderSalesmanLock[i]);
				/*printf("Salesman[%d] released goodsLoaderSalesmanLock[%d]", threadId, i);*/
			}

			if(myGoodsLoader==-1)
			{
				Acquire(goodsLoaderLineLock);	
				/*printf("Salesman[%d] acquired goodsLoaderLineLock", threadId);*/
				goodsLoaderLineLength++;
				
				/*printf("Salesman[%d] waits on goodsLoaderLineLock\n", threadId);*/
				Wait(goodsLoaderLineCV,goodsLoaderLineLock);						/*salesman waiting for goods loader*/
				for(i=0; i< noOfGoodsLoaders; i++)
				{
					Acquire(goodsLoaderSalesmanLock[i]);
					/*printf("Salesman[%d] acquired goodsLoaderSalesmanLock[%d]", threadId, i);*/
					if(goodsLoaderStatus[i]==GLOADER_SIGNAL_TO_SALESMAN)
					{
						Release(goodsLoaderLineLock);
						myGoodsLoader=i;
						goodsLoaderStatus[i]=GLOADER_BUSY;
						break;		
					}
					Release(goodsLoaderSalesmanLock[i]);
					/*printf("Salesman[%d] released goodsLoaderSalesmanLock[%d]", threadId, i);*/
				}
			}
			
			Acquire(goodsSalesServing);					/*setting the salesman ID for GoodsLoader to inform it*/
			salesmansGL = threadId;
			Release(goodsSalesServing);
			
			goodsSalesRestocking[myGoodsLoader]=itemToRestockArray[threadId];  	
			Signal(goodsLoaderSalesmanCV[myGoodsLoader],goodsLoaderSalesmanLock[myGoodsLoader]);
			Acquire(printLock);
			Print3("DepartmentSalesman [%d] informs the GoodsLoader [%d] that [%d] is out of stock.\n", threadId, myGoodsLoader, itemToRestockArray[threadId]);
			Release(printLock);
			Wait(goodsLoaderSalesmanCV[myGoodsLoader],goodsLoaderSalesmanLock[myGoodsLoader]);
			
			Acquire(printLock);
			Print3("DepartmentSalesman [%d] is informed by the GoodsLoader [%d] that [%d] is restocked.\n", threadId, myGoodsLoader, itemToRestockArray[threadId]);
			Release(printLock);
			
			Release(goodsLoaderSalesmanLock[myGoodsLoader]);
			/*printf("Salesman[%d] released on goodsLoaderSalesmanLock[%d]", threadId, myGoodsLoader);
			printf("Salesman [%d] got restocking Signal and released goodsLoaderLock\n", threadId);*/	

			Signal(custSalesTalkCV[threadId],salesmanLock[threadId]);							/*saleman got restock signalfrom goodloader*/
			
			Acquire(salesmanServing);
			if(customerData[salesmanServingCustomer[threadId]].isPrivilageCustomer){									
				Acquire(printLock);
				Print3("DepartmentSalesman [%d] informs the PrivilegeCustomer [%d] that [%d] is restocked.\n", threadId, salesmanServingCustomer[threadId], itemToRestockArray[threadId]);
				Release(printLock);
			}
			else {
				Acquire(printLock);
				Print3("DepartmentSalesman [%d] informs the Customer [%d] that [%d] is restocked.\n", threadId, salesmanServingCustomer[threadId], itemToRestockArray[threadId]);
				Release(printLock);
			}
			Release(salesmanServing);
			
			Wait(custSalesTalkCV[threadId],salesmanLock[threadId]);
			myGoodsLoader = -1;
			
			Release(salesmanLock[threadId]); 
			/*printf("Salesman[%d] released salesmanLock[%d]", threadId, threadId);*/
		}
		else
		{
			/*printf("10. Salesman[%d]: myDepartment: %d\n", threadId, myDepartment);*/
			Release(complainLineLock[myDepartment]);
			Acquire(printLock);
			Print2("Salesman[%d] released complainLineLock[%d]\n", threadId, myDepartment);
			Release(printLock);
		}


		/*-------- COMPLAINING CUSTOMER CODE ENDS------------
		printf("11. Salesman[%d]: myDepartment: %d\n", threadId, myDepartment);*/
		


		Acquire(departmentLineLock[myDepartment]);
		/*printf("12. Salesman[%d]: myDepartment: %d\n", threadId, myDepartment);*/
		Acquire(complainLineLock[myDepartment]);
		/*printf("13. Salesman[%d]: myDepartment: %d\n", threadId, myDepartment);*/
		if(departmentLineLength[myDepartment] == 0 && complainLineLength[myDepartment] == 0)				/*checks if the lines are empty  then keeps on waiting.   */
		{																								                                                              
			/*acq salesmanLock[]		*/																                                                              
			Acquire(salesmanLock[threadId]);															/*sets the staus to be free         */
			/*printf("Salesman[%d] acquired salesmanLock[%d]\n", threadId, threadId);					                                                              
			set status = free	*/																		                                                              
			salesmanStatus[threadId]=SALESMAN_FREE;														                       
			/*printf("14. Salesman[%d]: myDepartment: %d\n", threadId, myDepartment);	*/				                                                              
			Release(complainLineLock[myDepartment]);													                                                              
			/*printf("15. Salesman[%d]: myDepartment: %d\n", threadId, myDepartment);	*/				                                                              
			Release(departmentLineLock[myDepartment]);												                                                            
																										
			/*wait for someone to signal																                                                              
			printf("Salesman[%d] will wait on salesmanLock[%d] for someone to signal\n", threadId, threadId);*/
			Wait(custSalesTalkCV[threadId],salesmanLock[threadId]);									/*keep on waiting until customer comes */

			if(itemToRestockArray[threadId] == -1)							/*if the customer is a new customer to department*/
			{
				/*signal to welcome */
				salesmanStatus[threadId]=SALESMAN_SIGNAL_TO_CUSTOMER;						/*set status to signal to customer*/
				Signal(custSalesTalkCV[threadId],salesmanLock[threadId]);					/*Signal to welcome the customer*/
				/*printf("Salesman[%d] signaled on salesmanLock[%d] to welcome customer\n", threadId, threadId);
				wait for customer leaving
				printf("Salesman[%d] will wait on salesmanLock[%d] for customer to leave\n", threadId, threadId);*/
				
				Acquire(salesmanServing);
				if(customerData[salesmanServingCustomer[threadId]].isPrivilageCustomer){									
					Acquire(printLock);
					Print3("DepartmentSalesman [%d] welcomes PrivilegeCustomer [%d] to Department [%d].\n", threadId, salesmanServingCustomer[threadId], myDepartment);
					Release(printLock);
				}
				else {
					Acquire(printLock);
					Print3("DepartmentSalesman [%d] welcomes Customer [%d] to Department [%d].\n", threadId, salesmanServingCustomer[threadId], myDepartment);
					Release(printLock);
				}
				Release(salesmanServing);
				
				Wait(custSalesTalkCV[threadId],salesmanLock[threadId]);			/*Wait for Customer to leave*/
				/*release salesmanLock*/
				Release(salesmanLock[threadId]);
			}
			else												/*If the customer is Complaining Customer*/
			{
				/*myGoodsLoader = -1;*/
				for(i=0; i< noOfGoodsLoaders; i++)
				{
					Acquire(goodsLoaderSalesmanLock[i]);											/*Check to see if any GoodsLoader is free*/
					/*printf("Salesman[%d] acquired goodsLoaderSalesmanLock[%d]", threadId, i);*/
					if(goodsLoaderStatus[i]==GLOADER_FREE)       /*check if a particular goodsLoader is free*/
					{
						myGoodsLoader=i;
						goodsLoaderStatus[i]=GLOADER_BUSY;
						break;									/*if free then break so that salesman can talk to goodsLoader*/
					}
					Release(goodsLoaderSalesmanLock[i]);
					/*printf("Salesman[%d] released goodsLoaderSalesmanLock[%d]", threadId, i);*/
				}

				if(myGoodsLoader==-1)								/*if still no GoodsLoader found free*/
				{
					Acquire(goodsLoaderLineLock);				/*Get in GoodsLoader Line and wait for some goodsLoader to be free*/
					goodsLoaderLineLength++;
					
				/*	printf("Salesman[%d] will wait on goodsLoaderLineLock", threadId);*/
					Wait(goodsLoaderLineCV,goodsLoaderLineLock); 
					for(i=0; i< noOfGoodsLoaders; i++)						/*when Got the signal, check which goodsLoader Signaled*/
					{
						Acquire(goodsLoaderSalesmanLock[i]);
						/*printf("Salesman[%d] acquired goodsLoaderSalesmanLock[%d]", threadId, i);*/
						if(goodsLoaderStatus[i]==GLOADER_SIGNAL_TO_SALESMAN)
						{
							Release(goodsLoaderLineLock);
							myGoodsLoader=i;
							goodsLoaderStatus[i]=GLOADER_BUSY;
							break;		
						}
						Release(goodsLoaderSalesmanLock[i]);
						/*printf("Salesman[%d] released goodsLoaderSalesmanLock[%d]", threadId, i);*/
					}
					
				}
				
				Acquire(goodsSalesServing);					/*setting the salesman ID for GoodsLoader to inform it*/
				salesmansGL = threadId;
				Release(goodsSalesServing);
				
				goodsSalesRestocking[myGoodsLoader]=itemToRestockArray[threadId];  			/*Tell the goodsLoader, which Item is to be restocked*/
				/*printf("Salesman[%d] signals on goodsLoaderSalesmanLock[%d]", threadId, myGoodsLoader);*/
				Signal(goodsLoaderSalesmanCV[myGoodsLoader],goodsLoaderSalesmanLock[myGoodsLoader]);
				
				Acquire(printLock);
				Print3("DepartmentSalesman [%d]] informs the GoodsLoader [%d] that [%d] is out of stock.\n", threadId, myGoodsLoader, itemToRestockArray[threadId]);
				Release(printLock);
				/*printf("Salesman[%d] will wait on goodsLoaderSalesmanLock[%d]", threadId, myGoodsLoader);*/
				Wait(goodsLoaderSalesmanCV[myGoodsLoader],goodsLoaderSalesmanLock[myGoodsLoader]);	/*Wait for the GoodsLoader to restock and Signal BAck*/
				Acquire(printLock);
				Print3("DepartmentSalesman [%d] is informed by the GoodsLoader [%d] that [%d] ] is restocked.\n", threadId, myGoodsLoader, itemToRestockArray[threadId]);
				Release(printLock);
				
				Release(goodsLoaderSalesmanLock[myGoodsLoader]);
				/*printf("Salesman[%d] released goodsLoaderSalesmanLock[%d]", threadId, myGoodsLoader);
					
				printf("Salesman[%d] signals on salesmanLock[%d]", threadId, threadId);*/
				Signal(custSalesTalkCV[threadId],salesmanLock[threadId]);						/*inform the customer that the Restocking is done*/
				
				Acquire(salesmanServing);
				if(customerData[salesmanServingCustomer[threadId]].isPrivilageCustomer){									
					Acquire(printLock);
					Print3("DepartmentSalesman [%d] informs the PrivilegeCustomer [%d] that [%d] is restocked.\n", threadId, salesmanServingCustomer[threadId], itemToRestockArray[threadId]);
					Release(printLock);
				}
				else {
					Acquire(printLock);
					Print3("DepartmentSalesman [%d] informs the Customer [%d] that [%d] is restocked.\n", threadId, salesmanServingCustomer[threadId], itemToRestockArray[threadId]);
					Release(printLock);
				}
				Release(salesmanServing);
				
				/*printf("Salesman[%d] waits on salesmanLock[%d]", threadId, threadId);*/
				Wait(custSalesTalkCV[threadId],salesmanLock[threadId]);
				myGoodsLoader = -1;
				
				/*printf("Salesman[%d] released salesmanLock[%d]", threadId, threadId);*/
				/*set arrayIndex = -1*/
				itemToRestockArray[threadId]=-1;
				Release(salesmanLock[threadId]); 	
			}

		}
		else									/*if both lines are not empty, release both lineLocks*/
		{
			/*printf("16. Salesman[%d]: myDepartment: %d\n", threadId, myDepartment);*/
			Release(complainLineLock[myDepartment]);
			/*printf("17. Salesman[%d]: myDepartment: %d\n", threadId, myDepartment);*/
			Release(departmentLineLock[myDepartment]);
			
		}
	}

}

/*============End of Salesman Thread Code==================*/



/* ============ Manager Thread Code STARTS HERE ==================*/



void Manager()
{    
    int amountToRemove;
   /*  Print("Manager Thread Starts\n"); */
    int i;
    int salesmanOnBreak;
    int cashierOnBreak;
    int bill;
    int myCashier;
    int myCustomer;
    int managerSales=0;
    
    while(noOfCustomers!=noOfCustomersExited)
    {
        /*
        customerNumberLock->Acquire();
        if(noOfCustomers == noOfCustomersExited)
        {
            customerNumberLock->Release();
            break;
        }
        customerNumberLock->Release();
        */
        salesmanOnBreak=GetRand()%noOfSalesman;        /*randomly select which salesman to send on break*/
        cashierOnBreak=GetRand()%noOfCashiers;            /*randomly select which Cashier to send on break*/
        bill = 0;
        /*
        if(GetRand()%4==3)
        {
            managerSalesmanBreakLock[salesmanOnBreak]->Acquire();
            isSalesmanOnBreak[salesmanOnBreak]=TRUE;
            printf("Manager sends DepartmentSalesman [%d] of Department [%d] on break.\n", salesmanOnBreak, salesmanOnBreak/noOfSalesman);
            managerSalesmanBreakLock[salesmanOnBreak]->Release();       
            printf("Manager Released managerSalesmanBreakLock[%d]", salesmanOnBreak);
        }
        if(GetRand()%4==2)
        {
            managerCashierBreakLock[cashierOnBreak]->Acquire();
            isCashierOnBreak[cashierOnBreak]=TRUE;
            printf("Manager sends Cashier [%d] on break.\n", cashierOnBreak);
            managerCashierBreakLock[cashierOnBreak]->Release();
        }
        */
        /*
        for(int i=0;i<noOfSalesman;i++)
        {
            departmentLineLock[i/noOfSalesman]->Acquire();
            if(departmentLineLength[i/noOfSalesman]>3)
            {
                if(isSalesmanOnBreak[i]==TRUE)
                {
                    isSalesmanOnBreak[i]=FALSE;
                    printf("Manager brings back DepartmentSalesman [%d] of Department [%d] from break.\n", salesmanOnBreak, salesmanOnBreak/noOfSalesman);                    
                }
            }
            departmentLineLock[i/noOfSalesman]->Release();
        }
           
         
        for(int i=0;i<noOfCashiers;i++)
        {
            cashierLineLock[i]->Acquire();
            if(cashierLineLength[i]>3)
            {
                if(isCashierOnBreak[i]==TRUE)
                {
                    isCashierOnBreak[i]=FALSE;
                    printf("Manager brings back Cashier [%d] from break.\n", i);
                }
            }
            cashierLineLock[i]->Release();
            
            privilageCashierLineLock[i]->Acquire();
            if(privilageCashierLineLength[i]>3)
            {
                if(isCashierOnBreak[i]==TRUE)
                {
                    isCashierOnBreak[i]=FALSE;
                    printf("Manager brings back Cashier [%d] from break.\n", i);
                }
            }
            privilageCashierLineLock[i]->Release();
            
        }
        */
        for(i=0;i<noOfCashiers;i++)        /*Go to each Cashier and Collect the amount collected by them*/
        {
            Acquire(managerCashierLock[i]);
            totalRevenue+=cashierSalesArray[i];
            cashierSalesArray[i]=0;
           /* printf("Manager emptied Counter [%d] drawer.",i);*/
            Release(managerCashierLock[i]);   
        }
        totalRevenue+=managerSales;
        managerSales=0;
        Acquire(printLock);
        Print1("-------Total Sale of the entire store until now is $[%d]---------\n", totalRevenue);
        Release(printLock);
        Acquire(managerLock);
        
        if(managerLineLength==0)                                /*if there is no one waiting, release the managerLock*/
        {
           /* printf("ManagerLineLength = 0, releasing Lock\n");*/
            Release(managerLock);
        
        }
        else if(managerLineLength!=0)                    /*if the lineLength is greater than 0*/
        {
                Signal(managerCashierCV,managerLock);    /*send signal to the cashier to come*/
                /*printf("manager will server cashier\n");*/
                
                
                
                managerLineLength--;
                Acquire(printLock);
                Print1("Manager's Cashier before Setting: %d\n", managersCashier);
				Release(printLock);
				Acquire(printLock);
                Print1("myCashier: %d\n", myCashier);
                Release(printLock);
                
                
                Acquire(managerCashierInteractionLock);
                Release(managerLock);
                Wait(managerCashierInteractionCV,managerCashierInteractionLock); /*waits for cashier to tell its query*/
                
                myCashier = managersCashier;
                Acquire(printLock);
                Print1("Manager's Cashier After: %d\n", managersCashier);
                Release(printLock);
				
				Acquire(printLock);
                Print1("Manager got a call from Cashier [%d].\n", myCashier);
                Release(printLock);   
                
				Acquire(printLock);
                Print1("Manager Signals Cashier [%d] to send the Customer\n", myCashier);    
                Release(printLock);                
                
				Signal(managerCashierInteractionCV,managerCashierInteractionLock);
                
				Acquire(printLock);
                Print("Now Manager acquires managerCustomerInteractionLock\n");
                Release(printLock);
                
				Acquire(managerCustomerInteractionLock);
                Release(managerCashierInteractionLock);
                
				Acquire(printLock);    
                Print("manager will wait on ManagerCustomeInteractionCV\n");
                Release(printLock);                
                
				Wait(managerCustomerInteractionCV,managerCustomerInteractionLock);
                
				Acquire(printLock);    
                Print("1111. manager got signal on ManagerCustomeInteractionCV\n");
                Release(printLock);
                
				/*managerCustomerServnig->Acquire();*/
                myCustomer = managersCustomer;                    /*find out which customer */
                /*managerCustomerServnig->Release();*/
                
                bill=customerManagerItemExchange;
                Acquire(printLock);    
                Print2("myCustomer: %d\n His Bill: %d\n", myCustomer, bill);
                /*printf("Customer Total Bill: %d\n", bill);*/
                Release(printLock);
				
				Acquire(printLock);
                Print1("Manager: Give me 1st Item to Customer [%d]", myCustomer);
                Release(printLock);
				
                Signal(managerCustomerInteractionCV,managerCustomerInteractionLock); /*signals the customer to give the 1st item*/
                
                customerManagerItemExchange=0;            /*sends the customer intial amount deducted = 0*/
                
                Wait(managerCustomerInteractionCV,managerCustomerInteractionLock);    /*waiting for customer to send the 1st item*/
                
                /*printf("Manager got 1st Item from Customer [%d]\n", myCustomer);*/
                
                while(customerManagerItemExchange!=-1)            /*scan item that customer sends, until he is done with*/
                {
                    if(customerData[myCustomer].isPrivilageCustomer)
                    {
                        Acquire(printLock);    
                        Print2("Manager removes [%d] from the trolly of PrivilegeCustomer [%d].\n",customerManagerItemExchange, myCustomer);
                        Release(printLock);
                    }
                    else{
                        Acquire(printLock);    
                        Print2("Manager removes [%d] from the trolly of Customer [%d].\n",customerManagerItemExchange, myCustomer);
                        Release(printLock);
                    }
                    Acquire(printLock);    
                    Print1("itemExchange: %d\n", customerManagerItemExchange);
					Release(printLock);
                    /*for(int i=0; i<customerData[myCustomer].noOfItemsToPurchase; i++)
                    {
                        printf("ProductData[%d]: [%d]\n",i, productData[i].price);
                    }
                    */
                    
					Acquire(printLock); 
                    Print1("Price of Item: %d\n", productData[customerManagerItemExchange].price);
                    Release(printLock);
					
                    bill = bill - productData[customerManagerItemExchange].price;
                    
                    Acquire(cancelledItemLock);
                    cancelledItemArray[customerManagerItemExchange]++;        /*add the item collected from customer to myCollectedList*/
                    
					Acquire(printLock);    
                    Print1("customerManagerItemExchange: %d\n",customerManagerItemExchange);
                    Release(printLock);
                    
					Release(cancelledItemLock);
                    
                    customerManagerItemExchange= bill;
                    
					Acquire(printLock);    
                    Print1("Bill Sent: %d\n",customerManagerItemExchange);
                    Release(printLock);
                    
                    Signal(managerCustomerInteractionCV,managerCustomerInteractionLock);        /*tell the customer to give new item and give him new bill*/
                    
					Acquire(printLock);    
                    Print("Manager Signals for 1st item\n");
                    Release(printLock);
                    
					Wait(managerCustomerInteractionCV,managerCustomerInteractionLock);        /*wait for customer to give another item*/
                    
					Acquire(printLock);    
                    Print("After Wait for 1st Item\n");
                    Release(printLock);
                }
                
                if(customerData[myCustomer].isPrivilageCustomer)
                {
                    Acquire(printLock);    
                    Print1("Manager gives receipt to PrivilegeCustomer [%d].\n", myCustomer);
                    Release(printLock);
                }
                else{
                    Acquire(printLock);    
                    Print1("Manager gives receipt to PrivilegeCustomer [%d].\n", myCustomer);
                    Release(printLock);
                }
                
                managerSales+=bill;
                
                Signal(managerCustomerInteractionCV,managerCustomerInteractionLock);        /*give receipt to customer
                managerCustomerServnig->Acquire();
                myCustomer = -1;                        
                after serving the customer, set myCustomer value to -1
                managerCustomerServnig->Release();
                */
                Release(managerCustomerInteractionLock);
                
                /*managerLock->Release();*/
                Acquire(printLock);    
                Print1("Manager has total sale of $[%d].\n", managerSales);
                Release(printLock);
           
        }
        for(i=0; i<1000; i++)
        {
            Yield();                        /*Yield the currentThread for 1000 times*/
        }
        
    }
	Exit(0);
}

/*============End of Manager Thread Code==================*/


/* ============ GoodsLoader Thread Code STARTS HERE ================== */


void GoodsLoader()
{
    int threadId;
	int productID;
	int mySalesman;
	int restockDepartment;
	int goodsLoaderWalkingTime;
	
    Acquire(glThreadCntrLock);
    threadId=glThreadCntr;
    glThreadCntr++;
    Release(glThreadCntrLock);
/*===================================GoodsLoader Line Lock Getting code below===================================
    TODO: REMOVE PRINTF
    printf("\nGoodsLoader [%d] thread Starts\n", threadId);*/
    while(noOfCustomers!=noOfCustomersExited)
    {
        goodsLoaderWalkingTime = (GetRand()%20)+20;     /*Random generated between 20 and 40 which is the time taken by goods loader to walk from stock room to aisle*/
        
        Acquire(goodsLoaderLineLock);         /*After the salesman releases goodsLoaderLineLock, the goodsloader acquires to check if any salesman waiting.*/
        /*printf("goodsLoader[%d] acquired goodsLoaderLineLock", threadId);*/
        if(goodsLoaderLineLength==0)             /*if the goodloader finds the queue to be empty, it releases the lock*/
        {
            goodsLoaderStatus[threadId] = GLOADER_FREE;
            
			Acquire(printLock);
            Print1("GoodsLoader [%d] is waiting for orders to restock.\n", threadId);
            Release(printLock);
            
			/*printf("goodsLoader[%d] acquired goodsLoaderSalesmanLock[%d]", threadId, threadId);*/
            Acquire(goodsLoaderSalesmanLock[threadId]); /*lock for talk between salesman and goodsloader*/
        }
        else
        {
            goodsLoaderStatus[threadId] = GLOADER_SIGNAL_TO_SALESMAN;            
            Signal(goodsLoaderLineCV,goodsLoaderLineLock); /*Goodsloader signals the salesman*/
            /*goodsLoaderSalesmanCV[threadId]->Signal(goodsLoaderLineLock); Goodsloader signals the salesman*/
            /*printf("goodsLoader[%d] signaled on goodsLoaderLineLock", threadId);*/
            goodsLoaderLineLength--;                                    /*decrement the line length */
            /*printf("goodsLoader[%d] will wait on goodsLoaderLineLock", threadId);*/
           
           /* goodsLoaderLineCV->Wait(goodsLoaderLineLock); */
            
            Acquire(goodsLoaderSalesmanLock[threadId]);
            /*printf("goodsLoader[%d] acquired goodsLoaderSalesmanLock[%d]", threadId, threadId);*/
        }
            
            Release(goodsLoaderLineLock);                                /* Releasing goodsloader lock*/
            
            /*printf("goodsLoader[%d] released goodsLoaderLineLock", threadId);
            printf("goodsLoader[%d] will wait on goodsLoaderSalesmanLock[%d]", threadId, threadId);*/
            Wait(goodsLoaderSalesmanCV[threadId],goodsLoaderSalesmanLock[threadId]); 
            
            Acquire(goodsSalesServing);
            mySalesman = salesmansGL;
            Release(goodsSalesServing);
            Acquire(printLock);/*TODO*/
            Print3("GoodsLoader [%d] is informed by DepartmentSalesman [%d] of Department [%d]\n", threadId, mySalesman, mySalesman/noOfDepartments);
            Print1("to restock [%d]", goodsSalesRestocking[threadId]);
            Release(printLock);
      
            Acquire(aisleAccessLock[goodsSalesRestocking[threadId]]);                  /*Acquiring aisle lock so that no other customer
            printf("goodsLoader[%d] acquired aisleAccessLock[%d]", threadId, goodsSalesRestocking[threadId]); */  
            
        AisleLockLabel:                                                                  /*can pick item when goodsloader is putting on them.*/
            Acquire(cancelledItemLock);                                 /*Acquiring lock on cancelled items.Before the goods loader checks stock room, */
                                                                        /*it check first whether the item is found in the cancelled item array. */
            /*printf("goodsLoader[%d] acquired cancelledItemLock", threadId);      */
            if(cancelledItemArray[goodsSalesRestocking[threadId]]>0)
            {    
                cancelledItemArray[goodsSalesRestocking[threadId]]--;                             /*If the goods lader finds the item, 
                                                                        he takes them from cancelled array and puts them into aisle one by one.*/
                Release(cancelledItemLock);                             /*Releases the cancelled item lock
                printf("goodsLoader[%d] released cancelledItemLock", threadId);          */
            }
            else
            {
                Release(cancelledItemLock);                             /*Releases the cancelled item lock*/
                                                                            
                Acquire(stockRoomLineLock);                             /*stock room line lock acquire so that at a time*/
                
				Acquire(printLock);                                                        /*only one goods loader can pick item from store*/
                Print2("GoodsLoader [%d] is in the StockRoom and got [%d].\n", threadId, goodsSalesRestocking[threadId]);                                                            
                Release(printLock);
                
				Release(stockRoomLineLock);                            /*Releasing stock room line lock*/
                
				Acquire(printLock);
                Print1("GoodsLoader [%d] leaves StockRoom.\n", threadId);
                Release(printLock);
            }
            
            /*
            for(int i=0;i<goodsLoaderWalkingTime;i++)
            {
                currentThread->Yield();                                 time to walk from stock room to aisle by goods loader
            }
            */
            
            aisleQuantityArray[goodsSalesRestocking[threadId]]++;                    /*increments the quantity of the product*/
            

            if(aisleQuantityArray[goodsSalesRestocking[threadId]]<itemQuantity)     /*if the aisle quantity not to max, restock*/
            {
                goto AisleLockLabel;
            }
            Release(aisleAccessLock[goodsSalesRestocking[threadId]]); 
            /*printf("goodsLoader[%d] released aisleAccessLock[%d]\n", threadId, goodsSalesRestocking[threadId]);  */
            restockDepartment = (goodsSalesRestocking[threadId]/noOfDepartments);
            
			Acquire(printLock);
            Print3("GoodsLoader [%d] has restocked [%d] in Department [%d].\n", threadId, goodsSalesRestocking[threadId], mySalesman/noOfDepartments);
            Release(printLock);
            
            Signal(goodsLoaderSalesmanCV[threadId],goodsLoaderSalesmanLock[threadId]); 
        /*    goodsLoaderStatus[threadId] = GLOADER_FREE;*/
            Release(goodsLoaderSalesmanLock[threadId]);                                 /*goodsLoaderLock release*/
            
            for(productID= 0; productID<(noOfDepartments*MAX_NO_OF_ITEMS); productID++)                     /*iterate for the number of products*/
            {
                while(aisleQuantityArray[productID]<=itemQuantity)                         /*compares if the no.of item in aisle matches the MAX number=5*/
                {   Acquire(printLock);
                    Print1("aisleLock-[%d] to be acquired\n", productID);
                    Release(printLock);
                    
					Acquire(aisleAccessLock[productID]);
                    
					Acquire(printLock);
                    Print1("aisleLock-[%d] Acquired\n", productID);
                    Release(printLock);
                    
					/*Acquiring aisle lock of the product to be restocked*/
                    /*printf("goodsLoader[%d] acquired aisleAccessLock[%d]", threadId, productID);*/                                                            
                    Acquire(cancelledItemLock);                         /*acquiring cancelled item lock*/
                    /*printf("goodsLoader[%d] acquired cancelledItemLock", threadId);*/                                                            
                    if(cancelledItemArray[productID]>0)             /* if the product is present in the cancelled item array then get it from there*/
                    {    
                        cancelledItemArray[productID]--;         /*take product from cancelled item array and put it to shelf and decrementing its qty by 1*/
                        Release(cancelledItemLock);                    /*release the cancelled item lock
                        printf("goodsLoader[%d] released cancelledItemLock", threadId);*/
                    }
                    else                                                 /*else go to stock room */
                    {
                        Release(cancelledItemLock);                     /*release the cancelled item lock*/
                        /*printf("goodsLoader[%d] released cancelledItemLock", threadId);*/                                                            
                        Acquire(stockRoomLineLock);                     /*stock room line lock acquire */
                        /*printf("goodsLoader[%d] acquired stockRoomLineLock", threadId);*/                                                            
                        
                        Release(stockRoomLineLock);                     /*stock room line lock release
                        printf("goodsLoader[%d] released stockRoomLineLock", threadId);    */                                                        
                        
                    }
                    /*
                    for(int i=0;i<goodsLoaderWalkingTime;i++) 
                    {
                        currentThread->Yield();                         time to walk from stock room to aisle by goods loader
                    }
                    */
                    
                    /*TODO incrementing the quantity of the product*/
                    aisleQuantityArray[productID]++;
                
                    /* set values of product id to 1.*/
                                
                    Release(aisleAccessLock[productID]);                /*releasing the aisle for the product id. */            
                    /*printf("goodsLoader[%d] released aisleAccessLock[%d]", threadId, productID);*/                                                            
                }
            }
            
            Acquire(goodsSalesServing);
            mySalesman = -1;                /*resets the item after the interaction*/
            Release(goodsSalesServing);
            
            Acquire(goodsLoaderSalesmanLock[threadId]);
            goodsLoaderStatus[threadId] = GLOADER_FREE;
            Release(goodsLoaderSalesmanLock[threadId]);
            
                 
            /*printf("goodsLoader[%d] released goodssLoaderSalesmanLock[%d]", threadId, threadId);*/                                                            

    }
        
    
}
/*===========End of GoodsLoader Thread Code==================*/


/*============Start of Cashier Thread Code==================*/


void Cashier()
{
	/* TODO: to initialize cashier thread counter and cashierthread counter lock*/
	int threadId;
	int myCustomer;
	int totalBill;
	Acquire(cashierThreadCounterLock);
	threadId=cashierThreadCounter;
	cashierThreadCounter++;
	Release(cashierThreadCounterLock);
	
	
	while(noOfCustomers!=noOfCustomersExited)
	{
		
		myCustomer=-1;
		/*printf("Cashier [%d] Starts\n", threadId);*/
		totalBill = 0;
		Acquire(privilageCashierLineLock[threadId]);
		Acquire(printLock);
		Print2("Cashier [%d] acquired privilageCashierLineLock[%d]\n", threadId, threadId);
		Release(printLock);
		if(privilageCashierLineLength[threadId]>0)		/*if someone there in line*/
		{
			cashierStatus[threadId]=CASHIER_SIGNAL_TO_CUSTOMER;
			privilageCashierLineLength[threadId]--;
			Signal(privilageCashierLineCV[threadId],privilageCashierLineLock[threadId]); 	/*cashier signals customer to come*/
			Acquire(printLock);
			Print2("Cashier [%d] signaled on privilageCashierLineLock[%d]\n", threadId, threadId);
			Release(printLock);
			Acquire(custCashierTalkLock[threadId]);		/*cashier acquires interaction lock with cashier*/
			Acquire(printLock);
			Print2("Cashier [%d] acquired custCashierTalkLock[%d]\n", threadId, threadId);
			Release(printLock);
			/*cashierLineCV[threadId]->Wait(cashierLineLock[threadId]);*/
			Release(privilageCashierLineLock[threadId]);
			Acquire(printLock);
			Print2("Cashier [%d] released privilageCashierLineLock[%d]\n", threadId, threadId);
			Release(printLock);
		}
		else
		{
			Release(privilageCashierLineLock[threadId]);
			Acquire(printLock);
			Print2("Cashier [%d] released privilageCashierLineLock[%d]\n", threadId, threadId);
			Release(printLock);
			Acquire(cashierLineLock[threadId]);
			Acquire(printLock);
			Print2("Cashier [%d] acquired cashierLineLock[%d]\n", threadId, threadId);
			Release(printLock);
			if(cashierLineLength[threadId]>0)		/*if someone there in line*/
			{
				cashierStatus[threadId]=CASHIER_SIGNAL_TO_CUSTOMER;
				cashierLineLength[threadId]--;
				Signal(cashierLineCV[threadId],cashierLineLock[threadId]); 	/*cashier signals customer to come*/
				Print1("Cashier [%d] signaled on cashierLineLock\n", threadId);
				Acquire(custCashierTalkLock[threadId]);		/*cashier acquires interaction lock with cashier*/
				Acquire(printLock);
				Print2("Cashier [%d] acquired custCashierTalkLock[%d]\n", threadId, threadId);
				Release(printLock);
				/*cashierLineCV[threadId]->Wait(cashierLineLock[threadId]);*/
			}
			else
			{
				Acquire(custCashierTalkLock[threadId]);
				Acquire(printLock);
				Print2("Cashier [%d] acquired cashierLineLock[%d]\n", threadId, threadId);
				Release(printLock);
				cashierStatus[threadId]=CASHIER_FREE;
				/*printf("Cashier [%d] acquired custCashierTalkLock[%d]\n", threadId, threadId);*/
			}
			Release(cashierLineLock[threadId]);
			Acquire(printLock);
			Print2("Cashier [%d] released cashierLineLock[%d]\n", threadId, threadId);
			Release(printLock);
		}
		
		
		/*printf("Cashier [%d] released cashierLineLock[%d]\n", threadId, threadId);*/
		/*printf("Cashier[%d] waiting for Customer to give an item to scan\n",threadId);*/
		Wait(custCashierTalkCV[threadId],custCashierTalkLock[threadId]);
		
		Acquire(printLock);
		Print2("Cashier [%d] got signal from Customer[%d]\n", threadId, cashierServingCustomer[threadId]);
		Release(printLock);
		
		Acquire(cashierServing);
		myCustomer = cashierServingCustomer[threadId];		/*gets the customer ID for interaction*/
		Release(cashierServing);
		
		while(custCashItemArray[threadId]!= -1)			/*gets items until customer tells that no more items remaining*/
		{
			
			Acquire(cashierServing);
			if(customerData[cashierServingCustomer[threadId]].isPrivilageCustomer){									
				Acquire(printLock);
				Print3("Cashier [%d] got [%d] from trolly of PrivilegedCustomer [%d].\n", threadId, custCashItemArray[threadId], myCustomer);
				Release(printLock);
			}
			else {
				Acquire(printLock);
				Print3("Cashier [%d] got [%d] from trolly of Customer [%d].\n", threadId, custCashItemArray[threadId], myCustomer);
				Release(printLock);
			}
			Release(cashierServing);
			
			
			
			totalBill += productData[custCashItemArray[threadId]].price;			/*adds price of current item to total*/
			Signal(custCashierTalkCV[threadId],custCashierTalkLock[threadId]);		/*signals customer to give another item*/
			Wait(custCashierTalkCV[threadId],custCashierTalkLock[threadId]);		/*waits for customer to give an item*/
			
		}
		custCashItemArray[threadId]= totalBill;
		Signal(custCashierTalkCV[threadId],custCashierTalkLock[threadId]); /*tell customer the price of the goods purchased*/
		
		Acquire(cashierServing);
		if(customerData[cashierServingCustomer[threadId]].isPrivilageCustomer){									
			Acquire(printLock);
			Print3("Cashier [%d] tells PrivilegedCustomer [%d] total cost is $[%d].\n", threadId, myCustomer, totalBill);
			Release(printLock);
		}
		else {
			Acquire(printLock);
			Print3("Cashier [%d] tells Customer [%d] total cost is $[%d].\n", threadId, myCustomer, totalBill);
			Release(printLock);
		}
		Release(cashierServing);
		
		
		Wait(custCashierTalkCV[threadId],custCashierTalkLock[threadId]); /*wait for the customer to give the money
		//printf("Cashier[%d] waking up from custCashierTalkCV[%d]\n",threadId, threadId);
		//TODO check boolean variable to determin the action on if and else
		//if TRUE -> receipt
		//if FALSE -> 
		//if(hasCustomerMoney==TRUE)
		//{
		//	custCashierTalkCV[threadId]->Signal(custCashierTalkLock[threadId]); //Receipt
		//	custCashierTalkCV[threadId]->Wait(custCashierTalkLock[threadId]);
			//cashierStatus[threadId]= CASHIER_FREE;
		//}*/
		if(hasCustomerMoney[myCustomer]==FALSE)
		{
			Acquire(managerLock);
			
			/*printf("Cashier[%d] acquired Manager Lock\n", threadId);*/
			managerLineLength++;
			/*isCashier = TRUE;*/
			Wait(managerCashierCV,managerLock);	/*wait for manager to reply*/
			
			/*managerCashierServing->Acquire();*/
			managersCashier = threadId;			/*cashier sets its id for interacting with manager*/
			/*managerCashierServing->Release();*/
			
			Acquire(managerCashierInteractionLock);
			Release(managerLock);
			
			if(customerData[myCustomer].isPrivilageCustomer){								
				Acquire(printLock);
				Print2("Cashier [%d] informs the Manager that PrivilegedCustomer [%d] does not have enough money.\n", threadId, myCustomer);
				Release(printLock);
			}
			else {
				Acquire(printLock);
				Print2("Cashier [%d] informs the Manager that Customer [%d] does not have enough money.\n", threadId, myCustomer);
				Release(printLock);
			}
			
			Signal(managerCashierInteractionCV,managerCashierInteractionLock);		/*tell the manager that customer has no money*/
			
			Wait(managerCashierInteractionCV,managerCashierInteractionLock);
			
			/*managerCashierCV->Signal(managerLock);	//tell the manager that customer has no money
			printf("Cashier [%d] waiting for ManagerSignal\n", threadId);*/
			
			Release(managerCashierInteractionLock);
			
			
			if(customerData[myCustomer].isPrivilageCustomer){								
				Acquire(printLock);
				Print2("Cashier [%d] asks PrivilagedCustomer [%d] to wait for manager\n", threadId, myCustomer);
				Release(printLock);
			}
			else {
				Acquire(printLock);
				Print2("Cashier [%d] asks Customer [%d] to wait for manager\n", threadId, myCustomer);
				Release(printLock);
			}
			
			
			/*printf("Cashier [%d] released Manager Lock\n", threadId);*/
			Signal(custCashierTalkCV[threadId],custCashierTalkLock[threadId]); /* signals the customer to go to manager*/
			
			Wait(custCashierTalkCV[threadId],custCashierTalkLock[threadId]);	/*wait for customer to leave*/
			Acquire(printLock);
			Print2("Cashier [%d] got signal from customer [%d] that he is leaving\n",threadId, myCustomer );
			Release(printLock);
			/*isCashier = FALSE;*/
		}
		else
		{
			Acquire(managerCashierLock[threadId]);
            cashierSalesArray[threadId] += totalBill;		/*cashier adds the customer bill to drawer*/
			totalDrawerCollection[threadId] += totalBill;	/*cashier adds totalBill to its sale count*/
            Release(managerCashierLock[threadId]);  	
			
			if(customerData[myCustomer].isPrivilageCustomer){									
				Acquire(printLock);
				Print3("Cashier [%d] got money $[%d] from PrivilegedCustomer [%d].\n", threadId, totalBill, myCustomer);
				Release(printLock);
			}
			else {
				Acquire(printLock);
				Print3("Cashier [%d] got money $[%d] from Customer [%d].\n", threadId, totalBill, myCustomer);
				Release(printLock);
			}
			
			Signal(custCashierTalkCV[threadId],custCashierTalkLock[threadId]); /*cashier gives receipt to customer*/
			
			if(customerData[myCustomer].isPrivilageCustomer){								
				Acquire(printLock);
				Print2("Cashier [%d] gave the Receipt to PrivilagedCustomer [%d] and waiting for him to leave\n", threadId, myCustomer);
				Release(printLock);
			}
			else {
				Acquire(printLock);
				Print2("Cashier [%d] gave the Receipt to Customer [%d] and waiting for him to leave\n", threadId, myCustomer);
				Release(printLock);
			}
			
			Wait(custCashierTalkCV[threadId],custCashierTalkLock[threadId]);	/*wait for customer to leave*/
		}
		
			/*if(customerData[myCustomer].isPrivilageCustomer){								
				printf("Cashier[%d] gave the Receipt to PrivilagedCustomer [%d] and waiting for him to leave\n", threadId,  myCustomer);
			}
			else {
				printf("Cashier[%d] gave the Receipt to Customer and waiting for him to leave\n", threadId, totalBill, myCustomer);
			}
			*/
				
		/*cashierServing->Acquire();
		cashierServingCustomer[threadId] = -1;		//removing the index position
		cashierServing->Release();*/
		
		Release(custCashierTalkLock[threadId]);
		Acquire(printLock);
		Print2("Cashier [%d] released custCashierTalkLock[%d]\n", threadId, threadId);
		Release(printLock);
	}
}



/*============End of Cashier Thread Code==================*/



void locksAndCVDeclare()
{
	int i;
	
	/* ======================== Creating all the Locks Declared above =============================== */
	trollyLock = CreateLock("trollyLock", sizeof("trollyLock"));
	stockRoomLineLock= CreateLock("stockRoomLineLock", sizeof("stockRoomLineLock"));
	goodsLoaderLineLock= CreateLock("goodsLoaderLineLock", sizeof("goodsLoaderLineLock"));
	cancelledItemLock= CreateLock("cancelledItemLock", sizeof("cancelledItemLock"));
	shortestLineLock = CreateLock("shortestLineLock", sizeof("shortestLineLock"));		
	managerLock = CreateLock("managerLock", sizeof("managerLock"));
	managerCustomerInteractionLock = CreateLock("managerCustomerInteractionLock", sizeof("managerCustomerInteractionLock"));
	managerCashierInteractionLock = CreateLock("managerCashierInteractionLock", sizeof("managerCashierInteractionLock"));
	customerNumberLock = CreateLock("customerNumberLock", sizeof("customerNumberLock"));
	salesmanServing = CreateLock("salesmanServing", sizeof("salesmanServing"));
	cashierServing = CreateLock("cashierServing", sizeof("cashierServing"));
	/*
	custThreadCntrLock = CreateLock("custThreadCntrLock", sizeof("custThreadCntrLock"));
	cashierThreadCounterLock = CreateLock("cashierThreadCounterLock", sizeof("cashierThreadCounterLock"));
	salesmanThreadCounterLock = CreateLock("salesmanThreadCounterLock", sizeof("salesmanThreadCounterLock"));
	glThreadCntrLock = CreateLock("glThreadCntrLock", sizeof("glThreadCntrLock"));
	*/
	managerCashierServing = CreateLock("managerCashierServing", sizeof("managerCashierServing"));
	managerCustomerServnig = CreateLock("managerCustomerServnig", sizeof("managerCustomerServnig"));
	goodsSalesServing = CreateLock("goodsSalesServing", sizeof("goodsSalesServing"));
	
	

	for(i=0; i<(noOfDepartments*MAX_NO_OF_ITEMS); i++)
	{
		
		aisleAccessLock[i]=CreateLock("aisleAccessLock",sizeof("aisleAccessLock"));
	}

	for(i=0; i<noOfDepartments; i++)
	{
		
		departmentLineLock[i]=CreateLock("departmentLineLock",sizeof("departmentLineLock"));
	}

	for(i=0; i<noOfDepartments; i++)
	{
		
		complainLineLock[i]=CreateLock("complainLineLock",sizeof("complainLineLock"));
	}

	for(i=0; i< noOfSalesman*noOfDepartments; i++)
	{
		
		salesmanLock[i]=CreateLock("salesmanLock",sizeof("salesmanLock"));
	}

	for(i=0; i< noOfGoodsLoaders; i++)
	{

		goodsLoaderSalesmanLock[i]=CreateLock("goodsLoaderSalesmanLock",sizeof("goodsLoaderSalesmanLock"));
	}
	
	for(i=0; i< noOfCashiers; i++)
	{

		cashierLineLock[i]=CreateLock("cashierLineLock",sizeof("cashierLineLock"));
	}

	for(i=0; i< noOfCashiers; i++)
	{
	
		privilageCashierLineLock[i]=CreateLock("privilageCashierLineLock",sizeof("privilageCashierLineLock"));
	}
	
	for(i=0; i< noOfCashiers; i++)
	{

		managerCashierLock[i]=CreateLock("managerCashierLock",sizeof("managerCashierLock"));
	}
	
	for(i=0; i< noOfCashiers; i++)
	{

		custCashierTalkLock[i]=CreateLock("custCashierTalkLock",sizeof("custCashierTalkLock"));
	}
	

	/* ======================== Condition Variables =============================== */
	
	
	goodsLoaderLineCV=CreateCondition("goodsLoaderLineCV", sizeof("goodsLoaderLineCV"));
	customerTrollyWaitingCV = CreateCondition("customerTrollyWaitingCV", sizeof("customerTrollyWaitingCV"));
	managerCustomerCV=CreateCondition("managerCustomerCV", sizeof("managerCustomerCV"));
	managerCashierCV=CreateCondition("managerCashierCV", sizeof("managerCashierCV"));
	managerCustomerInteractionCV = CreateCondition ("managerCustomerInteractionCV", sizeof("managerCustomerInteractionCV"));
	managerCashierInteractionCV = CreateCondition ("managerCashierInteractionCV", sizeof("managerCashierInteractionCV"));
	

	
	for(i=0; i< noOfDepartments; i++)
	{
		
		deptLineCV[i]=CreateCondition("deptLineCV", sizeof("deptLineCV"));
	}
	
	for(i=0; i< noOfGoodsLoaders; i++)
	{
		
		goodsLoaderSalesmanCV[i]=CreateCondition("goodsLoaderSalesmanCV", sizeof("goodsLoaderSalesmanCV"));
	}

	for(i=0; i< noOfDepartments*noOfSalesman; i++)
	{
		
		custSalesTalkCV[i]=CreateCondition("custSalesTalkCV", sizeof("custSalesTalkCV"));
	}
	
	for(i=0; i< noOfDepartments*noOfSalesman; i++)
	{
		
		complainLineCV[i]=CreateCondition("complainLineCV", sizeof("complainLineCV"));
	}
	
	for(i=0; i< noOfCashiers; i++)
	{
		
		cashierLineCV[i]=CreateCondition("cashierLineCV", sizeof("cashierLineCV"));
	}
	
	for(i=0; i< noOfCashiers; i++)
	{
		
		privilageCashierLineCV[i]=CreateCondition("privilageCashierLineCV", sizeof("privilageCashierLineCV"));
	}
	
	for(i=0; i< noOfCashiers; i++)
	{
	
		custCashierTalkCV[i]=CreateCondition("custCashierTalkCV", sizeof("custCashierTalkCV"));
	}
}




/*====================================================================================
 Other Functions
====================================================================================*/

void totalItemDecleration()
{
	int i;
	int tmpForProdId = 0;
		Print1("\n\nNumber of Cashiers = [%d]\n",noOfCashiers);
	Print1("Number of Goods Loaders = [%d]\n",noOfGoodsLoaders);
	Print1("Number of PrivilegedCustomers = [%d]\n", noOfPrivilageCustomers);
	Print1("Number of Customers = [%d]\n",noOfCustomers);
	Print("Number of Managers = [1]\n");								
	Print1("Number of DepartmentSalesmen = [%d]\n",noOfSalesman*noOfDepartments);
    Print("Items:\n");
    Print("\tNumber - Price\n");	
	for(i=0; i<noOfDepartments*MAX_NO_OF_ITEMS; i++)
	{
		Print2("[%d]    -   $[%d]\n",i ,productData[i].price);
	}
		

	

	
	/* ========================Defining Variables=============================== */

	itemQuantity = 5;
	for(i=0; i<noOfDepartments*MAX_NO_OF_ITEMS; i++)
	{
		productData[i].departmentNo = (int) (i/MAX_NO_OF_ITEMS);
		productData[i].aisleId = i;
		productData[i].price = (GetRand()%100)+1;
	}
	
	for(i=0; i<noOfDepartments*MAX_NO_OF_ITEMS; i++)
	{
		aisleQuantityArray[i] = itemQuantity;
	}
	
	for(i=0; i<noOfCustomers; i++)
	{
		hasCustomerMoney[i] = FALSE;
	}

	for(i=0; i<noOfSalesman*noOfDepartments; i++)
	{
		itemToRestockArray[i] = -1;
	}
	for(i=0; i<noOfDepartments; i++)
	{
		departmentLineLength[i] = 0;
		complainLineLength[i] = 0;
	}
	for(i=0; i<noOfCashiers; i++)
	{
		cashierLineLength[i] = 0;
		custCashItemArray[i] = -2;
		privilageCashierLineLength[i] = 0;
		totalDrawerCollection[i] = 0;
		cashierSalesArray[i] = 0;
	}
	
	maxMoney = 1000;
	/* maxMoney = TOTAL_MAX_MONEY;	 
	itemQuantity = MAX_QUANTITY; */
	totalTrollyCount = MAX_TROLLY_COUNT;
	
	
	locksAndCVDeclare();

	

	for(i=0;i<noOfCustomers;i++)
	{
		Fork(Customer);
	}

	for(i=0;i<noOfGoodsLoaders;i++)
	{
		Fork(GoodsLoader);
	}
	
	for(i=0;i<noOfCashiers;i++)
	{
		Fork(Cashier);
	}
	 
	for(i=0;i<(noOfSalesman*noOfDepartments);i++)
	{
		Fork(Salesman);
	}	
	
	Fork(Manager);
}
/*
void getUserData()
{
	
	printf("\nEnter the no of Customers:");
	scanf("%d",&noOfCustomers);
	// noOfCustomers=10;


	if(noOfCustomers>MAX_NO_OF_CUSTOMERS || noOfCustomers<30)
	{
		printf("\nINVALID number of customers. Enter value btw 30 and %d. Program Ends...\n\n",MAX_NO_OF_CUSTOMERS);

		return;
	}

	printf("\nEnter the no of Cashiers: ");
	scanf("%d",&noOfCashiers);
	// noOfCashiers=4;

	if(noOfCashiers>MAX_NO_OF_CASHIER || noOfCashiers<3)
	{
		printf("\nINVALID number of Cashiers. Enter value btw 3 and 5. Program Ends...\n\n");
		return;
	}

	printf("\nEnter the no of Salesman: ");
	scanf("%d",&noOfSalesman);
	// noOfSalesman=2;
	
	
	if(noOfSalesman>MAX_NO_OF_SALESMAN || noOfSalesman<2)
	{
		printf("\nINVALID number of Salesman. Enter value btw 2 and %d Program Ends...\n\n", MAX_NO_OF_SALESMAN);
		return;
	}

	printf("\nEnter the no of Departments: ");
	scanf("%d",&noOfDepartments);
	noOfDepartments=2;
	
	
	if(noOfDepartments>MAX_NO_OF_DEPARTMENTS || noOfDepartments<2)
	{
		printf("\nINVALID number of Departments. Enter value btw 2 and %d. Program Ends...\n\n", MAX_NO_OF_DEPARTMENTS);
		return;
	}

	printf("\nEnter the no of Goods Loaders: ");
	scanf("%d",&noOfGoodsLoaders);
	noOfGoodsLoaders=3;
	
	
	if(noOfGoodsLoaders>MAX_NO_OF_GOODSLOADER || noOfGoodsLoaders<3)
	{
		printf("\nINVALID number of GoodsLoader. Enter value btw 3 and %d. Program Ends...\n\n", MAX_NO_OF_GOODSLOADER);
		return;
	}

	totalItemDecleration();
}
*/

void initCustomer()
{
	int i;
	for(i=0; i<noOfCustomers; i++)
	{
		if(GetRand()%5==4){
			customerData[i].isPrivilageCustomer=TRUE;
			noOfPrivilageCustomers++;
		}
		else{
			customerData[i].isPrivilageCustomer=FALSE;
		}
		customerData[i].isComplaining = FALSE;
	}
}

void testcase1()
{
	int i;
	noOfCashiers = 2;
	noOfDepartments = 1;
	noOfSalesman = 3;
	noOfCustomers = 10;
	itemQuantity = MAX_QUANTITY;
	maxMoney = 1000;
	totalTrollyCount = MAX_TROLLY_COUNT;
	
	initCustomer();
	
	for(i=0; i<noOfDepartments*MAX_NO_OF_ITEMS; i++)
	{
		productData[i].departmentNo = (int) (i/MAX_NO_OF_ITEMS);
		productData[i].aisleId = i;
		productData[i].price = (GetRand()%100)+1;
	}
	
	for(i=0; i<noOfDepartments*MAX_NO_OF_ITEMS; i++)
	{
		aisleQuantityArray[i] = itemQuantity;
	}
	
	for(i=0; i<noOfCustomers; i++)
	{
		hasCustomerMoney[i] = FALSE;
	}

	for(i=0; i<noOfSalesman*noOfDepartments; i++)
	{
		itemToRestockArray[i] = -1;
	}
	for(i=0; i<noOfDepartments; i++)
	{
		departmentLineLength[i] = 0;
		complainLineLength[i] = 0;
	}
	for(i=0; i<noOfCashiers; i++)
	{
		cashierLineLength[i] = 0;
		custCashItemArray[i] = -2;
		privilageCashierLineLength[i] = 0;
		totalDrawerCollection[i] = 0;
		cashierSalesArray[i] = 0;
	}
	
	
	Print1("\n\nNumber of Cashiers = [%d]\n",noOfCashiers);
	Print1("Number of Goods Loaders = [%d]\n",noOfGoodsLoaders);
	Print1("Number of PrivilegedCustomers = [%d]\n", noOfPrivilageCustomers);
	Print1("Number of Customers = [%d]\n",noOfCustomers);
	Print("Number of Managers = [1]\n");								
	Print1("Number of DepartmentSalesmen = [%d]\n",noOfSalesman*noOfDepartments);
    Print("Items:\n");
    Print("\tNumber - Price\n");	
	for(i=0; i<noOfDepartments*MAX_NO_OF_ITEMS; i++)
	{
		Print2("[%d]    -   $[%d]\n",i ,productData[i].price);
	}
	
	locksAndCVDeclare();
	
	for(i=0; i<noOfCustomers; i++)
	{
		Fork(Customer);
	}
	
	for(i=0; i<noOfDepartments*noOfSalesman; i++)
	{
		Fork(Salesman);
	}
	
	for(i=0; i< noOfCashiers; i++)
	{
		Fork(Cashier);
	}
}

void testcase2()
{
	int i;
	noOfCashiers = 1;
	noOfDepartments = 1;
	noOfSalesman = 1;
	noOfCustomers = 1;
	itemQuantity = MAX_QUANTITY;
	maxMoney = 1;

	initCustomer();
	
	for(i=0; i<noOfDepartments*MAX_NO_OF_ITEMS; i++)
	{
		productData[i].departmentNo = (int) (i/MAX_NO_OF_ITEMS);
		productData[i].aisleId = i;
		productData[i].price = (GetRand()%100)+1;
	}
	for(i=0; i<noOfDepartments*MAX_NO_OF_ITEMS; i++)
	{
		aisleQuantityArray[i] = itemQuantity;
	}
	for(i=0; i<noOfCustomers; i++)
	{
		hasCustomerMoney[i] = FALSE;
	}

	for(i=0; i<noOfSalesman*noOfDepartments; i++)
	{
		itemToRestockArray[i] = -1;
	}
	for(i=0; i<noOfDepartments; i++)
	{
		departmentLineLength[i] = 0;
		complainLineLength[i] = 0;
	}
	for(i=0; i<noOfCashiers; i++)
	{
		cashierLineLength[i] = 0;
		custCashItemArray[i] = -2;
		privilageCashierLineLength[i] = 0;
		totalDrawerCollection[i] = 0;
		cashierSalesArray[i] = 0;
	}
	
	Print1("\n\nNumber of Cashiers = [%d]\n",noOfCashiers);
	Print1("Number of Goods Loaders = [%d]\n",noOfGoodsLoaders);
	Print1("Number of PrivilegedCustomers = [%d]\n", noOfPrivilageCustomers);
	Print1("Number of Customers = [%d]\n",noOfCustomers);
	Print("Number of Managers = [1]\n");								
	Print1("Number of DepartmentSalesmen = [%d]\n",noOfSalesman*noOfDepartments);
    Print("Items:\n");
    Print("\tNumber - Price\n");	
	for(i=0; i<noOfDepartments*MAX_NO_OF_ITEMS; i++)
	{
		Print2("[%d]    -   $[%d]\n",i ,productData[i].price);
	}
	
	locksAndCVDeclare();
	
	for(i=0; i<noOfCustomers; i++)
	{
		Fork(Customer);
	}
	for(i=0; i<noOfDepartments*noOfSalesman; i++)
	{
		Fork(Salesman);
	}
	for(i=0; i< noOfCashiers; i++)
	{
		Fork(Cashier);
	}
	
	for(i=0; i<10000; i++)
    {
        Yield();
    }
	
	Fork(Manager);
}

void testcase3()
{
	int i;
	noOfCashiers = 2;
	noOfDepartments = 1;
	noOfSalesman = 3;
	noOfCustomers = 2;
	itemQuantity = MAX_QUANTITY;
	maxMoney = 1000;
	totalTrollyCount = MAX_TROLLY_COUNT;
	
	initCustomer();
	
	for(i=0; i<noOfDepartments*MAX_NO_OF_ITEMS; i++)
	{
		productData[i].departmentNo = (int) (i/MAX_NO_OF_ITEMS);
		productData[i].aisleId = i;
		productData[i].price = (GetRand()%100)+1;
	}
	
	for(i=0; i<noOfDepartments*MAX_NO_OF_ITEMS; i++)
	{
		aisleQuantityArray[i] = itemQuantity;
	}
	
	for(i=0; i<noOfCustomers; i++)
	{
		hasCustomerMoney[i] = FALSE;
	}

	for(i=0; i<noOfSalesman*noOfDepartments; i++)
	{
		itemToRestockArray[i] = -1;
	}
	for(i=0; i<noOfDepartments; i++)
	{
		departmentLineLength[i] = 0;
		complainLineLength[i] = 0;
	}
	for(i=0; i<noOfCashiers; i++)
	{
		cashierLineLength[i] = 0;
		custCashItemArray[i] = -2;
		privilageCashierLineLength[i] = 0;
		totalDrawerCollection[i] = 0;
		cashierSalesArray[i] = 0;
	}
	
	Print1("\n\nNumber of Cashiers = [%d]\n",noOfCashiers);
	Print1("Number of Goods Loaders = [%d]\n",noOfGoodsLoaders);
	Print1("Number of PrivilegedCustomers = [%d]\n", noOfPrivilageCustomers);
	Print1("Number of Customers = [%d]\n",noOfCustomers);
	Print("Number of Managers = [1]\n");								
	Print1("Number of DepartmentSalesmen = [%d]\n",noOfSalesman*noOfDepartments);
    Print("Items:\n");
    Print("\tNumber - Price\n");	
	for(i=0; i<noOfDepartments*MAX_NO_OF_ITEMS; i++)
	{
		Print2("[%d]    -   $[%d]\n",i ,productData[i].price);
	}
	
	locksAndCVDeclare();
	
	for(i=0; i<noOfCustomers; i++)
	{
		Fork(Customer);
	}
	
	for(i=0; i<noOfDepartments*noOfSalesman; i++)
	{
		Fork(Salesman);
	}
	
	for(i=0; i< noOfCashiers; i++)
	{
		Fork(Cashier);
	}
}

void testcase4()
{
	int i;
	noOfCashiers = 1;
	noOfDepartments = 1;
	noOfSalesman = 1;
	noOfCustomers = 1;
	itemQuantity = MAX_QUANTITY;
	maxMoney = 1000;
	totalTrollyCount = MAX_TROLLY_COUNT;
	
	initCustomer();
	
	for(i=0; i<noOfDepartments*MAX_NO_OF_ITEMS; i++)
	{
		productData[i].departmentNo = (int) (i/MAX_NO_OF_ITEMS);
		productData[i].aisleId = i;
		productData[i].price = (GetRand()%100)+1;
	}
	
	for(i=0; i<noOfDepartments*MAX_NO_OF_ITEMS; i++)
	{
		aisleQuantityArray[i] = itemQuantity;
	}
	
	for(i=0; i<noOfCustomers; i++)
	{
		hasCustomerMoney[i] = FALSE;
	}

	for(i=0; i<noOfSalesman*noOfDepartments; i++)
	{
		itemToRestockArray[i] = -1;
	}
	for(i=0; i<noOfDepartments; i++)
	{
		departmentLineLength[i] = 0;
		complainLineLength[i] = 0;
	}
	for(i=0; i<noOfCashiers; i++)
	{
		cashierLineLength[i] = 0;
		custCashItemArray[i] = -2;
		privilageCashierLineLength[i] = 0;
		totalDrawerCollection[i] = 0;
		cashierSalesArray[i] = 0;
	}
	
	Print1("\n\nNumber of Cashiers = [%d]\n",noOfCashiers);
	Print1("Number of Goods Loaders = [%d]\n",noOfGoodsLoaders);
	Print1("Number of PrivilegedCustomers = [%d]\n", noOfPrivilageCustomers);
	Print1("Number of Customers = [%d]\n",noOfCustomers);
	Print("Number of Managers = [1]\n");								
	Print1("Number of DepartmentSalesmen = [%d]\n",noOfSalesman*noOfDepartments);
    Print("Items:\n");
    Print("\tNumber - Price\n");	
	for(i=0; i<noOfDepartments*MAX_NO_OF_ITEMS; i++)
	{
		Print2("[%d]    -   $[%d]\n",i ,productData[i].price);
	}
	
	locksAndCVDeclare();
	
	for(i=0; i<noOfCustomers; i++)
	{
		Fork(Customer);
	}
	
	for(i=0; i<noOfDepartments*noOfSalesman; i++)
	{
		Fork(Salesman);
	}
	
	for(i=0; i< noOfCashiers; i++)
	{
		Fork(Cashier);
	}
}

void testcase5()
{
	int i;
	noOfCashiers = 3;
	noOfDepartments = 3;
	noOfSalesman = 3;
	noOfCustomers = 45;
	itemQuantity = MAX_QUANTITY;
	maxMoney = 1000;
	totalTrollyCount = MAX_TROLLY_COUNT;
	
	initCustomer();
	
	for(i=0; i<noOfDepartments*MAX_NO_OF_ITEMS; i++)
	{
		productData[i].departmentNo = (int) (i/MAX_NO_OF_ITEMS);
		productData[i].aisleId = i;
		productData[i].price = (GetRand()%100)+1;
	}
	
	for(i=0; i<noOfDepartments*MAX_NO_OF_ITEMS; i++)
	{
		aisleQuantityArray[i] = itemQuantity;
	}
	
	for(i=0; i<noOfCustomers; i++)
	{
		hasCustomerMoney[i] = FALSE;
	}

	for(i=0; i<noOfSalesman*noOfDepartments; i++)
	{
		itemToRestockArray[i] = -1;
	}
	for(i=0; i<noOfDepartments; i++)
	{
		departmentLineLength[i] = 0;
		complainLineLength[i] = 0;
	}
	for(i=0; i<noOfCashiers; i++)
	{
		cashierLineLength[i] = 0;
		custCashItemArray[i] = -2;
		privilageCashierLineLength[i] = 0;
		totalDrawerCollection[i] = 0;
		cashierSalesArray[i] = 0;
	}
	
	
	Print1("\n\nNumber of Cashiers = [%d]\n",noOfCashiers);
	Print1("Number of Goods Loaders = [%d]\n",noOfGoodsLoaders);
	Print1("Number of PrivilegedCustomers = [%d]\n", noOfPrivilageCustomers);
	Print1("Number of Customers = [%d]\n",noOfCustomers);
	Print("Number of Managers = [1]\n");								
	Print1("Number of DepartmentSalesmen = [%d]\n",noOfSalesman*noOfDepartments);
    Print("Items:\n");
    Print("\tNumber - Price\n");	
	for(i=0; i<noOfDepartments*MAX_NO_OF_ITEMS; i++)
	{
		Print2("[%d]    -   $[%d]\n",i ,productData[i].price);
	}
	
	locksAndCVDeclare();
	
	for(i=0; i<noOfCustomers; i++)
	{
		Fork(Customer);
	}
	
	for(i=0; i<noOfDepartments*noOfSalesman; i++)
	{
		Fork(Salesman);
	}
	
	for(i=0; i< noOfCashiers; i++)
	{
		Fork(Cashier);
	}
}

void testcase6()
{
	int i;
	noOfCashiers = 2;
	noOfDepartments = 2;
	noOfSalesman = 2;
	noOfCustomers = 30;
	itemQuantity = MAX_QUANTITY;
	maxMoney = 1000;
	totalTrollyCount = MAX_TROLLY_COUNT;
	
	initCustomer();
	
	for(i=0; i<noOfDepartments*MAX_NO_OF_ITEMS; i++)
	{
		productData[i].departmentNo = (int) (i/MAX_NO_OF_ITEMS);
		productData[i].aisleId = i;
		productData[i].price = (GetRand()%100)+1;
	}
	
	for(i=0; i<noOfDepartments*MAX_NO_OF_ITEMS; i++)
	{
		aisleQuantityArray[i] = itemQuantity;
	}
	
	for(i=0; i<noOfCustomers; i++)
	{
		hasCustomerMoney[i] = FALSE;
	}

	for(i=0; i<noOfSalesman*noOfDepartments; i++)
	{
		itemToRestockArray[i] = -1;
	}
	for(i=0; i<noOfDepartments; i++)
	{
		departmentLineLength[i] = 0;
		complainLineLength[i] = 0;
	}
	for(i=0; i<noOfCashiers; i++)
	{
		cashierLineLength[i] = 0;
		custCashItemArray[i] = -2;
		privilageCashierLineLength[i] = 0;
		totalDrawerCollection[i] = 0;
		cashierSalesArray[i] = 0;
	}
	
	
	Print1("\n\nNumber of Cashiers = [%d]\n",noOfCashiers);
	Print1("Number of Goods Loaders = [%d]\n",noOfGoodsLoaders);
	Print1("Number of PrivilegedCustomers = [%d]\n", noOfPrivilageCustomers);
	Print1("Number of Customers = [%d]\n",noOfCustomers);
	Print("Number of Managers = [1]\n");								
	Print1("Number of DepartmentSalesmen = [%d]\n",noOfSalesman*noOfDepartments);
    Print("Items:\n");
    Print("\tNumber - Price\n");	
	for(i=0; i<noOfDepartments*MAX_NO_OF_ITEMS; i++)
	{
		Print2("[%d]    -   $[%d]\n",i ,productData[i].price);
	}
	
	locksAndCVDeclare();
	
	for(i=0; i<noOfCustomers; i++)
	{
		Fork(Customer);
	}
	
	for(i=0; i<noOfDepartments*noOfSalesman; i++)
	{
		Fork(Salesman);
	}
	
	for(i=0; i< noOfCashiers; i++)
	{
		Fork(Cashier);
	}
}

void testcase7()
{
	int i;
	noOfCashiers = 3;
	noOfDepartments = 3;
	noOfSalesman = 2;
	noOfCustomers = 5;
	itemQuantity = 5;
	maxMoney = 1000;
	totalTrollyCount = MAX_TROLLY_COUNT;
	noOfGoodsLoaders = 3;
	
	initCustomer();
	
	for(i=0; i<noOfDepartments*MAX_NO_OF_ITEMS; i++)
	{
		productData[i].departmentNo = (int) (i/MAX_NO_OF_ITEMS);
		productData[i].aisleId = i;
		productData[i].price = (GetRand()%100)+1;
	}
	
	for(i=0; i<noOfDepartments*MAX_NO_OF_ITEMS; i++)
	{
		aisleQuantityArray[i] = itemQuantity;
	}
	
	for(i=0; i<noOfCustomers; i++)
	{
		hasCustomerMoney[i] = FALSE;
	}

	for(i=0; i<noOfSalesman*noOfDepartments; i++)
	{
		itemToRestockArray[i] = -1;
	}
	for(i=0; i<noOfDepartments; i++)
	{
		departmentLineLength[i] = 0;
		complainLineLength[i] = 0;
	}
	for(i=0; i<noOfCashiers; i++)
	{
		cashierLineLength[i] = 0;
		custCashItemArray[i] = -2;
		privilageCashierLineLength[i] = 0;
		totalDrawerCollection[i] = 0;
		cashierSalesArray[i] = 0;
	}
	
	Print1("\n\nNumber of Cashiers = [%d]\n",noOfCashiers);
	Print1("Number of Goods Loaders = [%d]\n",noOfGoodsLoaders);
	Print1("Number of PrivilegedCustomers = [%d]\n", noOfPrivilageCustomers);
	Print1("Number of Customers = [%d]\n",noOfCustomers);
	Print("Number of Managers = [1]\n");								
	Print1("Number of DepartmentSalesmen = [%d]\n",noOfSalesman*noOfDepartments);
    Print("Items:\n");
    Print("\tNumber - Price\n");	
	for(i=0; i<noOfDepartments*MAX_NO_OF_ITEMS; i++)
	{
		Print2("[%d]    -   $[%d]\n",i ,productData[i].price);
	}
		
	locksAndCVDeclare();

	for(i=0;i<noOfCustomers;i++)
	{
		Fork(Customer);
	}

	for(i=0;i<noOfGoodsLoaders;i++)
	{
		Fork(GoodsLoader);
	}
	
	for(i=0;i<noOfCashiers;i++)
	{
		Fork(Cashier);
	}
	 
	for(i=0;i<(noOfSalesman*noOfDepartments);i++)
	{
		Fork(Salesman);
	}	
	
	Fork(Manager);
}

void testcase8()
{
	int i;
/* Goods Loader don't try to restock an item on a shelf when a Customer is trying to take an item off the shelf */
	noOfCashiers = 2;
	noOfDepartments = 1;
	noOfSalesman = 2;
	noOfGoodsLoaders=1;
	noOfCustomers = 3;
	itemQuantity = 1;
	maxMoney = 1000;
	totalTrollyCount = MAX_TROLLY_COUNT;
	
	initCustomer();
	
	for(i=0; i<noOfDepartments*MAX_NO_OF_ITEMS; i++)
	{
		productData[i].departmentNo = (int) (i/MAX_NO_OF_ITEMS);
		productData[i].aisleId = i;
		productData[i].price = (GetRand()%100)+1;
	}
	for(i=0; i<noOfDepartments*MAX_NO_OF_ITEMS; i++)
	{
		aisleQuantityArray[i] = itemQuantity;
	}
	for(i=0; i<noOfCustomers; i++)
	{
		hasCustomerMoney[i] = FALSE;
	}

	for(i=0; i<noOfSalesman*noOfDepartments; i++)
	{
		itemToRestockArray[i] = -1;
	}
	for(i=0; i<noOfDepartments; i++)
	{
		departmentLineLength[i] = 0;
		complainLineLength[i] = 0;
	}
	for(i=0; i<noOfCashiers; i++)
	{
		cashierLineLength[i] = 0;
		custCashItemArray[i] = -2;
		privilageCashierLineLength[i] = 0;
		totalDrawerCollection[i] = 0;
		cashierSalesArray[i] = 0;
	}

	Print1("\n\nNumber of Cashiers = [%d]\n",noOfCashiers);
	Print1("Number of Goods Loaders = [%d]\n",noOfGoodsLoaders);
	Print1("Number of PrivilegedCustomers = [%d]\n", noOfPrivilageCustomers);
	Print1("Number of Customers = [%d]\n",noOfCustomers);
	Print("Number of Managers = [1]\n");								
	Print1("Number of DepartmentSalesmen = [%d]\n",noOfSalesman*noOfDepartments);
    Print("Items:\n");
    Print("\tNumber - Price\n");	
	for(i=0; i<noOfDepartments*MAX_NO_OF_ITEMS; i++)
	{
		Print2("[%d]    -   $[%d]\n",i ,productData[i].price);
	}

	locksAndCVDeclare();
	for(i=0; i<noOfCustomers; i++)
	{
		Fork(Customer);
	}
	for(i=0;i<noOfGoodsLoaders;i++)
	{
		Fork(GoodsLoader);
	}
		
	for(i=0; i<noOfDepartments*noOfSalesman; i++)
	{
		Fork(Salesman);
	}

}

void testcase9()
{
	int i;
/* Only one Goods Loader enters the stock room at a time. */
	noOfDepartments = 1;
	noOfSalesman = 2;
	noOfGoodsLoaders=2;
	noOfCustomers = 2;
	itemQuantity = 0;
	maxMoney = 1000;
	totalTrollyCount = MAX_TROLLY_COUNT;
	
	initCustomer();
	
	for(i=0; i<noOfDepartments*MAX_NO_OF_ITEMS; i++)
	{
		productData[i].departmentNo = (int) (i/MAX_NO_OF_ITEMS);
		productData[i].aisleId = i;
		productData[i].price = (GetRand()%100)+1;
	}
	for(i=0; i<noOfDepartments*MAX_NO_OF_ITEMS; i++)
	{
		aisleQuantityArray[i] = itemQuantity;
	}
	for(i=0; i<noOfCustomers; i++)
	{
		hasCustomerMoney[i] = FALSE;
	}

	for(i=0; i<noOfSalesman*noOfDepartments; i++)
	{
		itemToRestockArray[i] = -1;
	}
	for(i=0; i<noOfDepartments; i++)
	{
		departmentLineLength[i] = 0;
		complainLineLength[i] = 0;
	}
	for(i=0; i<noOfCashiers; i++)
	{
		cashierLineLength[i] = 0;
		custCashItemArray[i] = -2;
		privilageCashierLineLength[i] = 0;
		totalDrawerCollection[i] = 0;
		cashierSalesArray[i] = 0;
	}
	
	Print1("\n\nNumber of Cashiers = [%d]\n",noOfCashiers);
	Print1("Number of Goods Loaders = [%d]\n",noOfGoodsLoaders);
	Print1("Number of PrivilegedCustomers = [%d]\n", noOfPrivilageCustomers);
	Print1("Number of Customers = [%d]\n",noOfCustomers);
	Print("Number of Managers = [1]\n");								
	Print1("Number of DepartmentSalesmen = [%d]\n",noOfSalesman*noOfDepartments);
    Print("Items:\n");
    Print("\tNumber - Price\n");	
	for(i=0; i<noOfDepartments*MAX_NO_OF_ITEMS; i++)
	{
		Print2("[%d]    -   $[%d]\n",i ,productData[i].price);
	}
	
	locksAndCVDeclare();
	
	for(i=0; i<noOfCustomers; i++)
	{
		Fork(Customer);
	}
	for(i=0;i<noOfGoodsLoaders;i++)
	{
		Fork(GoodsLoader);
	}
		
	for(i=0; i<noOfDepartments*noOfSalesman; i++)
	{
		Fork(Salesman);
	}

}

void testcase10()
{
	int i;
/* Customers wait for items to be restocked - when told by the Department Salesman */
	noOfDepartments = 1;
	noOfSalesman = 1;
	noOfGoodsLoaders=1;
	noOfCustomers = 1;
	itemQuantity = 0;
	maxMoney = 1000;
	totalTrollyCount = MAX_TROLLY_COUNT;
	
	initCustomer();
	
	for(i=0; i<noOfDepartments*MAX_NO_OF_ITEMS; i++)
	{
		productData[i].departmentNo = (int) (i/MAX_NO_OF_ITEMS);
		productData[i].aisleId = i;
		productData[i].price = (GetRand()%100)+1;
	}
	for(i=0; i<noOfDepartments*MAX_NO_OF_ITEMS; i++)
	{
		aisleQuantityArray[i] = itemQuantity;
	}
	for(i=0; i<noOfCustomers; i++)
	{
		hasCustomerMoney[i] = FALSE;
	}

	for(i=0; i<noOfSalesman*noOfDepartments; i++)
	{
		itemToRestockArray[i] = -1;
	}
	for(i=0; i<noOfDepartments; i++)
	{
		departmentLineLength[i] = 0;
		complainLineLength[i] = 0;
	}
	for(i=0; i<noOfCashiers; i++)
	{
		cashierLineLength[i] = 0;
		custCashItemArray[i] = -2;
		privilageCashierLineLength[i] = 0;
		totalDrawerCollection[i] = 0;
		cashierSalesArray[i] = 0;
	}
	
	Print1("\n\nNumber of Cashiers = [%d]\n",noOfCashiers);
	Print1("Number of Goods Loaders = [%d]\n",noOfGoodsLoaders);
	Print1("Number of PrivilegedCustomers = [%d]\n", noOfPrivilageCustomers);
	Print1("Number of Customers = [%d]\n",noOfCustomers);
	Print("Number of Managers = [1]\n");								
	Print1("Number of DepartmentSalesmen = [%d]\n",noOfSalesman*noOfDepartments);
    Print("Items:\n");
    Print("\tNumber - Price\n");	
	for(i=0; i<noOfDepartments*MAX_NO_OF_ITEMS; i++)
	{
		Print2("[%d]    -   $[%d]\n",i ,productData[i].price);
	}
	
	locksAndCVDeclare();
	
	for(i=0; i<noOfCustomers; i++)
	{
		Fork(Customer);
	}
	for(i=0;i<noOfGoodsLoaders;i++)
	{
		Fork(GoodsLoader);
	}
		
	for(i=0; i<noOfDepartments*noOfSalesman; i++)
	{
		Fork(Salesman);
	}

}

void testcase11()
{
	int i;
	noOfCashiers = 3;
	noOfDepartments = 3;
	noOfSalesman = 2;
	noOfCustomers = 32;
	itemQuantity = 5;
	maxMoney = 1000;
	totalTrollyCount = MAX_TROLLY_COUNT;
	noOfGoodsLoaders = 3;
	
	initCustomer();
	
	for(i=0; i<noOfDepartments*MAX_NO_OF_ITEMS; i++)
	{
		productData[i].departmentNo = (int) (i/MAX_NO_OF_ITEMS);
		productData[i].aisleId = i;
		productData[i].price = (GetRand()%100)+1;
	}
	
	for(i=0; i<noOfDepartments*MAX_NO_OF_ITEMS; i++)
	{
		aisleQuantityArray[i] = itemQuantity;
	}
	
	for(i=0; i<noOfCustomers; i++)
	{
		hasCustomerMoney[i] = FALSE;
	}

	for(i=0; i<noOfSalesman*noOfDepartments; i++)
	{
		itemToRestockArray[i] = -1;
	}
	for(i=0; i<noOfDepartments; i++)
	{
		departmentLineLength[i] = 0;
		complainLineLength[i] = 0;
	}
	for(i=0; i<noOfCashiers; i++)
	{
		cashierLineLength[i] = 0;
		custCashItemArray[i] = -2;
		privilageCashierLineLength[i] = 0;
		totalDrawerCollection[i] = 0;
		cashierSalesArray[i] = 0;
	}
	for(i=0; i<MAX_NO_OF_ITEMS*noOfDepartments; i++)
	{
		cancelledItemArray[i] = 0;
	}
	Print1("\n\nNumber of Cashiers = [%d]\n",noOfCashiers);
	Print1("Number of Goods Loaders = [%d]\n",noOfGoodsLoaders);
	Print1("Number of PrivilegedCustomers = [%d]\n", noOfPrivilageCustomers);
	Print1("Number of Customers = [%d]\n",noOfCustomers);
	Print("Number of Managers = [1]\n");								
	Print1("Number of DepartmentSalesmen = [%d]\n",noOfSalesman*noOfDepartments);
    Print("Items:\n");
    Print("\tNumber - Price\n");	
	for(i=0; i<noOfDepartments*MAX_NO_OF_ITEMS; i++)
	{
		Print2("[%d]    -   $[%d]\n",i ,productData[i].price);
	}
	
	locksAndCVDeclare();
	

	for(i=0;i<noOfCustomers;i++)
	{
		/*Print("-------------------------------forkig customer---------");*/
		Fork(Customer);
	}

	for(i=0;i<noOfGoodsLoaders;i++)
	{
		/*Print("-------------------------------goodsloader forking---------");*/
		Fork(GoodsLoader);
	}
	
	for(i=0;i<noOfCashiers;i++)
	{
		/*Print("-------------------------------cashiers customer---------");*/
		Fork(Cashier);
	}
	 
	for(i=0;i<(noOfSalesman*noOfDepartments);i++)
	{
		/*Print("-------------------------------salesman forking---------");*/
		Fork(Salesman);
	}	
	
	Fork(Manager);
}

void testcase12()
{
	/*getUserData();*/
}



/*====================================================================================
 First function Call "Problem2()" Part 2 Simulation: Assign-1
==================================================================================== */


void main()
{
	int testcase;
	
	printLock = CreateLock("printLock", sizeof("printLock"));
	custThreadCntrLock = CreateLock("custThreadCntrLock", sizeof("custThreadCntrLock"));
	cashierThreadCounterLock = CreateLock("cashierThreadCounterLock", sizeof("cashierThreadCounterLock"));
	salesmanThreadCounterLock = CreateLock("salesmanThreadCounterLock", sizeof("salesmanThreadCounterLock"));
	glThreadCntrLock = CreateLock("glThreadCntrLock", sizeof("glThreadCntrLock"));
	
	Acquire(printLock);
	Print("\n Test Case Menu: Please enter a value from the following for the appropriate test case\n");
	Print(" 1. Customers always take the shortest line, but no 2 customers ever choose the same shortest line at the same time\n");
	Print(" 2. Managers can only talk to one Cashier, or one Customer, at a time\n");
	Print(" 3. Customers do not leave until they are given their receipt by the Cashier. The Cashier does not start on another customer until they know that the last Customer has left their area\n");
	Print(" 4. Cashier scans the items till the trolly is empty\n");
	Print(" 5. Cashiers are sent on break by the Manager randomly\n");
	Print(" 6. Managers get Cashiers off their break when lines have 3, or more, Customers\n");
	Print(" 7. Total sales never suffers from a race condition\n"); 
	Print(" 8. Goods Loader don't try to restock an item on a shelf when a Customer is trying to take an item off the shelf.\n"); 
	Print(" 9. Only one Goods Loader enters the stock room at a time\n"); 
	Print("10. Customers wait for items to be restocked - when told by the Department Salesman.\n"); 
	Print("11. General simulation with minimum requirements : \n"); 
	Print("12. General Simulation with user input values : \n"); 

	Print("\nEnter Test Case Number:  ");
	Release(printLock);
	/*testcase=Scan();*/
	 testcase = 11; 
	 Yield();
	switch(testcase)
	{
	case 1:
		testcase1();
		break;
	case 2:
		testcase2();
		break;
	case 3:
		testcase3();
		break;
	case 4:
		testcase4();
		break;
	case 5:
		testcase5();
		break;
	case 6:
		testcase6();
		break;
	case 7:
		testcase7();
		break;
	case 8:
		testcase8();
		break;
	case 9:
		testcase9();
		break;
	case 10:
		testcase10();
		break;
	case 11:
		testcase11();
		break;
	case 12:
		testcase12();
		break;
	default:
		Acquire(printLock);
		Print("\n\n\tPlease enter a valid test case number for nachos command\n\n\n");
		Release(printLock);
	}
	Exit(0);
}
