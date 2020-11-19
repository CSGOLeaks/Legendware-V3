// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "utils\protect.h"
#include "includes.hpp"
#include "utils\ctx.hpp"
#include "utils\recv.h"
#include "utils\imports.h"
#include "utils\anti_debug.h"
#include "nSkinz\SkinChanger.h"
#include "utils\sha-256.h"
#include <string>

enum error_type
{
	ERROR_NONE,
	ERROR_DEBUG,
	ERROR_OPEN_KEY,
	ERROR_QUERY_LOGIN,
	ERROR_QUERY_PASSWORD,
	ERROR_CONNECT,
	ERROR_INCORRECT_LOGIN_OR_PASSWORD,
	ERROR_NO_SUBSCRIPTION,
	ERROR_INCORRECT_HWID,
	ERROR_SERVER,
	ERROR_CHECK_HASH
};

PVOID base_address = nullptr;
Anti_debugger anti_debugger;
volatile error_type error = ERROR_NONE;

LONG CALLBACK ExceptionHandler(EXCEPTION_POINTERS* ExceptionInfo);
__forceinline void crash(bool debug = false);
__forceinline void setup_render();
__forceinline void setup_netvars();
__forceinline void setup_skins();
__forceinline void setup_hooks();

DWORD WINAPI main(PVOID base)
{
#if RELEASE
	if (anti_debugger.is_debugging())
	{
		error = ERROR_DEBUG;

		crash(true);
		return EXIT_SUCCESS;
	}

	HKEY key = nullptr;

	if (IFH(RegOpenKeyEx)(HKEY_CURRENT_USER, crypt_str("SOFTWARE\\lw-project\\Loader\\user"), 0, KEY_ALL_ACCESS, &key))
	{
		error = ERROR_OPEN_KEY;
		std::this_thread::sleep_for(std::chrono::milliseconds(5000));

		crash();
		return EXIT_SUCCESS;
	}

	DWORD login_size = 64;
    char login[64];

	if (IFH(RegQueryValueEx)(key, crypt_str("login"), nullptr, nullptr, (LPBYTE)&login, &login_size))
	{
		IFH(RegCloseKey)(key);

		error = ERROR_QUERY_LOGIN;
		std::this_thread::sleep_for(std::chrono::milliseconds(5000));

		crash();
		return EXIT_SUCCESS;
	}

	DWORD password_size = 64;
	char password[64];

	if (IFH(RegQueryValueEx)(key, crypt_str("Pass"), nullptr, nullptr, (LPBYTE)&password, &password_size))
	{
		IFH(RegCloseKey)(key);

		error = ERROR_QUERY_PASSWORD;
		std::this_thread::sleep_for(std::chrono::milliseconds(5000));

		crash();
		return EXIT_SUCCESS;
	}

	IFH(RegCloseKey)(key);
	nlohmann::json request_data;

	request_data[(std::string)crypt_str("type")] = crypt_str("get_hui");
	request_data[(std::string)crypt_str("login")] = login;
	request_data[(std::string)crypt_str("pass")] = password;
	request_data[(std::string)crypt_str("hwid")] = get_hwid();
	request_data[(std::string)crypt_str("beta")] = BETA;

	auto data = get_data(crypt_str("/forum/qwerty_new.php?data=") + request_data.dump());
	request_data.clear();

	if (data.empty())
	{
		error = ERROR_CONNECT;
		std::this_thread::sleep_for(std::chrono::milliseconds(5000));

		crash();
		return EXIT_SUCCESS;
	}
	else if (data.front() == 'e')
	{
		switch (data.at(2))
		{
		case '1':
			error = ERROR_INCORRECT_LOGIN_OR_PASSWORD;
			std::this_thread::sleep_for(std::chrono::milliseconds(5000));

			crash();
			return EXIT_SUCCESS;
		case '2':
			error = ERROR_NO_SUBSCRIPTION;
			std::this_thread::sleep_for(std::chrono::milliseconds(5000));

			crash();
			return EXIT_SUCCESS;
		case '3':
			error = ERROR_INCORRECT_HWID;
			std::this_thread::sleep_for(std::chrono::milliseconds(5000));

			crash();
			return EXIT_SUCCESS;
		case '4':
			error = ERROR_SERVER;
			std::this_thread::sleep_for(std::chrono::milliseconds(5000));

			crash();
			return EXIT_SUCCESS;
		}
	}
	else if (sha256(data) == crypt_str("1a331ec95fcdc0e960f004acdc22cc211faa31a68a20d2cf03dbb1767717f021"))
	{
		auto i = 0;

		while (true)
		{
			std::string str;

			if (i >= data.size())
				break;

			for (; i < data.size() && data.at(i) != '\r'; i++)
				str.push_back(data.at(i));

			if (str.front() == 's')
			{
				std::string signature;

				for (auto i = 2; i < str.size(); i++)
					signature.push_back(str.at(i));

				g_ctx.signatures.emplace_back(signature);
			}
			else  if (str.front() == 'i')
			{
				std::string index;

				for (auto i = 2; i < str.size(); i++)
					index.push_back(str.at(i));

				g_ctx.indexes.emplace_back(atoi(index.c_str()));
			}

			i += 2;
		}

		g_ctx.username = login;

		while (!IFH(GetModuleHandle)(crypt_str("serverbrowser.dll")))
			std::this_thread::sleep_for(std::chrono::milliseconds(5000));

		base_address = base;
		AddVectoredExceptionHandler(TRUE, ExceptionHandler);

		setup_sounds();
		setup_skins();

		setup_netvars();
		setup_render();

		cfg_manager->setup();
		c_lua::get().initialize();
		key_binds::get().initialize_key_binds();

		setup_hooks();
		Netvars::Netvars();

		return EXIT_SUCCESS;
	}
	else
	{
		error = ERROR_CHECK_HASH;
		std::this_thread::sleep_for(std::chrono::milliseconds(5000));

		crash();
		return EXIT_SUCCESS;
	}
#else	
	g_ctx.signatures =
	{
		crypt_str("A1 ? ? ? ? 50 8B 08 FF 51 0C"),
		crypt_str("B9 ?? ?? ?? ?? A1 ?? ?? ?? ?? FF 10 A1 ?? ?? ?? ?? B9"),
		crypt_str("0F 11 05 ?? ?? ?? ?? 83 C8 01"),
		crypt_str("8B 0D ?? ?? ?? ?? 8B 46 08 68"),
		crypt_str("B9 ? ? ? ? F3 0F 11 04 24 FF 50 10"),
		crypt_str("8B 3D ? ? ? ? 85 FF 0F 84 ? ? ? ? 81 C7"),
		crypt_str("A1 ? ? ? ? 8B 0D ? ? ? ? 6A 00 68 ? ? ? ? C6"),
		crypt_str("80 3D ? ? ? ? ? 53 56 57 0F 85"),
		crypt_str("55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 89 7C 24 0C"),
		crypt_str("80 3D ? ? ? ? ? 74 06 B8"),
		crypt_str("55 8B EC 83 E4 F0 B8 D8"),
		crypt_str("55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 56 8B F1 57 89 74 24 1C"),
		crypt_str("55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 56 8B 75 08 57 8B F9 85 F6"),
		crypt_str("55 8B EC 51 56 8B F1 80 BE ? ? ? ? ? 74 36"),
		crypt_str("56 8B F1 8B 8E ? ? ? ? 83 F9 FF 74 21"),
		crypt_str("55 8B EC 83 E4 F8 83 EC 5C 53 8B D9 56 57 83"),
		crypt_str("55 8B EC A1 ? ? ? ? 83 EC 10 56 8B F1 B9"),
		crypt_str("57 8B F9 8B 07 8B 80 ? ? ? ? FF D0 84 C0 75 02"),
		crypt_str("55 8B EC 81 EC ? ? ? ? 53 8B D9 89 5D F8 80"),
		crypt_str("53 0F B7 1D ? ? ? ? 56"),
		crypt_str("8B 0D ? ? ? ? 8D 95 ? ? ? ? 6A 00 C6")
	};

	g_ctx.indexes =
	{
		5,
		33,
		339,
		218,
		219,
		34,
		157,
		75,
		460,
		482,
		452,
		483,
		284,
		223,
		246,
		27,
		17,
		123
	};

	while (!IFH(GetModuleHandle)(crypt_str("serverbrowser.dll")))
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	base_address = base;
	AllocConsole();

	if (!freopen(crypt_str("CONOUT$"), crypt_str("w"), stdout))
	{
		FreeConsole();
		return EXIT_SUCCESS;
	}

	std::cout << crypt_str("Setuping sounds...\n");
	setup_sounds();

	std::cout << crypt_str("Setuping skins...\n");
	setup_skins();

	std::cout << crypt_str("Setuping netvars...\n");
	setup_netvars();

	std::cout << crypt_str("Setuping render...\n");
	setup_render();

	std::cout << crypt_str("Setuping configs manager...\n");
	cfg_manager->setup();

	std::cout << crypt_str("Setuping scripts manager...\n");
	c_lua::get().initialize();

	std::cout << crypt_str("Setuping key binds manager...\n");
	key_binds::get().initialize_key_binds();

	std::cout << crypt_str("Setuping hooks...\n");
	setup_hooks();
	Netvars::Netvars();

	std::cout << crypt_str("Done!");
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	FreeConsole();
	return EXIT_SUCCESS;
#endif
}

