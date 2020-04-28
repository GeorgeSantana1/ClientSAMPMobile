#include "main.h"
#include "game/game.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "scoreboard.h"
#include "dialog.h"
#include "settings.h"
#include "keyboard.h"
#include "vendor/imgui/imgui_internal.h"

extern CGUI *pGUI;
extern CGame *pGame;
extern CNetGame *pNetGame;
extern CSettings *pSettings;
extern CDialogWindow *pDialogWindow;

CScoreBoard::CScoreBoard()
{
	m_fSizeFont = pGUI->ScaleY( pSettings->Get().fFontSize );
	m_bIsActive = false;
    szInfo = " ";
    playersCount = 0;
    out = false;
}

CScoreBoard::~CScoreBoard()
{
	m_bIsActive = false;
    szInfo = " ";
    playersCount = 0;
    out = false;
}

void CScoreBoard::Show(bool bShow)
{
	if(pGame) 
        pGame->FindPlayerPed()->TogglePlayerControllable(!bShow);
	
    if(bShow != false) out = false;
	
	m_bIsActive = bShow;
}

void CScoreBoard::Clear()
{
	m_bIsActive = false;
    szInfo = " ";
    playersCount = 0;
    out = false;
}

bool ProcessInlineHexColor(const char* start, const char* end, ImVec4& color);

void TextColors(const char* fmt, ...)
{
	char tempStr[4096];

	va_list argPtr;
	va_start(argPtr, fmt);
	vsnprintf(tempStr, sizeof(tempStr), fmt, argPtr);
	va_end(argPtr);
	tempStr[sizeof(tempStr) - 1] = '\0';

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	CLocalPlayer *pLocalPlayer = pPlayerPool->GetLocalPlayer();
	
	bool pushedColorStyle = false;
	const char* textStart = tempStr;
	const char* textCur = tempStr;
	uint32_t dwColor = pLocalPlayer->GetPlayerColor();
	ImColor color = ImColor(dwColor);
	
	while(textCur < (tempStr + sizeof(tempStr)) && *textCur != '\0')
	{
		if (*textCur == '{')
		{
			// Print accumulated text
			if (textCur != textStart)
			{
				ImGui::TextUnformatted(textStart, textCur);
				ImGui::SameLine(0.0f, 0.0f);
			}

			// Process color code
			const char* colorStart = textCur + 1;
			do
			{
				++textCur;
			} while (*textCur != '\0' && *textCur != '}');

			// Change color
			if (pushedColorStyle)
			{
				ImGui::PopStyleColor();
				pushedColorStyle = false;
			}

			ImVec4 textColor;
			if (ProcessInlineHexColor(colorStart, textCur, textColor))
			{
				color = textColor;
				ImGui::PushStyleColor(ImGuiCol_Text, (uint32_t)color);
				pushedColorStyle = true;
			}

			textStart = textCur + 1;
		}
		else if (*textCur == '\n')
		{
			// Print accumulated text an go to next line
			ImGui::TextUnformatted(textStart, textCur);
			textStart = textCur + 1;
		}

		++textCur;
	}

	if (textCur != textStart)
		ImGui::TextUnformatted(textStart, textCur);
	else
		ImGui::NewLine();

	if(pushedColorStyle)
		ImGui::PopStyleColor();
}

void TextWithColors(const char* fmt, ...);

