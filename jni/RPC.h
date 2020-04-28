#pragma once

class CRPC
{
public:
	CRPC();
	~CRPC();

	void Render();
	void Clear();
	void SetRPCID(int iRPCID);
	int GetRPCID();
	void Show(bool bShow);
	
public:
	bool		m_bIsActive;
	int 		rpcid;
	int 		rpcupdate;
	bool 		oldShow;
	int 		rpcupdateold;
};