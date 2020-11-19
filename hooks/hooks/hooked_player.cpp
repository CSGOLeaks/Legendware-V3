// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "..\hooks.hpp"
#include "..\..\cheats\lagcompensation\local_animations.h"
#include "..\..\cheats\misc\prediction_system.h"

_declspec(noinline)bool hooks::setupbones_detour(void* ecx, matrix3x4_t* bone_world_out, int max_bones, int bone_mask, float current_time)
{
	if (g_ctx.globals.setuping_bones)
		return ((SetupBonesFn)original_setupbones)(ecx, bone_world_out, max_bones, bone_mask, current_time);

	auto player = reinterpret_cast <player_t*> ((uintptr_t)ecx - 0x4);

	if (player == g_ctx.local())
		return ((SetupBonesFn)original_setupbones)(ecx, bone_world_out, max_bones, bone_mask, current_time);

	if (!g_cfg.ragebot.enable || !g_ctx.local()->is_alive())
		return ((SetupBonesFn)original_setupbones)(ecx, bone_world_out, max_bones, bone_mask, current_time);

	if (!player->valid(false))
		return ((SetupBonesFn)original_setupbones)(ecx, bone_world_out, max_bones, bone_mask, current_time);

	if (bone_world_out)
	{
		if (max_bones > MAXSTUDIOBONES)
			max_bones = MAXSTUDIOBONES;

		memcpy(bone_world_out, player->m_CachedBoneData().Base(), max_bones * sizeof(matrix3x4_t));
	}

	return true;
}

bool __fastcall hooks::hooked_setupbones(void* ecx, void* edx, matrix3x4_t* bone_world_out, int max_bones, int bone_mask, float current_time)
{
	return setupbones_detour(ecx, bone_world_out, max_bones, bone_mask, current_time);
}

_declspec(noinline)void hooks::standardblendingrules_detour(player_t* player, int i, CStudioHdr* hdr, Vector* pos, Quaternion* q, float curtime, int boneMask)
{
	auto backup_effects = player->m_fEffects();

	if (player == g_ctx.local())
		player->m_fEffects() |= 8;

	((StandardBlendingRulesFn)original_standardblendingrules)(player, hdr, pos, q, curtime, boneMask);

	if (player == g_ctx.local())
		player->m_fEffects() = backup_effects;
}

void __fastcall hooks::hooked_standardblendingrules(player_t* player, int i, CStudioHdr* hdr, Vector* pos, Quaternion* q, float curtime, int boneMask)
{
	return standardblendingrules_detour(player, i, hdr, pos, q, curtime, boneMask);
}

_declspec(noinline)void hooks::doextrabonesprocessing_detour(player_t* player, CStudioHdr* hdr, Vector* pos, Quaternion* q, const matrix3x4_t& matrix, uint8_t* bone_list, void* context)
{

}

void __fastcall hooks::hooked_doextrabonesprocessing(player_t* player, void* edx, CStudioHdr* hdr, Vector* pos, Quaternion* q, const matrix3x4_t& matrix, uint8_t* bone_list, void* context)
{
	return doextrabonesprocessing_detour(player, hdr, pos, q, matrix, bone_list, context);
}

_declspec(noinline)void hooks::updateclientsideanimation_detour(player_t* player, uint32_t i)
{
	if (g_ctx.globals.updating_animation)
		return ((UpdateClientSideAnimationFn)original_updateclientsideanimation)(player, i);

	if (player == g_ctx.local())
		return ((UpdateClientSideAnimationFn)original_updateclientsideanimation)(player, i);

	if ((!g_cfg.ragebot.enable || !g_ctx.local()->is_alive()) && player != g_ctx.local())
		return ((UpdateClientSideAnimationFn)original_updateclientsideanimation)(player, i);

	if (!player->valid(false))
		return ((UpdateClientSideAnimationFn)original_updateclientsideanimation)(player, i);
}

void __fastcall hooks::hooked_updateclientsideanimation(player_t* player, uint32_t i)
{
	return updateclientsideanimation_detour(player, i);
}

_declspec(noinline)void hooks::physicssimulate_detour(player_t* player)
{
	auto simulation_tick = *(int*)(uintptr_t(player) + 0x2AC);

	if (player != g_ctx.local() || !g_ctx.local()->is_alive() || m_globals()->m_tickcount == simulation_tick)
	{
		((PhysicsSimulateFn)original_physicssimulate)(player);
		return;
	}

	engineprediction::get().restore_netvars();
	((PhysicsSimulateFn)original_physicssimulate)(player);
	engineprediction::get().store_netvars();
}

void __fastcall hooks::hooked_physicssimulate(player_t* player)
{
	return physicssimulate_detour(player);
}

_declspec(noinline)void hooks::modifyeyeposition_detour(c_baseplayeranimationstate* state, Vector& position)
{
	if (state && g_ctx.globals.should_modify_eye_position)
		return ((ModifyEyePositionFn)original_modifyeyeposition)(state, position);
}

void __fastcall hooks::hooked_modifyeyeposition(c_baseplayeranimationstate* state, void* edx, Vector& position)
{
	return modifyeyeposition_detour(state, position);
}

_declspec(noinline)void hooks::calcviewmodelbob_detour(player_t* player, Vector& position)
{
	if (!g_cfg.esp.removals[REMOVALS_LANDING_BOB] || player != g_ctx.local() || !g_ctx.local()->is_alive())
		return ((CalcViewmodelBobFn)original_calcviewmodelbob)(player, position);
}

void __fastcall hooks::hooked_calcviewmodelbob(player_t* player, void* edx, Vector& position)
{
	return calcviewmodelbob_detour(player, position);
}

bool __fastcall hooks::hooked_shouldskipanimframe()
{
	return false;
}

int hooks::processinterpolatedlist()
{
	static auto allow_extrapolation = *(bool**)(util::FindSignature(crypt_str("client.dll"), crypt_str("A2 ? ? ? ? 8B 45 E8")) + 0x1);

	if (allow_extrapolation)
		*allow_extrapolation = false;

	return ((ProcessInterpolatedListFn)original_processinterpolatedlist)();
}