#if RELEASE
DWORD WINAPI music(PVOID base)
{
#if BETA
	IFH(PlaySound)((LPCSTR)welcome, nullptr, SND_MEMORY);
#else
	Beep(400, 400);
	Beep(600, 400);
#endif

	return EXIT_SUCCESS;
}

DWORD WINAPI message_box(PVOID base)
{
	if (anti_debugger.is_debugging())
	{
		error = ERROR_DEBUG;

		crash(true);
		return EXIT_SUCCESS;
	}

	while (error == ERROR_NONE)
	{
		if (anti_debugger.is_debugging())
		{
			error = ERROR_DEBUG;

			crash(true);
			return EXIT_SUCCESS;
		}

		IFH(Sleep)(1000);
	}

	switch (error) //-V719
	{
	case ERROR_OPEN_KEY:
		IFH(MessageBox)(nullptr, crypt_str("Error ID: 0x1"), crypt_str("Fatal error"), MB_ICONERROR);
		break;
	case ERROR_QUERY_LOGIN:
		IFH(MessageBox)(nullptr, crypt_str("Error ID: 0x2"), crypt_str("Fatal error"), MB_ICONERROR);
		break;
	case ERROR_QUERY_PASSWORD:
		IFH(MessageBox)(nullptr, crypt_str("Error ID: 0x3"), crypt_str("Fatal error"), MB_ICONERROR);
		break;
	case ERROR_CONNECT:
		IFH(MessageBox)(nullptr, crypt_str("Error ID: 0x4"), crypt_str("Fatal error"), MB_ICONERROR);
		break;
	case ERROR_INCORRECT_LOGIN_OR_PASSWORD:
		IFH(MessageBox)(nullptr, crypt_str("Error ID: 0x5"), crypt_str("Fatal error"), MB_ICONERROR);
		break;
	case ERROR_NO_SUBSCRIPTION:
		IFH(MessageBox)(nullptr, crypt_str("Error ID: 0x6"), crypt_str("Fatal error"), MB_ICONERROR);
		break;
	case ERROR_INCORRECT_HWID:
		IFH(MessageBox)(nullptr, crypt_str("Error ID: 0x7"), crypt_str("Fatal error"), MB_ICONERROR);
		break;
	case ERROR_SERVER:
		IFH(MessageBox)(nullptr, crypt_str("Error ID: 0x8"), crypt_str("Fatal error"), MB_ICONERROR);
		break;
	case ERROR_CHECK_HASH:
		IFH(MessageBox)(nullptr, crypt_str("Error ID: 0x9"), crypt_str("Fatal error"), MB_ICONERROR);
		break;
	}

	return EXIT_SUCCESS;
}