int lastPlayerCount = 0;
void CScoreBoard::Render()
{
	if(!m_bIsActive) return;

    CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();

	ImGuiIO &io = ImGui::GetIO();
    
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	
	PLAYERID pID;
	
    char motd[256];
	char* playername = (char*)pPlayerPool->GetLocalPlayerName();
	int playerscore = (int)pPlayerPool->GetLocalPlayerScore();
	uint32_t playerping = (uint32_t)pPlayerPool->GetLocalPlayerPing();
	int max_name = MAX_PLAYER_NAME;
	int max_players = MAX_PLAYERS;
	int max_score = playerscore; 

    if(pNetGame) sprintf(motd, "> %.64s          Players: %i", pNetGame->m_szHostName, playersCount);

    ImGui::Begin("ScoreBoard", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
	
	TextWithColors(motd);
	
    ImGui::ItemSize( ImVec2(0, 5) );
	
	if(m_fSizeFont >= 20 && m_fSizeFont <= 20) m_fSizeFont = 20;
	
    ImGui::Text("  id");
	
	ImGui::SameLine(0, 150);
	
	ImGui::Text("name");

	ImGui::SameLine(0, 250);
	
	ImGui::Text("score");
	
	ImGui::SameLine(0, 150);
	
	ImGui::Text("ping");
	
    ImGui::BeginChild("playersChild", ImVec2(750, 440), true, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	
	if(pPlayerPool)
	{	
		TextColors("%d", pPlayerPool->GetLocalPlayerID());
			
		ImGui::SameLine(0, 0);
		if(playersCount > 0) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 150);
		else if(playersCount > 10) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 150);
		else if(playersCount > 100) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 150);
		else if(playersCount > 1000) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 150);
		TextColors("%s", playername);
		
		ImGui::SameLine(0, 0);
		if(max_name ==  1) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 480);
		else if(max_name ==  2) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 470);
		else if(max_name ==  3) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 460);
		else if(max_name ==  4) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 450);
		else if(max_name ==  5) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 440);
		else if(max_name ==  6) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 430);
		else if(max_name ==  7) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 420);
		else if(max_name ==  8) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 410);
		else if(max_name ==  9) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 400);
		else if(max_name ==  10) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 390);
		else if(max_name ==  11) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 380);
		else if(max_name ==  12) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 370);
		else if(max_name ==  13) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 360);
		else if(max_name ==  14) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 350);
		else if(max_name ==  15) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 340);
		else if(max_name ==  16) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 330);
		else if(max_name ==  17) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 320);
		else if(max_name ==  18) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 310);
		else if(max_name ==  19) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 300);
		else if(max_name ==  20) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 290);
		else if(max_name ==  21) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 280);
		else if(max_name ==  22) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 270);
		else if(max_name ==  23) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 260);
		else if(max_name ==  24) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 250);
		ImGui::Text("%d", playerscore);
		
		ImGui::SameLine(0, 0);
		if(max_score >= 0) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 150);
		else if(max_score >= 10) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 140);
		else if(max_score >= 100) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 130);
		else if(max_score >= 1000) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 120);
		else if(max_score >= 10000) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 110);
		else if(max_score >= 100000) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 100);
		ImGui::Text("%d", playerping);
	}
	
    char szBuf[4096] = "";

    for(PLAYERID playerId = 0; playerId < playersCount; playerId++)
    {
        if(pPlayerPool->GetSlotState(playerId) == true)
        {
            char* pName = (char*)pPlayerPool->GetPlayerName(playerId);
			int pScore = (int)pPlayerPool->GetPlayerScore(playerId);
			uint32_t pPing = (uint32_t)pPlayerPool->GetPlayerPing(playerId);
			max_score = pScore;
			
			TextColors("%d", playerId);
			
			ImGui::SameLine(0, 0);
			if(playersCount > 0) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 150);
			else if(playersCount > 10) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 150);
			else if(playersCount > 100) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 150);
			else if(playersCount > 1000) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 150);
			TextColors("%s", pName);
		
			ImGui::SameLine(0, 0);
			if(max_name ==  1) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 480);
			else if(max_name ==  2) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 470);
			else if(max_name ==  3) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 460);
			else if(max_name ==  4) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 450);
			else if(max_name ==  5) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 440);
			else if(max_name ==  6) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 430);
			else if(max_name ==  7) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 420);
			else if(max_name ==  8) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 410);
			else if(max_name ==  9) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 400);
			else if(max_name ==  10) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 390);
			else if(max_name ==  11) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 380);
			else if(max_name ==  12) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 370);
			else if(max_name ==  13) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 360);
			else if(max_name ==  14) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 350);
			else if(max_name ==  15) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 340);
			else if(max_name ==  16) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 330);
			else if(max_name ==  17) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 320);
			else if(max_name ==  18) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 310);
			else if(max_name ==  19) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 300);
			else if(max_name ==  20) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 290);
			else if(max_name ==  21) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 280);
			else if(max_name ==  22) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 270);
			else if(max_name ==  23) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 260);
			else if(max_name ==  24) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 250);
			ImGui::Text("%d", pScore);
		
			ImGui::SameLine(0, 0);
			if(max_score >= 0) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 150);
			else if(max_score >= 10) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 140);
			else if(max_score >= 100) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 130);
			else if(max_score >= 1000) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 120);
			else if(max_score >= 10000) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 110);
			else if(max_score >= 100000) ImGui::SetCursorPosX(ImGui::GetCursorPos().x + 100);
			ImGui::Text("%d", pPing);
        }
    }
	
    ImGui::EndChild();

    ImGui::SetWindowSize(ImVec2(-1, -1));
    ImVec2 size = ImGui::GetWindowSize();
    ImGui::SetWindowPos( ImVec2( ((io.DisplaySize.x - size.x)/2), ((io.DisplaySize.y - size.y)/2) ) );
    ImGui::End();
    ImGui::PopStyleVar();
}