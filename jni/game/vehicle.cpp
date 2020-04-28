#include "main.h"
#include "chatwindow.h"
#include "game.h"
#include "net/netgame.h"

extern CGame *pGame;
extern CNetGame *pNetGame;
extern CChatWindow *pChatWindow;

uint32_t dwLastCreatedVehicleID = 0;
VEHICLE_TYPE *pLastVehicle;

CVehicle::CVehicle(int iType, float fPosX, float fPosY, float fPosZ, float fRotation, bool bSiren)
{
	MATRIX4X4 mat;
	uint32_t dwRetID = 0;

	m_pVehicle = nullptr;
	m_dwGTAId = 0;
	m_pTrailer = nullptr;

	if( (iType != TRAIN_PASSENGER_LOCO) &&
		(iType != TRAIN_FREIGHT_LOCO) &&
		(iType != TRAIN_PASSENGER) &&
		(iType != TRAIN_FREIGHT) &&
		(iType != TRAIN_TRAM)) 
	{
		// normal vehicle
		if(!pGame->IsModelLoaded(iType))
		{
			pGame->RequestModel(iType);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(iType)) usleep(10);
		}

		ScriptCommand(&create_car, iType, fPosX, fPosY, fPosZ, &dwRetID);
		ScriptCommand(&set_car_z_angle, dwRetID, fRotation);
		ScriptCommand(&car_gas_tank_explosion,dwRetID, 0);
		ScriptCommand(&set_car_hydraulics, dwRetID, 0);
		ScriptCommand(&toggle_car_tires_vulnerable, dwRetID, 0);
			
		m_pVehicle = (VEHICLE_TYPE*)GamePool_Vehicle_GetAt(dwRetID);
		m_pEntity = (ENTITY_TYPE*)m_pVehicle;
		m_dwGTAId = dwRetID;
		
		if(bSiren) 
		{
			SetSirenOn(1);
		}else{
			SetSirenOn(0);
		}
		
		if(m_pVehicle)
		{
			m_pVehicle->dwDoorsLocked = 0;
			m_bIsLocked = false;

			GetMatrix(&mat);
			mat.pos.X = fPosX;
			mat.pos.Y = fPosY;
			mat.pos.Z = fPosZ;

			if( GetVehicleSubtype() != VEHICLE_SUBTYPE_BIKE && 
				GetVehicleSubtype() != VEHICLE_SUBTYPE_PUSHBIKE)
				mat.pos.Z += 0.25f;

			SetMatrix(mat);
		}
	}
	else if((iType == TRAIN_PASSENGER_LOCO) ||
			(iType == TRAIN_FREIGHT_LOCO) ||
			(iType == TRAIN_TRAM))
	{
		// TRAIN LOCOMOTIVES

		if(iType == TRAIN_PASSENGER_LOCO) iType = 5;
		else if(iType == TRAIN_FREIGHT_LOCO) iType = 3;
		else if(iType == TRAIN_TRAM) iType = 9;

		uint32_t dwDirection=0;
		if(fRotation > 180.0f) {
			dwDirection = 1;
		}
		pGame->RequestModel(TRAIN_PASSENGER_LOCO);
		pGame->RequestModel(TRAIN_PASSENGER);
		pGame->RequestModel(TRAIN_FREIGHT_LOCO);
		pGame->RequestModel(TRAIN_FREIGHT);
		pGame->RequestModel(TRAIN_TRAM);
		pGame->LoadRequestedModels();
		while(!pGame->IsModelLoaded(TRAIN_PASSENGER_LOCO)) usleep(0);
		while(!pGame->IsModelLoaded(TRAIN_PASSENGER)) usleep(0);
		while(!pGame->IsModelLoaded(TRAIN_FREIGHT_LOCO)) usleep(0);
		while(!pGame->IsModelLoaded(TRAIN_FREIGHT)) usleep(0);
		while(!pGame->IsModelLoaded(TRAIN_TRAM)) usleep(0);
	
		ScriptCommand(&create_train,iType,fPosX,fPosY,fPosZ,dwDirection,&dwRetID);

		m_pVehicle = GamePool_Vehicle_GetAt(dwRetID);
		m_pEntity = (ENTITY_TYPE *)m_pVehicle; 
		m_dwGTAId = dwRetID;
		dwLastCreatedVehicleID = dwRetID;
		pLastVehicle = m_pVehicle;

		//GamePrepareTrain(m_pVehicle);
		//ScriptCommand(&set_train_flag, &dwRetID, 0);
	}
	else if((iType == TRAIN_PASSENGER) ||
			iType == TRAIN_FREIGHT)
	{
		if(!pLastVehicle) {
			m_pEntity = 0;
			m_pVehicle = 0;
			pLastVehicle = 0;
			return;
		}

		m_pVehicle = (VEHICLE_TYPE *)pLastVehicle->VehicleAttachedBottom;

		if(!m_pVehicle) {
			pChatWindow->AddDebugMessage("Warning: bad train carriages");
			m_pEntity = 0;
			m_pVehicle = 0;
			pLastVehicle = 0;
			return;
		}

		dwRetID = GamePool_Vehicle_GetIndex(m_pVehicle);
		m_pEntity = (ENTITY_TYPE *)m_pVehicle;
		m_dwGTAId = dwRetID;
		dwLastCreatedVehicleID = dwRetID;
		pLastVehicle = m_pVehicle;
	}

	m_byteObjectiveVehicle = 0;
	m_bSpecialMarkerEnabled = false;
	m_bDoorsLocked = false;
	m_dwMarkerID = 0;
	m_bIsInvulnerable = false;
}

