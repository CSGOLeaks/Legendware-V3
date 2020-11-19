#include "..\..\includes.hpp"

class knifebot : public singleton <knifebot>
{
private:
	bool best_target_knife();
	int get_minimal_damage(weapon_t* weapon);

	player_t* best_entity = nullptr;
	int best_distance;
	int best_index;
public:
	void knife_run(CUserCmd* cmd);
};