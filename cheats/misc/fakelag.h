#pragma once

#include "..\..\includes.hpp"
#include "..\ragebot\antiaim.h"

class fakelag : public singleton <fakelag>
{
public:
	void Fakelag(CUserCmd* m_pcmd);
	void Createmove();
	bool FakelagCondition(CUserCmd* m_pcmd);

	bool condition = true;
	int max_choke;
};