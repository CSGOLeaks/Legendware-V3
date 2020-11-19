// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "spammers.h"

void spammers::clan_tag()
{
	auto apply = [](const char* tag) -> void
	{
		using Fn = int(__fastcall*)(const char*, const char*);
		static auto fn = reinterpret_cast<Fn>(util::FindSignature(crypt_str("engine.dll"), crypt_str("53 56 57 8B DA 8B F9 FF 15")));

		fn(tag, tag);
	};

	static auto removed = false;

	if (!g_cfg.misc.clantag_spammer && !removed)
	{
		removed = true;
		apply(crypt_str(""));
		return;
	}

	if (g_cfg.misc.clantag_spammer)
	{
		auto nci = m_engine()->GetNetChannelInfo();

		if (!nci)
			return;

		static auto time = -1;
		auto ticks = TIME_TO_TICKS(nci->GetAvgLatency(FLOW_OUTGOING)) + (float)m_globals()->m_tickcount; //-V807
		auto intervals = 0.5f / m_globals()->m_intervalpertick;

		auto main_time = (int)(ticks / intervals) % 22;

		if (main_time != time && !m_clientstate()->m_iChockedCommands)
		{
			auto tag = crypt_str("");

			switch (main_time)
			{
			case 0:
				tag = crypt_str("legendware "); //-V1037
				break;
			case 1:
				tag = crypt_str("egendware ");
				break;
			case 2:
				tag = crypt_str("gendware ");
				break;
			case 3:
				tag = crypt_str("endware ");
				break;
			case 4:
				tag = crypt_str("ndware ");
				break;
			case 5:
				tag = crypt_str("dware ");
				break;
			case 6:
				tag = crypt_str("ware ");
				break;
			case 7:
				tag = crypt_str("are ");
				break;
			case 8:
				tag = crypt_str("re ");
				break;
			case 9:
				tag = crypt_str("e ");
				break;
			case 10:
				tag = crypt_str("");
				break;
			case 11:
				tag = crypt_str("l");
				break;
			case 12:
				tag = crypt_str("le");
				break;
			case 13:
				tag = crypt_str("leg");
				break;
			case 14:
				tag = crypt_str("lege");
				break;
			case 15:
				tag = crypt_str("legen");
				break;
			case 16:
				tag = crypt_str("legend");
				break;
			case 17:
				tag = crypt_str("legendw");
				break;
			case 18:
				tag = crypt_str("legendwa");
				break;
			case 19:
				tag = crypt_str("legendwar");
				break;
			case 20:
				tag = crypt_str("legendware");
				break;
			case 21:
				tag = crypt_str("legendware ");
				break;
			}

			apply(tag);
			time = main_time;
		}

		removed = false;
	}
}