CVehicle::~CVehicle()
{
	m_pVehicle = GamePool_Vehicle_GetAt(m_dwGTAId);

	if(m_pVehicle)
	{
		if(m_dwMarkerID)
		{
			ScriptCommand(&disable_marker, m_dwMarkerID);
			m_dwMarkerID = 0;
		}

		RemoveEveryoneFromVehicle();
		
		if(IsSirenOn()) 
			SetSirenOn(0);
		
		if(m_pTrailer)
		{
			DetachTrailer();
		}

		// тут еще какая-то интересная шняга

		if( m_pVehicle->entity.nModelIndex == TRAIN_PASSENGER_LOCO ||
			m_pVehicle->entity.nModelIndex == TRAIN_FREIGHT_LOCO )
		{
			ScriptCommand(&destroy_train, m_dwGTAId);
		}
		else
		{
			int nModelIndex = m_pVehicle->entity.nModelIndex;
			ScriptCommand(&destroy_car, m_dwGTAId);

			if( !GetModelReferenceCount(nModelIndex) &&
				//!m_bKeepModelLoaded &&
				//(pGame->GetVehicleModelsCount() > 80) &&
				pGame->IsModelLoaded(nModelIndex))
			{
				// CStreaming::RemoveModel
				(( void (*)(int))(g_libGTASA+0x290C4C+1))(nModelIndex);
			}
		}
	}
}

void CVehicle::AttachTrailer()
{
	if (m_pTrailer)
		ScriptCommand(&put_trailer_on_cab, m_pTrailer->m_dwGTAId, m_dwGTAId);
}

void CVehicle::DetachTrailer()
{
	if (m_pTrailer)
		ScriptCommand(&detach_trailer_from_cab, m_pTrailer->m_dwGTAId, m_dwGTAId);
}

void CVehicle::SetTrailer(CVehicle *pTrailer)
{
	m_pTrailer = pTrailer;
}

CVehicle* CVehicle::GetTrailer()
{
	if (!m_pVehicle) return NULL;

	// Try to find associated trailer
	uint32_t dwTrailerGTAPtr = m_pVehicle->dwTrailer;

	if(pNetGame && dwTrailerGTAPtr) {
		CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
		VEHICLEID TrailerID = (VEHICLEID)pVehiclePool->FindIDFromGtaPtr((VEHICLE_TYPE*)dwTrailerGTAPtr);
		if(TrailerID < MAX_VEHICLES && pVehiclePool->GetSlotState(TrailerID)) {
			return pVehiclePool->GetAt(TrailerID);
		}
	}

	return NULL;
}

