/*
 * server.cc
 *
 *  Created on: Jun 4, 2013
 *      Author: cvora
 */

#include "server.h"

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

void locksForServer::Acquire(int machineId,int mailBoxId)
{
	if(isHeldByCurrentClient(machineId,mailBoxId))
	{
		printf("THe Lock is already acquired by the Client");
		char* buf = new char[MaxMailSize];
		sprintf(buf,"%s","1");
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
		waitingQueueOfLocks->Append((void*)clientRply);
	}

	return;
}

bool locksForServer::Release(int machineId,int mailBoxId)
{
	if(!isHeldByCurrentClient(machineId,mailBoxId))
	{
		printf("Error releasing since the current cient does not hold the lock");
		return false;
	}

	clientReply* clientRply = (clientReply*)waitingQueueOfLocks->Remove();
	if(clientRply!=NULL)
	{
		ownerMachineId = clientRply->outPktHdr.to;
		ownerMailBoxId = clientRply->outMailHdr.to;

		SendMessage(clientRply->outPktHdr.to,clientRply->outMailHdr.to,clientRply->data);
		delete clientRply;
	}
	else
	{
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

void cvsForServer::Signal(int machineId,int mailBoxId,int conditionLockId)
{
	if(waitingCVOnLock != conditionLockId)
	{
		char* response = new char[100];
		sprintf(response,"%s","-1");
		SendMessage(machineId,mailBoxId,response);
		delete [] response;
		return;
	}

	if(waitingQueueOfCVS->IsEmpty())
	{
		char* response = new char[100];
		sprintf(response,"%s","-1");
		SendMessage(machineId,mailBoxId,response);
		delete [] response;
		return;
	}

	clientReply* clientRply= (clientReply*)waitingQueueOfCVS->Remove();
	serverLockTable.serverLocksArray[conditionLockId].serverLock->Acquire(clientRply->outPktHdr.to,
			clientRply->outMailHdr.to);

	char *response = new char[100];
	sprintf(response,"%s","1");
	SendMessage(machineId,mailBoxId,response);
	delete [] response;
	return;
}

void cvsForServer :: Wait(int machineId, int mailBoxId, int conditionLockId)
{

	if(waitingCVOnLock != conditionLockId)
	{
		//Illegal lock , so cannot signa..
		char *response = new char[100];
		sprintf(response,"%s","-1");
		SendMessage(machineId,mailBoxId,response);
		delete [] response;
		return;
	}

	if(waitingCVOnLock ==-1)
	//no one is waiting on the lock
	{
	waitingCVOnLock= conditionLockId;
	}

	//create message
	clientReply* clientRply = new clientReply();

	//ready the reply message
	sprintf(clientRply->data,"%s","1");
	clientRply->outPktHdr.to = machineId;
	clientRply->outMailHdr.to= mailBoxId;
	clientRply->outMailHdr.from= 0;
	clientRply->outMailHdr.length=strlen((clientRply->data)+1);

	//now append clientMsg to wait queue
	waitingQueueOfCVS-> Append((void*)clientRply);

	//release
	serverLockTable.serverLocksArray[conditionLockId].serverLock->Release(machineId, mailBoxId);

	//TODO: whether to do acquire here or not..as aft release we need to acquire lock
	//as we come out of wait..
	//we need a lock to proceed
}


void cvsForServer :: Broadcast(int machineId, int mailBoxId, int conditionLockId)
{
	if(waitingCVOnLock != conditionLockId)
	{
		//Illegal lock , so cannot signa..
		char *response = new char[100];
		sprintf(response,"%s","-1");
		SendMessage(machineId,mailBoxId,response);
		delete [] response;
		return;
	}

	if(waitingQueueOfCVS -> IsEmpty())
	{
		//send signal even if queie is empty
		char *response = new char[100];
		sprintf(response,"%s","1");
		SendMessage(machineId,mailBoxId,response);
		delete [] response;
		return;
	}

	//now if we are here we have to broad cast to all waiting clients on CV
	//so for this remove clientMsg from waitQueue one by one and
	//acquire lock..so this has to be done is a while loop

	clientReply* clientRply;
	while(!(waitingQueueOfCVS -> IsEmpty()))
	{
		clientRply= (clientReply*)waitingQueueOfCVS->Remove();
		serverLockTable.serverLocksArray[conditionLockId].serverLock->
		Acquire(clientRply->outPktHdr.to, clientRply->outMailHdr.to);
	}

	char *response = new char[100];
	sprintf(response,"%s","1");
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
				&& (serverLockTable.serverLocksArray[i].serverLock->name,name)==0)
		{
			(serverLockTable.serverLocksArray[i].sLockCounter)++;
			char *buf = new char[100];
			sprintf(buf,"%d",i); //since lock is already crated give the lock id in message
			printf("Lock already created so sent the lockId to the client\n");
			SendMessage(machineId,mailBoxId,buf);
			delete [] buf;
			return;
		}

	}
//check to find whether there is place to create new lock
	if((index=(serverLockTable.sLockBitMap->Find())==-1))
	{
		printf("No place to create new lock, Max limit of locks reached\n");
		char *buf = new char[100];
		sprintf(buf,"%s","-1");
		printf("message sent to client that there is no place to create new lock\n");
		SendMessage(machineId,mailBoxId,buf);
		delete [] buf;
		return;
	}
	else
	{
		serverLockTable.serverLocksArray[index].serverLock = new locksForServer(name);
		serverLockTable.serverLocksArray[index].valid = true;
		(serverLockTable.serverLocksArray[index].sLockCounter)++;
		char *buf = new char[100];
		sprintf(buf,"%d",index);
		printf("message sent to client that lock is created with lockId %d\n",index);
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
		char *buf2 = new char[100];
		sprintf(buf2,"%s","-1");
		SendMessage(machineId,mailBoxId,buf2);
		delete [] buf2;
		return;
	}


	if(serverLockTable.serverLocksArray[lockId].valid == false)
	{
		printf("Lock already deleted cannot destroy lock");
		char *buf2 = new char[100];
		sprintf(buf2,"%s","-1");
		SendMessage(machineId,mailBoxId,buf2);
		delete [] buf2;
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
	char *buff2 = new char[100];
	sprintf(buff2,"%s","1");
	SendMessage(machineId,mailBoxId,buff2);
	delete [] buff2;
	return;
	}
	else
	{
	printf("Lock cannot be deleted as it is in use\n");
	char *buff2 = new char[100];
	sprintf(buff2,"%s","-1");
	SendMessage(machineId,mailBoxId,buff2);
	delete [] buff2;
	return;
	}
}

