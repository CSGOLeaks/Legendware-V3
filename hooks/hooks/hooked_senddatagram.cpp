// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "..\hooks.hpp"

using PacketStart_t = void(__thiscall*)(void*, int, int);

void __fastcall hooks::hooked_packetstart(void* ecx, void* edx, int incoming, int outgoing)
{
	static auto original_fn = clientstate_hook->get_func_address <PacketStart_t> (5);

    if (!g_cfg.antiaim.fakelag)
        return original_fn(ecx, incoming, outgoing);

    if (!g_ctx.available())
        return original_fn(ecx, incoming, outgoing);

	if (!g_ctx.local()->is_alive())
		return original_fn(ecx, incoming, outgoing);
   
    if (g_ctx.globals.commands.empty())
        return original_fn(ecx, incoming, outgoing);

    if (m_gamerules()->m_bIsValveDS())
        return original_fn(ecx, incoming, outgoing);

    for (auto it = g_ctx.globals.commands.rbegin(); it != g_ctx.globals.commands.rend(); ++it)
    {
        if (!it->is_outgoing)
            continue;

        if (it->command_number == outgoing || outgoing > it->command_number && (!it->is_used || it->previous_command_number == outgoing))
        {
            it->previous_command_number = outgoing;
            it->is_used = true;
            original_fn(ecx, incoming, outgoing);
            break;
        }
    }

    auto result = false;

    for (auto it = g_ctx.globals.commands.begin(); it != g_ctx.globals.commands.end();) 
    {
        if (outgoing == it->command_number || outgoing == it->previous_command_number)
            result = true;

        if (outgoing > it->command_number && outgoing > it->previous_command_number) 
            it = g_ctx.globals.commands.erase(it);
        else 
            ++it;
    }

    if (!result)
        original_fn(ecx, incoming, outgoing);
}

void __fastcall hooks::hooked_checkfilecrcswithserver(void* ecx, void* edx)
{
    
}

bool __fastcall hooks::hooked_loosefileallowed(void* ecx, void* edx)
{
    return true;
}