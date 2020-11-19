#pragma once

#include "..\..\includes.hpp"
#include "..\..\sdk\structs.hpp"

struct target_info {
	float fov;
};

class legit_bot : public singleton <legit_bot>
{
public:
	void createmove(CUserCmd* cmd);

	void find_target();
	void find_best_point(CUserCmd* cmd, float fov);

	void auto_pistol(CUserCmd* cmd);
	void do_rcs(CUserCmd* cmd);
	void calc_fov();
	void calc_smooth();
	void do_smooth(const Vector& currentAngle, const Vector& aimAngle, Vector& angle);

	player_t* target;
	int target_index = -1;
	float target_switch_delay;

	float target_fov;
	float fov_t;

	Vector point = Vector(0, 0, 0);
private:
	target_info targets[65];
	float smooth_t;
	float reaction_t;
	
	Vector aim_punch = Vector(0, 0, 0);
	Vector aim_angle = Vector(0, 0, 0);
};
