/*
 * server.cc
 *
 *  Created on: Jun 4, 2013
 *      Author: cvora
 */

#include <stdio.h>
#include "server.h"

#if 1

// Send Message to notify the client for RPC

void SendMessage(int machineId,int mailBoxId,char* data)
{
    PacketHeader outPktHdr, inPktHdr;
    MailHeader outMailHdr, inMailHdr;

    // construct packet, mail header for original message
    // To: destination machine, mailbox 0
    // From: our machine, reply to: mailbox 1
    outPktHdr.to = machineId;
    outMailHdr.to = mailBoxId;
    outMailHdr.from = 0;
    outMailHdr.length = strlen(data) + 1;

    // Send the first message
    bool success = postOffice->Send(outPktHdr, outMailHdr, data);

    if ( !success )
    {
      printf("The postOffice Send failed. You must not have the other Nachos running. Terminating Nachos.\n");
      interrupt->Halt();
    }
}

locksForServer::locksForServer(char* debugName)
{
	name = debugName;
	serverLockStatusObj = SERVERLOCKFREE;
	waitingQueueOfLocks = new List();
	ownerMachineId = -1;
	ownerMailBoxId = -1;
}

locksForServer::~locksForServer()
{
	delete waitingQueueOfLocks;
}


bool locksForServer::isHeldByCurrentClient(int machineId,int mailBoxId)
{
	if((machineId == ownerMachineId) && (mailBoxId == ownerMailBoxId))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void locksForServer::Acquire(int lockId,int machineId,int mailBoxId)
{
	if(isHeldByCurrentClient(machineId,mailBoxId))
	{
		char* buf = new char[MaxMailSize];
		sprintf(buf,"%s","1");
		printf("LOCK %d ALREADY ACQUIRED MESSAGE SENT TO machineId = %d mailBoxId = %d \n",lockId,
				machineId,mailBoxId);
		SendMessage(machineId,mailBoxId,buf);
		delete [] buf;
		return;
	}
	if(serverLockStatusObj == SERVERLOCKFREE)
	{
		serverLockStatusObj = SERVERLOCKBUSY;
		ownerMachineId = machineId;
		ownerMailBoxId = mailBoxId;
		char* buf = new char[MaxMailSize];
		sprintf(buf,"%s","1");
		printf("LOCK %d ACQUIRED MESSAGE SENT TO machineId = %d mailBoxId = %d \n",
				lockId,machineId,mailBoxId);
		SendMessage(machineId,mailBoxId,buf);
		delete [] buf;
	}
	else
	{
		clientReply *clientRply = new clientReply();
		sprintf(clientRply->data,"%s","1");
		clientRply->outPktHdr.to = machineId;
		clientRply->outMailHdr.to = mailBoxId;
		clientRply->outMailHdr.from = 0;
		clientRply->outMailHdr.length = strlen(clientRply->data + 1);
		printf("LOCK %d CURRENTLY ACQUIRED BY machineId = %d, mailBoxId = %d SO WAITING IN LOCK QUEUE MESSAGE SENT TO "
				"machineId = %d mailBoxId = %d \n",lockId,
				ownerMachineId,ownerMailBoxId,machineId,mailBoxId);
		waitingQueueOfLocks->Append((void*)clientRply);
	}

	return;
}

bool locksForServer::Release(int lockId,int machineId,int mailBoxId)
{
	if(!isHeldByCurrentClient(machineId,mailBoxId))
	{
		return false;
	}

	clientReply* clientRply = (clientReply*)waitingQueueOfLocks->Remove();
	if(clientRply!=NULL)
	{
		ownerMachineId = clientRply->outPktHdr.to;
		ownerMailBoxId = clientRply->outMailHdr.to;
		printf("LOCK %d RELEASED MESSAGE SENT TO  : machineId = %d mailBoxId = %d \n",lockId,
				clientRply->outPktHdr.to,clientRply->outMailHdr.to);
		SendMessage(clientRply->outPktHdr.to,clientRply->outMailHdr.to,clientRply->data);
		delete clientRply;
	}
	else
	{
		printf("LOCK %d RELEASED TO NO ONE AND IS FREE TO BE ACQUIRED MESSAGE SENT TO !!!!!! \n",lockId);
		serverLockStatusObj = SERVERLOCKFREE;
		ownerMachineId = -1;
		ownerMailBoxId = -1;
	}
	return true;
}

cvsForServer::cvsForServer(char* debugName)
{
	waitingCVOnLock = -1;
	name = debugName;
	waitingQueueOfCVS = new List();
}

cvsForServer::~cvsForServer()
{
	delete waitingQueueOfCVS;
}

void cvsForServer::Signal(int machineId,int mailBoxId,int conditionId,int conditionLockId)
{
	if(waitingCVOnLock != conditionLockId)
	{
		char* response = new char[100];
		sprintf(response,"%s","-1");
		printf(" CONDITION %d SIGNAL FAIL INVALID CONDITION LOCK ID %d PASSED MESSAGE SENT TO  machineId = %d mailBoxId = %d \n",
				conditionId,conditionLockId,machineId,mailBoxId);
		SendMessage(machineId,mailBoxId,response);
		delete [] response;
		return;
	}

	if(waitingQueueOfCVS->IsEmpty())
	{
		char* response = new char[100];
		sprintf(response,"%s","-1");
		printf(" CONDITION %d SIGNAL FAIL NO ONE WAITING FOR THIS CONDITION MESSAGE SENT TO  machineId = %d mailBoxId = %d \n",
				conditionId,machineId,mailBoxId);
		SendMessage(machineId,mailBoxId,response);
		delete [] response;
		waitingCVOnLock = -1;
		return;
	}

	clientReply* clientRply= (clientReply*)waitingQueueOfCVS->Remove();
	if(clientRply != NULL)
	{
		printf("CONDITION %d SIGNAL WITH LOCK %d TO machineId = %d mailBoxId = %d \n",
				conditionId,conditionLockId,clientRply->outPktHdr.to,clientRply->outMailHdr.to);
		serverLockTable.serverLocksArray[conditionLockId].serverLock->Acquire(conditionLockId,clientRply->outPktHdr.to,
			clientRply->outMailHdr.to);
	}

	char *response = new char[100];
	sprintf(response,"%s","1");
	printf("CONDITION %d SIGNAL WITH LOCK %d DONE MESSAGE SENT TO  "
			"machineId = %d mailBoxId = %d \n",
			conditionId,conditionLockId,machineId,mailBoxId);
	SendMessage(machineId,mailBoxId,response);
	delete [] response;
	return;
}

void cvsForServer :: Wait(int machineId, int mailBoxId,int conditionId, int conditionLockId)
{

	if(waitingCVOnLock ==-1)
	{
		//no one is waiting on the lock
		waitingCVOnLock= conditionLockId;
	}

	if(waitingCVOnLock != conditionLockId)
	{
		char *response = new char[100];
		sprintf(response,"%s","-1");
		printf(" CONDITION %d WAIT FAIL INVALID CONDITION LOCK ID %d PASSED MESSAGE SENT TO  machineId = %d mailBoxId = %d \n",
				conditionId,conditionLockId,machineId,mailBoxId);
		SendMessage(machineId,mailBoxId,response);
		delete [] response;
		return;
	}


	//create message
	clientReply* clientRply = new clientReply();

	//ready the reply message
	sprintf(clientRply->data,"%s","1");
	clientRply->outPktHdr.to = machineId;
	clientRply->outMailHdr.to= mailBoxId;
	clientRply->outMailHdr.from= 0;
	clientRply->outMailHdr.length=strlen((clientRply->data)+1);

	printf("CONDITION %d WITH LOCK %d APPENDING TO CONDITION WAIT QUEUE CONDITION machineId = %d mailBoxId = %d \n",
			conditionId,conditionLockId,machineId, mailBoxId);
	//now append clientMsg to wait queue
	waitingQueueOfCVS-> Append((void*)clientRply);

	printf("CONDITION %d WITH LOCK %d CONDITION WAIT RELEASING LOCK  machineId = %d mailBoxId = %d \n",
			conditionId,conditionLockId,machineId, mailBoxId);
	//release
	serverLockTable.serverLocksArray[conditionLockId].serverLock->Release(conditionLockId,machineId, mailBoxId);

	//TODO: whether to do acquire here or not..as aft release we need to acquire lock
	//as we come out of wait..
	//we need a lock to proceed
}


void cvsForServer :: Broadcast(int machineId, int mailBoxId,int conditionId, int conditionLockId)
{
	if(waitingCVOnLock != conditionLockId)
	{
		//Illegal lock , so cannot signa..
		char *response = new char[100];
		sprintf(response,"%s","-1");
		printf(" CONDITION %d BROADCAST FAIL INVALID CONDITION LOCK ID %d PASSED MESSAGE SENT TO  machineId = %d mailBoxId = %d \n",
				conditionId,conditionLockId,machineId,mailBoxId);
		SendMessage(machineId,mailBoxId,response);
		delete [] response;
		return;
	}

	if(waitingQueueOfCVS -> IsEmpty())
	{
		//send signal even if queie is empty
		printf(" CONDITION %d BROADCAST FAIL NO ONE WAITING FOR THIS CONDITION MESSAGE SENT TO  machineId = %d mailBoxId = %d \n",
				conditionId,machineId,mailBoxId);
		char *response = new char[100];
		sprintf(response,"%s","1");
		SendMessage(machineId,mailBoxId,response);
		delete [] response;
		waitingCVOnLock = -1;
		return;
	}

	//now if we are here we have to broad cast to all waiting clients on CV
	//so for this remove clientMsg from waitQueue one by one and
	//acquire lock..so this has to be done in a while loop

	printf(" BROADCASTING ALL THE CLIENTS WAITING FOR THIS CONDITION  \n");
	clientReply* clientRply;
	while(!(waitingQueueOfCVS -> IsEmpty()))
	{
		clientRply= (clientReply*)waitingQueueOfCVS->Remove();
		printf(" REMOVING CLIENT  machineId = %d mailBoxId = %d\n",clientRply->outPktHdr.to,clientRply->outMailHdr.to);
		serverLockTable.serverLocksArray[conditionLockId].serverLock->
		Acquire(conditionLockId,clientRply->outPktHdr.to, clientRply->outMailHdr.to);
	}
	if(waitingQueueOfCVS -> IsEmpty())
	{
		waitingCVOnLock = -1;
	}

	char *response = new char[100];
	sprintf(response,"%s","1");
	printf(" CONDITION %d WITH LOCK %d BROADCAST DONE MESSAGE SENT TO machineId = %d mailBoxId = %d \n",
			conditionId,conditionLockId,machineId, mailBoxId);
	SendMessage(machineId,mailBoxId,response);
	delete [] response;
	return;
}

//...........................start of syscalls................................................................................
void CreateLock(int machineId, int mailBoxId, char * name)
{
	int index;
	// step 1 : check whether lock was already created or not..if yes then increement lockcounter
	//and send msg

	for(int i=0; i < MAX_LOCKS; i++)
	{
		if(serverLockTable.serverLocksArray[i].valid == true
				&& strcmp(serverLockTable.serverLocksArray[i].serverLock->name,name)==0)
		{
			(serverLockTable.serverLocksArray[i].sLockCounter)++;
			char *buf = new char[MaxMailSize];
			memset(buf,0,MaxMailSize);
			sprintf(buf,"%d",i); //since lock is already crated give the lock id in message
			printf("LOCK %d ALREADY CREATED MESSAGE SENT TO machineId = %d mailBoxId = %d \n",i,machineId,mailBoxId);
			SendMessage(machineId,mailBoxId,buf);
			delete [] buf;
			return;
		}

	}
//check to find whether there is place to create new lock
	if((index = serverLockTable.sLockBitMap->Find()) == -1)
	{
		char *buf = new char[MaxMailSize];
		memset(buf,0,MaxMailSize);
		sprintf(buf,"%s","-1");
		printf("NEW LOCK CANT BE CREATED. LOCK MAX LIMIT REACHED MESSAGE SENT TO  machineId = %d mailBoxId = %d \n",machineId,mailBoxId);
		SendMessage(machineId,mailBoxId,buf);
		delete [] buf;
		return;
	}
	else
	{
		serverLockTable.serverLocksArray[index].serverLock = new locksForServer(name);
		serverLockTable.serverLocksArray[index].valid = true;
		(serverLockTable.serverLocksArray[index].sLockCounter)++;
		char *buf = new char[MaxMailSize];
		memset(buf,0,MaxMailSize);
		sprintf(buf,"%d",index);
		printf("NEW LOCK %d CREATED MESSAGE SENT TO machineId = %d mailBoxId = %d \n",index,machineId,mailBoxId);
		SendMessage(machineId,mailBoxId,buf);
		delete [] buf;
		return;

	}

}



//DESTROY LOCK FOR SERVER

void DestroyLock(int machineId, int mailBoxId, int lockId)
{

	if(lockId< 0 || lockId > MAX_LOCKS)
	{
		printf("Invalid lockId\n");
		char *buf = new char[MaxMailSize];
		memset(buf,0,MaxMailSize);
		sprintf(buf,"%s","-1");
		printf("LOCK %d CANT BE DESTROYED AS INVALID LOCK MESSAGE SENT TO machineId = %d mailBoxId = %d \n",lockId,machineId,mailBoxId);
		SendMessage(machineId,mailBoxId,buf);
		delete [] buf;
		return;
	}


	if(serverLockTable.serverLocksArray[lockId].valid == false)
	{
		printf("LOCK %d CANT BE DESTROYED AS ALREADY DESTROYED MESSAGE SENT TO machineId = %d mailBoxId = %d \n",lockId,machineId,mailBoxId);
		char *buf = new char[MaxMailSize];
		memset(buf,0,MaxMailSize);
		sprintf(buf,"%s","-1");
		SendMessage(machineId,mailBoxId,buf);
		delete [] buf;
		return;
	}


	//if we are here means lock is valid..so can be destroyed

	(serverLockTable.serverLocksArray[lockId].sLockCounter)--;	// decrement the lock counter
	//which is to deleted
	//lock can be destroyed only if the server lock counter is 0
	if((serverLockTable.serverLocksArray[lockId].sLockCounter)==0)
	{
		serverLockTable.sLockBitMap->Clear(lockId);
		delete serverLockTable.serverLocksArray[lockId].serverLock;
		serverLockTable.serverLocksArray[lockId].valid = false;
		//(serverLockTable.serverLocksArray[lockId].sLockCounter)=0;
		char *buf = new char[MaxMailSize];
		memset(buf,0,MaxMailSize);
		sprintf(buf,"%s","1");
		printf("LOCK %d DESTROYED MESSAGE SENT TO machineId = %d mailBoxId = %d \n",lockId,machineId,mailBoxId);
		SendMessage(machineId,mailBoxId,buf);
		delete [] buf;
		return;
	}
	else
	{
		printf("LOCK %d CANT BE DESTROYED AS USED BY OTHERS MESSAGE SENT TO machineId = %d mailBoxId = %d \n",
				lockId,machineId,mailBoxId);
		char *buf = new char[MaxMailSize];
		memset(buf,0,MaxMailSize);
		sprintf(buf,"%s","-1");
		SendMessage(machineId,mailBoxId,buf);
		delete [] buf;
		return;
	}
}

void AcquireMethod(int machineId,int mailBoxId,int lockId)
{
	//check lock id range
	if(lockId<0||lockId>=MAX_LOCKS)
	{
		printf("LOCK %d CANT BE ACQUIRED AS INVALID LOCK MESSAGE SENT TO machineId = %d mailBoxId = %d \n",
				lockId,machineId,mailBoxId );
		char *data = new char[5];
		sprintf(data,"%s","-1");
		SendMessage(machineId, mailBoxId,data);
		delete[] data;
		return;
	}

	if(serverLockTable.serverLocksArray[lockId].valid==false)
	{
		printf("LOCK %d CANT BE ACQUIRED AS INVALID LOCK MESSAGE SENT TO machineId = %d mailBoxId = %d \n",
				lockId,machineId,mailBoxId );
		char *data = new char[5];
		sprintf(data,"%s","-1");
		SendMessage(machineId, mailBoxId,data);
		delete[] data;
		return;
	}
	serverLockTable.serverLocksArray[lockId].serverLock->Acquire(lockId,machineId,mailBoxId);

}

void ReleaseMethod(int machineId,int mailBoxId,int lockId)
{
	bool reply;
		//check lock id range
	if(lockId<0||lockId>=MAX_LOCKS)
	{
		printf("LOCK %d CANT BE RELEASED AS INVALID LOCK MESSAGE SENT TO machineId = %d mailBoxId = %d \n",
				lockId,machineId,mailBoxId );
		char *data = new char[5];
		sprintf(data,"%s","-1");
		SendMessage(machineId, mailBoxId,data);
		delete[] data;
		return;
	}

	//Check if a lock with the given id was already created
	if(serverLockTable.serverLocksArray[lockId].valid==false){
		printf("LOCK %d CANT BE RELEASED AS INVALID LOCK MESSAGE SENT TO machineId = %d mailBoxId = %d \n",
				lockId,machineId,mailBoxId );
		char *data = new char[5];
		sprintf(data,"%s","-1");
		SendMessage(machineId, mailBoxId,data);
		delete[] data;
		return;
	}

	reply = serverLockTable.serverLocksArray[lockId].serverLock->Release(lockId,machineId,mailBoxId);
	//returns true if lock can be released
	//Reply message will be sent by the Release method
	if(reply==false)
	{
		char *buf = new char[MaxMailSize];
		sprintf(buf,"%s", "-1");
		printf("LOCK %d CANT BE RELEASED AS YOU ARE NOT CURRENT OWNER OF LOCK MESSAGE SENT TO machineId = %d mailBoxId = %d \n",
				lockId,machineId,mailBoxId);
		SendMessage(machineId, mailBoxId, buf);
		delete[] buf;
		return;
	}
	else
	{
		char *buf = new char[MaxMailSize];
		sprintf(buf,"%s", "1");
		SendMessage(machineId, mailBoxId, buf);
		delete[] buf;
	}
}

void CreateCondition(int machineId, int mailBoxId, char * name)
{
	int index;
// step 1 : check whether lock was already created or not..if yes then increement lockcounter and send msg

	for(int i=0; i < MAX_CVS; i++)
	{
		if(serverCVTable.serverCVSArray[i].valid == true &&
				strcmp(serverCVTable.serverCVSArray[i].serverCV->name,name)==0)
		{
			(serverCVTable.serverCVSArray[i].sCVCounter)++;
			char *buf = new char[MaxMailSize];
			memset(buf,0,MaxMailSize);
			sprintf(buf,"%d",i); //since lock is already crated give the lock id in message
			printf("CONDITION %d ALREADY CREATED MESSAGE SENT TO machineId = %d mailBoxId = %d \n",i,machineId,mailBoxId);
			SendMessage(machineId,mailBoxId,buf);
			delete [] buf;
			return;
		}

	}
//check to find whether there is place to create new lock
	if((index=serverCVTable.sCVBitMap->Find())==-1)
	{
		printf("NEW CONDITION CANT BE CREATED. CONDITION MAX LIMIT REACHED MESSAGE SENT TO  "
				"machineId = %d mailBoxId = %d \n",machineId,mailBoxId);
		char *buf = new char[MaxMailSize];
		memset(buf,0,MaxMailSize);
		sprintf(buf,"%s","-1");
		SendMessage(machineId,mailBoxId,buf);
		delete [] buf;
		return;
	}
	else
	{
		serverCVTable.serverCVSArray[index].serverCV = new cvsForServer(name);
		serverCVTable.serverCVSArray[index].valid = true;
		(serverCVTable.serverCVSArray[index].sCVCounter)++;
		char *buf = new char[MaxMailSize];
		memset(buf,0,MaxMailSize);
		sprintf(buf,"%d",index);
		printf("NEW CONDITION %d CREATED MESSAGE SENT TO machineId = %d mailBoxId = %d \n",index,machineId,mailBoxId);
		SendMessage(machineId,mailBoxId,buf);
		delete [] buf;
		return;
	}

}


void DestroyCondition(int machineId, int mailBoxId, int cvId)
{
	if(cvId< 0 || cvId > MAX_CVS)
	{

		printf("CONDITION %d CANT BE DESTROYED AS INVALID CONDITION MESSAGE SENT TO machineId = %d mailBoxId = %d \n",
				cvId,machineId,mailBoxId);
		char *buf = new char[MaxMailSize];
		memset(buf,0,MaxMailSize);
		sprintf(buf,"%s","-1");
		SendMessage(machineId,mailBoxId,buf);// TODO : juzz send -1
		delete [] buf;
		return;
	}


	if(serverCVTable.serverCVSArray[cvId].valid == false)
	{
		printf("CONDITION %d CANT BE DESTROYED AS ALREADY DESTROYED MESSAGE SENT TO machineId = %d mailBoxId = %d \n",
				cvId,machineId,mailBoxId);
		char *buf = new char[MaxMailSize];
		memset(buf,0,MaxMailSize);
		sprintf(buf,"%s","-1");
		SendMessage(machineId,mailBoxId,buf);
		delete [] buf;
		return;
	}


	//if we are here means CV is valid..so can be destroyed

	(serverCVTable.serverCVSArray[cvId].sCVCounter)--;	// decrement the CV counter which is to deleted
	//CV can be destroyed only if the server CV counter is 0
	if((serverCVTable.serverCVSArray[cvId].sCVCounter)==0)
	{
		serverCVTable.sCVBitMap->Clear(cvId);
		delete serverCVTable.serverCVSArray[cvId].serverCV;
		serverCVTable.serverCVSArray[cvId].valid = false;
		char *buf = new char[MaxMailSize];
		memset(buf,0,MaxMailSize);
		sprintf(buf,"%s","1");
		printf("CONDITION %d DESTROYED MESSAGE SENT TO machineId = %d mailBoxId = %d \n",cvId,machineId,mailBoxId);
		SendMessage(machineId,mailBoxId,buf);
		delete [] buf;
		return;
	}
	else
	{
		printf("CONDITION %d CANT BE DESTROYED AS USED BY OTHERS MESSAGE SENT TO machineId = %d mailBoxId = %d \n",
			cvId,machineId,mailBoxId);
		char *buf = new char[MaxMailSize];
		memset(buf,0,MaxMailSize);
		sprintf(buf,"%s","-1");
		SendMessage(machineId,mailBoxId,buf);
		delete [] buf;
		return;
	}
}

void WaitMethod(int machineId, int mailBoxId, int lockId, int cvId)
{
	if(lockId<0 || lockId>MAX_LOCKS || cvId<0 || cvId> MAX_CVS)
	{
		printf("CONDITION WAIT FAILED AS LOCK %d OR CONDITION %d ARE INVALID MESSAGE SENT TO machineId = %d mailBoxId = %d \n",
				lockId,cvId,machineId,mailBoxId);
		char *buf = new char[MaxMailSize];
		memset(buf,0,MaxMailSize);
		sprintf(buf,"%s","-1");
		SendMessage(machineId,mailBoxId,buf);
		delete [] buf;
		return;
	}


	if(serverLockTable.serverLocksArray[lockId].valid == false || serverCVTable.serverCVSArray[cvId].valid
			== false)
	{
		printf("CONDITION WAIT FAILED AS LOCK %d OR CONDITION %d ARE INVALID MESSAGE SENT TO machineId = %d mailBoxId = %d \n",
				lockId,cvId,machineId,mailBoxId);
		char *buf = new char[MaxMailSize];
		memset(buf,0,MaxMailSize);
		sprintf(buf,"%s","-1");
		SendMessage(machineId,mailBoxId,buf);
		delete [] buf;
		return;
	}

	//If we are here then the lock and CV both are valid
	serverCVTable.serverCVSArray[cvId].serverCV->Wait(machineId,mailBoxId,cvId,lockId);
	//TODO : see this declaration ie order of parameters of serverWaitfunction
}



void SignalMethod(int machineId, int mailBoxId, int lockId, int cvId)
{
	if(lockId<0 || lockId>MAX_LOCKS || cvId<0 || cvId> MAX_CVS)
	{
		printf("CONDITION SIGNAL FAILED AS LOCK %d OR CONDITION %d ARE INVALID MESSAGE SENT TO machineId = %d mailBoxId = %d \n",
					lockId,cvId,machineId,mailBoxId);
		char *buf = new char[MaxMailSize];
		memset(buf,0,MaxMailSize);
		sprintf(buf,"%s","-1");
		SendMessage(machineId,mailBoxId,buf);
		delete [] buf;
		return;
	}


	if(serverLockTable.serverLocksArray[lockId].valid == false || serverCVTable.serverCVSArray[cvId].valid
			== false)
	{
		printf("CONDITION SIGNAL FAILED AS LOCK %d OR CONDITION %d ARE INVALID MESSAGE SENT TO machineId = %d mailBoxId = %d \n",
					lockId,cvId,machineId,mailBoxId);
		char *buf = new char[MaxMailSize];
		memset(buf,0,MaxMailSize);
		sprintf(buf,"%s","-1");
		SendMessage(machineId,mailBoxId,buf);
		delete [] buf;
		return;
	}

	//If we are here then the lock and CV both are valid
	serverCVTable.serverCVSArray[cvId].serverCV->Signal(machineId,mailBoxId,cvId,lockId);
	//TODO : see this declaration ie order of parameters of serverWaitfunction
}


void BroadcastMethod(int machineId, int mailBoxId, int lockId, int cvId)
{
	if(lockId<0 || lockId>MAX_LOCKS || cvId<0 || cvId> MAX_CVS)
	{
		printf("CONDITION BROADCAST FAILED AS LOCK %d OR CONDITION %d ARE INVALID MESSAGE SENT TO machineId = %d mailBoxId = %d \n",
					lockId,cvId,machineId,mailBoxId);
		char *buf = new char[MaxMailSize];
		memset(buf,0,MaxMailSize);
		sprintf(buf,"%s","-1");
		SendMessage(machineId,mailBoxId,buf);
		delete [] buf;
		return;
	}


	if(serverLockTable.serverLocksArray[lockId].valid == false || serverCVTable.serverCVSArray[cvId].valid
			== false)
	{
		printf("CONDITION BROADCAST FAILED AS LOCK %d OR CONDITION %d ARE INVALID MESSAGE SENT TO machineId = %d mailBoxId = %d \n",
					lockId,cvId,machineId,mailBoxId);
		char *buf = new char[MaxMailSize];
		memset(buf,0,MaxMailSize);
		sprintf(buf,"%s","-1");
		SendMessage(machineId,mailBoxId,buf);
		delete [] buf;
		return;
	}

	//If we are here then the lock and CV both are valid
	serverCVTable.serverCVSArray[cvId].serverCV->Broadcast(machineId,mailBoxId,cvId,lockId);
	//TODO : see this declaration ie order of parameters of serverWaitfunction
}

#endif


//Server function
void Server()
{
#if 1

	PacketHeader outPacketHdr, inPacketHdr;
	MailHeader outMailHdr, inMailHdr;
	char buffer[MaxMailSize];
	int messageType;
	char *name;
	int lockId, cvId, mvId,arrayIndex;
	int setValue;



	while(1)
	{
		printf("Server Loop Started !!!!!!!!!!! \n");

		postOffice->Receive(0,&inPacketHdr,&inMailHdr,buffer);
		printf("Received some data... Will Parse the data \n");
		outPacketHdr.to = inMailHdr.from;
		outMailHdr.to =inMailHdr.from;
		fflush(stdout);
		sscanf(buffer,"%d",&messageType);

		switch(messageType)
		{


#define SC_CreateLock 				11
#define SC_AcquireLock 				12
#define SC_ReleaseLock 				13
#define SC_DestroyLock 				14
#define SC_CreateCondition 			15
#define SC_Signal 					16
#define SC_Wait 					17
#define SC_Broadcast 				18
#define SC_DestroyCondition 		19

			case SC_CreateLock:
			{
				name = new char[inMailHdr.length];
				sscanf(buffer,"%d %s",&messageType,name);
				printf("CreateLock of lock name  %s requested from Client with Machine id: %d and MailBox id: %d\n",\
																									name, inPacketHdr.from,inMailHdr.from);
				CreateLock(inPacketHdr.from,inMailHdr.from,name);
				break;
			}
			case SC_DestroyLock:
			{
				sscanf(buffer,"%d %d",&messageType, &lockId);
				printf("DestroyLock with lock id /%d requested from Client with Machine id: %d and MailBox id: %d\n",\
																									lockId, inPacketHdr.from,inMailHdr.from);
				DestroyLock(inPacketHdr.from,inMailHdr.from,lockId);
				break;
			}
			case SC_CreateCondition:
			{
				name = new char[inMailHdr.length];
				sscanf(buffer,"%d %s",&messageType,name);
				printf("CreateCondition with condition name %s requested from Client with Machine id: %d, MailBox id: %d \n",\
																									name, inPacketHdr.from,inMailHdr.from);
				CreateCondition(inPacketHdr.from,inMailHdr.from,name);
				break;
			}
			case SC_DestroyCondition:
			{
				sscanf(buffer,"%d %d",&messageType, &cvId);
				printf("DestroyCondition request from Client with Machine id: %d, MailBox id: %d for CV id: %d\n",\
																									inPacketHdr.from,inMailHdr.from,cvId);
				DestroyCondition(inPacketHdr.from,inMailHdr.from,cvId);
				break;
			}
			case SC_AcquireLock:
			{
				sscanf(buffer,"%d %d",&messageType, &lockId);
				printf("AcquireMethod request from Client with Machine id: %d, MailBox id: %d for lock id: %d\n",\
																									inPacketHdr.from,inMailHdr.from,lockId);
				AcquireMethod(inPacketHdr.from,inMailHdr.from,lockId);
				break;

			}
			case SC_ReleaseLock:
			{
				sscanf(buffer,"%d %d",&messageType, &lockId);
				printf("ReleaseMethod request from Client with Machine id: %d, MailBox id: %d for lock id: %d\n",\
																									inPacketHdr.from,inMailHdr.from,lockId);
				ReleaseMethod(inPacketHdr.from,inMailHdr.from,lockId);
				break;
			}
			case SC_Broadcast:
			{
				sscanf(buffer,"%d %d %d",&messageType, &cvId, &lockId);
				printf("Broadcast request from Client with Machine id: %d, MailBox id: %d for CV id: %d with lock id: %d\n",\
																									inPacketHdr.from,inMailHdr.from,cvId,lockId);
				BroadcastMethod(inPacketHdr.from,inMailHdr.from,lockId,cvId);
				break;
			}
			case SC_Wait:
			{
				sscanf(buffer,"%d %d %d",&messageType, &cvId, &lockId);
				printf("Wait request from Client with Machine id: %d, MailBox id: %d for CV id: %d with lock id: %d\n",\
																									inPacketHdr.from,inMailHdr.from,cvId,lockId);
				WaitMethod(inPacketHdr.from,inMailHdr.from,lockId,cvId);
				break;
			}
			case SC_Signal:
			{
				sscanf(buffer,"%d %d %d",&messageType, &cvId, &lockId);
				printf("Signal request from Client with Machine id: %d, MailBox id: %d for CV id: %d with lock id: %d\n",\
																									inPacketHdr.from,inMailHdr.from,cvId,lockId);
				SignalMethod(inPacketHdr.from,inMailHdr.from,lockId,cvId);
				break;
			}
			default:
			{
				printf("Invalid arguements passed to the server");
			}
			break;
		}
	}
#endif

}


