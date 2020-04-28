#pragma once

class CScoreBoard
{
public:
	CScoreBoard();
	~CScoreBoard();

	void Render();
	void Clear();
	void Show(bool bShow);

public:
	bool		m_bIsActive;
	
	char*		szInfo;
	int			playersCount;
	bool 		out;
	
private:
	float m_fSizeFont;
};