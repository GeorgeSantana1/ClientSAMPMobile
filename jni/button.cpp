#include "main.h"
#include "game/game.h"
#include "net/netgame.h"
#include "game/common.h"
#include "gui/gui.h"
#include "button.h"
#include "vendor/imgui/imgui_internal.h"
#include "keyboard.h"
#include "chatwindow.h"
#include "net/vehiclepool.h"
#include "settings.h"
#include "dialog.h"
#include "net/playerpool.h"
#include "net/localplayer.h"
#include "scoreboard.h"

extern CGUI *pGUI;
extern CGame *pGame;
extern CNetGame *pNetGame;
extern CKeyBoard *pKeyBoard;
extern CChatWindow *pChatWindow;
extern CSettings *pSettings;
extern CDialogWindow *pDialogWindow;
extern CScoreBoard *pScoreBoard;

CButton::CButton()
{
	m_bShowItem = false;
	m_bIsActive = false;
	m_bPassengerDriveByModes = false;
}

CButton::~CButton()
{
	m_bShowItem = false;
	m_bIsActive = false;
	m_bPassengerDriveByModes = false;
}

void CButton::Show(bool bShow)
{
	m_bShowItem = false;
	
	if(pDialogWindow->m_bIsActive)
	{
		m_bIsActive = false;
	}else{
		m_bIsActive = bShow;
	}
	
	if(bShow != true)
	{
		m_bShowItem = false;
		m_bIsActive = false;
		m_bPassengerDriveByModes = false;
	}
}

void CButton::Clear()
{
	m_bShowItem = false;
	m_bIsActive = false;
	m_bPassengerDriveByModes = false;
}

void CButton::SendKeyUsing(int keyId)
{
    CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
    CLocalPlayer *pLocalPlayer = 0;
    if(pPlayerPool) pLocalPlayer = pPlayerPool->GetLocalPlayer();
	
    switch(keyId)
	{
        case 0:
		pGame->FindPlayerPed()->StartPassengerDriveByMode();
		m_bPassengerDriveByModes = true;
		break;
		
		case 1:
		pGame->FindPlayerPed()->ExitCurrentVehicle();
		m_bPassengerDriveByModes = false;
		break;
		
		case 2:
		pChatWindow->AddClientMessage(dwColor, "Fitur ini sedang ditutup, Mohon kesabaran untuk tersedianya fitur ini.");
		//pScoreBoard->Show(true);
		break;
		
		case 3:
		pScoreBoard->Show(false);
		break;
		
		case 4:
		m_bShowItem = true;
		break;
		
		case 5:
		m_bShowItem = false;
		break;
		
		case 6:
		pLocalPlayer->HandlePassengerEntryByCommand();
		break;
	}
}

void CButton::Render()
{
	if(!m_bIsActive) return;

	ImGuiIO &io = ImGui::GetIO();

    ImGui::GetStyle().ButtonTextAlign = ImVec2(0.5f, 0.5f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

    ImGui::Begin("Button", nullptr, 
    ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | 
    ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings);
	
	PLAYERID playerId;
	
    CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
    CLocalPlayer *pLocalPlayer = 0;
	CRemotePlayer* pPlayer = pPlayerPool->GetAt(playerId);
	
    if(pPlayerPool) pLocalPlayer = pPlayerPool->GetLocalPlayer();
	
	dwColor = pLocalPlayer->GetPlayerColor();
	
    m_fButWidth = ImGui::CalcTextSize("QWERTYUIO").x;
	m_fButWidth2 = ImGui::CalcTextSize("QWER").x;
    m_fButHeight = 45;
	
	ImGuiStyle style;
	style.Colors[ImGuiCol_FrameBg] = ImGui::GetStyle().Colors[ImGuiCol_FrameBgActive];
	
	if(m_bShowItem == true)
	{
		if(pGame->FindPlayerPed()->IsAPassenger())
		{
			if(!m_bPassengerDriveByModes)
			{
				if(!pScoreBoard->m_bIsActive)
				{
					if(ImGui::Button("TAB", ImVec2(m_fButWidth, m_fButHeight)))
					{
						SendKeyUsing(2);
					}
				}else{
					if(ImGui::Button("X", ImVec2(m_fButWidth, m_fButHeight)))
					{
						SendKeyUsing(3);
					}
				}
				
				ImGui::SameLine(0, 5);
			
				if(pGame->FindPlayerPed()->GetCurrentWeapon())
				{
					if(ImGui::Button("H", ImVec2(m_fButWidth, m_fButHeight)))
					{
						SendKeyUsing(0);
					}
				}
				
				ImGui::SameLine(0, 5);
				
				if(ImGui::Button("<<", ImVec2(m_fButWidth2, m_fButHeight)))
				{
					SendKeyUsing(5);
				}
			}else{
				if(!pScoreBoard->m_bIsActive)
				{
					if(ImGui::Button("TAB", ImVec2(m_fButWidth, m_fButHeight)))
					{
						SendKeyUsing(2);
					}
				}else{
					if(ImGui::Button("X", ImVec2(m_fButWidth, m_fButHeight)))
					{
						SendKeyUsing(3);
					}
				}
				
				ImGui::SameLine(0, 5);
				
				if(ImGui::Button("EXIT", ImVec2(m_fButWidth, m_fButHeight)))
				{
					pChatWindow->AddClientMessage(dwColor, "Untuk kembali kemode biasa silahkan tekan Icon Senjata Atau Icon Tinju");
					SendKeyUsing(1);
				}
				
				ImGui::SameLine(0, 5);
				
				if(ImGui::Button("<<", ImVec2(m_fButWidth2, m_fButHeight)))
				{
					SendKeyUsing(5);
				}
			}
		}else{
			if(pGame->FindPlayerPed()->IsInVehicle())
			{
				if(!pScoreBoard->m_bIsActive)
				{
					if(ImGui::Button("TAB", ImVec2(m_fButWidth, m_fButHeight)))
					{
						SendKeyUsing(2);
					}
				}else{
					if(ImGui::Button("X", ImVec2(m_fButWidth, m_fButHeight)))
					{
						SendKeyUsing(3);
					}
				}
				
				ImGui::SameLine(0, 5);
				
				if(ImGui::Button("<<", ImVec2(m_fButWidth2, m_fButHeight)))
				{
					SendKeyUsing(5);
				}
			}else{
				if(!pScoreBoard->m_bIsActive)
				{
					if(ImGui::Button("TAB", ImVec2(m_fButWidth, m_fButHeight)))
					{
						SendKeyUsing(2);
					}
				}else{
					if(ImGui::Button("X", ImVec2(m_fButWidth, m_fButHeight)))
					{
						SendKeyUsing(3);
					}
				}
				
				ImGui::SameLine(0, 5);
				
				if(ImGui::Button("G", ImVec2(m_fButWidth, m_fButHeight)))
				{
					SendKeyUsing(6);
				}	
				
				ImGui::SameLine(0, 5);
				
				if(ImGui::Button("<<", ImVec2(m_fButWidth2, m_fButHeight)))
				{
					SendKeyUsing(5);
				}
			}
		}
	}else{
		if(ImGui::Button(">>", ImVec2(m_fButWidth2, m_fButHeight)))
		{
			SendKeyUsing(4);
		}
	}
    
    ImGui::SetWindowSize(ImVec2(-1, -1));
    ImVec2 size = ImGui::GetWindowSize();
    
    ImGui::SetWindowPos(ImVec2(pGUI->ScaleX(10), pGUI->ScaleY(350)));
    ImGui::End();
}