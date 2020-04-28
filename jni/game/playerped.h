#pragma once

class CPlayerPed : public CEntity
{
public:
	CPlayerPed();	// local
	CPlayerPed(uint8_t bytePlayerNumber, int iSkin, float fX, float fY, float fZ, float fRotation); // remote
	~CPlayerPed();

	void Destroy();
	
	ENTITY_TYPE* GetGtaContactEntity();
	
	bool IsOnGround();
	uint32_t GetStateFlags();
	void SetStateFlags(uint32_t dwState);
	// 0.3.7
	bool IsInVehicle();
	// 0.3.7
	bool IsAPassenger();
	bool StartPassengerDriveByMode();
	// 0.3.7
	VEHICLE_TYPE* GetGtaVehicle();
	// 0.3.7
	void RemoveFromVehicleAndPutAt(float fX, float fY, float fZ);
	// 0.3.7
	void SetInitialState();
	void StartJetpack();
	bool IsInJetpackMode();
	void HandsUP();
	bool HasHandsUP();
	void PlayDance(int danceId);
	void StopDance();
	bool IsDance();
	
	void ToggleCellphone(int iOn);
	
	int IsCellphoneEnabled();
	
	// 0.3.7
	void SetHealth(float fHealth);
	void SetArmour(float fArmour);
	// 0.3.7
	float GetHealth();
	float GetArmour();
	// 0.3.7
	void TogglePlayerControllable(bool bToggle);
	// 0.3.7
	void SetModelIndex(unsigned int uiModel);
	
	void SetDead();

	void SetInterior(uint8_t byteID);

	void PutDirectlyInVehicle(int iVehicleID, int iSeat);
	void EnterVehicle(int iVehicleID, bool bPassenger);
	// 0.3.7
	void ExitCurrentVehicle();
	// 0.3.7
	int GetCurrentVehicleID();
	int GetVehicleSeatID();

	ENTITY_TYPE* GetEntityUnderPlayer();

	// 0.3.7
	void RestartIfWastedAt(VECTOR *vecRestart, float fRotation);
	// 0.3.7
	void ForceTargetRotation(float fRotation);
	// 0.3.7
	uint8_t GetActionTrigger();
	void SetActionTrigger(uint8_t byteTrigger);
	// 0.3.7
	bool IsDead();
	uint16_t GetKeys(uint16_t *lrAnalog, uint16_t *udAnalog);
	void SetKeys(uint16_t wKeys, uint16_t lrAnalog, uint16_t udAnalog);
	// 0.3.7
	void SetMoney(int iAmount);
	// 0.3.7
	void ShowMarker(uint32_t iMarkerColorID);
	// 0.3.7
	void HideMarker();
	// 0.3.7
	void SetFightingStyle(int iStyle);
	int GetFightingStyle();
	// 0.3.7
	void SetRotation(float fRotation);
	// 0.3.7
	void ApplyAnimation( char *szAnimName, char *szAnimFile, float fT, int opt1, int opt2, int opt3, int opt4, int iUnk );
	// 0.3.7
	void GetBonePosition(int iBoneID, VECTOR* vecOut);
	// roflan
	void FindDeathReasonAndResponsiblePlayer(PLAYERID *nPlayer);
	
	void DestroyFollowPedTask();
	
	void GiveWeapon(int iWeaponID, int iAmmo);
	
	void SetArmedWeapon(int iWeaponType);
	
	void SetAmmo(uint8_t byteWeapon, uint16_t wordAmmo);
	uint16_t GetAmmo();
	
	void ClearAllWeapons();
	
	uint8_t GetCurrentWeapon();
	
	int GetCurrentCharWeapon();
	
	int GetCurrentWeaponSlot(int iWeaponID);
	
	void DisableAutoAim();
	
	VECTOR GetFullAim();
	
	float GetAimZ();
	
	void ResetDamageEntity();
	
	PED_TYPE * GetGtaActor() { return m_pPed; };

public:
	PED_TYPE*	m_pPed;
	int			m_byteCurrentWeapon;
	int			m_byteFightingStyle;
	int			m_iCellPhoneEnabled;
	int			m_iDanceState;
	int			m_iDanceStyle;
	int			m_iIsJetPack;
	int			m_iHasHandsUP;
	uint8_t		m_bytePlayerNumber;
	uint32_t	m_dwArrow;
	bool		act;
	float 		sHealth;
	uint8_t 	m_byteTeamId;
};