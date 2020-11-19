// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "knifebot.h"

void knifebot::knife_run(CUserCmd* cmd)
{
	best_index = -1;

	if (!g_ctx.globals.weapon->is_knife())
		return;

	if (!best_target_knife())
		return;

	auto best_angle = best_entity->hitbox_position(HITBOX_CHEST);
	auto entity_angle = math::calculate_angle(g_ctx.globals.eye_pos, best_angle);
	auto health = best_entity->m_iHealth();

	auto stab = false;

	if (best_entity->m_bHasHeavyArmor())
	{
		if (health <= 55 && health > get_minimal_damage(g_ctx.globals.weapon))
			stab = true;
	}
	else
	{
		if (health <= 65 && health > get_minimal_damage(g_ctx.globals.weapon))
			stab = true;
	}

	if (stab)
		stab = best_distance < 60;

	static auto cl_interp = m_cvar()->FindVar(crypt_str("cl_interp")); //-V807
	static auto cl_interp_ratio = m_cvar()->FindVar(crypt_str("cl_interp_ratio"));
	static auto sv_client_min_interp_ratio = m_cvar()->FindVar(crypt_str("sv_client_min_interp_ratio"));
	static auto sv_client_max_interp_ratio = m_cvar()->FindVar(crypt_str("sv_client_max_interp_ratio"));
	static auto cl_updaterate = m_cvar()->FindVar(crypt_str("cl_updaterate"));
	static auto sv_minupdaterate = m_cvar()->FindVar(crypt_str("sv_minupdaterate"));
	static auto sv_maxupdaterate = m_cvar()->FindVar(crypt_str("sv_maxupdaterate"));

	auto updaterate = math::clamp(cl_updaterate->GetFloat(), sv_minupdaterate->GetFloat(), sv_maxupdaterate->GetFloat());
	auto lerp_ratio = math::clamp(cl_interp_ratio->GetFloat(), sv_client_min_interp_ratio->GetFloat(), sv_client_max_interp_ratio->GetFloat());

	cmd->m_viewangles = entity_angle;
	cmd->m_buttons |= stab ? IN_ATTACK2 : IN_ATTACK;
	cmd->m_tickcount = TIME_TO_TICKS(best_entity->m_flSimulationTime() + math::clamp(lerp_ratio / updaterate, cl_interp->GetFloat(), 1.0f));
}

bool knifebot::best_target_knife()
{
	auto good_distance = 75.0f;

	for (auto i = 1; i < m_globals()->m_maxclients; i++)
	{
		auto e = (player_t*)m_entitylist()->GetClientEntity(i);

		if (!e->valid(true))
			continue;

		if (e->m_bGunGameImmunity() || e->m_fFlags() & FL_FROZEN)
			continue;

		if (g_cfg.player_list.white_list[i])
			continue;

		auto local_position = g_ctx.local()->GetAbsOrigin();
		local_position.z += 50.0f;

		auto entity_position = e->GetAbsOrigin();
		entity_position.z += 50.0f;

		if (!util::visible(local_position, entity_position, e, g_ctx.local()))
			continue;

		float current_distance = local_position.DistTo(entity_position);

		if (current_distance < good_distance)
		{
			good_distance = current_distance;
			best_index = i;
			best_entity = e;
		}
	}

	best_distance = good_distance;

	return best_index != -1;
}

int knifebot::get_minimal_damage(weapon_t* weapon)
{
	if (g_ctx.globals.fixed_time > weapon->m_flNextPrimaryAttack() + 0.4f)
		return 34;

	return 21;
}