/*
 * server.h
 *
 *  Created on: Jun 4, 2013
 *      Author: cvora
 */

#ifndef SERVER_H_
#define SERVER_H_

#include "network.h"
#include "post.h"
#include "list.h"
#include "system.h"


#if 1
/**
 * Handling Locks on Server for RPC
 */

enum serverLockStatus {SERVERLOCKFREE, SERVERLOCKBUSY};

class locksForServer
{
public:
	locksForServer(char* debugName);
	~locksForServer();

	bool isHeldByCurrentClient(int machineId,int mailBoxId);
	char* getName() {return name;}

	void Acquire(int lockId,int machineId,int mailBoxId);
	bool Release(int lockId,int machineId,int mailBoxId);
	List* waitingQueueOfLocks;
	char* name;

	serverLockStatus serverLockStatusObj;

	int ownerMachineId;
	int ownerMailBoxId;

};


typedef struct paramtersOfServerLockTable
{
	bool valid;
	int sLockCounter;
	locksForServer *serverLock;
}paramtersOfServerLockTableObject;

typedef struct serverLockTableStruct
{
	BitMap *sLockBitMap;
	paramtersOfServerLockTableObject serverLocksArray[MAX_LOCKS];

	serverLockTableStruct()
	{
		for(int i=0;i<MAX_LOCKS;i++)
		{
			serverLocksArray[i].valid = false;
			serverLocksArray[i].sLockCounter = 0;
			serverLocksArray[i].serverLock = NULL;
		}
		sLockBitMap = new BitMap(MAX_LOCKS);
	}

}serverLockTableStructObject;

serverLockTableStructObject serverLockTable;

/**
 * Handling CV's on Server for RPC
 */

class cvsForServer
{
public:
	cvsForServer(char* debugName);
	~cvsForServer();
	char* getName() {return name ;}
	void Wait(int machineId,int mailBoxId,int conditionId,int conditionLock);
	void Broadcast(int machineId,int mailBoxId,int conditionId,int conditionLock);
	void Signal(int machineId,int mailBoxId,int conditionId,int conditionLock);

	List* waitingQueueOfCVS;
	char* name;

	int waitingCVOnLock;

};

typedef struct parametersOfServerCVTable
{
	bool valid;
	int sCVCounter;
	cvsForServer *serverCV;

}parametersOfServerCVTableObject;

typedef struct serverCVTableStruct
{
	BitMap *sCVBitMap;
	parametersOfServerCVTableObject serverCVSArray[MAX_CVS];

	serverCVTableStruct()
		{
			for (int i=0; i < MAX_CVS; i++)
			{
				serverCVSArray[i].valid = false;
				serverCVSArray[i].sCVCounter = 0;
				serverCVSArray[i].serverCV = NULL;
			}
			sCVBitMap= new BitMap(MAX_CVS);

		}
}serverCVTableObject;

serverCVTableObject serverCVTable;

typedef struct clientReplyMessage
{
	char data[MaxMailSize];
	PacketHeader outPktHdr;
	MailHeader outMailHdr;

	clientReplyMessage()
	{
		outPktHdr.to = -1;
		outMailHdr.to = -1;

		outPktHdr.from = -1;
		outMailHdr.from = -1;

		outPktHdr.length = 0;
		outMailHdr.length = 0;

	}
}clientReply;

#endif

#endif /* SERVER_H_ */