void crash_log(const char* text, ...)
{
	if (!text)
		return;

	char path[MAX_PATH];
	IFH(GetEnvironmentVariable)(crypt_str("USERPROFILE"), path, MAX_PATH);

	strcat(path, crypt_str("\\Desktop\\legendware.log"));
	auto file = std::ofstream(path, std::ios::app);

	if (!file.is_open())
		return;

	va_list va_alist;
	char buffer[1024] = "\0";

	va_start(va_alist, text);
	_vsnprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), text, va_alist); //-V2007
	va_end(va_alist);

	file << buffer << std::endl;
	file.close();
}

LONG CALLBACK ExceptionHandler(EXCEPTION_POINTERS* ExceptionInfo)
{
	if (error != ERROR_NONE)
		return EXCEPTION_EXECUTE_HANDLER;

	static auto logged = false;

	if (logged)
		return EXCEPTION_CONTINUE_SEARCH;

	auto m_ExceptionCode = ExceptionInfo->ExceptionRecord->ExceptionCode;

	if (m_ExceptionCode != EXCEPTION_ACCESS_VIOLATION && m_ExceptionCode != EXCEPTION_ARRAY_BOUNDS_EXCEEDED && 
		m_ExceptionCode != EXCEPTION_DATATYPE_MISALIGNMENT && m_ExceptionCode != EXCEPTION_FLT_DENORMAL_OPERAND &&
		m_ExceptionCode != EXCEPTION_FLT_DIVIDE_BY_ZERO && m_ExceptionCode != EXCEPTION_FLT_INEXACT_RESULT &&
		m_ExceptionCode != EXCEPTION_FLT_INVALID_OPERATION && m_ExceptionCode != EXCEPTION_FLT_OVERFLOW &&
		m_ExceptionCode != EXCEPTION_FLT_STACK_CHECK && m_ExceptionCode != EXCEPTION_FLT_UNDERFLOW && 
		m_ExceptionCode != EXCEPTION_ILLEGAL_INSTRUCTION &&  m_ExceptionCode != EXCEPTION_IN_PAGE_ERROR && 
		m_ExceptionCode != EXCEPTION_INT_DIVIDE_BY_ZERO && m_ExceptionCode != EXCEPTION_INT_OVERFLOW && 
		m_ExceptionCode != EXCEPTION_INVALID_DISPOSITION && m_ExceptionCode != EXCEPTION_NONCONTINUABLE_EXCEPTION && 
		m_ExceptionCode != EXCEPTION_PRIV_INSTRUCTION && m_ExceptionCode != EXCEPTION_STACK_OVERFLOW)
		return EXCEPTION_CONTINUE_SEARCH;

	auto m_ExceptionAddress = ExceptionInfo->ExceptionRecord->ExceptionAddress;

	MODULEINFO module_info;
	GetModuleInformation(GetCurrentProcess(), GetModuleHandle(nullptr), &module_info, sizeof(MODULEINFO));

	if (m_ExceptionAddress < (HMODULE)base_address || m_ExceptionAddress > (HMODULE)base_address + module_info.SizeOfImage)
		return EXCEPTION_CONTINUE_SEARCH;

	logged = true;
	char path[MAX_PATH];

	IFH(GetEnvironmentVariable)(crypt_str("USERPROFILE"), path, MAX_PATH);
	strcat(path, crypt_str("\\Desktop\\legendware.log"));

	remove(path);

	crash_log(crypt_str("Time: %.3f"), (float)clock() / CLOCKS_PER_SEC);
	crash_log(crypt_str("Base address: 0x%p"), (HMODULE)base_address);
	crash_log(crypt_str("Exception at address: 0x%p"), m_ExceptionAddress);

	auto m_exceptionInfo_0 = ExceptionInfo->ExceptionRecord->ExceptionInformation[0];
	auto m_exceptionInfo_1 = ExceptionInfo->ExceptionRecord->ExceptionInformation[1];
	auto m_exceptionInfo_2 = ExceptionInfo->ExceptionRecord->ExceptionInformation[2];

	switch (m_ExceptionCode)
	{
	case EXCEPTION_ACCESS_VIOLATION:
		crash_log(crypt_str("Cause: EXCEPTION_ACCESS_VIOLATION"));

		if (!m_exceptionInfo_0)
			crash_log(crypt_str("Attempted to read from: 0x%08x", m_exceptionInfo_1));
		else if (m_exceptionInfo_0 == 1)
			crash_log(crypt_str("Attempted to write to: 0x%08x", m_exceptionInfo_1));
		else if (m_exceptionInfo_0 == 8)
			crash_log(crypt_str("Data Execution Prevention (DEP) at: 0x%08x", m_exceptionInfo_1));
		else
			crash_log(crypt_str("Unknown access violation at: 0x%08x", m_exceptionInfo_1));

		break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		crash_log(crypt_str("Cause: EXCEPTION_ARRAY_BOUNDS_EXCEEDED"));
		break;
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		crash_log(crypt_str("Cause: EXCEPTION_DATATYPE_MISALIGNMENT"));
		break;
	case EXCEPTION_FLT_DENORMAL_OPERAND:
		crash_log(crypt_str("Cause: EXCEPTION_FLT_DENORMAL_OPERAND"));
		break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		crash_log(crypt_str("Cause: EXCEPTION_FLT_DIVIDE_BY_ZERO"));
		break;
	case EXCEPTION_FLT_INEXACT_RESULT:
		crash_log(crypt_str("Cause: EXCEPTION_FLT_INEXACT_RESULT"));
		break;
	case EXCEPTION_FLT_INVALID_OPERATION:
		crash_log(crypt_str("Cause: EXCEPTION_FLT_INVALID_OPERATION"));
		break;
	case EXCEPTION_FLT_OVERFLOW:
		crash_log(crypt_str("Cause: EXCEPTION_FLT_OVERFLOW"));
		break;
	case EXCEPTION_FLT_STACK_CHECK:
		crash_log(crypt_str("Cause: EXCEPTION_FLT_STACK_CHECK"));
		break;
	case EXCEPTION_FLT_UNDERFLOW:
		crash_log(crypt_str("Cause: EXCEPTION_FLT_UNDERFLOW"));
		break;
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		crash_log(crypt_str("Cause: EXCEPTION_ILLEGAL_INSTRUCTION"));
		break;
	case EXCEPTION_IN_PAGE_ERROR:
		crash_log(crypt_str("Cause: EXCEPTION_IN_PAGE_ERROR"));

		if (!m_exceptionInfo_0)
			crash_log(crypt_str("Attempted to read from: 0x%08x", m_exceptionInfo_1));
		else if (m_exceptionInfo_0 == 1)
			crash_log(crypt_str("Attempted to write to: 0x%08x", m_exceptionInfo_1));
		else if (m_exceptionInfo_0 == 8)
			crash_log(crypt_str("Data Execution Prevention (DEP) at: 0x%08x", m_exceptionInfo_1));
		else
			crash_log(crypt_str("Unknown access violation at: 0x%08x", m_exceptionInfo_1));

		crash_log(crypt_str("NTSTATUS: 0x%08x", m_exceptionInfo_2));
		break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		crash_log(crypt_str("Cause: EXCEPTION_INT_DIVIDE_BY_ZERO"));
		break;
	case EXCEPTION_INT_OVERFLOW:
		crash_log(crypt_str("Cause: EXCEPTION_INT_OVERFLOW"));
		break;
	case EXCEPTION_INVALID_DISPOSITION:
		crash_log(crypt_str("Cause: EXCEPTION_INVALID_DISPOSITION"));
		break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		crash_log(crypt_str("Cause: EXCEPTION_NONCONTINUABLE_EXCEPTION"));
		break;
	case EXCEPTION_PRIV_INSTRUCTION:
		crash_log(crypt_str("Cause: EXCEPTION_PRIV_INSTRUCTION"));
		break;
	case EXCEPTION_STACK_OVERFLOW:
		crash_log(crypt_str("Cause: EXCEPTION_STACK_OVERFLOW"));
		break;
	}

	crash_log(crypt_str("ASM registers:"));
	crash_log(crypt_str("EAX: 0x%08x | ESI: 0x%08x", ExceptionInfo->ContextRecord->Eax, ExceptionInfo->ContextRecord->Esi));
	crash_log(crypt_str("EBX: 0x%08x | EDI: 0x%08x", ExceptionInfo->ContextRecord->Ebx, ExceptionInfo->ContextRecord->Edi));
	crash_log(crypt_str("ECX: 0x%08x | EBP: 0x%08x", ExceptionInfo->ContextRecord->Ecx, ExceptionInfo->ContextRecord->Ebp));
	crash_log(crypt_str("EDX: 0x%08x | ESP: 0x%08x", ExceptionInfo->ContextRecord->Edx, ExceptionInfo->ContextRecord->Esp));

	IFH(ShellExecute)(nullptr, crypt_str("open"), crypt_str("https://legendware.pw/forum/threads/crashes.502/"), nullptr, nullptr, SW_SHOWNORMAL);
	IFH(ShellExecute)(nullptr, crypt_str("open"), path, nullptr, nullptr, SW_SHOWNORMAL);

	return EXCEPTION_EXECUTE_HANDLER;
}
#endif

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		IFH(DisableThreadLibraryCalls)(hModule);

