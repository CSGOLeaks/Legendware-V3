#pragma once

#include "..\..\includes.hpp"
#include "..\..\sdk\structs.hpp"

enum
{
	SIDE_RESOLVER_FIRST,
	SIDE_RESOLVER_SECOND
};

enum resolver_side
{
	NONE,
	RIGHT,
	LEFT
};

class c_player_resolver
{
	void resolve();
	void check_aa();
	void check_side(int resolve_index);
	void check_lby();
	void pitch_resolve();
public:
	player_t* m_e = nullptr;
	float pitch = FLT_MAX;
	float resolve_yaw = 0.0f;
	float yaw = FLT_MAX;

	void run();
};

struct player_record_t
{
	player_t* m_e = nullptr;
	c_player_resolver* m_resolver = new c_player_resolver();
};

class adjust_data //-V730
{
public:
	player_t* player = nullptr;
	int i = -1;

	resolver_side side = NONE;

	AnimationLayer layers[15];
	matrix3x4_t matrix[MAXSTUDIOBONES];

	bool dormant = false;

	int flags = 0;
	int bone_count = 0;

	float simulation_time = 0.0f;
	float old_simulation_time = 0.0f;

	float duck_amount = 0.0f;
	float lby = 0.0f;

	Vector angles = ZERO;
	Vector abs_angles = ZERO;
	Vector velocity = ZERO;
	Vector origin = ZERO;
	Vector mins = ZERO;
	Vector maxs = ZERO;

	adjust_data() //-V730
	{
		player = nullptr;
		i = -1;

		side = NONE;
		dormant = false;

		flags = 0;
		bone_count = 0;

		simulation_time = 0.0f;
		old_simulation_time = 0.0f;

		duck_amount = 0.0f;
		lby = 0.0f;

		angles = ZERO;
		abs_angles = ZERO;
		velocity = ZERO;
		origin = ZERO;
		mins = ZERO;
		maxs = ZERO;
	}

	adjust_data(player_t* e)
	{
		player = e;

		if (!player)
			return;

		i = player->EntIndex();
		side = (resolver_side)g_ctx.globals.side[i];

		memcpy(layers, player->get_animlayers(), player->animlayer_count() * sizeof(AnimationLayer));
		memcpy(matrix, player->m_CachedBoneData().Base(), player->get_bone_count() * sizeof(matrix3x4_t));

		dormant = player->IsDormant();

		flags = player->m_fFlags();
		bone_count = player->get_bone_count();

		simulation_time = player->m_flSimulationTime();
		old_simulation_time = player->m_flOldSimulationTime();

		duck_amount = player->m_flDuckAmount();
		lby = player->m_flLowerBodyYawTarget();

		angles = player->m_angEyeAngles();
		abs_angles = player->GetAbsAngles();
		velocity = player->m_vecVelocity();
		origin = player->GetAbsOrigin();
		origin.z = player->m_vecOrigin().z;
		mins = player->GetCollideable()->OBBMins();
		maxs = player->GetCollideable()->OBBMaxs();
	}

	void store_data(player_t* e)
	{
		player = e;

		if (!player)
			return;

		i = player->EntIndex();
		side = (resolver_side)g_ctx.globals.side[i];

		memcpy(layers, player->get_animlayers(), player->animlayer_count() * sizeof(AnimationLayer));
		memcpy(matrix, player->m_CachedBoneData().Base(), player->get_bone_count() * sizeof(matrix3x4_t));

		dormant = player->IsDormant();

		flags = player->m_fFlags();
		bone_count = player->get_bone_count();

		simulation_time = player->m_flSimulationTime();
		old_simulation_time = player->m_flOldSimulationTime();

		duck_amount = player->m_flDuckAmount();
		lby = player->m_flLowerBodyYawTarget();

		angles = player->m_angEyeAngles();
		abs_angles = player->GetAbsAngles();
		velocity = player->m_vecVelocity();
		origin = player->GetAbsOrigin();
		origin.z = player->m_vecOrigin().z;
		mins = player->GetCollideable()->OBBMins();
		maxs = player->GetCollideable()->OBBMaxs();
	}

	void adjust_player()
	{
		if (!player)
			return;

		memcpy(player->get_animlayers(), layers, player->animlayer_count() * sizeof(AnimationLayer));
		memcpy(player->m_CachedBoneData().Base(), matrix, player->get_bone_count() * sizeof(matrix3x4_t));

		player->m_fFlags() = flags;
		player->get_bone_count() = bone_count;

		player->m_flSimulationTime() = simulation_time;
		player->m_flOldSimulationTime() = old_simulation_time;

		player->m_flDuckAmount() = duck_amount;
		player->m_flLowerBodyYawTarget() = lby;

		player->m_angEyeAngles() = angles;
		player->set_abs_angles(abs_angles);
		player->m_vecVelocity() = velocity;
		player->set_abs_origin(origin);
		player->m_vecOrigin().z = origin.z;
		player->GetCollideable()->OBBMins() = mins;
		player->GetCollideable()->OBBMaxs() = maxs;
	}
};

class lagcompensation : public singleton <lagcompensation>
{
public:
	void fsn(ClientFrameStage_t stage, int force_update);

	bool valid(int i, player_t* e, ClientFrameStage_t stage);

	void apply_interpolation_flags(player_t* e, bool flag);

	void min_matrix(player_t* e, player_record_t& player_record);
	void max_matrix(player_t* e, player_record_t& player_record);
	void update_player_animations(player_t* e, player_record_t& player_record);

	float get_interpolation();

	player_record_t players[65];
	adjust_data player_data[65];

	bool is_dormant[65];
	float previous_yaw[65];
	int first_ticks[65];
	int second_ticks[65];
};