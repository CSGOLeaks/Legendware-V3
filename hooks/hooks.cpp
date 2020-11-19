// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.hpp"

#include <tchar.h>
#include <iostream>
#include <d3d9.h>
#include <dinput.h>

#include "..\cheats\misc\logs.h"
#include "..\cheats\misc\misc.h"
#include "..\cheats\visuals\other_esp.h"
#include "..\cheats\visuals\radar.h"
#include "../ImGui/imgui_freetype.h"

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")
#pragma comment(lib, "freetype.lib")

#include <shlobj.h>
#include <shlwapi.h>
#include <thread>
#include "..\cheats\menu.h"
#include "..\cheats\imgui_on_render.h"
#include "../Bytesa.h"

auto _visible = true;

auto rage_weapon = 0;
auto legit_weapon = 0;

auto itemIndex = 0;


static bool d3d_init = false;

namespace INIT
{
	HMODULE Dll;
	HWND Window;
	WNDPROC OldWindow;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern IMGUI_IMPL_API LRESULT ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace hooks
{
	bool menu_open = false;
	bool input_shouldListen = false;

	ButtonCode_t* input_receivedKeyval;

	LRESULT __stdcall Hooked_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		static auto is_down = true;
		static auto is_clicked = false;

		if (GetAsyncKeyState(VK_INSERT))
		{
			is_clicked = false;
			is_down = true;
		}
		else if (!GetAsyncKeyState(VK_INSERT) && is_down)
		{
			is_clicked = true;
			is_down = false;
		}
		else
		{
			is_clicked = false;
			is_down = false;
		}

		if (is_clicked)
		{
			menu_open = !menu_open;

			if (menu_open && g_ctx.available())
			{
				if (g_ctx.globals.current_weapon != -1)
				{
					if (g_cfg.ragebot.enable)
						rage_weapon = g_ctx.globals.current_weapon;
					else if (g_cfg.legitbot.enabled)
						legit_weapon = g_ctx.globals.current_weapon;
				}

				if (g_cfg.skins.enable && g_ctx.local()->is_alive())
				{
					auto weapon = g_ctx.local()->m_hActiveWeapon().Get();

					if (weapon->is_knife())
						itemIndex = 0;
					else
					{
						for (auto i = 2; i < 36; i++)
						{
							if (weapon->m_iItemDefinitionIndex() == game_data::weapon_names[i].definition_index)
							{
								itemIndex = i;
								break;
							}
						}
					}
				}
			}
		}

		auto pressed_buttons = false;
		auto pressed_menu_key = uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP || uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONUP || uMsg == WM_MOUSEWHEEL;

		if (g_ctx.available() && g_ctx.local()->is_alive() && g_ctx.get_command() && !pressed_menu_key && !g_ctx.globals.focused_on_input && !ImGui::GetIO().WantTextInput)
			pressed_buttons = true;

		if (!pressed_buttons && d3d_init && menu_open && ImGui_ImplDX9_WndProcHandler(hWnd, uMsg, wParam, lParam) && !input_shouldListen)
			return true;

		if (menu_open && !input_shouldListen)
		{
			switch (uMsg)
			{
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_MOUSEMOVE:
				return false;
			}
		}

		return CallWindowProc(INIT::OldWindow, hWnd, uMsg, wParam, lParam);
	}

	long __stdcall Hooked_EndScene(IDirect3DDevice9* pDevice)
	{
		static auto original_fn = directx_hook->get_func_address <EndSceneFn>(42);

		return original_fn(pDevice);
	}

	void GUI_Init(IDirect3DDevice9* pDevice)
	{
		ImGui::CreateContext();

		ImGui_ImplWin32_Init(INIT::Window);
		ImGui_ImplDX9_Init(pDevice);

		auto& io = ImGui::GetIO();
		auto& style = ImGui::GetStyle();

		style.WindowMinSize = ImVec2(10, 10);

		ImFontConfig m_config;
		m_config.OversampleH = m_config.OversampleV = 3;
		m_config.PixelSnapH = false;

		ImFontConfig m_config_esp;
		m_config.OversampleH = m_config.OversampleV = 3;
		m_config.PixelSnapH = false;
		m_config.RasterizerFlags = ImGuiFreeType::ForceAutoHint;

		static const ImWchar ranges[] =
		{
			0x0020, 0x00FF,
			0x0400, 0x044F,
			0
		};

		char windows_directory[64];
		GetWindowsDirectory(windows_directory, 64);

		auto verdana_directory = (std::string)windows_directory + "\\Fonts\\Verdana.ttf";

		// Menu fonts
		c_menu::get().futura_small = io.Fonts->AddFontFromMemoryTTF((void*)futuramediumc, sizeof(futuramediumc), 15.f, &m_config, ranges);
		c_menu::get().futura = io.Fonts->AddFontFromMemoryTTF((void*)futuramediumc, sizeof(futuramediumc), 17.f, &m_config, ranges);
		c_menu::get().futura_large = io.Fonts->AddFontFromMemoryTTF((void*)futuramediumc, sizeof(futuramediumc), 20.f, &m_config, ranges);
		c_menu::get().gotham = io.Fonts->AddFontFromMemoryTTF((void*)GothamPro, sizeof(GothamPro), 13.f, &m_config, ranges);

		// Icon fonts
		c_menu::get().ico_menu = io.Fonts->AddFontFromMemoryTTF((void*)icomenu, sizeof(icomenu), 20.f, &m_config, io.Fonts->GetGlyphRangesCyrillic());
		c_menu::get().ico_bottom = io.Fonts->AddFontFromMemoryTTF((void*)iconbot, sizeof(iconbot), 20.f, &m_config, io.Fonts->GetGlyphRangesCyrillic());

		// ESP fonts	
		playeresp::get().logs_f = io.Fonts->AddFontFromMemoryTTF((void*)lucida, sizeof(lucida), 10.f, &m_config_esp, ranges);
		playeresp::get().esp_f = io.Fonts->AddFontFromMemoryTTF((void*)SMALLESTPIXEL, sizeof(SMALLESTPIXEL), 11.f, &m_config_esp, ranges);
		playeresp::get().name_f = io.Fonts->AddFontFromFileTTF(verdana_directory.c_str(), 12.f, &m_config_esp, ranges);
		playeresp::get().weapons_f = io.Fonts->AddFontFromMemoryTTF((void*)undefeated, sizeof(undefeated), 13.f, &m_config_esp, ranges);
		playeresp::get().knifes_f = io.Fonts->AddFontFromMemoryTTF((void*)icomoon, sizeof(icomoon), 13.f, &m_config_esp, ranges);
		playeresp::get().grenades_f = io.Fonts->AddFontFromMemoryTTF((void*)undefeated, sizeof(undefeated), 20.f, &m_config_esp, ranges);
		playeresp::get().indicator_f = io.Fonts->AddFontFromFileTTF(verdana_directory.c_str(), 25.f, &m_config_esp, ranges);
		playeresp::get().dmg_marker_f = io.Fonts->AddFontFromFileTTF(verdana_directory.c_str(), 16.f, &m_config_esp, ranges);

		ImGuiFreeType::BuildFontAtlas(io.Fonts);
		ImGui_ImplDX9_CreateDeviceObjects();

		d3d_init = true;
	}

