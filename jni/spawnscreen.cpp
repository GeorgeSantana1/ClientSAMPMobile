#include "main.h"
#include "gui/gui.h"
#include "spawnscreen.h"
#include "chatwindow.h"
#include "settings.h"
#include "game/game.h"
#include "net/netgame.h"
#include "vendor/imgui/imgui_internal.h"

extern CGUI *pGUI;
extern CGame *pGame;
extern CNetGame *pNetGame;
extern CChatWindow *pChatWindow;
extern CSettings *pSettings;

CSpawnScreen::CSpawnScreen()
{
	Log("Initializing Spawn Screen..");
	m_fPosX = pGUI->ScaleX( pSettings->Get().fSpawnScreenPosX );
	m_fPosY = pGUI->ScaleY( pSettings->Get().fSpawnScreenPosY );
	m_fSizeX = pGUI->ScaleX( pSettings->Get().fSpawnScreenSizeX );
	m_fSizeY = pGUI->ScaleY( pSettings->Get().fSpawnScreenSizeY );
	m_fButWidth = m_fSizeX / 3;
	m_fButHeight = m_fSizeY * 0.9;
	Log("Spawn Screen pos: %f, %f, size: %f, %f", m_fPosX, m_fPosY, m_fSizeX, m_fSizeY);

	m_bEnabled = false;
}

void CSpawnScreen::Show(bool bShow)
{
	if(pGame) 
		pGame->FindPlayerPed()->TogglePlayerControllable(1);
	
	m_bEnabled = bShow;
}

void CSpawnScreen::Render()
{
	if(!m_bEnabled) return;
	
	ImGuiIO &io = ImGui::GetIO();
	
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	CLocalPlayer *pLocalPlayer = 0;
	if(pPlayerPool) pLocalPlayer = pPlayerPool->GetLocalPlayer();

    ImGui::GetStyle().ButtonTextAlign = ImVec2(0.5f, 0.5f);
	
	ImGui::Begin("SpawnScreen", nullptr, 
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | 
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings);
	
    m_fButWidth = ImGui::CalcTextSize("QWERTYUIOPAS").x;
    m_fButHeight = 45;
	
	ImGuiStyle style;
	style.Colors[ImGuiCol_FrameBg] = ImGui::GetStyle().Colors[ImGuiCol_FrameBgActive];
	style.AntiAliasedLines = true;
	style.AntiAliasedFill = true;
	
	if (ImGui::Button("<<", ImVec2(m_fButWidth, m_fButHeight)))
	{
		pLocalPlayer->SendPrevClass();
	}
	ImGui::SameLine(0, 5);
	if (ImGui::Button("Spawn", ImVec2(m_fButWidth, m_fButHeight)))
	{
		pLocalPlayer->SendSpawn();
	}
	ImGui::SameLine(0, 5);
	if (ImGui::Button(">>", ImVec2(m_fButWidth, m_fButHeight)))
	{
		pLocalPlayer->SendNextClass();
	}
	
	ImGui::SetWindowSize(ImVec2(-1, -1));
	ImVec2 size = ImGui::GetWindowSize();
	
	ImGui::SetWindowPos( ImVec2( ((io.DisplaySize.x - size.x)/2), ((io.DisplaySize.y * 0.95) - size.y) ) );
	ImGui::End();
}