void CVehicle::LinkToInterior(int iInterior)
{
	if(GamePool_Vehicle_GetAt(m_dwGTAId)) 
	{
		ScriptCommand(&link_vehicle_to_interior, m_dwGTAId, iInterior);
	}
}

void CVehicle::SetColor(int iColor1, int iColor2)
{
	if(m_pVehicle)
	{
		if(GamePool_Vehicle_GetAt(m_dwGTAId))
		{
			m_pVehicle->byteColor1 = (uint8_t)iColor1;
			m_pVehicle->byteColor2 = (uint8_t)iColor2;
		}
	}

	m_byteColor1 = (uint8_t)iColor1;
	m_byteColor2 = (uint8_t)iColor2;
	m_bColorChanged = true;
}

void CVehicle::SetHealth(float fHealth)
{
	if(m_pVehicle)
	{
		m_pVehicle->fHealth = fHealth;
	}
}

float CVehicle::GetHealth()
{
	if(m_pVehicle) return m_pVehicle->fHealth;
	else return 0.0f;
}

// 0.3.7
void CVehicle::SetInvulnerable(bool bInv)
{
	if(!m_pVehicle) return;
	if(!GamePool_Vehicle_GetAt(m_dwGTAId)) return;
	if(m_pVehicle->entity.vtable == g_libGTASA+0x5C7358) return;

	if(bInv) 
	{
		ScriptCommand(&set_car_immunities, m_dwGTAId, 1,1,1,1,1);
		ScriptCommand(&toggle_car_tires_vulnerable, m_dwGTAId, 0);
		m_bIsInvulnerable = true;
	} 
	else 
	{ 
		ScriptCommand(&set_car_immunities, m_dwGTAId, 0,0,0,0,0);
		ScriptCommand(&toggle_car_tires_vulnerable, m_dwGTAId, 1);
		m_bIsInvulnerable = false;
	}
}

// 0.3.7
bool CVehicle::IsDriverLocalPlayer()
{
	if(m_pVehicle)
	{
		if((PED_TYPE*)m_pVehicle->pDriver == GamePool_FindPlayerPed())
			return true;
	}

	return false;
}

// 0.3.7
bool CVehicle::HasSunk()
{
	if(!m_pVehicle) return false;
	return ScriptCommand(&has_car_sunk, m_dwGTAId);
}

bool CVehicle::IsRCVehicle()
{
	if (!m_pVehicle) return false;
	if (!GamePool_Vehicle_GetAt(m_dwGTAId)) return false;
	
	if( m_pVehicle->entity.nModelIndex == 441 || 
		m_pVehicle->entity.nModelIndex == 464 ||
		m_pVehicle->entity.nModelIndex == 465 || 
		m_pVehicle->entity.nModelIndex == 594 ||
		m_pVehicle->entity.nModelIndex == 501 || 
		m_pVehicle->entity.nModelIndex == 564 )
	{
		return true;
	}

	return false;
}

bool CVehicle::IsATrainPart()
{
	int nModel;
	if(m_pVehicle) {
		nModel = m_pVehicle->entity.nModelIndex;
		if(nModel == TRAIN_PASSENGER_LOCO) return true;
		if(nModel == TRAIN_PASSENGER) return true;
		if(nModel == TRAIN_FREIGHT_LOCO) return true;
		if(nModel == TRAIN_FREIGHT) return true;
		if(nModel == TRAIN_TRAM) return true;
	}
	return false;
}

