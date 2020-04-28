#include "main.h"
#include "game/game.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "vendor/imgui/imgui_internal.h"
#include "keyboard.h"

#include "RPC.h"
#include "settings.h"
#include "timer.hpp"

#include <stdlib.h>
#include <string.h>

extern CGUI *pGUI;
extern CGame *pGame;
extern CNetGame *pNetGame;
extern CKeyBoard *pKeyBoard;
extern CSettings *pSettings;

CRPC::CRPC()
{
	m_bIsActive = false;
	RPC_ScrAttachObjectToVehicle = 1;
	rpcupdate = 1;
	rpcid = 1;
}

CRPC::~CRPC()
{
	RPC_ScrAttachObjectToVehicle = 255;
	rpcupdate = 0;
	rpcid = 0;
}

void CRPC::Show(bool bShow)
{
	m_bIsActive = bShow;
	
	if(bShow != true)
	{
		rpcupdate = 1;
		rpcid = GetRPCID();
	}
	if(oldShow)
	{
		oldShow = true;
	}else{
		oldShow = false;
	}
}

void CRPC::Clear()
{
	m_bIsActive = false;
	RPC_ScrAttachObjectToVehicle = 255;
	rpcupdate = 0;
	rpcid = 0;
	oldShow = false;
}

void CRPC::SetRPCID(int iRPCID)
{
	rpcid = iRPCID;
}

int CRPC::GetRPCID()
{
	return rpcid;
}

void CRPC::Render()
{
	if(!m_bIsActive) return;

	ImGuiIO &io = ImGui::GetIO();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8,8));
	ImGui::GetStyle().ButtonTextAlign = ImVec2(0.5f, 0.5f); // center | center

	ImGui::Begin("RPC ID Search", nullptr, 
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize);

	ImGui::ItemSize( ImVec2(0, pGUI->GetFontSize()/2 + 5) );
	
	ImGui::Text("RPC ID: %d", GetRPCID());
	if(oldShow) ImGui::Text("Prev RPC ID: %d", rpcupdateold);

	if(ImGui::Button("RPC +", ImVec2(200, 50)))
	{
		rpcupdate = (GetRPCID() + 1);
		SetRPCID(rpcupdate);
		RPC_ScrAttachObjectToVehicle = rpcupdate;
		Log("RPCID: %d", rpcid);
	}
	
	if(ImGui::Button("RPC -", ImVec2(200, 50)))
	{
		rpcupdate = (GetRPCID() - 1);
		SetRPCID(rpcupdate);
		RPC_ScrAttachObjectToVehicle = rpcupdate;
		Log("RPCID: %d", rpcid);
	}

	if(ImGui::Button("Save RPC ID", ImVec2(200, 50)))
	{
		oldShow = true;
		rpcupdate = GetRPCID();
		rpcupdateold = GetRPCID();
		SetRPCID(rpcupdate);
		RPC_ScrAttachObjectToVehicle = rpcupdate;
		Log("RPCID Saved: %d", rpcid);
		Show(false);
	}

	ImGui::SetWindowSize(ImVec2(-1, -1));
	ImVec2 size = ImGui::GetWindowSize();
	ImGui::SetWindowPos( ImVec2( ((io.DisplaySize.x - size.x + 435)/2), ((io.DisplaySize.y - size.y)/2)) );
	ImGui::End();

	ImGui::PopStyleVar();
}