#pragma once

class CButton
{
public:
	CButton();
	~CButton();

	void Render();
	void Clear();
	void Show(bool bShow);

	void SendKeyUsing(int keyId);

public:
	bool	m_bIsActive;
	bool 	m_bPassengerDriveByModes;
	bool 	m_bShowItem;
	
private:
	float	m_fPosX;
	float	m_fPosY;
	float 	m_fSizeX;
	float 	m_fSizeY;
	float 	m_fButWidth;
	float 	m_fButHeight;
	float 	m_fButWidth2;
	uint32_t 	dwColor;
};