void CVehicle::RemoveEveryoneFromVehicle()
{
	if(!m_pVehicle) return;
	if(!GamePool_Vehicle_GetAt(m_dwGTAId)) return;

	float fPosX = m_pVehicle->entity.mat->pos.X;
	float fPosY = m_pVehicle->entity.mat->pos.Y;
	float fPosZ = m_pVehicle->entity.mat->pos.Z;

	int iPlayerID = 0;
	if(m_pVehicle->pDriver)
	{
		iPlayerID = GamePool_Ped_GetIndex( m_pVehicle->pDriver );
		ScriptCommand(&remove_actor_from_car_and_put_at, iPlayerID, fPosX, fPosY, fPosZ + 2.0f);
	}

	for(int i = 0; i<7; i++)
	{
		if(m_pVehicle->pPassengers[i] != nullptr)
		{
			iPlayerID = GamePool_Ped_GetIndex( m_pVehicle->pPassengers[i] );
			ScriptCommand(&remove_actor_from_car_and_put_at, iPlayerID, fPosX, fPosY, fPosZ + 2.0f);
		}
	}
}

// 0.3.7
bool CVehicle::IsOccupied()
{
	if(m_pVehicle)
	{
		if(m_pVehicle->pDriver) return true;
		if(m_pVehicle->pPassengers[0]) return true;
		if(m_pVehicle->pPassengers[1]) return true;
		if(m_pVehicle->pPassengers[2]) return true;
		if(m_pVehicle->pPassengers[3]) return true;
		if(m_pVehicle->pPassengers[4]) return true;
		if(m_pVehicle->pPassengers[5]) return true;
		if(m_pVehicle->pPassengers[6]) return true;
	}

	return false;
}

void CVehicle::ProcessMarkers()
{
	if(!m_pVehicle) return;

	if(m_byteObjectiveVehicle)
	{
		if(!m_bSpecialMarkerEnabled)
		{
			if(m_dwMarkerID)
			{
				ScriptCommand(&disable_marker, m_dwMarkerID);
				m_dwMarkerID = 0;
			}

			ScriptCommand(&tie_marker_to_car, m_dwGTAId, 1, 3, &m_dwMarkerID);
			ScriptCommand(&set_marker_color, m_dwMarkerID, 1006);
			ScriptCommand(&show_on_radar, m_dwMarkerID, 3);
			m_bSpecialMarkerEnabled = true;
		}

		return;
	}

	if(m_byteObjectiveVehicle && m_bSpecialMarkerEnabled)
	{
		if(m_dwMarkerID)
		{
			ScriptCommand(&disable_marker, m_dwMarkerID);
			m_bSpecialMarkerEnabled = false;
			m_dwMarkerID = 0;
		}
	}

	if(GetDistanceFromLocalPlayerPed() < 200.0f && !IsOccupied())
	{
		if(!m_dwMarkerID)
		{
			// show
			ScriptCommand(&tie_marker_to_car, m_dwGTAId, 1, 2, &m_dwMarkerID);
			ScriptCommand(&set_marker_color, m_dwMarkerID, 1004);
		}
	}

	else if(IsOccupied() || GetDistanceFromLocalPlayerPed() >= 200.0f)
	{
		// remove
		if(m_dwMarkerID)
		{
			ScriptCommand(&disable_marker, m_dwMarkerID);
			m_dwMarkerID = 0;
		}
	}
}

void CVehicle::UpdateDamageStatus(VEHICLEID VehicleID, uint32_t dwPanelDamage, uint32_t dwDoorDamage, uint8_t byteLightDamage)
{
	Log("UpdateDamageStatus");

	RakNet::BitStream bsSend;
	bsSend.Write(m_dwGTAId);
	bsSend.Write(dwPanelDamage);
	bsSend.Write(dwDoorDamage);
	bsSend.Write(byteLightDamage);
	pNetGame->GetRakClient()->RPC(&RPC_DamageVehicle, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, 0);
}

void CVehicle::SetWheelPopped(uint32_t wheelid, uint32_t popped)
{
	
}

uint8_t CVehicle::GetWheelPopped(uint32_t wheelid)
{
	
}