void AcquireMethod(int machineId,int mailBoxId,int lockId)
{
	//check lock id range
	if(lockId<0||lockId>=MAX_LOCKS)
	{
		printf("lockid out of range");
		char *data = new char[5];
		sprintf(data,"%s","-1");
		SendMessage(machineId, mailBoxId,data);
		delete[] data;
		return;
	}

	if(serverLockTable.serverLocksArray[lockId].valid==false){
		printf("lock id does not exist");
		char *data = new char[5];
		sprintf(data,"%s","-1");
		SendMessage(machineId, mailBoxId,data);
		delete[] data;
		return;
	}
	serverLockTable.serverLocksArray[lockId].serverLock->Acquire(machineId,mailBoxId);

}

void ReleaseMethod(int machineId,int mailBoxId,int lockId)
{
	bool reply;
		//check lock id range
	if(lockId<0||lockId>=MAX_LOCKS)
	{
		printf("lockid out of range");
		char *data = new char[5];
		sprintf(data,"%s","-1");
		SendMessage(machineId, mailBoxId,data);
		delete[] data;
		return;
	}

	//Check if a lock with the given id was already created
	if(serverLockTable.serverLocksArray[lockId].valid==false){
		printf("lock id does not exist");
		char *data = new char[5];
		sprintf(data,"%s","-1");
		SendMessage(machineId, mailBoxId,data);
		delete[] data;
		return;
	}

	reply = serverLockTable.serverLocksArray[lockId].serverLock->Release(machineId,mailBoxId);
	//returns true if lock can be released
	//Reply message will be sent by the Release method
	if(reply==false)
	{
		char *buf = new char[MaxMailSize];
		sprintf(buf,"%s", "-1");
		printf("Lock ID %d can not be  released \n",lockId);
		SendMessage(machineId, mailBoxId, buf);
		delete[] buf;
		return;
	}
	else{
		char *buf = new char[MaxMailSize];
		sprintf(buf,"%s", "1");
		printf("Lock ID %d is released \n",lockId);
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
			char *buf6 = new char[100];
			sprintf(buf6,"%d",i); //since lock is already crated give the lock id in message
			printf("CV already created so sent the CVId to the client\n");
			SendMessage(machineId,mailBoxId,buf6);
			delete [] buf6;
			return;
		}

	}
