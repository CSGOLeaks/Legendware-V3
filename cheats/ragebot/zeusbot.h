#include "..\..\includes.hpp"
#include "aimbot.h"
#include "..\misc\fakelag.h"

class zeusbot : public singleton <zeusbot>
{
private:
	bool best_target_zeus();

	player_t* best_entity = nullptr;
	int best_distance;
	int best_index;
public:
	void zeus_run(CUserCmd* cmd);
};