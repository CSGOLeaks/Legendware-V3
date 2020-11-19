// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "animnation_system.h"
#include "..\misc\misc.h"
#include "..\misc\logs.h"


void lagcompensation::fsn(ClientFrameStage_t stage, int force_update)
{
	for (auto i = 1; i < m_globals()->m_maxclients; i++)
	{
		auto e = static_cast<player_t*>(m_entitylist()->GetClientEntity(i));

		auto& player = players[i];
		player.m_e = e;

		if (!valid(i, e, stage))
			continue;

		switch (stage)
		{
		case FRAME_NET_UPDATE_POSTDATAUPDATE_END:
			apply_interpolation_flags(e, false);
			break;
		case FRAME_NET_UPDATE_END:
			e->set_abs_origin(e->m_vecOrigin());

			if (i == force_update || is_dormant[i] || e->m_flSimulationTime() != e->m_flOldSimulationTime())
			{
				update_player_animations(e, player);

				if (e->m_iTeamNum() != g_ctx.local()->m_iTeamNum())
					backtrack::get().store_record(e);
			}
			break;
		case FRAME_RENDER_START:
			e->set_abs_origin(e->m_vecOrigin());
			break;
		}
	}
}

bool lagcompensation::valid(int i, player_t* e, ClientFrameStage_t stage)
{
	auto& player = players[i];

	if (!e)
	{
		player.m_e = nullptr;
		return false;
	}

	switch (stage)
	{
	case FRAME_NET_UPDATE_POSTDATAUPDATE_END:
		if (!g_cfg.ragebot.enable || !g_ctx.local()->is_alive() || !e->valid(false) || e == g_ctx.local())
			return false;

		break;
	case FRAME_NET_UPDATE_END:
		if (!g_cfg.ragebot.enable || !g_ctx.local()->is_alive() || !e->valid(false) || e == g_ctx.local())
		{
			if (!e->is_alive())
			{
				is_dormant[e->EntIndex()] = false;
				g_ctx.globals.check_aa[i] = false;
				g_ctx.globals.check_side[i] = SIDE_RESOLVER_FIRST;
				g_ctx.globals.fired_shots[i] = 0;
				g_ctx.globals.missed_shots[i] = 0;
			}
			else if (e->IsDormant())
			{
				is_dormant[i] = true;

				first_ticks[i] = 16;
				second_ticks[i] = 16;
			}

			delete player.m_resolver;
			player.m_resolver = new c_player_resolver();

			backtrack::get().clear_records(e);
			return false;
		}

		break;
	case FRAME_RENDER_START:
		if (!e->valid(false) || e == g_ctx.local())
			return false;

		break;
	}

	return true;
}

void lagcompensation::apply_interpolation_flags(player_t* e, bool flag)
{
	auto map = e->var_mapping();

	if (!map)
		return;

	for (auto i = 0; i < map->m_nInterpolatedEntries; i++)
		map->m_Entries[i].m_bNeedsToInterpolate = flag;
}

void lagcompensation::update_player_animations(player_t* e, player_record_t& player_resolver)
{
	auto animstate = e->get_animation_state();

	if (!animstate)
		return;
	auto backup_ñurtime = m_globals()->m_curtime; //-V807
	auto backup_frametime = m_globals()->m_frametime;
	auto backup_flags = e->m_fFlags();
	auto backup_eflags = e->m_iEFlags();

	m_globals()->m_curtime = e->m_flSimulationTime();
	m_globals()->m_frametime = m_globals()->m_intervalpertick;

	if (e->get_animlayers()[5].m_flWeight) //-V550
		e->m_fFlags() |= FL_ONGROUND;

	if (is_dormant[e->EntIndex()])
	{
		is_dormant[e->EntIndex()] = false;
		animstate->time_since_in_air() = 0.0f;
	}

	e->m_iEFlags() &= ~0x1000;
	e->m_vecAbsVelocity() = e->m_vecVelocity();

	AnimationLayer animlayers[15];
	memcpy(animlayers, e->get_animlayers(), e->animlayer_count() * sizeof(AnimationLayer));

	if (animstate->m_iLastClientSideAnimationUpdateFramecount == m_globals()->m_framecount)
		--animstate->m_iLastClientSideAnimationUpdateFramecount;

	e->m_bClientSideAnimation() = true;

	player_resolver.m_resolver->pitch = FLT_MAX;
	player_resolver.m_resolver->yaw = FLT_MAX;



#if RELEASE
	player_info_t player_info;
	m_engine()->GetPlayerInfo(e->EntIndex(), &player_info);

	if (!player_info.fakeplayer && g_ctx.local()->is_alive() && e->m_iTeamNum() != g_ctx.local()->m_iTeamNum())
	{
		player_resolver.m_resolver->m_e = e;
		player_resolver.m_resolver->run();
	}
#else
	if (g_ctx.local()->is_alive() && e->m_iTeamNum() != g_ctx.local()->m_iTeamNum())
	{
		player_resolver.m_resolver->m_e = e;
		player_resolver.m_resolver->run();
	}
#endif

	if (player_resolver.m_resolver->pitch != FLT_MAX) //-V550
		e->m_angEyeAngles().x = player_resolver.m_resolver->pitch;

	if (player_resolver.m_resolver->yaw != FLT_MAX) //-V550
	{
		animstate->m_flGoalFeetYaw = player_resolver.m_resolver->yaw;

		if (player_resolver.m_resolver->resolve_yaw > 0.0f)
			g_ctx.globals.side[e->EntIndex()] = RIGHT;
		else if (player_resolver.m_resolver->resolve_yaw < 0.0f)
			g_ctx.globals.side[e->EntIndex()] = LEFT;
		else
			g_ctx.globals.side[e->EntIndex()] = NONE;
	}
	else
		g_ctx.globals.side[e->EntIndex()] = NONE;

	g_ctx.globals.updating_animation = true;
	e->update_clientside_animation();
	g_ctx.globals.updating_animation = false;

	memcpy(e->get_animlayers(), animlayers, e->animlayer_count() * sizeof(AnimationLayer));

	m_globals()->m_curtime = backup_ñurtime;
	m_globals()->m_frametime = backup_frametime;

	e->m_fFlags() = backup_flags;
	e->m_iEFlags() = backup_eflags;

	e->invalidate_bone_cache();
	e->SetupBonesFixed();
}

float lagcompensation::get_interpolation()
{
	static auto cl_interp = m_cvar()->FindVar("cl_interp"); //-V807
	static auto cl_interp_ratio = m_cvar()->FindVar("cl_interp_ratio");
	static auto sv_client_min_interp_ratio = m_cvar()->FindVar("sv_client_min_interp_ratio");
	static auto sv_client_max_interp_ratio = m_cvar()->FindVar("sv_client_max_interp_ratio");
	static auto cl_updaterate = m_cvar()->FindVar("cl_updaterate");
	static auto sv_minupdaterate = m_cvar()->FindVar("sv_minupdaterate");
	static auto sv_maxupdaterate = m_cvar()->FindVar("sv_maxupdaterate");

	auto updaterate = math::clamp(cl_updaterate->GetFloat(), sv_minupdaterate->GetFloat(), sv_maxupdaterate->GetFloat());
	auto lerp_ratio = math::clamp(cl_interp_ratio->GetFloat(), sv_client_min_interp_ratio->GetFloat(), sv_client_max_interp_ratio->GetFloat());

	return math::clamp(lerp_ratio / updaterate, cl_interp->GetFloat(), 1.0f);
}