// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "..\hooks.hpp"

struct RenderableInfo_t
{
	IClientRenderable* m_pRenderable;
	void* m_pAlphaProperty;
	int m_EnumCount;
	int m_nRenderFrame;
	unsigned short m_FirstShadow;
	unsigned short m_LeafList;
	short m_Area;
	uint16_t m_Flags;
	uint16_t m_Flags2;
	Vector m_vecBloatedAbsMins;
	Vector m_vecBloatedAbsMaxs;
	Vector m_vecAbsMins;
	Vector m_vecAbsMaxs;
	int pad;
};

using ListLeavesInBox_t = int(__thiscall*)(void*, const Vector&, const Vector&, unsigned short*, int);

int __fastcall hooks::hooked_listleavesinbox(void* ecx, void* edx, Vector& mins, Vector& maxs, unsigned short* list, int list_max)
{
	static auto original_fn = bspquery_hook->get_func_address <ListLeavesInBox_t> (6);

	if (!g_ctx.local())
		return original_fn(ecx, mins, maxs, list, list_max);

	if (!g_cfg.player.type[ENEMY].chams[PLAYER_CHAMS_VISIBLE] && !g_cfg.player.type[TEAM].chams[PLAYER_CHAMS_VISIBLE] && !g_cfg.player.type[LOCAL].chams[PLAYER_CHAMS_VISIBLE] && !g_cfg.player.fake_chams_enable && !g_cfg.player.backtrack_chams)
		return original_fn(ecx, mins, maxs, list, list_max);

	if (*(uint32_t*)_ReturnAddress() != 0x14244489)
		return original_fn(ecx, mins, maxs, list, list_max);

	auto info = *(RenderableInfo_t**)((uintptr_t)_AddressOfReturnAddress() + 0x14);

	if (!info)
		return original_fn(ecx, mins, maxs, list, list_max);

	if (!info->m_pRenderable)
		return original_fn(ecx, mins, maxs, list, list_max);

	auto e = info->m_pRenderable->GetIClientUnknown()->GetBaseEntity();

	if (!e->is_player())
		return original_fn(ecx, mins, maxs, list, list_max);

	info->m_Flags &= ~0x100;
	info->m_Flags2 |= 0xC0;

	static auto map_min = Vector(-16384.0f, -16384.0f, -16384.0f);
	static auto map_max = Vector(16384.0f, 16384.0f, 16384.0f);

	return original_fn(ecx, map_min, map_max, list, list_max);
}

using GetInt_t = int(__thiscall*)(void*);
int __fastcall hooks::hooked_getint(void* ecx, void* edx)
{
	static auto ofunc = cvar_hook->get_func_address<GetInt_t>(13);

	static auto mat_wireframe = util::FindSignature(crypt_str("client.dll"), crypt_str("85 C0 75 31 8B 46 FC"));
	static auto r_drawmodelstatsoverlay = util::FindSignature(crypt_str("client.dll"), crypt_str("85 C0 75 54 8B 0D ? ? ? ?"));

	if ((uintptr_t)_ReturnAddress() == mat_wireframe)
		return 1;

	if ((uintptr_t)_ReturnAddress() == r_drawmodelstatsoverlay)
		return 1;

	return ofunc(ecx);
}