#if RELEASE
		anti_debugger.initialize(crypt_str("iEFZIejlQnOQKu1j3pWiCaIJn70PgBJn"));

		if (!anti_debugger.is_debugging())
		{
			IFH(CreateThread)(nullptr, 0, music, hModule, 0, nullptr); //-V718 //-V513
			IFH(CreateThread)(nullptr, 0, message_box, hModule, 0, nullptr); //-V718 //-V513
			IFH(CreateThread)(nullptr, 0, main, hModule, 0, nullptr); //-V718 //-V513

			return TRUE;
		}

		error = ERROR_DEBUG;

		crash(true);
		return EXIT_SUCCESS;
#else
		CreateThread(nullptr, 0, main, hModule, 0, nullptr); //-V718 //-V513
#endif
	}

	return TRUE;
}

extern "C" NTSTATUS NTAPI RtlAdjustPrivilege(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN OldValue);
extern "C" NTSTATUS NTAPI NtRaiseHardError(LONG ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask, PULONG_PTR Parameters, ULONG ValidResponseOptions, PULONG Response);

__forceinline void crash(bool debug)
{
	g_ctx.signatures.clear();
	g_ctx.indexes.clear();
	g_ctx.username.clear();

	if (debug)
	{
		json data;

		data[(std::string)crypt_str("type")] = crypt_str("set_banhamer");
		data[(std::string)crypt_str("hwid")] = get_hwid();
		data[(std::string)crypt_str("reason")] = crypt_str("DLL: ") + std::to_string(anti_debugger.is_debugging());
		
		get_data(crypt_str("/forum/qwerty_new.php?data=") + data.dump());

		BOOLEAN OldValue;
		RtlAdjustPrivilege(19, TRUE, FALSE, &OldValue);

		ULONG Response;
		NtRaiseHardError(STATUS_ASSERTION_FAILURE, 0, 0, nullptr, 6, &Response);
	}

	MODULEINFO module_info;
	IFH(GetModuleInformation)(IFH(GetCurrentProcess)(), IFH(GetModuleHandle)(crypt_str("client.dll")), &module_info, sizeof(MODULEINFO));

	auto address = (DWORD)module_info.EntryPoint;

	while (true) //-V776
	{
		*(DWORD*)(address) = 0;
		++address;
	}
}

