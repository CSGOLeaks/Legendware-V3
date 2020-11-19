// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "..\hooks.hpp"
#include "..\..\cheats\misc\prediction_system.h"
#include "..\..\cheats\lagcompensation\local_animations.h"
#include "..\..\cheats\misc\misc.h"

using RunCommand_t = void(__thiscall*)(void*, player_t*, CUserCmd*, IMoveHelper*);

void __fastcall hooks::hooked_runcommand(void* ecx, void* edx, player_t* player, CUserCmd* m_pcmd, IMoveHelper* move_helper)
{
	static auto original_fn = prediction_hook->get_func_address <RunCommand_t> (19);


	original_fn(ecx, player, m_pcmd, move_helper);

	player->m_vphysicsCollisionState() = 0;
}

using InPrediction_t = bool(__thiscall*)(void*);

bool __stdcall hooks::hooked_inprediction()
{
	static auto original_fn = prediction_hook->get_func_address <InPrediction_t> (14);
	static auto maintain_sequence_transitions = util::FindSignature(crypt_str("client.dll"), crypt_str("84 C0 74 17 8B 87"));

	if (g_cfg.ragebot.enable && (uintptr_t)_ReturnAddress() == maintain_sequence_transitions)
		return true;

	return original_fn(m_prediction());
}

using WriteUsercmdDeltaToBuffer_t = bool(__thiscall*)(void*, int, void*, int, int, bool);
void WriteUser—md(void* buf, CUserCmd* incmd, CUserCmd* outcmd);

bool __fastcall hooks::hooked_writeusercmddeltatobuffer(void* ecx, void* edx, int slot, bf_write* buf, int from, int to, bool is_new_command)
{
	static auto original_fn = client_hook->get_func_address <WriteUsercmdDeltaToBuffer_t>(24);
	return original_fn(ecx, slot, buf, from, to, is_new_command);
}

void WriteUser—md(void* buf, CUserCmd* incmd, CUserCmd* outcmd) 
{

}