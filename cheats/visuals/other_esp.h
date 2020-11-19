#pragma once

#include "..\..\includes.hpp"
#include "..\..\sdk\structs.hpp"

class otheresp : public singleton< otheresp >
{
public:
	void penetration_reticle();
	void indicators();
	void hitmarker_paint();
	void damage_marker_paint();
	void spread_crosshair(LPDIRECT3DDEVICE9 device);
	void automatic_peek_indicator();

	struct Hitmarker
	{
		float hurt_time = FLT_MIN;
		Color hurt_color = Color::White;
		Vector point = ZERO;
	} hitmarker;

	struct Damage_marker
	{
		Vector position = ZERO;
		float hurt_time = FLT_MIN;
		Color hurt_color = Color::White;
		int damage = -1;
		int hitgroup = -1;

		void reset()
		{
			position.Zero();
			hurt_time = FLT_MIN;
			hurt_color = Color::White;
			damage = -1;
			hitgroup = -1;
		}
	} damage_marker[65];
};