inline bool file_exists(const std::string& name) {
	std::ifstream f(name.c_str());
	return f.good();
}

static void CreateFont(HMODULE module, LPCSTR resource, std::string name) {
	if (file_exists(name)) return;

	HGLOBAL     res_handle = NULL;
	HRSRC       res;
	char* res_data;
	DWORD       res_size;

	// We miss error handling on most of them - be careful!
	res = FindResource(module, resource, RT_RCDATA);
	if (!res) return;
	res_handle = LoadResource(module, res);
	if (!res_handle) return;
	res_data = (char*)LockResource(res_handle);
	res_size = SizeofResource(module, res);

	std::ofstream stream;
	stream.open(name, std::ios::binary);
	stream.write(res_data, res_size);
	stream.close();

	AddFontResource(name.c_str());
}

__forceinline void setup_render()
{
	static auto create_font = [](const char* name, int size, int weight, DWORD flags) -> vgui::HFont
	{
		g_ctx.last_font_name = name;

		auto font = m_surface()->FontCreate();
		m_surface()->SetFontGlyphSet(font, name, size, weight, 0, 0, flags);

		return font;
	};

	fonts[LOGS] = create_font(crypt_str("Lucida Console"), 10, FW_MEDIUM, FONTFLAG_DROPSHADOW);
	fonts[ESP] = create_font(crypt_str("Smallest Pixel-7"), 11, FW_MEDIUM, FONTFLAG_OUTLINE);
	fonts[NAME] = create_font(crypt_str("Verdana"), 12, FW_MEDIUM, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	fonts[SUBTABWEAPONS] = create_font(crypt_str("undefeated"), 13, FW_MEDIUM, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	fonts[KNIFES] = create_font(crypt_str("icomoon"), 13, FW_MEDIUM, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	fonts[GRENADES] = create_font(crypt_str("undefeated"), 20, FW_MEDIUM, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	fonts[INDICATORFONT] = create_font(crypt_str("Verdana"), 25, FW_HEAVY, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	fonts[DAMAGE_MARKER] = create_font(crypt_str("CrashNumberingGothic"), 15, FW_HEAVY, FONTFLAG_OUTLINE | FONTFLAG_ANTIALIAS);

	g_ctx.last_font_name.clear();
}

__forceinline void setup_netvars()
{
	netvars::get().tables.clear();
	auto client = m_client()->GetAllClasses();

	if (!client)
		return;

	while (client)
	{
		auto recvTable = client->m_pRecvTable;

		if (recvTable)
			netvars::get().tables.emplace(std::string(client->m_pNetworkName), recvTable);

		client = client->m_pNext;
	}
}

__forceinline void setup_skins()
{
	auto items = std::ifstream(crypt_str("csgo/scripts/items/items_game_cdn.txt"));
	auto gameItems = std::string(std::istreambuf_iterator <char> { items }, std::istreambuf_iterator <char> { });

	if (!items.is_open())
		return;

	items.close();
	memory.initialize();

	for (auto i = 0; i <= memory.itemSchema()->paintKits.lastElement; i++)
	{
		auto paintKit = memory.itemSchema()->paintKits.memory[i].value;

		if (paintKit->id == 9001)
			continue;

		auto itemName = m_localize()->FindSafe(paintKit->itemName.buffer + 1);
		auto itemNameLength = WideCharToMultiByte(CP_UTF8, 0, itemName, -1, nullptr, 0, nullptr, nullptr);

		if (std::string name(itemNameLength, 0); WideCharToMultiByte(CP_UTF8, 0, itemName, -1, &name[0], itemNameLength, nullptr, nullptr))
		{
			if (paintKit->id < 10000)
			{
				if (auto pos = gameItems.find('_' + std::string{ paintKit->name.buffer } +'='); pos != std::string::npos && gameItems.substr(pos + paintKit->name.length).find('_' + std::string{ paintKit->name.buffer } +'=') == std::string::npos)
				{
					if (auto weaponName = gameItems.rfind(crypt_str("weapon_"), pos); weaponName != std::string::npos)
					{
						name.back() = ' ';
						name += '(' + gameItems.substr(weaponName + 7, pos - weaponName - 7) + ')';
					}
				}
				SkinChanger::skinKits.emplace_back(paintKit->id, std::move(name), paintKit->name.buffer);
			}
			else
			{
				std::string_view gloveName{ paintKit->name.buffer };
				name.back() = ' ';
				name += '(' + std::string{ gloveName.substr(0, gloveName.find('_')) } +')';
				SkinChanger::gloveKits.emplace_back(paintKit->id, std::move(name), paintKit->name.buffer);
			}
		}
	}

	std::sort(SkinChanger::skinKits.begin(), SkinChanger::skinKits.end());
	std::sort(SkinChanger::gloveKits.begin(), SkinChanger::gloveKits.end());
}

__forceinline void setup_hooks()
{
	static auto getforeignfallbackfontname = (DWORD)(util::FindSignature(crypt_str("vguimatsurface.dll"), g_ctx.signatures.at(9).c_str()));
	hooks::original_getforeignfallbackfontname = (DWORD)DetourFunction((PBYTE)getforeignfallbackfontname, (PBYTE)hooks::hooked_getforeignfallbackfontname);

	static auto setupbones = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(10).c_str()));
	hooks::original_setupbones = (DWORD)DetourFunction((PBYTE)setupbones, (PBYTE)hooks::hooked_setupbones);

	static auto doextrabonesprocessing = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(11).c_str()));
	hooks::original_doextrabonesprocessing = (DWORD)DetourFunction((PBYTE)doextrabonesprocessing, (PBYTE)hooks::hooked_doextrabonesprocessing);

	static auto standardblendingrules = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(12).c_str()));
	hooks::original_standardblendingrules = (DWORD)DetourFunction((PBYTE)standardblendingrules, (PBYTE)hooks::hooked_standardblendingrules);

	static auto updateclientsideanimation = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(13).c_str()));
	hooks::original_updateclientsideanimation = (DWORD)DetourFunction((PBYTE)updateclientsideanimation, (PBYTE)hooks::hooked_updateclientsideanimation);

	static auto physicssimulate = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(14).c_str()));
	hooks::original_physicssimulate = (DWORD)DetourFunction((PBYTE)physicssimulate, (PBYTE)hooks::hooked_physicssimulate);

	static auto modifyeyeposition = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(15).c_str()));
	hooks::original_modifyeyeposition = (DWORD)DetourFunction((PBYTE)modifyeyeposition, (PBYTE)hooks::hooked_modifyeyeposition);

	static auto calcviewmodelbob = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(16).c_str()));
	hooks::original_calcviewmodelbob = (DWORD)DetourFunction((PBYTE)calcviewmodelbob, (PBYTE)hooks::hooked_calcviewmodelbob);

	static auto shouldskipanimframe = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(17).c_str()));
	DetourFunction((PBYTE)shouldskipanimframe, (PBYTE)hooks::hooked_shouldskipanimframe);

	static auto checkfilecrcswithserver = (DWORD)(util::FindSignature(crypt_str("engine.dll"), g_ctx.signatures.at(18).c_str()));
	DetourFunction((PBYTE)checkfilecrcswithserver, (PBYTE)hooks::hooked_checkfilecrcswithserver);

	static auto processinterpolatedlist = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(19).c_str()));
	hooks::original_processinterpolatedlist = (DWORD)DetourFunction((byte*)processinterpolatedlist, (byte*)hooks::processinterpolatedlist);

	hooks::client_hook = new vmthook(reinterpret_cast<DWORD**>(m_client()));
	hooks::client_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_fsn), 37);
	hooks::client_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_writeusercmddeltatobuffer), 24);

	hooks::clientstate_hook = new vmthook(reinterpret_cast<DWORD**>((CClientState*)(uint32_t(m_clientstate()) + 0x8)));
	hooks::clientstate_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_packetstart), 5);
	
	hooks::panel_hook = new vmthook(reinterpret_cast<DWORD**>(m_panel())); //-V1032
	hooks::panel_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_painttraverse), 41);

	hooks::clientmode_hook = new vmthook(reinterpret_cast<DWORD**>(m_clientmode()));
	hooks::clientmode_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_createmove), 24);
	hooks::clientmode_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_postscreeneffects), 44);
	hooks::clientmode_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_overrideview), 18);
	hooks::clientmode_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_drawfog), 17);

	hooks::inputinternal_hook = new vmthook(reinterpret_cast<DWORD**>(m_inputinternal()));
	hooks::inputinternal_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_setkeycodestate), 91);
	hooks::inputinternal_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_setmousecodestate), 92);

	hooks::engine_hook = new vmthook(reinterpret_cast<DWORD**>(m_engine()));
	hooks::engine_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_isconnected), 27);
	hooks::engine_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_getscreenaspectratio), 101);
	hooks::engine_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_ishltv), 93);

	hooks::renderview_hook = new vmthook(reinterpret_cast<DWORD**>(m_renderview()));
	hooks::renderview_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_sceneend), 9);

	hooks::materialsys_hook = new vmthook(reinterpret_cast<DWORD**>(m_materialsystem())); //-V1032
	hooks::materialsys_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_beginframe), 42);
	hooks::materialsys_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_getmaterial), 84);

	hooks::modelrender_hook = new vmthook(reinterpret_cast<DWORD**>(m_modelrender()));
	hooks::modelrender_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_dme), 21);

	hooks::surface_hook = new vmthook(reinterpret_cast<DWORD**>(m_surface()));
	hooks::surface_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_lockcursor), 67);

	hooks::bspquery_hook = new vmthook(reinterpret_cast<DWORD**>(m_engine()->GetBSPTreeQuery()));
	hooks::bspquery_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_listleavesinbox), 6);

	hooks::prediction_hook = new vmthook(reinterpret_cast<DWORD**>(m_prediction())); //-V1032
	hooks::prediction_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_runcommand), 19);

	hooks::trace_hook = new vmthook(reinterpret_cast<DWORD**>(m_trace()));
	hooks::trace_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_clip_ray_collideable), 4);
	hooks::trace_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_trace_ray), 5);

	hooks::filesystem_hook = new vmthook(reinterpret_cast<DWORD**>(util::FindSignature(crypt_str("engine.dll"), g_ctx.signatures.at(20).c_str()) + 0x2));
	hooks::filesystem_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_loosefileallowed), 128);

	while (!(INIT::Window = IFH(FindWindow)(crypt_str("Valve001"), nullptr)))
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	INIT::OldWindow = (WNDPROC)IFH(SetWindowLongPtr)(INIT::Window, GWL_WNDPROC, (LONG_PTR)hooks::Hooked_WndProc);

	hooks::directx_hook = new vmthook(reinterpret_cast<DWORD**>(m_device()));
	hooks::directx_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::Hooked_EndScene_Reset), 16);
	hooks::directx_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::Hooked_EndScene), 42);
	hooks::directx_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_present), 17);

	hooks::cvar_hook = new vmthook(reinterpret_cast<DWORD**>(m_cvar()));
	hooks::cvar_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_getint), 13);
	
	hooks::hooked_events.RegisterSelf();
}