void CVehicle::SetDoorState(int iState)
{
	if(iState) 
	{
		m_pVehicle->dwDoorsLocked = 2;
		m_bDoorsLocked = true;
	} 
	else 
	{
		m_pVehicle->dwDoorsLocked = 0;
		m_bDoorsLocked = false;
	}
}

bool CVehicle::GetDoorState()
{
	if(m_pVehicle->dwDoorsLocked == 4)
		return 1; 
			else return 0;
}

void CVehicle::SetEngineState(bool bState)
{
	if(!bState)
	{
		ScriptCommand(&turn_car_engine, m_dwGTAId, 0);
	}else{
		ScriptCommand(&turn_car_engine, m_dwGTAId, 1);
	}
}

void CVehicle::SetLightsState(bool bState)
{
	if(!bState)
	{
		ScriptCommand(&set_car_lights_on, m_dwGTAId, 0);
		ScriptCommand(&force_car_lights, m_dwGTAId, 1);
		m_bLightIsOn = 0;
	}else{
		ScriptCommand(&set_car_lights_on, m_dwGTAId, 1);
		ScriptCommand(&force_car_lights, m_dwGTAId, 2);
		m_bLightIsOn = 1;
	}
}

bool CVehicle::AreLightsEnabled()
{
	return (m_bLightIsOn == 1);
}

void CVehicle::SetSirenOn(bool bState)
{
	if(!bState)
	{
		ScriptCommand(&switch_car_siren, m_dwGTAId, 0);
		m_bSirenOn = 0;
	}else{
		ScriptCommand(&switch_car_siren, m_dwGTAId, 1);
		m_bSirenOn = 1;
	}
}

bool CVehicle::IsSirenOn()
{
	return (m_bSirenOn == 1);
}

void CVehicle::SetObjective(uint8_t byteObjective)
{
	if(byteObjective)
	{
		m_byteObjectiveVehicle = 1;
		m_bSpecialMarkerEnabled = false;
	}
}

void CVehicle::SetAlarmState(bool bState)
{
	if(!bState)
	{
		m_bAlarmOn = 0;
	}else{
		m_bAlarmOn = 1;
	}
}

void CVehicle::SetBonnetState(bool bState)
{
	if(!bState)
	{
		m_bBonnet = 0;
	}else{
		m_bBonnet = 1;
	}
}

void CVehicle::SetBootState(bool bState)
{
	if(!bState)
	{
		m_bBoot = 0;
	}else{
		m_bBoot = 1;
	}
}

float CVehicle::GetTrainSpeed()
{
	return m_pVehicle->fTrainSpeed;
}

void CVehicle::SetTrainSpeed(float fSpeed)
{
	m_pVehicle->fTrainSpeed = fSpeed;
}

unsigned int CVehicle::GetVehicleSubtype()
{
	if(m_pVehicle)
	{
		if(m_pVehicle->entity.vtable == g_libGTASA+0x5CC9F0) // 0x871120
		{
			return VEHICLE_SUBTYPE_CAR;
		}
		else if(m_pVehicle->entity.vtable == g_libGTASA+0x5CCD48) // 0x8721A0
		{
			return VEHICLE_SUBTYPE_BOAT;
		}
		else if(m_pVehicle->entity.vtable == g_libGTASA+0x5CCB18) // 0x871360
		{
			return VEHICLE_SUBTYPE_BIKE;
		}
		else if(m_pVehicle->entity.vtable == g_libGTASA+0x5CD0B0) // 0x871948
		{
			return VEHICLE_SUBTYPE_PLANE;
		}
		else if(m_pVehicle->entity.vtable == g_libGTASA+0x5CCE60) // 0x871680
		{
			return VEHICLE_SUBTYPE_HELI;
		}
		else if(m_pVehicle->entity.vtable == g_libGTASA+0x5CCC30) // 0x871528
		{
			return VEHICLE_SUBTYPE_PUSHBIKE;
		}
		else if(m_pVehicle->entity.vtable == g_libGTASA+0x5CD428) // 0x872370
		{
			return VEHICLE_SUBTYPE_TRAIN;
		}
	}

	return 0;
}