//check to find whether there is place to create new lock
	if((index=(serverCVTable.sCVBitMap->Find())==-1))
	{
		printf("No place to create new CV, Max limit of CVS reached\n");
		char *buf6 = new char[100];
		sprintf(buf6,"%s","-1");
		printf("message sent to client that there is no place to create new CV\n");
		SendMessage(machineId,mailBoxId,buf6);
		delete [] buf6;
		return;
	}
	else
	{
		serverCVTable.serverCVSArray[index].serverCV = new cvsForServer(name);
		serverCVTable.serverCVSArray[index].valid = true;
		(serverCVTable.serverCVSArray[index].sCVCounter)++;
		char *buf6 = new char[100];
		sprintf(buf6,"%s","1");
		printf("message sent to client that CV is created with CVId %d\n",index);
		SendMessage(machineId,mailBoxId,buf6);
		delete [] buf6;
		return;

	}

}


void DestroyCondition(int machineId, int mailBoxId, int cvId)
{
	if(cvId< 0 || cvId > MAX_CVS)
	{

		printf("Invalid cvId\n");
		char *buf7 = new char[100];
		sprintf(buf7,"%s","-1");
		SendMessage(machineId,mailBoxId,buf7);// TODO : juzz send -1
		delete [] buf7;
		return;
	}


	if(serverCVTable.serverCVSArray[cvId].valid == false)
	{
		printf("CV already deleted cannot destroy CV");
		char *buf7 = new char[100];
		sprintf(buf7,"%s","-1");
		SendMessage(machineId,mailBoxId,buf7);
		delete [] buf7;
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
	char *buf7 = new char[100];
	sprintf(buf7,"%s","1");
	SendMessage(machineId,mailBoxId,buf7);
	delete [] buf7;
	return;
	}
	else
	{
	printf("CV cannot be deleted as it is in use\n");
	char *buf7 = new char[100];
	sprintf(buf7,"%s","-1");
	SendMessage(machineId,mailBoxId,buf7);
	delete [] buf7;
	return;
	}
}

void WaitMethod(int machineId, int mailBoxId, int lockId, int cvId)
{
	if(lockId<0 || lockId>MAX_LOCKS || cvId<0 || cvId> MAX_CVS)
	{
	printf("Lock and CV out of range");
	char *buf3 = new char[100];
	sprintf(buf3,"%s","-1");
	SendMessage(machineId,mailBoxId,buf3);
	delete [] buf3;
	return;
	}


	if(serverLockTable.serverLocksArray[lockId].valid == false || serverCVTable.serverCVSArray[cvId].valid
			== false)
	{
	printf("Lock or CV is not valid\n");
	char *buf3 = new char[100];
	sprintf(buf3,"%s","-1");
	SendMessage(machineId,mailBoxId,buf3);
	delete [] buf3;
	return;
	}

	//If we are here then the lock and CV both are valid
	serverCVTable.serverCVSArray[cvId].serverCV->Wait(machineId,mailBoxId,lockId);
	//TODO : see this declaration ie order of parameters of serverWaitfunction
}



void SignalMethod(int machineId, int mailBoxId, int lockId, int cvId)
{
	if(lockId<0 || lockId>MAX_LOCKS || cvId<0 || cvId> MAX_CVS)
	{
	printf("Lock and CV out of range");
	char *buf4 = new char[100];
	sprintf(buf4,"%s","-1");
	SendMessage(machineId,mailBoxId,buf4);
	delete [] buf4;
	return;
	}


	if(serverLockTable.serverLocksArray[lockId].valid == false || serverCVTable.serverCVSArray[cvId].valid
			== false)
	{
	printf("Lock or CV is not valid\n");
	char *buf4 = new char[100];
	sprintf(buf4,"%s","-1");
	SendMessage(machineId,mailBoxId,buf4);
	delete [] buf4;
	return;
	}

	//If we are here then the lock and CV both are valid
	serverCVTable.serverCVSArray[cvId].serverCV->Signal(machineId,mailBoxId,lockId);
	//TODO : see this declaration ie order of parameters of serverWaitfunction
}


void BroadcastMethod(int machineId, int mailBoxId, int lockId, int cvId)
{
	if(lockId<0 || lockId>MAX_LOCKS || cvId<0 || cvId> MAX_CVS)
	{
	printf("Lock and CV out of range");
	char *buf5 = new char[100];
	sprintf(buf5,"%s","-1");
	SendMessage(machineId,mailBoxId,buf5);
	delete [] buf5;
	return;
	}


	if(serverLockTable.serverLocksArray[lockId].valid == false || serverCVTable.serverCVSArray[cvId].valid
			== false)
	{
	printf("Lock or CV is not valid\n");
	char *buf5 = new char[100];
	sprintf(buf5,"%s","-1");
	SendMessage(machineId,mailBoxId,buf5);
	delete [] buf5;
	return;
	}

	//If we are here then the lock and CV both are valid
	serverCVTable.serverCVSArray[cvId].serverCV->Broadcast(machineId,mailBoxId,lockId);
	//TODO : see this declaration ie order of parameters of serverWaitfunction
}

