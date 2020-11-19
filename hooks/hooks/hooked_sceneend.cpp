// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "..\hooks.hpp"
#include "..\..\cheats\misc\fakelag.h"

using SceneEnd_t = void(__thiscall*)(void*);

void __fastcall hooks::hooked_sceneend(void* ecx, void* edx)
{
	static auto original_fn = renderview_hook->get_func_address <SceneEnd_t> (9);

	if (!g_ctx.local())
		return original_fn(ecx);

	if (!g_cfg.player.type[ENEMY].ragdoll_chams && !g_cfg.player.type[TEAM].ragdoll_chams)
		return original_fn(ecx);

	return original_fn(ecx);
}