#pragma once
#include "vendor/imgui/imgui.h"

enum eTouchType
{
	TOUCH_POP = 1,
	TOUCH_PUSH = 2,
	TOUCH_MOVE = 3
};

class CGUI
{
public:
	CGUI();
	~CGUI();

	void Render();

	float ScaleX(float x) { return m_vecScale.x * x; }
	float ScaleY(float y) { return m_vecScale.y * y; }
	ImFont* GetFont() { return m_pFont; }
	ImFont* GetSampFont() { return m_pSampFont; }
	float GetFontSize() { return m_fFontSize; }

	bool OnTouchEvent(int type, bool multi, int x, int y);

	void RenderText(ImVec2& pos, ImU32 col, bool bOutline, const char* text_begin, const char* text_end = nullptr);

private:
	void RenderRakNetStatistics();
	void RenderVersion();
	void RenderTime();
	void SetupStyleColors();

private:
 	char 	 	path[0xFF];

	ImFont* 	m_pFont;
	ImFont*		m_pSampFont;

	ImVec2		m_vecScale;
	float 		m_fFontSize;

	bool 		m_bMousePressed;
	ImVec2		m_vecMousePos;

	bool		m_bNeedClearMousePos;
	
	uint32_t	m_dwLastTotalBytesSent;
	uint32_t	m_dwLastTotalBytesRecv;
	uint32_t 	m_dwLastUpdateTick;
	uint32_t 	m_dwBPSUpload;
	uint32_t 	m_dwBPSDownload;
	
	float 		fDown;
	float 		fUp;
	
	char  		buffer[80];
	char 		buff[2048];
	
	time_t  	seconds;
	char*  	  	format;
	
	char 		szSvrDispBuf[10000];
	char 		szSvrStatBuf[10000];
};