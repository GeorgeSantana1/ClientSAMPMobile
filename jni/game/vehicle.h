#pragma once

class CVehicle : public CEntity
{
public:
	CVehicle(int iType, float fPosX, float fPosY, float fPosZ, float fRotation = 0.0f, bool bSiren = false);
	~CVehicle();
	
	void AttachTrailer();
	void DetachTrailer();
	void SetTrailer(CVehicle *pTrailer);
	CVehicle* GetTrailer();
	
	void LinkToInterior(int iInterior);
	void SetColor(int iColor1, int iColor2);

	void SetHealth(float fHealth);
	float GetHealth();

	// 0.3.7
	bool IsOccupied();

	void ProcessMarkers();

	// 0.3.7
	void SetInvulnerable(bool bInv);
	// 0.3.7
	bool IsDriverLocalPlayer();
	// 0.3.7
	bool HasSunk();
	
	bool IsRCVehicle();
	
	bool IsATrainPart();

	void RemoveEveryoneFromVehicle();

	void UpdateDamageStatus(VEHICLEID VehicleID, uint32_t dwPanelDamage, uint32_t dwDoorDamage, uint8_t byteLightDamage);
	
	void SetWheelPopped(uint32_t wheelid, uint32_t popped);
	uint8_t GetWheelPopped(uint32_t wheelid);
	void SetDoorState(int iState);
	void SetEngineState(bool bState);
	void SetLightsState(bool bState);
	bool AreLightsEnabled();
	void SetSirenOn(bool bState);
	bool IsSirenOn();
	void SetObjective(uint8_t byteObjective);
	void SetAlarmState(bool bState);
	void SetBonnetState(bool bState);
	void SetBootState(bool bState);
	
	bool GetDoorState();
	float GetTrainSpeed();
	void SetTrainSpeed(float fSpeed);
	
	unsigned int GetVehicleSubtype();

public:
	VEHICLE_TYPE	*m_pVehicle;
	bool 			m_bIsLocked;
	CVehicle		*m_pTrailer;
	uint32_t		m_dwMarkerID;
	bool 			m_bIsInvulnerable;
	bool 			m_bDoorsLocked;
	uint8_t			m_byteObjectiveVehicle; // Is this a special objective vehicle? 0/1
	uint8_t			m_bSpecialMarkerEnabled;


	uint8_t			m_byteColor1;
	uint8_t			m_byteColor2;
	bool 			m_bColorChanged;
	uint8_t			m_bLightIsOn;
	uint8_t			m_bSirenOn;
	uint8_t			m_bAlarmOn;
	uint8_t			m_bBonnet;
	uint8_t			m_bBoot;
};