//Server function
void Server()
{
	printf("Server started..");
	PacketHeader outPacketHdr, inPacketHdr;
	MailHeader outMailHdr, inMailHdr;
	char buffer[MaxMailSize];
	int messageType;
	char *name;
	int lockId, cvId, mvId,arrayIndex;
	int setValue;
	printf("Server started..");
	while(1)
	{
		postOffice->Receive(0,&inPacketHdr,&inMailHdr,buffer);
		outPacketHdr.to = inMailHdr.from;
		outMailHdr.to =inMailHdr.from;
		fflush(stdout);
		sscanf(buffer,"%d",&messageType);

		switch(messageType)
		{
			case 1:
			{
				name = new char[inMailHdr.length];
				sscanf(buffer,"%d %s",&messageType,name);
				printf("CreateLock of lock name  %s requested from Client with Machine id: %d and MailBox id: %d\n",\
																									name, inPacketHdr.from,inMailHdr.from);
				CreateLock(outPacketHdr.to,outMailHdr.to,name);
				break;
			}
			case 2:
			{
				sscanf(buffer,"%d %d",&messageType, &lockId);
				printf("DestroyLock with lock id /%d requested from Client with Machine id: %d and MailBox id: %d\n",\
																									lockId, inPacketHdr.from,inMailHdr.from);
				DestroyLock(outPacketHdr.to,outMailHdr.to,lockId);
				break;
			}
			case 3:
			{
				name = new char[inMailHdr.length];
				sscanf(buffer,"%d %s",&messageType,name);
				printf("CreateCondition with condition name %s requested from Client with Machine id: %d, MailBox id: %d \n",\
																									name, inPacketHdr.from,inMailHdr.from);
				CreateCondition(outPacketHdr.to,outMailHdr.to,name);
				break;
			}
			case 4:
			{
				sscanf(buffer,"%d %d",&messageType, &cvId);
				printf("DestroyCondition request from Client with Machine id: %d, MailBox id: %d for CV id: %d\n",\
																									inPacketHdr.from,inMailHdr.from,cvId);
				DestroyCondition(outPacketHdr.to,outMailHdr.to,cvId);
				break;
			}
			case 5:
			{
				sscanf(buffer,"%d %d",&messageType, &lockId);
				printf("AcquireMethod request from Client with Machine id: %d, MailBox id: %d for lock id: %d\n",\
																									inPacketHdr.from,inMailHdr.from,lockId);
				AcquireMethod(outPacketHdr.to,outMailHdr.to,lockId);
				break;

			}
			case 6:
			{
				sscanf(buffer,"%d %d",&messageType, &lockId);
				printf("ReleaseMethod request from Client with Machine id: %d, MailBox id: %d for lock id: %d\n",\
																									inPacketHdr.from,inMailHdr.from,lockId);
				ReleaseMethod(outPacketHdr.to,outMailHdr.to,lockId);
				break;
			}
			case 7:
			{
				sscanf(buffer,"%d %d %d",&messageType, &cvId, &lockId);
				printf("Broadcast request from Client with Machine id: %d, MailBox id: %d for CV id: %d with lock id: %d\n",\
																									inPacketHdr.from,inMailHdr.from,cvId,lockId);
				BroadcastMethod(outPacketHdr.to,outMailHdr.to,lockId,cvId);
				break;
			}
			case 8:
			{
				sscanf(buffer,"%d %d %d",&messageType, &cvId, &lockId);
				printf("Wait request from Client with Machine id: %d, MailBox id: %d for CV id: %d with lock id: %d\n",\
																									inPacketHdr.from,inMailHdr.from,cvId,lockId);
				WaitMethod(outPacketHdr.to,outMailHdr.to,lockId,cvId);
				break;
			}
			case 9:
			{
				sscanf(buffer,"%d %d %d",&messageType, &cvId, &lockId);
				printf("Signal request from Client with Machine id: %d, MailBox id: %d for CV id: %d with lock id: %d\n",\
																									inPacketHdr.from,inMailHdr.from,cvId,lockId);
				SignalMethod(outPacketHdr.to,outMailHdr.to,lockId,cvId);
				break;
			}
			case 10:
			{
				//space for mv functions
			}
			break;
			case 11:
			{
				//space for mv functions
			}
			break;
			case 12:
			{
				//space for mv functions
			}
			break;
			case 13:
			{
				//space for mv functions
			}
			break;
			default:
			{
				printf("Invalid arguements passed to the server");
			}
			break;
		}
	}

}


