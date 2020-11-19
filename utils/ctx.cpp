// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "ctx.hpp"
ctx_t g_ctx;

bool ctx_t::available() 
{
	if (!g_ctx.local())
		return false;

	if (!m_engine()->IsInGame())
		return false;

	if (!m_engine()->IsConnected())
		return false;

	return true;
}

player_t* ctx_t::local(player_t* e, bool fsn)
{
	static player_t* local = nullptr;

	if (fsn)
		local = e;

	return local;
}

CUserCmd* ctx_t::get_command() 
{
	return m_pcmd;
}