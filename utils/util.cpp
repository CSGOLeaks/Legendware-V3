// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "util.hpp"
#include "..\cheats\visuals\player_esp.h"
#include "..\cheats\misc\misc.h"
#include "..\cheats\lagcompensation\animation_system.h"
#include "..\cheats\imgui_on_render.h"
#include <thread>

#define INRANGE(x, a, b) (x >= a && x <= b)  //-V1003
#define GETBITS(x) (INRANGE((x & (~0x20)),'A','F') ? ((x & (~0x20)) - 'A' + 0xA) : (INRANGE(x, '0', '9') ? x - '0' : 0)) //-V1003
#define GETBYTE(x) (GETBITS(x[0]) << 4 | GETBITS(x[1]))

namespace util
{
	int epoch_time()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

	uintptr_t find_pattern(const char* module_name, const char* pattern, const char* mask)
	{
		MODULEINFO module_info = {};
		K32GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(module_name), &module_info, sizeof(MODULEINFO));
		const auto address = reinterpret_cast<std::uint8_t*>(module_info.lpBaseOfDll);
		const auto size = module_info.SizeOfImage;
		std::vector < std::pair < std::uint8_t, bool>> signature;
		for (auto i = 0u; mask[i]; i++)
			signature.emplace_back(std::make_pair(pattern[i], mask[i] == 'x'));
		auto ret = std::search(address, address + size, signature.begin(), signature.end(),
			[](std::uint8_t curr, std::pair<std::uint8_t, bool> curr_pattern)
		{
			return (!curr_pattern.second) || curr == curr_pattern.first;
		});
		return ret == address + size ? 0 : std::uintptr_t(ret);
	}

	uint64_t FindSignature(const char* szModule, const char* szSignature)
	{
		MODULEINFO modInfo;
		GetModuleInformation(GetCurrentProcess(), GetModuleHandle(szModule), &modInfo, sizeof(MODULEINFO));

		uintptr_t startAddress = (DWORD)modInfo.lpBaseOfDll;
		uintptr_t endAddress = startAddress + modInfo.SizeOfImage;

		const char* pat = szSignature;
		uintptr_t firstMatch = 0;

		for (auto pCur = startAddress; pCur < endAddress; pCur++)
		{
			if (!*pat)
				return firstMatch;

			if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == GETBYTE(pat))
			{
				if (!firstMatch)
					firstMatch = pCur;

				if (!pat[2])
					return firstMatch;

				if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?')
					pat += 3;
				else
					pat += 2;
			}
			else
			{
				pat = szSignature;
				firstMatch = 0;
			}
		}

		return 0;
	}

	bool visible(const Vector& start, const Vector& end, entity_t* entity, player_t* from)
	{
		trace_t trace;

		Ray_t ray;
		ray.Init(start, end);

		CTraceFilter filter;
		filter.pSkip = from;

		g_ctx.globals.autowalling = true;
		m_trace()->TraceRay(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &trace);
		g_ctx.globals.autowalling = false;

		return trace.hit_entity == entity || trace.fraction == 1.0f; //-V550
	}

	bool is_button_down(int code)
	{
		if (code <= KEY_NONE || code >= KEY_MAX)
			return false;

		if (!m_engine()->IsActiveApp())
			return false;

		if (m_engine()->Con_IsVisible())
			return false;

		static auto cl_mouseenable = m_cvar()->FindVar(crypt_str("cl_mouseenable"));

		if (!cl_mouseenable->GetBool())
			return false;

		return m_inputsys()->IsButtonDown((ButtonCode_t)code);
	}

	void movement_fix(Vector& wish_angle, CUserCmd* m_pcmd)
	{
		Vector view_fwd, view_right, view_up, cmd_fwd, cmd_right, cmd_up;
		auto viewangles = m_pcmd->m_viewangles;
		viewangles.Normalized();

		math::angle_vectors(wish_angle, &view_fwd, &view_right, &view_up);
		math::angle_vectors(viewangles, &cmd_fwd, &cmd_right, &cmd_up);

		float v8 = sqrtf((view_fwd.x * view_fwd.x) + (view_fwd.y * view_fwd.y));
		float v10 = sqrtf((view_right.x * view_right.x) + (view_right.y * view_right.y));
		float v12 = sqrtf(view_up.z * view_up.z);

		Vector norm_view_fwd((1.f / v8) * view_fwd.x, (1.f / v8) * view_fwd.y, 0.f);
		Vector norm_view_right((1.f / v10) * view_right.x, (1.f / v10) * view_right.y, 0.f);
		Vector norm_view_up(0.f, 0.f, (1.f / v12) * view_up.z);

		float v14 = sqrtf((cmd_fwd.x * cmd_fwd.x) + (cmd_fwd.y * cmd_fwd.y));
		float v16 = sqrtf((cmd_right.x * cmd_right.x) + (cmd_right.y * cmd_right.y));
		float v18 = sqrtf(cmd_up.z * cmd_up.z);

		Vector norm_cmd_fwd((1.f / v14) * cmd_fwd.x, (1.f / v14) * cmd_fwd.y, 0.f);
		Vector norm_cmd_right((1.f / v16) * cmd_right.x, (1.f / v16) * cmd_right.y, 0.f);
		Vector norm_cmd_up(0.f, 0.f, (1.f / v18) * cmd_up.z);

		float v22 = norm_view_fwd.x * m_pcmd->m_forwardmove;
		float v26 = norm_view_fwd.y * m_pcmd->m_forwardmove;
		float v28 = norm_view_fwd.z * m_pcmd->m_forwardmove;
		float v24 = norm_view_right.x * m_pcmd->m_sidemove;
		float v23 = norm_view_right.y * m_pcmd->m_sidemove;
		float v25 = norm_view_right.z * m_pcmd->m_sidemove;
		float v30 = norm_view_up.x * m_pcmd->m_upmove;
		float v27 = norm_view_up.z * m_pcmd->m_upmove;
		float v29 = norm_view_up.y * m_pcmd->m_upmove;

		m_pcmd->m_forwardmove = ((((norm_cmd_fwd.x * v24) + (norm_cmd_fwd.y * v23)) + (norm_cmd_fwd.z * v25))
			+ (((norm_cmd_fwd.x * v22) + (norm_cmd_fwd.y * v26)) + (norm_cmd_fwd.z * v28)))
			+ (((norm_cmd_fwd.y * v30) + (norm_cmd_fwd.x * v29)) + (norm_cmd_fwd.z * v27));
		m_pcmd->m_sidemove = ((((norm_cmd_right.x * v24) + (norm_cmd_right.y * v23)) + (norm_cmd_right.z * v25))
			+ (((norm_cmd_right.x * v22) + (norm_cmd_right.y * v26)) + (norm_cmd_right.z * v28)))
			+ (((norm_cmd_right.x * v29) + (norm_cmd_right.y * v30)) + (norm_cmd_right.z * v27));
		m_pcmd->m_upmove = ((((norm_cmd_up.x * v23) + (norm_cmd_up.y * v24)) + (norm_cmd_up.z * v25))
			+ (((norm_cmd_up.x * v26) + (norm_cmd_up.y * v22)) + (norm_cmd_up.z * v28)))
			+ (((norm_cmd_up.x * v30) + (norm_cmd_up.y * v29)) + (norm_cmd_up.z * v27));

		static auto cl_sidespeed = m_cvar()->FindVar(crypt_str("cl_sidespeed"));
		auto side_speed = cl_sidespeed->GetFloat();

		m_pcmd->m_forwardmove = math::clamp(m_pcmd->m_forwardmove, -side_speed, side_speed);
		m_pcmd->m_sidemove = math::clamp(m_pcmd->m_sidemove, -side_speed, side_speed);
		m_pcmd->m_upmove = math::clamp(m_pcmd->m_upmove, -320.f, 320.f);
	}

	unsigned int find_in_datamap(datamap_t * map, const char *name)
	{
		while (map)
		{
			for (int i = 0; i < map->m_data_num_fields; i++)
			{
				if (!map->m_data_desc[i].fieldName)
					continue;

				if (!strcmp(name, map->m_data_desc[i].fieldName))
					return map->m_data_desc[i].fieldOffset[TD_OFFSET_NORMAL];

				if (map->m_data_desc[i].fieldType == FIELD_EMBEDDED)
				{
					if (map->m_data_desc[i].td)
					{
						unsigned int offset;

						if ((offset = find_in_datamap(map->m_data_desc[i].td, name)) != 0)
							return offset;
					}
				}
			}

			map = map->m_base_map;
		}

		return 0;
	}

	bool get_bbox(entity_t* e, Box& box, bool player_esp)
	{
		auto collideable = e->GetCollideable();
		auto m_rgflCoordinateFrame = e->m_rgflCoordinateFrame();

		auto min = collideable->OBBMins();
		auto max = collideable->OBBMaxs();

		Vector points[8] =
		{
			Vector(min.x, min.y, min.z),
			Vector(min.x, max.y, min.z),
			Vector(max.x, max.y, min.z),
			Vector(max.x, min.y, min.z),
			Vector(max.x, max.y, max.z),
			Vector(min.x, max.y, max.z),
			Vector(min.x, min.y, max.z),
			Vector(max.x, min.y, max.z)
		};

		Vector pointsTransformed[8];

		for (auto i = 0; i < 8; i++)
			math::vector_transform(points[i], m_rgflCoordinateFrame, pointsTransformed[i]);

		Vector pos = e->GetAbsOrigin();
		Vector flb;
		Vector brt;
		Vector blb;
		Vector frt;
		Vector frb;
		Vector brb;
		Vector blt;
		Vector flt;

		auto bFlb = ImGuiRendering::get().world_to_screen(pointsTransformed[3], flb);
		auto bBrt = ImGuiRendering::get().world_to_screen(pointsTransformed[5], brt);
		auto bBlb = ImGuiRendering::get().world_to_screen(pointsTransformed[0], blb);
		auto bFrt = ImGuiRendering::get().world_to_screen(pointsTransformed[4], frt);
		auto bFrb = ImGuiRendering::get().world_to_screen(pointsTransformed[2], frb);
		auto bBrb = ImGuiRendering::get().world_to_screen(pointsTransformed[1], brb);
		auto bBlt = ImGuiRendering::get().world_to_screen(pointsTransformed[6], blt);
		auto bFlt = ImGuiRendering::get().world_to_screen(pointsTransformed[7], flt);

		if (!bFlb && !bBrt && !bBlb && !bFrt && !bFrb && !bBrb && !bBlt && !bFlt)
			return false;

		Vector arr[8] =
		{
			flb,
			brt,
			blb,
			frt,
			frb,
			brb,
			blt,
			flt
		};

		auto left = flb.x;
		auto top = flb.y;
		auto right = flb.x;
		auto bottom = flb.y;

		for (auto i = 1; i < 8; i++)
		{
			if (left > arr[i].x)
				left = arr[i].x;
			if (top < arr[i].y)
				top = arr[i].y;
			if (right < arr[i].x)
				right = arr[i].x;
			if (bottom > arr[i].y)
				bottom = arr[i].y;
		}

		box.x = left;
		box.y = bottom;
		box.w = right - left;
		box.h = top - bottom;

		return true;
	}

	void trace_line(Vector& start, Vector& end, unsigned int mask, CTraceFilter* filter, CGameTrace* tr)
	{
		Ray_t ray;
		ray.Init(start, end);

		m_trace()->TraceRay(ray, mask, filter, tr);
	}

	void clip_trace_to_players(IClientEntity* e, const Vector& start, const Vector& end, unsigned int mask, CTraceFilter* filter, CGameTrace* tr)
	{
		Vector mins = e->GetCollideable()->OBBMins(), maxs = e->GetCollideable()->OBBMaxs();

		Vector dir(end - start);
		dir.Normalize();

		Vector
			center = (maxs + mins) / 2,
			pos(center + e->GetAbsOrigin());

		Vector to = pos - start;
		float range_along = dir.Dot(to);

		float range;
		if (range_along < 0.f)
			range = -to.Length();

		else if (range_along > dir.Length())
			range = -(pos - end).Length();

		else {
			auto ray(pos - (dir * range_along + start));
			range = ray.Length();
		}

		if (range <= 60.f) {
			trace_t trace;

			Ray_t ray;
			ray.Init(start, end);

			m_trace()->ClipRayToEntity(ray, mask, e, &trace);

			if (tr->fraction > trace.fraction)
				*tr = trace;
		}
	}

	float server_time(CUserCmd* cmd)
	{
		static auto tickbase = 0;

		if (cmd)
		{
			static CUserCmd* last_cmd = nullptr;

			if (!last_cmd || last_cmd->m_predicted)
				tickbase = g_ctx.local()->m_nTickBase();
			else
				++tickbase;

			last_cmd = cmd;
		}

		return TICKS_TO_TIME(tickbase);
	}

	void RotateMovement(CUserCmd* cmd, float yaw)
	{
		Vector viewangles;
		m_engine()->GetViewAngles(viewangles);

		float rotation = DEG2RAD(viewangles.y - yaw);

		float cos_rot = cos(rotation);
		float sin_rot = sin(rotation);

		float new_forwardmove = cos_rot * cmd->m_forwardmove - sin_rot * cmd->m_sidemove;
		float new_sidemove = sin_rot * cmd->m_forwardmove + cos_rot * cmd->m_sidemove;

		cmd->m_forwardmove = new_forwardmove;
		cmd->m_sidemove = new_sidemove;
	}

	void color_modulate(float color[3], IMaterial* material)
	{
		auto found = false;
		auto var = material->FindVar(crypt_str("$envmaptint"), &found);

		if (found)
			var->set_vec_value(color[0], color[1], color[2]);

		m_renderview()->SetColorModulation(color[0], color[1], color[2]);
	}

	bool get_backtrack_matrix(player_t* e, matrix3x4_t* matrix)
	{
		return false;
		/*
		if (!g_cfg.ragebot.enable)
			return false;

		if (!g_ctx.local()->is_alive())
			return false;

		auto nci = m_engine()->GetNetChannelInfo();

		if (!nci)
			return false;

		auto i = e->EntIndex();

		if (i < 1 || i > 64)
			return false;

		auto records = &m_players[i].m_records;

		if (records->size() < 2)
			return false;

		for (auto record = records->rbegin(); record != records->rend(); ++record)
		{
			auto curtime = m_globals()->m_curtime;
			auto range = 0.2f;

			if (g_ctx.local()->is_alive())
			{
				if (g_ctx.globals.weapon->can_fire(true) && (g_ctx.globals.weapon->can_double_tap() && misc::get().double_tap_enabled && !g_ctx.globals.double_tap_aim || misc::get().hide_shots_enabled))
				{
					curtime = TICKS_TO_TIME(g_ctx.globals.fixed_tickbase);
					range += g_ctx.globals.fixed_time - curtime;
				}
				else
					curtime = g_ctx.globals.fixed_time;
			}

			if (!record->valid())
				continue;

			if (record->m_origin.DistTo(e->GetAbsOrigin()) < 1.0f)
				return false;
			
			auto next_record = record + 1;
			auto end = next_record == records->rend();

			auto next = end ? e->GetAbsOrigin() : next_record->m_origin;
			auto time_next = end ? e->m_flSimulationTime() : next_record->m_sim_time;

			auto correct = nci->GetLatency(FLOW_OUTGOING) + nci->GetLatency(FLOW_INCOMING) + util::get_interpolation();
			auto time_delta = time_next - record->m_sim_time;

			auto add = end ? range : time_delta;
			auto deadtime = record->m_sim_time + correct + add;
			auto delta = deadtime - curtime;

			auto mul = 1.0f / add;
			auto lerp = math::lerp(next, record->m_origin, math::clamp(delta * mul, 0.0f, 1.0f));

			matrix3x4_t result[MAXSTUDIOBONES];
			memcpy(result, record->m_bones, MAXSTUDIOBONES * sizeof(matrix3x4_t));

			for (auto j = 0; j < MAXSTUDIOBONES; j++)
			{
				auto matrix_delta = math::matrix_get_origin(record->m_bones[j]) - record->m_origin;
				math::matrix_set_origin(matrix_delta + lerp, result[j]);
			}

			memcpy(matrix, result, MAXSTUDIOBONES * sizeof(matrix3x4_t));
			return true;
		}

		return false;*/
	}

	void create_state(c_baseplayeranimationstate* state, player_t* e)
	{
		using Fn = void(__thiscall*)(c_baseplayeranimationstate*, player_t*);
		static auto fn = reinterpret_cast <Fn> (util::FindSignature(crypt_str("client.dll"), crypt_str("55 8B EC 56 8B F1 B9 ? ? ? ? C7 46")));

		fn(state, e);
	}

	void update_state(c_baseplayeranimationstate* state, const Vector& angles)
	{
		using Fn = void(__vectorcall*)(void*, void*, float, float, float, void*);
		static auto fn = reinterpret_cast <Fn> (util::FindSignature(crypt_str("client.dll"), crypt_str("55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24")));

		fn(state, nullptr, 0.0f, angles[1], angles[0], nullptr);
	}

	void reset_state(c_baseplayeranimationstate* state)
	{
		using Fn = void(__thiscall*)(c_baseplayeranimationstate*);
		static auto fn = reinterpret_cast <Fn> (util::FindSignature(crypt_str("client.dll"), crypt_str("56 6A 01 68 ? ? ? ? 8B F1")));

		fn(state);
	}

	void copy_command(CUserCmd* cmd)
	{
		auto commands_to_add = 0;

		do
		{
			auto sequence_number = commands_to_add + cmd->m_command_number;

			auto command = m_input()->GetUserCmd(sequence_number);
			auto verified_command = m_input()->GetVerifiedUserCmd(sequence_number);

			memcpy(command, cmd, sizeof(CUserCmd)); //-V598

			command->m_command_number = sequence_number;
			command->m_predicted = command->m_tickcount == 0x7F7FFFFF;

			if (g_cfg.ragebot.slow_teleport)
			{
				command->m_forwardmove = 0.0f;
				command->m_sidemove = 0.0f;
			}

			++m_clientstate()->m_iChockedCommands;

			command->m_viewangles.Normalize();
			command->m_viewangles.Clamp();

			memcpy(&verified_command->m_cmd, command, sizeof(CUserCmd)); //-V598
			verified_command->m_crc = command->GetChecksum();

			++commands_to_add;
		} 
		while (commands_to_add != 14);

		*(bool*)((uintptr_t)m_prediction() + 0x24) = true;
		*(int*)((uintptr_t)m_prediction() + 0x1C) = 0;
	}

	float get_interpolation()
	{
		static auto cl_interp = m_cvar()->FindVar(crypt_str("cl_interp")); //-V807
		static auto cl_interp_ratio = m_cvar()->FindVar(crypt_str("cl_interp_ratio"));
		static auto sv_client_min_interp_ratio = m_cvar()->FindVar(crypt_str("sv_client_min_interp_ratio"));
		static auto sv_client_max_interp_ratio = m_cvar()->FindVar(crypt_str("sv_client_max_interp_ratio"));
		static auto cl_updaterate = m_cvar()->FindVar(crypt_str("cl_updaterate"));
		static auto sv_minupdaterate = m_cvar()->FindVar(crypt_str("sv_minupdaterate"));
		static auto sv_maxupdaterate = m_cvar()->FindVar(crypt_str("sv_maxupdaterate"));

		auto updaterate = math::clamp(cl_updaterate->GetFloat(), sv_minupdaterate->GetFloat(), sv_maxupdaterate->GetFloat());
		auto lerp_ratio = math::clamp(cl_interp_ratio->GetFloat(), sv_client_min_interp_ratio->GetFloat(), sv_client_max_interp_ratio->GetFloat());

		return math::clamp(lerp_ratio / updaterate, cl_interp->GetFloat(), 1.0f);
	}
}