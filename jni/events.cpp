#include "main.h"
#include "game/game.h"
#include "net/netgame.h"
#include "chatwindow.h"

#include "util/armhook.h"

#define EVENT_TYPE_PAINTJOB			1
#define EVENT_TYPE_CARCOMPONENT		2
#define EVENT_TYPE_CARCOLOR			3
#define EVENT_ENTEREXIT_MODSHOP		4

extern CGame *pGame;
extern CNetGame *pNetGame;
extern CChatWindow *pChatWindow;

extern bool bFirstSpawn;

void SendScmEvent(int iEventType, uint32_t dwParam1, uint32_t dwParam2, uint32_t dwParam3);

void ProcessIncommingEvent(PLAYERID playerId, int iEventType, uint32_t dwParam1, uint32_t dwParam2, uint32_t dwParam3)
{
	if(!pNetGame) return;
	if(bFirstSpawn) return; // Local player has never spawned.
	
	uint32_t v;
	int iVehicleID;
	int iPaintJob;
	int iComponent;
	int iWait;
	
	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	CVehicle* pVehicle = pVehiclePool->GetAt(iVehicleID);
	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	CRemotePlayer *pRemotePlayer = pPlayerPool->GetAt(playerId);
	
	switch(iEventType) 
	{
		case EVENT_TYPE_PAINTJOB:
		iVehicleID = pVehiclePool->FindGtaIDFromID(dwParam1);
		iPaintJob = (int)dwParam2;
		if(iVehicleID) ScriptCommand(&give_vehicle_paintjob, iVehicleID, dwParam2);
		break;

		case EVENT_TYPE_CARCOMPONENT:
		iVehicleID = pVehiclePool->FindGtaIDFromID(dwParam1);
		iComponent = (int)dwParam2;
		pGame->RequestModel(iComponent);
		pGame->LoadRequestedModels();
		ScriptCommand(&request_vehicle_mod, iComponent);

		iWait = 10;
		while(!ScriptCommand(&has_vehicle_mod_loaded, iComponent) && iWait) 
		{
			usleep(5);
			iWait--;
		}
		if(!iWait) 
		{
			Log("Timeout on car Component.");
			break;
		}
		ScriptCommand(&add_vehicle_mod, iVehicleID, iComponent, &v);
		Log("Added car component: %d", iComponent);
		break;

		case EVENT_TYPE_CARCOLOR:
		pVehicle = pVehiclePool->GetAt((VEHICLEID)dwParam1);
		if(pVehicle) pVehicle->SetColor((int)dwParam2, (int)dwParam3);
		break;

		case EVENT_ENTEREXIT_MODSHOP:
		if(playerId > MAX_PLAYERS) return;
		pVehicle = pVehiclePool->GetAt((VEHICLEID)dwParam1);
		pRemotePlayer = pNetGame->GetPlayerPool()->GetAt(playerId);
		if(pRemotePlayer && pVehicle) {
			pRemotePlayer->m_iIsInAModShop = (int)dwParam2;
		}
		break;
	}
}

void ProcessOutgoingEvent(int iEventType, uint32_t dwParam1, uint32_t dwParam2, uint32_t dwParam3)
{
	if(!pNetGame) return;
	
	int iVehicleID;
	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	
	switch(iEventType) 
	{
		case EVENT_TYPE_PAINTJOB:
		iVehicleID = pVehiclePool->FindIDFromGtaPtr(GamePool_Vehicle_GetAt(dwParam1));
		if(iVehicleID != INVALID_VEHICLE_ID) {
			SendScmEvent(EVENT_TYPE_PAINTJOB, iVehicleID, dwParam2, 0);		
		}
		break;

		case EVENT_TYPE_CARCOMPONENT:
		iVehicleID = pVehiclePool->FindIDFromGtaPtr(GamePool_Vehicle_GetAt(dwParam1));
		if(iVehicleID != INVALID_VEHICLE_ID) {
			SendScmEvent(EVENT_TYPE_CARCOMPONENT, iVehicleID, dwParam2, 0);
		}
		break;

		case EVENT_TYPE_CARCOLOR:
		iVehicleID = pVehiclePool->FindIDFromGtaPtr(GamePool_Vehicle_GetAt(dwParam1));
		if(iVehicleID != INVALID_VEHICLE_ID) {
			SendScmEvent(EVENT_TYPE_CARCOLOR, iVehicleID, dwParam2, dwParam3);
		}
		break;

		case EVENT_ENTEREXIT_MODSHOP:
		iVehicleID = pVehiclePool->FindIDFromGtaPtr(GamePool_Vehicle_GetAt(dwParam1));
		if(iVehicleID != INVALID_VEHICLE_ID) {
			SendScmEvent(EVENT_ENTEREXIT_MODSHOP, iVehicleID, dwParam2, 0);
		}
		break;
	}
}

void SendMoneyIncrease(uint32_t dwIncreaseType, uint32_t dwAmount)
{
	if (!pNetGame) return;
	if (!pNetGame->GetPlayerPool()) return;
	int iAmount = dwAmount;

	// "Increase" types 4 and five for negative value's
	if (dwIncreaseType >= 4) {
		iAmount = 0 - iAmount;
		dwIncreaseType -= 2;
	}

	RakNet::BitStream bsSend;
	bsSend.Write(iAmount);
	bsSend.Write(dwIncreaseType);
	//pNetGame->GetRakClient()->RPC(&RPC_ScriptCash,&bsSend,HIGH_PRIORITY,RELIABLE_SEQUENCED,0,false);
}

void SendScmEvent(int iEventType, uint32_t dwParam1, uint32_t dwParam2, uint32_t dwParam3)
{
	Log("RPC: SendScmEvent");
	RakNet::BitStream bsSend;
	bsSend.Write(iEventType);
	bsSend.Write(dwParam1);
	bsSend.Write(dwParam2);
	bsSend.Write(dwParam3);
	pNetGame->GetRakClient()->RPC(&RPC_ScmEvent, &bsSend, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, NULL);
}