	long __stdcall hooked_present(IDirect3DDevice9* device, RECT* src_rect, RECT* dest_rect, HWND dest_wnd_override, RGNDATA* dirty_region) {
		static auto original = directx_hook->get_func_address<PresentFn>(17);

		if (!d3d_init)
			GUI_Init(device);

		IDirect3DVertexDeclaration9* vert_dec;
		if (device->GetVertexDeclaration(&vert_dec))
			return original(device, src_rect, dest_rect, dest_wnd_override, dirty_region);

		IDirect3DVertexShader9* vert_shader;
		if (device->GetVertexShader(&vert_shader))
			return original(device, src_rect, dest_rect, dest_wnd_override, dirty_region);
		
		c_menu::get().device = device;

		// setup draw_list
		ImGuiRendering::get().SetupPresent(device);

		// prepare to render
		ImGuiRendering::get().PreRender(device);

		// draw d3d thing$$$
		playeresp::get().imgui_paint();

		// draw menu
		c_menu::get().draw(menu_open);

		// drawlist setup end
		ImGuiRendering::get().PostRender(device);

		// end frames, call render
		ImGuiRendering::get().EndPresent(device);


		device->SetVertexShader(vert_shader);
		device->SetVertexDeclaration(vert_dec);

		return original(device, src_rect, dest_rect, dest_wnd_override, dirty_region);
	}

	long __stdcall Hooked_EndScene_Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		static auto ofunc = directx_hook->get_func_address<EndSceneResetFn>(16);

		if (!d3d_init)
			return ofunc(pDevice, pPresentationParameters);

		ImGuiRendering::get().InvalidateObjects();

		auto hr = ofunc(pDevice, pPresentationParameters);
		if (hr >= 0)
			ImGuiRendering::get().CreateObjects(pDevice);

		return hr;
	}


	DWORD original_getforeignfallbackfontname;
	DWORD original_setupbones;
	DWORD original_doextrabonesprocessing;
	DWORD original_standardblendingrules;
	DWORD original_updateclientsideanimation;
	DWORD original_physicssimulate;
	DWORD original_modifyeyeposition;
	DWORD original_calcviewmodelbob;
	DWORD original_processinterpolatedlist;

	vmthook* directx_hook;
	vmthook* client_hook;
	vmthook* clientstate_hook;
	vmthook* engine_hook;
	vmthook* clientmode_hook;
	vmthook* inputinternal_hook;
	vmthook* renderview_hook;
	vmthook* panel_hook;
	vmthook* modelcache_hook;
	vmthook* materialsys_hook;
	vmthook* modelrender_hook;
	vmthook* surface_hook;
	vmthook* bspquery_hook;
	vmthook* prediction_hook;
	vmthook* trace_hook;
	vmthook* filesystem_hook;
	vmthook* cvar_hook;

	C_HookedEvents hooked_events;
}

void __fastcall hooks::hooked_setkeycodestate(void* thisptr, void* edx, ButtonCode_t code, bool bDown)
{
	static auto original_fn = inputinternal_hook->get_func_address <SetKeyCodeState_t> (91);

	if (input_shouldListen && bDown)
	{
		input_shouldListen = false;

		if (input_receivedKeyval)
			*input_receivedKeyval = code;
	}

	return original_fn(thisptr, code, bDown);
}

void __fastcall hooks::hooked_setmousecodestate(void* thisptr, void* edx, ButtonCode_t code, MouseCodeState_t state)
{
	static auto original_fn = inputinternal_hook->get_func_address <SetMouseCodeState_t> (92);

	if (input_shouldListen && state == BUTTON_PRESSED)
	{
		input_shouldListen = false;

		if (input_receivedKeyval)
			*input_receivedKeyval = code;
	}

	return original_fn(thisptr, code, state);
}