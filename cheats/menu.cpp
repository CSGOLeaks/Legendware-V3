#include <ShlObj_core.h>
#include "menu.h"
#include "../ImGui/code_editor.h"
#include "../constchars.h"
#include "../cheats/misc/logs.h"

#define ALPHA (ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar| ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float)
#define NOALPHA (ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float)

// TODO: crypt_str check for all strings

std::vector <std::string> files;
std::vector <std::string> scripts;

int selected_script = 0;
static bool menu_setupped = false;
IDirect3DTexture9* all_skins[36];
std::string get_wep(int id, int custom_index = -1, bool knife = true)
{
	if (custom_index > -1)
	{
		if (knife)
		{
			switch (custom_index)
			{
			case 0: return "weapon_knife";
			case 1: return "weapon_bayonet";
			case 2: return "weapon_knife_css";
			case 3: return "weapon_knife_skeleton";
			case 4: return "weapon_knife_outdoor";
			case 5: return "weapon_knife_cord";
			case 6: return "weapon_knife_canis";
			case 7: return "weapon_knife_flip";
			case 8: return "weapon_knife_gut";
			case 9: return "weapon_knife_karambit";
			case 10: return "weapon_knife_m9_bayonet";
			case 11: return "weapon_knife_tactical";
			case 12: return "weapon_knife_falchion";
			case 13: return "weapon_knife_survival_bowie";
			case 14: return "weapon_knife_butterfly";
			case 15: return "weapon_knife_push";
			case 16: return "weapon_knife_ursus";
			case 17: return "weapon_knife_gypsy_jackknife";
			case 18: return "weapon_knife_stiletto";
			case 19: return "weapon_knife_widowmaker";
			}
		}
		else
		{
			switch (custom_index)
			{
			case 0: return "ct_gloves";
			case 1: return "studded_bloodhound_gloves";
			case 2: return "t_gloves";
			case 3: return "ct_gloves";
			case 4: return "sporty_gloves";
			case 5: return "slick_gloves";
			case 6: return "leather_handwraps";
			case 7: return "motorcycle_gloves";
			case 8: return "specialist_gloves";
			case 9: return "studded_hydra_gloves";
			}
		}
	}
	else
	{
		switch (id)
		{
		case 0: return "knife";
		case 1: return "gloves";
		case 2: return "weapon_ak47";
		case 3: return "weapon_aug";
		case 4: return "weapon_awp";
		case 5: return "weapon_cz75a";
		case 6: return "weapon_deagle";
		case 7: return "weapon_elite";
		case 8: return "weapon_famas";
		case 9: return "weapon_fiveseven";
		case 10: return "weapon_g3sg1";
		case 11: return "weapon_galilar";
		case 12: return "weapon_glock";
		case 13: return "weapon_m249";
		case 14: return "weapon_m4a1_silencer";
		case 15: return "weapon_m4a1";
		case 16: return "weapon_mac10";
		case 17: return "weapon_mag7";
		case 18: return "weapon_mp5sd";
		case 19: return "weapon_mp7";
		case 20: return "weapon_mp9";
		case 21: return "weapon_negev";
		case 22: return "weapon_nova";
		case 23: return "weapon_hkp2000";
		case 24: return "weapon_p250";
		case 25: return "weapon_p90";
		case 26: return "weapon_bizon";
		case 27: return "weapon_revolver";
		case 28: return "weapon_sawedoff";
		case 29: return "weapon_scar20";
		case 30: return "weapon_ssg08";
		case 31: return "weapon_sg556";
		case 32: return "weapon_tec9";
		case 33: return "weapon_ump45";
		case 34: return "weapon_usp_silencer";
		case 35: return "weapon_xm1014";
		default: return "unknown";
		}
	}
}

IDirect3DTexture9* get_skin_preview(const char* weapon_name, std::string skin_name, IDirect3DDevice9* device)
{
	IDirect3DTexture9* skin_image = nullptr;
	if (!skin_image)
	{
		std::string vpk_path;
		if (strcmp(weapon_name, "unknown") && strcmp(weapon_name, "knife") && strcmp(weapon_name, "gloves"))
		{
			if (skin_name == "" || skin_name == "default")
				vpk_path = "resource/flash/econ/weapons/base_weapons/" + std::string(weapon_name) + ".png";
			else
				vpk_path = "resource/flash/econ/default_generated/" + std::string(weapon_name) + "_" + std::string(skin_name) + "_light_large.png";
		}
		else
		{
			if (!strcmp(weapon_name, "knife"))
				vpk_path = "resource/flash/econ/weapons/base_weapons/weapon_knife.png";
			else if (!strcmp(weapon_name, "gloves"))
				vpk_path = "resource/flash/econ/weapons/base_weapons/ct_gloves.png";
			else if (!strcmp(weapon_name, "unknown"))
				vpk_path = "resource/flash/econ/weapons/base_weapons/weapon_snowball.png";

		}
		const auto handle = m_basefilesys()->Open(vpk_path.c_str(), "r", "GAME");
		if (handle)
		{
			int file_len = m_basefilesys()->Size(handle);
			char* image = new char[file_len];

			m_basefilesys()->Read(image, file_len, handle);
			m_basefilesys()->Close(handle);

			D3DXCreateTextureFromFileInMemory(device, image, file_len, &skin_image);

			delete[] image;
		}
	}
	if (skin_image != nullptr)
		return skin_image;
	else if (!skin_image)
	{
		std::string vpk_path;

	    if (strstr(weapon_name, "bloodhound") != NULL || strstr(weapon_name, "hydra") != NULL)
			vpk_path = "resource/flash/econ/weapons/base_weapons/ct_gloves.png";
		else
			vpk_path = "resource/flash/econ/weapons/base_weapons/" + std::string(weapon_name) + ".png";

		const auto handle = m_basefilesys()->Open(vpk_path.c_str(), "r", "GAME");
		if (handle)
		{
			int file_len = m_basefilesys()->Size(handle);
			char* image = new char[file_len];

			m_basefilesys()->Read(image, file_len, handle);
			m_basefilesys()->Close(handle);

			D3DXCreateTextureFromFileInMemory(device, image, file_len, &skin_image);

			delete[] image;
		}
	}

	return skin_image;
}

// setup some styles and colors, window size and bg alpha
// dpi setup
void c_menu::menu_setup(ImGuiStyle &style)
{
	ImGui::StyleColorsClassic(); // colors setup
	ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Once); // window pos setup
	ImGui::SetNextWindowBgAlpha(min(style.Alpha, 0.94f)); // window bg alpha setup

	styles.WindowPadding = style.WindowPadding;
	styles.WindowRounding = style.WindowRounding;
	styles.WindowMinSize = style.WindowMinSize;
	styles.ChildRounding = style.ChildRounding;
	styles.PopupRounding = style.PopupRounding;
	styles.FramePadding = style.FramePadding;
	styles.FrameRounding = style.FrameRounding;
	styles.ItemSpacing = style.ItemSpacing;
	styles.ItemInnerSpacing = style.ItemInnerSpacing;
	styles.TouchExtraPadding = style.TouchExtraPadding;
	styles.IndentSpacing = style.IndentSpacing;
	styles.ColumnsMinSpacing = style.ColumnsMinSpacing;
	styles.ScrollbarSize = style.ScrollbarSize;
	styles.ScrollbarRounding = style.ScrollbarRounding;
	styles.GrabMinSize = style.GrabMinSize;
	styles.GrabRounding = style.GrabRounding;
	styles.TabRounding = style.TabRounding;
	styles.TabMinWidthForUnselectedCloseButton = style.TabMinWidthForUnselectedCloseButton;
	styles.DisplayWindowPadding = style.DisplayWindowPadding;
	styles.DisplaySafeAreaPadding = style.DisplaySafeAreaPadding;
	styles.MouseCursorScale = style.MouseCursorScale;

	// setup skins preview
	for (auto i = 0; i < g_cfg.skins.skinChanger.size(); i++)
	{
		if (all_skins[i] == nullptr)
			all_skins[i] = get_skin_preview(get_wep(i, (i == 0 || i == 1) ? g_cfg.skins.skinChanger.at(i).definition_override_vector_index : -1, i == 0).c_str(), g_cfg.skins.skinChanger.at(i).skin_name, device);
	}

	dpi_scale = max(1, ImGui::GetIO().DisplaySize.y / 1080);
	update_dpi = true;
	menu_setupped = true; // we dont want to setup menu again
}

// resize current style sizes
void c_menu::dpi_resize(float scale_factor, ImGuiStyle &style)
{
	style.WindowPadding = (styles.WindowPadding * scale_factor);
	style.WindowRounding = (styles.WindowRounding * scale_factor);
	style.WindowMinSize = (styles.WindowMinSize * scale_factor);
	style.ChildRounding = (styles.ChildRounding * scale_factor);
	style.PopupRounding = (styles.PopupRounding * scale_factor);
	style.FramePadding = (styles.FramePadding * scale_factor);
	style.FrameRounding = (styles.FrameRounding * scale_factor);
	style.ItemSpacing = (styles.ItemSpacing * scale_factor);
	style.ItemInnerSpacing = (styles.ItemInnerSpacing * scale_factor);
	style.TouchExtraPadding = (styles.TouchExtraPadding * scale_factor);
	style.IndentSpacing = (styles.IndentSpacing * scale_factor);
	style.ColumnsMinSpacing = (styles.ColumnsMinSpacing * scale_factor);
	style.ScrollbarSize = (styles.ScrollbarSize * scale_factor);
	style.ScrollbarRounding = (styles.ScrollbarRounding * scale_factor);
	style.GrabMinSize = (styles.GrabMinSize * scale_factor);
	style.GrabRounding = (styles.GrabRounding * scale_factor);
	style.TabRounding = (styles.TabRounding * scale_factor);
	if (styles.TabMinWidthForUnselectedCloseButton != FLT_MAX)
		style.TabMinWidthForUnselectedCloseButton = (styles.TabMinWidthForUnselectedCloseButton * scale_factor);
	style.DisplayWindowPadding = (styles.DisplayWindowPadding * scale_factor);
	style.DisplaySafeAreaPadding = (styles.DisplaySafeAreaPadding * scale_factor);
	style.MouseCursorScale = (styles.MouseCursorScale * scale_factor);
}


std::string get_config_dir()
{
	std::string folder;
	static TCHAR path[MAX_PATH];

	if (SUCCEEDED(SHGetFolderPath(NULL, 0x001a, NULL, NULL, path)))
		folder = std::string(path) + crypt_str("\\Legendware\\Configs\\");

	CreateDirectory(folder.c_str(), NULL);
	return folder;
}

void load_config()
{
	if (cfg_manager->files.empty())
		return;

	cfg_manager->load(cfg_manager->files.at(g_cfg.selected_config), false);
	c_lua::get().unload_all_scripts();

	for (auto& script : g_cfg.scripts.scripts)
		c_lua::get().load_script(c_lua::get().get_script_id(script));

	scripts = c_lua::get().scripts;

	if (selected_script >= scripts.size())
		selected_script = scripts.size() - 1;

	for (auto& current : scripts)
	{
		if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
			current.erase(current.size() - 5, 5);
		else if (current.size() >= 4)
			current.erase(current.size() - 4, 4);
	}

	g_cfg.scripts.scripts.clear();

	cfg_manager->load(cfg_manager->files.at(g_cfg.selected_config), true);
	cfg_manager->config_files();

	eventlogs::get().add(crypt_str("Loaded ") + files.at(g_cfg.selected_config) + crypt_str(" config"), false);
}

void save_config()
{
	if (cfg_manager->files.empty())
		return;

	g_cfg.scripts.scripts.clear();

	for (auto i = 0; i < c_lua::get().scripts.size(); ++i)
	{
		auto script = c_lua::get().scripts.at(i);

		if (c_lua::get().loaded.at(i))
			g_cfg.scripts.scripts.emplace_back(script);
	}

	cfg_manager->save(cfg_manager->files.at(g_cfg.selected_config));
	cfg_manager->config_files();

	eventlogs::get().add(crypt_str("Saved ") + files.at(g_cfg.selected_config) + crypt_str(" config"), false);
}

void remove_config()
{
	if (cfg_manager->files.empty())
		return;

	eventlogs::get().add(crypt_str("Removed ") + files.at(g_cfg.selected_config) + crypt_str(" config"), false);

	cfg_manager->remove(cfg_manager->files.at(g_cfg.selected_config));
	cfg_manager->config_files();

	files = cfg_manager->files;

	if (g_cfg.selected_config >= files.size())
		g_cfg.selected_config = files.size() - 1;

	for (auto& current : files)
		if (current.size() > 2)
			current.erase(current.size() - 3, 3);
}

void add_config()
{
	auto empty = true;

	for (auto current : g_cfg.new_config_name)
	{
		if (current != ' ')
		{
			empty = false;
			break;
		}
	}

	if (empty)
		g_cfg.new_config_name = crypt_str("config");

	eventlogs::get().add(crypt_str("Added ") + g_cfg.new_config_name + crypt_str(" config"), false);

	if (g_cfg.new_config_name.find(crypt_str(".lw")) == std::string::npos)
		g_cfg.new_config_name += crypt_str(".lw");

	cfg_manager->save(g_cfg.new_config_name);
	cfg_manager->config_files();

	g_cfg.selected_config = cfg_manager->files.size() - 1;
	files = cfg_manager->files;

	for (auto& current : files)
		if (current.size() > 2)
			current.erase(current.size() - 3, 3);
}

__forceinline void padding(float x, float y)
{
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + x * c_menu::get().dpi_scale);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + y * c_menu::get().dpi_scale);
}


// title of content child
void child_title(const char* label)
{
	ImGui::PushFont(c_menu::get().futura_large);
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 1.f));

	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (12 * c_menu::get().dpi_scale));
	ImGui::Text(label);

	ImGui::PopStyleColor();
	ImGui::PopFont();
}

void draw_combo(const char* name, int& variable, const char* labels[], int count)
{
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 6 * c_menu::get().dpi_scale);
	ImGui::Text(name);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 5 * c_menu::get().dpi_scale);
	ImGui::Combo(std::string("##COMBO__" + std::string(name)).c_str(), &variable, labels, count);
}

void draw_multicombo(const char* name, std::vector<int>& variable, const char* labels[], int count, std::string &preview )
{
	padding(-3, -6);
	ImGui::Text((" " + (std::string)name).c_str());
	padding(0, -5);

	std::string hashname = "##" + std::string(name); // we dont want to render name of combo

	for (auto i = 0, j = 0; i < count; i++)
	{
		if (variable[i])
		{
			if (j)
				preview += crypt_str(", ") + (std::string)labels[i];
			else
				preview = labels[i];

			j++;
		}
	}

	if (ImGui::BeginCombo(crypt_str(hashname.c_str()), preview.c_str())) // draw start
	{
		ImGui::Spacing();    
		ImGui::BeginGroup();
		{

			for (auto i = 0; i < count; i++)
				ImGui::Selectable(labels[i], (bool*)&variable[i], ImGuiSelectableFlags_DontClosePopups);

		}
		ImGui::EndGroup();
		ImGui::Spacing();

		ImGui::EndCombo();
	}

	preview = crypt_str("None"); // reset preview to use later
}

bool LabelClick(const char* label, bool* v, const char* unique_id)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	// The concatoff/on thingies were for my weapon config system so if we're going to make that, we still need this aids.
	char Buf[64];
	_snprintf(Buf, 62, crypt_str("%s"), label);

	char getid[128];
	sprintf_s(getid, 128, crypt_str("%s%s"), label, unique_id);


	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(getid);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

	const ImRect check_bb(window->DC.CursorPos, ImVec2(label_size.y + style.FramePadding.y * 2 + window->DC.CursorPos.x, window->DC.CursorPos.y + label_size.y + style.FramePadding.y * 2));
	ImGui::ItemSize(check_bb, style.FramePadding.y);

	ImRect total_bb = check_bb;

	if (label_size.x > 0)
	{
		ImGui::SameLine(0, style.ItemInnerSpacing.x);
		const ImRect text_bb(ImVec2(window->DC.CursorPos.x, window->DC.CursorPos.y + style.FramePadding.y), ImVec2(window->DC.CursorPos.x + label_size.x, window->DC.CursorPos.y + style.FramePadding.y + label_size.y));

		ImGui::ItemSize(ImVec2(text_bb.GetWidth(), check_bb.GetHeight()), style.FramePadding.y);
		total_bb = ImRect(ImMin(check_bb.Min, text_bb.Min), ImMax(check_bb.Max, text_bb.Max));
	}

	if (!ImGui::ItemAdd(total_bb, id))
		return false;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);
	if (pressed)
		*v = !(*v);

	if (*v)
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(126 / 255.f, 131 / 255.f, 219 / 255.f, 1.f));
	if (label_size.x > 0.0f)
		ImGui::RenderText(ImVec2(check_bb.GetTL().x + 12, check_bb.GetTL().y), Buf);
	if (*v)
		ImGui::PopStyleColor();

	return pressed;

}


void draw_keybind(const char* label, key_bind* key_bind, const char* unique_id)
{
	// reset bind if we re pressing esc
	if (key_bind->key == KEY_ESCAPE)
		key_bind->key = KEY_NONE;

	auto clicked = false;
	auto text = (std::string)m_inputsys()->ButtonCodeToString(key_bind->key);

	if (key_bind->key <= KEY_NONE || key_bind->key >= KEY_MAX)
		text = crypt_str("None");

	// if we clicked on keybind
	if (hooks::input_shouldListen && hooks::input_receivedKeyval == &key_bind->key)
	{
		clicked = true;
		text = crypt_str("...");
	}

	auto textsize = ImGui::CalcTextSize(text.c_str()).x + 8 * c_menu::get().dpi_scale;
	auto labelsize = ImGui::CalcTextSize(label);

	ImGui::Text(label);
	ImGui::SameLine();

	ImGui::SetCursorPosX(ImGui::GetWindowSize().x - (ImGui::GetWindowSize().x - ImGui::CalcItemWidth()) - max(50 * c_menu::get().dpi_scale, textsize));
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3 * c_menu::get().dpi_scale);

	if (ImGui::KeybindButton(text.c_str(), unique_id, ImVec2(max(50 * c_menu::get().dpi_scale, textsize), 23 * c_menu::get().dpi_scale), clicked))
		clicked = true;


	if (clicked)
	{
		hooks::input_shouldListen = true;
		hooks::input_receivedKeyval = &key_bind->key;
	}

	static auto hold = false, toggle = false;

	switch (key_bind->mode)
	{
	case HOLD:
		hold = true;
		toggle = false;
		break;
	case TOGGLE:
		toggle = true;
		hold = false;
		break;
	}

	if (ImGui::BeginPopup(unique_id))
	{
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6 * c_menu::get().dpi_scale);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ((ImGui::GetCurrentWindow()->Size.x / 2) - (ImGui::CalcTextSize("Hold").x / 2)));
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 11);

		if (LabelClick(crypt_str("Hold"), &hold, unique_id))
		{
			if (hold)
			{
				toggle = false;
				key_bind->mode = HOLD;
			}
			else if (toggle)
			{
				hold = false;
				key_bind->mode = TOGGLE;
			}
			else
			{
				toggle = false;
				key_bind->mode = HOLD;
			}

			ImGui::CloseCurrentPopup();
		}

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ((ImGui::GetCurrentWindow()->Size.x / 2) - (ImGui::CalcTextSize("Toggle").x / 2)));
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 11);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 9 * c_menu::get().dpi_scale);

		if (LabelClick(crypt_str("Toggle"), &toggle, unique_id))
		{
			if (toggle)
			{
				hold = false;
				key_bind->mode = TOGGLE;
			}
			else if (hold)
			{
				toggle = false;
				key_bind->mode = HOLD;
			}
			else
			{
				hold = false;
				key_bind->mode = TOGGLE;
			}

			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void draw_semitabs(const char* labels[], int count, int &tab, ImGuiStyle style)
{
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - (2 * c_menu::get().dpi_scale));

	// center of main child
	float offset = 343 * c_menu::get().dpi_scale;

	// text size padding + frame padding
	for (int i = 0; i < count; i++)
		offset -= (ImGui::CalcTextSize(labels[i]).x) / 2 + style.FramePadding.x * 2;
	
	// set new padding
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
	ImGui::BeginGroup();

	for (int i = 0; i < count; i++)
	{
		// switch current tab
		if (ImGui::ContentTab(labels[i], tab == i))
			tab = i;

		// continue drawing on same line 
		if (i + 1 != count) 
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + style.ItemSpacing.x);
		}
	}

	ImGui::EndGroup();
}

__forceinline void tab_start()
{
	ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + (20 * c_menu::get().dpi_scale), ImGui::GetCursorPosY() + (5 * c_menu::get().dpi_scale)));
}

__forceinline void tab_end()
{
	ImGui::PopStyleVar();

	ImGui::SetWindowFontScale(c_menu::get().dpi_scale);
}

// TODO: do it for every script
void lua_edit(std::string window_name)
{
	const char* child_name = (window_name + window_name).c_str();

	ImGui::SetNextWindowSize(ImVec2(700, 600), ImGuiCond_Once);
	ImGui::Begin(window_name.c_str(), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 5.f);

	static bool load = false;
	static TextEditor editor;
	if (!load)
	{
		auto lang = TextEditor::LanguageDefinition::Lua();
		editor.SetLanguageDefinition(lang);
		editor.SetReadOnly(false);

		// TODO: get correct path to lua file (now it's Counter Strike../lua/..)
		static const char* fileToEdit = "lua/kon_utils.lua";

		{
			std::ifstream t(fileToEdit);
			if (t.good()) // does while exist?
			{
				std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
				editor.SetText(str); // setup script content
			}

		}

		load = true;
	}

	// dpi scale for font
	// we dont need to resize it for full scale
	ImGui::SetWindowFontScale(1.f + ((c_menu::get().dpi_scale - 1.0) * 0.5f));
	
	// new size depending on dpi scale
	ImGui::SetWindowSize(ImVec2(ImFloor(800 * (1.f + ((c_menu::get().dpi_scale - 1.0) * 0.5f))), ImFloor(700 * (1.f + ((c_menu::get().dpi_scale - 1.0) * 0.5f)))));

	editor.Render(child_name, ImGui::GetWindowSize() - ImVec2(0, 66 * (1.f + ((c_menu::get().dpi_scale - 1.0) * 0.5f))));

	// seperate code with buttons
	ImGui::Separator();
	
	// set cursor pos to right edge of window
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetWindowSize().x - (16.f * (1.f + ((c_menu::get().dpi_scale - 1.0) * 0.5f))) - (250.f * (1.f + ((c_menu::get().dpi_scale - 1.0) * 0.5f))));
	ImGui::BeginGroup();

	if (ImGui::CustomButton("Save", ("Save" + window_name).c_str(), ImVec2(125 * (1.f + ((c_menu::get().dpi_scale - 1.0) * 0.5f)), 0), true, c_menu::get().ico_bottom, "S"))
	{
		std::ofstream out;

		// TODO: correct file path
		out.open("lua/kon_utils.lua");
		out << editor.GetText() << std::endl;
		out.close();
	}

	ImGui::SameLine();

	// TOOD: close button will close window (return in start of function)
	if (ImGui::CustomButton("Close", ("Close" + window_name).c_str(), ImVec2(125 * (1.f + ((c_menu::get().dpi_scale - 1.0) * 0.5f)), 0)))
	{

	}

	ImGui::EndGroup();


	ImGui::PopStyleVar();
	ImGui::End();
}



// SEMITABS FUNCTIONS
void c_menu::draw_tabs_ragebot()
{
	const char* rage_sections[2] = { "General", "Weapon" };
	draw_semitabs(rage_sections, 2, rage_section, ImGui::GetStyle());
}

void c_menu::draw_ragebot(int child)
{
	if (!rage_section)
	{
		if (!child)
		{
			child_title("Ragebot");

			tab_start();
			ImGui::BeginChild("##RAGE1_FIRST", ImVec2(317 * dpi_scale, (child_height - 35) * dpi_scale));
			{

				ImGui::Checkbox(crypt_str("Enable"), &g_cfg.ragebot.enable);

				if (g_cfg.ragebot.enable)
					g_cfg.legitbot.enabled = false;

				ImGui::SliderInt(crypt_str("Field of view"), &g_cfg.ragebot.field_of_view, 1, 180);
				ImGui::Checkbox(crypt_str("Silent aim"), &g_cfg.ragebot.silent_aim);
				ImGui::Checkbox(crypt_str("Automatic wall"), &g_cfg.ragebot.autowall);
				ImGui::Checkbox(crypt_str("Aimbot with zeus"), &g_cfg.ragebot.zeus_bot);
				ImGui::Checkbox(crypt_str("Aimbot with knife"), &g_cfg.ragebot.knife_bot);
				ImGui::Checkbox(crypt_str("Automatic fire"), &g_cfg.ragebot.autoshoot);
				ImGui::Checkbox(crypt_str("Automatic scope"), &g_cfg.ragebot.autoscope);
				ImGui::Checkbox(crypt_str("Pitch desync correction"), &g_cfg.ragebot.pitch_antiaim_correction);

				ImGui::Spacing();

				ImGui::Checkbox(crypt_str("Double tap"), &g_cfg.ragebot.double_tap);

				if (g_cfg.ragebot.double_tap)
				{
					ImGui::SameLine();
					draw_keybind("", &g_cfg.ragebot.double_tap_key, "##HOTKEY_DT");
					ImGui::Checkbox(crypt_str("Slow teleport"), &g_cfg.ragebot.slow_teleport);				
				}

				ImGui::Checkbox(crypt_str("Hide shots"), &g_cfg.antiaim.hide_shots);

				if (g_cfg.antiaim.hide_shots)
				{
					ImGui::SameLine();
					draw_keybind("", &g_cfg.antiaim.hide_shots_key, "##HOTKEY_HIDESHOTS");
				}
			}
			ImGui::EndChild();
			tab_end();
		}
		else
		{
			child_title("Anti-aim");

			tab_start();
			ImGui::BeginChild("##RAGE2_FIRST", ImVec2(317 * dpi_scale, (child_height - 35) * dpi_scale));
			{

				static auto type = 0;

				ImGui::Checkbox(crypt_str("Enable"), &g_cfg.antiaim.enable);
				draw_combo(crypt_str("Anti-aim type"), g_cfg.antiaim.antiaim_type, antiaim_type, ARRAYSIZE(antiaim_type));

				if (g_cfg.antiaim.antiaim_type)
				{
					padding(0, 3);
					draw_combo(crypt_str("Desync"), g_cfg.antiaim.desync, desync, ARRAYSIZE(desync));

					if (g_cfg.antiaim.desync)
					{
						padding(0, 3);
						draw_combo(crypt_str("LBY type"), g_cfg.antiaim.legit_lby_type, lby_type, ARRAYSIZE(lby_type));

						if (g_cfg.antiaim.desync == 1)
						{
							draw_keybind("Invert desync", &g_cfg.antiaim.flip_desync, "##HOTKEY_INVERT_DESYNC");
						}
					}
				}
				else
				{
					draw_combo(crypt_str("Movement type"), type, movement_type, ARRAYSIZE(movement_type));
					padding(0, 3);
					draw_combo(crypt_str("Pitch"), g_cfg.antiaim.type[type].pitch, pitch, ARRAYSIZE(pitch));
					padding(0, 3);
					draw_combo(crypt_str("Yaw"), g_cfg.antiaim.type[type].yaw, yaw, ARRAYSIZE(yaw));
					padding(0, 3);
					draw_combo(crypt_str("Base angle"), g_cfg.antiaim.type[type].base_angle, baseangle, ARRAYSIZE(baseangle));

					if (g_cfg.antiaim.type[type].yaw)
					{
						ImGui::SliderInt(g_cfg.antiaim.type[type].yaw == 1 ? crypt_str("Jitter range") : crypt_str("Spin range"), &g_cfg.antiaim.type[type].range, 1, 180);

						if (g_cfg.antiaim.type[type].yaw == 2)
							ImGui::SliderInt(crypt_str("Spin speed"), &g_cfg.antiaim.type[type].speed, 1, 15);

						padding(0, 3);
					}

					draw_combo(crypt_str("Desync"), g_cfg.antiaim.type[type].desync, desync, ARRAYSIZE(desync));

					if (g_cfg.antiaim.type[type].desync)
					{
						if (type == ANTIAIM_STAND)
						{
							padding(0, 3);
							draw_combo(crypt_str("LBY type"), g_cfg.antiaim.lby_type, lby_type, ARRAYSIZE(lby_type));
						}

						if (type != ANTIAIM_STAND || !g_cfg.antiaim.lby_type)
						{
							ImGui::SliderInt(crypt_str("Desync range"), &g_cfg.antiaim.type[type].desync_range, 1, 100, "%.0f%%");
							ImGui::SliderInt(crypt_str("Body lean"), &g_cfg.antiaim.type[type].body_lean, 0, 100);
							ImGui::SliderInt(crypt_str("Inverted body lean"), &g_cfg.antiaim.type[type].inverted_body_lean, 0, 100);
						}

						if (g_cfg.antiaim.type[type].desync == 1)
						{
							draw_keybind("Invert desync", &g_cfg.antiaim.flip_desync, "##HOTKEY_INVERT_DESYNC");
						}
					}

					draw_keybind("Manual back", &g_cfg.antiaim.manual_back, "##HOTKEY_INVERT_BACK");

					draw_keybind("Manual left", &g_cfg.antiaim.manual_left, "##HOTKEY_INVERT_LEFT");

					draw_keybind("Manual right", &g_cfg.antiaim.manual_right, "##HOTKEY_INVERT_RIGHT");

					if (g_cfg.antiaim.manual_back.key > KEY_NONE && g_cfg.antiaim.manual_back.key < KEY_MAX || g_cfg.antiaim.manual_left.key > KEY_NONE && g_cfg.antiaim.manual_left.key < KEY_MAX || g_cfg.antiaim.manual_right.key > KEY_NONE && g_cfg.antiaim.manual_right.key < KEY_MAX)
					{
						ImGui::Checkbox(crypt_str("Manuals indicator"), &g_cfg.antiaim.flip_indicator);
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##invc"), &g_cfg.antiaim.flip_indicator_color, ALPHA);
					}
				}

			}
			ImGui::EndChild();
			tab_end();
		}
	}
	else
	{
		static int rage_weapon = 0;
		const char* rage_weapons[8] = { "R8 & Deagle", "Pistols", "SMGs", "Rifles", "Auto snipers", "Scout", "AWP", "Heavy" };
		if (!child)
		{
			child_title("General");

			tab_start();
			ImGui::BeginChild("##RAGE1_SECOND", ImVec2(317 * dpi_scale, (child_height - 35) * dpi_scale));
			{
				ImGui::Spacing();

				draw_combo(crypt_str("Current weapon"), rage_weapon, rage_weapons, ARRAYSIZE(rage_weapons));
				ImGui::Spacing();

				draw_combo(crypt_str("Target selection"), g_cfg.ragebot.weapon[rage_weapon].selection_type, selection, ARRAYSIZE(selection));
				padding(0, 3);
				draw_multicombo(crypt_str("Hitboxes"), g_cfg.ragebot.weapon[rage_weapon].hitscan, hitboxes, ARRAYSIZE(hitboxes), preview);

				// TODO: Uncomment me
				//ImGui::Checkbox(crypt_str("Enable max misses"), &g_cfg.ragebot.weapon[rage_weapon].max_misses);
				//
				//if (g_cfg.ragebot.weapon[rage_weapon].max_misses)
				//	ImGui::SliderInt("Max misses", &g_cfg.ragebot.weapon[rage_weapon].max_misses_amount, 0, 6);

				//ImGui::Checkbox(crypt_str("Prefer safe points"), &g_cfg.ragebot.weapon[rage_weapon].prefer_safe_points);
				//ImGui::Checkbox(crypt_str("Prefer body aim"), &g_cfg.ragebot.weapon[rage_weapon].prefer_body_aim);

				//draw_keybind("Force safe points", &g_cfg.ragebot.safe_point_key, "##HOKEY_FORCE_SAFE_POINTS");

				draw_keybind("Force body aim", &g_cfg.ragebot.baim_key, "##HOKEY_FORCE_BODY_AIM");

			}
			ImGui::EndChild();
			tab_end();
		}
		else
		{
			child_title("Accuracy");

			tab_start();
			ImGui::BeginChild("##RAGE2_SECOND", ImVec2(317 * dpi_scale, (child_height - 35) * dpi_scale));
			{
				// TODO: проверь, чтобы все настройки тут были из обычного меню текущего

				ImGui::Checkbox(crypt_str("Automatic stop"), &g_cfg.ragebot.weapon[rage_weapon].autostop);

				if (g_cfg.ragebot.weapon[rage_weapon].autostop)
					draw_multicombo("Modifiers", g_cfg.ragebot.weapon[rage_weapon].autostop_modifiers, autostop_modifiers, ARRAYSIZE(autostop_modifiers), preview);

				ImGui::Checkbox(crypt_str("Hitchance"), &g_cfg.ragebot.weapon[rage_weapon].hitchance);

				if (g_cfg.ragebot.weapon[rage_weapon].hitchance)
					ImGui::SliderInt(crypt_str("Hitchance amount"), &g_cfg.ragebot.weapon[rage_weapon].hitchance_amount, 1, 100);

				if (g_cfg.ragebot.double_tap && rage_weapon <= 4)
				{
					ImGui::Checkbox(crypt_str("Double tap hitchance"), &g_cfg.ragebot.weapon[rage_weapon].double_tap_hitchance);

					if (g_cfg.ragebot.weapon[rage_weapon].double_tap_hitchance)
						ImGui::SliderInt(crypt_str("Double tap hitchance amount"), &g_cfg.ragebot.weapon[rage_weapon].double_tap_hitchance_amount, 1, 100);
				}

				ImGui::SliderInt(crypt_str("Minimum visible damage"), &g_cfg.ragebot.weapon[rage_weapon].minimum_visible_damage, 1, 120);

				if (g_cfg.ragebot.autowall)
					ImGui::SliderInt(crypt_str("Minimum wall damage"), &g_cfg.ragebot.weapon[rage_weapon].minimum_damage, 1, 120);

				draw_keybind("Damage override", &g_cfg.ragebot.weapon[rage_weapon].damage_override_key, "##HOTKEY__DAMAGE_OVERRIDE");

				if (g_cfg.ragebot.weapon[rage_weapon].damage_override_key.key > KEY_NONE && g_cfg.ragebot.weapon[rage_weapon].damage_override_key.key < KEY_MAX)
					ImGui::SliderInt(crypt_str("Minimum override damage"), &g_cfg.ragebot.weapon[rage_weapon].minimum_override_damage, 1, 120);

			}
			ImGui::EndChild();
			tab_end();
		}
	}
}


/*


struct weapon_t
{

	int awall_dmg;

	float target_switch_delay;
} weapon[8];
*/

void c_menu::draw_tabs_legit()
{
	const char* legit_sections[1] = { "General" };
	draw_semitabs(legit_sections, 1, legit_section, ImGui::GetStyle());
}

void c_menu::draw_legit(int child)
{
	static int legit_weapon = 0;
	const char* legit_weapons[6] = { "Deagle", "Pistols", "Rifles", "SMGs", "Sniper", "Heavy" };
	const char* hitbox_legit[3] = { "Nearest", "Head", "Body" };

	if (!child)
	{
		child_title("Legitbot");

		tab_start();
		ImGui::BeginChild("##RAGE1_FIRST", ImVec2(317 * dpi_scale, (child_height - 35) * dpi_scale));
		{
			padding(0, 6);
			ImGui::Checkbox(crypt_str("Enabled"), &g_cfg.legitbot.enabled);
			ImGui::SameLine();
			draw_keybind(crypt_str(""), &g_cfg.legitbot.key, crypt_str("##HOTKEY_LGBT_KEY"));

			if (g_cfg.legitbot.enabled)
				g_cfg.ragebot.enable = false;

			ImGui::Checkbox(crypt_str("Friendly fire"), &g_cfg.legitbot.friendly_fire);
			ImGui::Checkbox(crypt_str("Auto pistols"), &g_cfg.legitbot.autopistol);

			ImGui::Checkbox(crypt_str("Auto scope"), &g_cfg.legitbot.autoscope);

			if (g_cfg.legitbot.autoscope)
				ImGui::Checkbox(crypt_str("Unscope after shot"), &g_cfg.legitbot.unscope);

			ImGui::Checkbox(crypt_str("Sniper in zoom only"), &g_cfg.legitbot.sniper_in_zoom_only);

			ImGui::Checkbox(crypt_str("Scan in air"), &g_cfg.legitbot.do_if_local_in_air);
			ImGui::Checkbox(crypt_str("Scan if flashed"), &g_cfg.legitbot.do_if_local_flashed);
			ImGui::Checkbox(crypt_str("Scan in smoke"), &g_cfg.legitbot.do_if_enemy_in_smoke);

			draw_keybind(crypt_str("Autofire key"), &g_cfg.legitbot.autofire_key, crypt_str("##HOTKEY_AUTOFIRE_LGBT_KEY"));
			ImGui::SliderInt(crypt_str("Autofire delay"), &g_cfg.legitbot.autofire_delay, 0, 12, (!g_cfg.legitbot.autofire_delay ? "No" : (g_cfg.legitbot.autofire_delay == 1 ? "%dtick" : "%dticks")));

		}
		ImGui::EndChild();

		
		tab_end();
	}
	else
	{
		child_title("Weapon settings");

		tab_start();
		ImGui::BeginChild("##RAGE1_FIRST", ImVec2(317 * dpi_scale, (child_height - 35) * dpi_scale));
		{
			ImGui::Spacing();
			draw_combo(crypt_str("Weapon preset"), legit_weapon, legit_weapons, ARRAYSIZE(legit_weapons));
			ImGui::Spacing();
			draw_combo(crypt_str("Hitbox"), g_cfg.legitbot.weapon[legit_weapon].priority, hitbox_legit, ARRAYSIZE(hitbox_legit));

			ImGui::Checkbox(crypt_str("Auto stop"), &g_cfg.legitbot.weapon[legit_weapon].auto_stop);

			draw_combo(crypt_str("Fov type"), g_cfg.legitbot.weapon[legit_weapon].fov_type, LegitFov, ARRAYSIZE(LegitFov));
			ImGui::SliderFloat(crypt_str("Fov amount"), &g_cfg.legitbot.weapon[legit_weapon].fov, 0.f, 30.f, "%.2f");

			ImGui::Spacing();

			ImGui::SliderFloat(crypt_str("Silent fov"), &g_cfg.legitbot.weapon[legit_weapon].silent_fov, 0.f, 30.f, (!g_cfg.legitbot.weapon[legit_weapon].silent_fov ? "No" : "%.2f"));

			ImGui::Spacing();

			draw_combo(crypt_str("Smooth type"), g_cfg.legitbot.weapon[legit_weapon].smooth_type, LegitSmooth, ARRAYSIZE(LegitSmooth));
			ImGui::SliderFloat(crypt_str("Smooth amount"), &g_cfg.legitbot.weapon[legit_weapon].smooth, 1.f, 12.f, "%.1f");

			ImGui::Spacing();

			draw_combo(crypt_str("RCS type"), g_cfg.legitbot.weapon[legit_weapon].rcs_type, RCSType, ARRAYSIZE(RCSType));
			ImGui::SliderFloat(crypt_str("RCS amount"), &g_cfg.legitbot.weapon[legit_weapon].rcs, 0.f, 100.f, "%.0f%%", 1.f);
			if (g_cfg.legitbot.weapon[legit_weapon].rcs > 0)
			{
				ImGui::SliderFloat(crypt_str("RCS Custom Fov"), &g_cfg.legitbot.weapon[legit_weapon].custom_rcs_fov, 0.f, 30.f, (!g_cfg.legitbot.weapon[legit_weapon].custom_rcs_fov ? "No" : "%.2f"));
				ImGui::SliderFloat(crypt_str("RCS Custom Smooth"), &g_cfg.legitbot.weapon[legit_weapon].custom_rcs_smooth, 0.f, 12.f, (!g_cfg.legitbot.weapon[legit_weapon].custom_rcs_smooth ? "No" : "%.1f"));
			}

			ImGui::Spacing();

			ImGui::SliderInt(crypt_str("Autowall damage"), &g_cfg.legitbot.weapon[legit_weapon].awall_dmg, 0, 100, (!g_cfg.legitbot.weapon[legit_weapon].awall_dmg ? "No" : "%d"));
			ImGui::SliderInt(crypt_str("Autofire hitchance"), &g_cfg.legitbot.weapon[legit_weapon].autofire_hitchance, 0, 100, (!g_cfg.legitbot.weapon[legit_weapon].autofire_hitchance ? "No" : "%d"));
			ImGui::SliderFloat(crypt_str("Target switch delay"), &g_cfg.legitbot.weapon[legit_weapon].target_switch_delay, 0.f, 1.f, "%.3fs");
		}
		ImGui::EndChild();


		tab_end();
	}
}



void c_menu::draw_tabs_visuals()
{
	const char* visuals_sections[3] = { "General", "Viewmodel", "Skinchanger" };
	draw_semitabs(visuals_sections, 3, visuals_section, ImGui::GetStyle());
}

void c_menu::draw_visuals(int child)
{
	if (visuals_section != 2)
	{

		if (!child)
		{
			if (!visuals_section)
			{
				child_title("General");

				tab_start();
				ImGui::BeginChild("##VISUAL1_FIRST", ImVec2(317 * dpi_scale, (child_height - 35) * dpi_scale));
				{
					ImGui::Checkbox(crypt_str("Enable"), &g_cfg.player.enable);

					draw_multicombo("Indicators", g_cfg.esp.indicators, indicators, ARRAYSIZE(indicators), preview);

					padding(0, 3);

					draw_multicombo("Removals", g_cfg.esp.removals, removals, ARRAYSIZE(removals), preview);
					if (g_cfg.esp.removals[REMOVALS_ZOOM])
						ImGui::Checkbox(crypt_str("Fix zoom sensivity"), &g_cfg.esp.fix_zoom_sensivity);

					ImGui::Checkbox(crypt_str("Grenade prediction"), &g_cfg.esp.grenade_prediction);
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("##grenpredcolor"), &g_cfg.esp.grenade_prediction_color, ALPHA);

					if (g_cfg.esp.grenade_prediction)
					{
						ImGui::Checkbox(crypt_str("On click"), &g_cfg.esp.on_click);
						ImGui::Text(crypt_str("Tracer color "));
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##tracergrenpredcolor"), &g_cfg.esp.grenade_prediction_tracer_color, ALPHA);
					}

					ImGui::Checkbox(crypt_str("Grenade projectiles"), &g_cfg.esp.projectiles);

					if (g_cfg.esp.projectiles)
						draw_multicombo("Grenade ESP", g_cfg.esp.grenade_esp, proj_combo, ARRAYSIZE(proj_combo), preview);				

					if (g_cfg.esp.grenade_esp[GRENADE_ICON] || g_cfg.esp.grenade_esp[GRENADE_TEXT])
					{
						ImGui::Text(crypt_str("Color "));
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##projectcolor"), &g_cfg.esp.projectiles_color, ALPHA);
					}

					if (g_cfg.esp.grenade_esp[GRENADE_BOX])
					{
						ImGui::Text(crypt_str("Box color "));
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##grenade_box_color"), &g_cfg.esp.grenade_box_color, ALPHA);
					}

					if (g_cfg.esp.grenade_esp[GRENADE_GLOW])
					{
						ImGui::Text(crypt_str("Glow color "));
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##grenade_glow_color"), &g_cfg.esp.grenade_glow_color, ALPHA);
					}

					ImGui::Checkbox(crypt_str("Fire timer"), &g_cfg.esp.molotov_timer);
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("##molotovcolor"), &g_cfg.esp.molotov_timer_color, ALPHA);

					ImGui::Checkbox(crypt_str("Bomb indicator"), &g_cfg.esp.bomb_timer);

					draw_multicombo("Weapon ESP", g_cfg.esp.weapon, weaponesp, ARRAYSIZE(weaponesp), preview);

					if (g_cfg.esp.weapon[WEAPON_ICON] || g_cfg.esp.weapon[WEAPON_TEXT] || g_cfg.esp.weapon[WEAPON_DISTANCE])
					{
						ImGui::Text(crypt_str("Color "));
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##weaponcolor"), &g_cfg.esp.weapon_color, ALPHA);
					}

					if (g_cfg.esp.weapon[WEAPON_BOX])
					{
						ImGui::Text(crypt_str("Box color "));
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##weaponboxcolor"), &g_cfg.esp.box_color, ALPHA);
					}

					if (g_cfg.esp.weapon[WEAPON_GLOW])
					{
						ImGui::Text(crypt_str("Glow color "));
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##weaponglowcolor"), &g_cfg.esp.weapon_glow_color, ALPHA);
					}

					if (g_cfg.esp.weapon[WEAPON_AMMO])
					{
						ImGui::Text(crypt_str("Ammo bar color "));
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##weaponammocolor"), &g_cfg.esp.weapon_ammo_color, ALPHA);
					}

					ImGui::Checkbox(crypt_str("Client bullet impacts"), &g_cfg.esp.client_bullet_impacts);
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("##clientbulletimpacts"), &g_cfg.esp.client_bullet_impacts_color, ALPHA);

					ImGui::Checkbox(crypt_str("Server bullet impacts"), &g_cfg.esp.server_bullet_impacts);
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("##serverbulletimpacts"), &g_cfg.esp.server_bullet_impacts_color, ALPHA);

					ImGui::Checkbox(crypt_str("Local bullet tracers"), &g_cfg.esp.bullet_tracer);
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("##bulltracecolor"), &g_cfg.esp.bullet_tracer_color, ALPHA);

					ImGui::Checkbox(crypt_str("Enemy bullet tracers"), &g_cfg.esp.enemy_bullet_tracer);
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("##enemybulltracecolor"), &g_cfg.esp.enemy_bullet_tracer_color, ALPHA);

					ImGui::Checkbox(crypt_str("Hitmarker"), &g_cfg.esp.hitmarker);
					ImGui::Checkbox(crypt_str("Damage marker"), &g_cfg.esp.damage_marker);
					ImGui::Checkbox(crypt_str("Kill effect"), &g_cfg.esp.kill_effect);

					if (g_cfg.esp.kill_effect)
						ImGui::SliderFloat(crypt_str("Duration"), &g_cfg.esp.kill_effect_duration, 0.01f, 3.0f);

					draw_keybind("Thirdperson", &g_cfg.misc.thirdperson_toggle, "##TPKEY__HOTKEY");

					ImGui::Checkbox(crypt_str("Thirdperson when spectating"), &g_cfg.misc.thirdperson_when_spectating);

					if (g_cfg.misc.thirdperson_toggle.key > KEY_NONE && g_cfg.misc.thirdperson_toggle.key < KEY_MAX)
						ImGui::SliderInt(crypt_str("Thirdperson distance"), &g_cfg.misc.thirdperson_distance, 100, 300);

					ImGui::SliderInt(crypt_str("Field of view"), &g_cfg.esp.fov, 0, 89);
					ImGui::Checkbox(crypt_str("Taser range"), &g_cfg.esp.taser_range);
					ImGui::Checkbox(crypt_str("Show spread"), &g_cfg.esp.show_spread);
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("##spredcolor"), &g_cfg.esp.show_spread_color, ALPHA);
					ImGui::Checkbox(crypt_str("Penetration crosshair"), &g_cfg.esp.penetration_reticle);
				}

				ImGui::EndChild();
				tab_end();
			}
			else
			{
				child_title("General");

				tab_start();
				ImGui::BeginChild("##VISUAL2_FIRST", ImVec2(317 * dpi_scale, (child_height - 35) * dpi_scale));
				{
					ImGui::Checkbox(crypt_str("Rare animations"), &g_cfg.skins.rare_animations);
					ImGui::SliderInt(crypt_str("Viewmodel field of view"), &g_cfg.esp.viewmodel_fov, 0, 89);
					ImGui::SliderInt(crypt_str("Viewmodel X"), &g_cfg.esp.viewmodel_x, -50, 50);
					ImGui::SliderInt(crypt_str("Viewmodel Y"), &g_cfg.esp.viewmodel_y, -50, 50);
					ImGui::SliderInt(crypt_str("Viewmodel Z"), &g_cfg.esp.viewmodel_z, -50, 50);
					ImGui::SliderInt(crypt_str("Viewmodel roll"), &g_cfg.esp.viewmodel_roll, -180, 180);
				}

				ImGui::EndChild();
				tab_end();
			}
		}
		else
		{
			if (!visuals_section)
			{
				child_title("World");

				tab_start();
				ImGui::BeginChild("##VISUAL1_SECOND", ImVec2(317 * dpi_scale, (child_height - 35) * dpi_scale));
				{
					ImGui::Checkbox(crypt_str("Full bright"), &g_cfg.esp.bright);

					draw_combo("Skybox", g_cfg.esp.skybox, skybox, ARRAYSIZE(skybox));

					ImGui::Text(crypt_str("Color "));
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("##skyboxcolor"), &g_cfg.esp.skybox_color, NOALPHA);

					if (g_cfg.esp.skybox == 21)
					{
						static char sky_custom[64] = "\0";

						if (!g_cfg.esp.custom_skybox.empty())
							strcpy_s(sky_custom, sizeof(sky_custom), g_cfg.esp.custom_skybox.c_str());

						ImGui::Text(crypt_str("Name"));

						if (ImGui::InputText(crypt_str("##customsky"), sky_custom, sizeof(sky_custom)))
							g_cfg.esp.custom_skybox = sky_custom;

					}
		
					ImGui::Checkbox(crypt_str("Night mode"), &g_cfg.esp.nightmode);

					if (g_cfg.esp.nightmode)
					{
						ImGui::Text(crypt_str("World color "));
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##worldcolor"), &g_cfg.esp.world_color, ALPHA);

						ImGui::Text(crypt_str("Props color "));
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##propscolor"), &g_cfg.esp.props_color, ALPHA);
					}

					ImGui::Checkbox(crypt_str("World modulation"), &g_cfg.esp.world_modulation);

					if (g_cfg.esp.world_modulation)
					{
						ImGui::SliderFloat(crypt_str("Bloom"), &g_cfg.esp.bloom, 0.0f, 750.0f);
						ImGui::SliderFloat(crypt_str("Exposure"), &g_cfg.esp.exposure, 0.0f, 2000.0f);
						ImGui::SliderFloat(crypt_str("Ambient"), &g_cfg.esp.ambient, 0.0f, 1500.0f);
					}

					ImGui::Checkbox(crypt_str("Fog modulation"), &g_cfg.esp.fog);

					if (g_cfg.esp.fog)
					{
						ImGui::SliderInt(crypt_str("Distance"), &g_cfg.esp.fog_distance, 0, 2500);
						ImGui::SliderInt(crypt_str("Density"), &g_cfg.esp.fog_density, 0, 100);

						ImGui::Text(crypt_str("Color "));
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##fogcolor"), &g_cfg.esp.fog_color, NOALPHA);
					}
				}

				ImGui::EndChild();
				tab_end();
			}
			else
			{
				child_title("Chams");

				tab_start();
				ImGui::BeginChild("##VISUAL2_SECOND", ImVec2(317 * dpi_scale, (child_height - 35) * dpi_scale));
				{
					ImGui::Checkbox(crypt_str("Arms chams"), &g_cfg.esp.arms_chams);
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("##armscolor"), &g_cfg.esp.arms_chams_color, ALPHA);


					draw_combo(crypt_str("Arms chams material"), g_cfg.esp.arms_chams_type, chamstype, ARRAYSIZE(chamstype));

					if (g_cfg.esp.arms_chams_type != 6)
					{
						ImGui::Checkbox(crypt_str("Arms double material "), &g_cfg.esp.arms_double_material);
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##armsdoublematerial"), &g_cfg.esp.arms_double_material_color, ALPHA);
					}

					ImGui::Checkbox(crypt_str("Arms animated material "), &g_cfg.esp.arms_animated_material);
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("##armsanimatedmaterial"), &g_cfg.esp.arms_animated_material_color, ALPHA);

					ImGui::Spacing();

					ImGui::Checkbox(crypt_str("Weapon chams"), &g_cfg.esp.weapon_chams);
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("##weaponchamscolors"), &g_cfg.esp.weapon_chams_color, ALPHA);

					draw_combo(crypt_str("Weapon chams material"), g_cfg.esp.weapon_chams_type, chamstype, ARRAYSIZE(chamstype));

					if (g_cfg.esp.weapon_chams_type != 6)
					{
						ImGui::Checkbox(crypt_str("Weapon double material "), &g_cfg.esp.weapon_double_material);
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##weapondoublematerial"), &g_cfg.esp.weapon_double_material_color, ALPHA);
					}

					ImGui::Checkbox(crypt_str("Weapon animated material "), &g_cfg.esp.weapon_animated_material);
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("##weaponanimatedmaterial"), &g_cfg.esp.weapon_animated_material_color, ALPHA);
				
				}

				ImGui::EndChild();
				tab_end();
			}
		}
	}
	else if (child == -1)
	{
		// hey stewen, what r u doing there? he, hm heee, DRUGS
		static bool drugs = false;

		// some animation logic(switch)
		static bool active_animation = false;
		static bool preview_reverse = false;
		static float switch_alpha = 1.f;
		static int next_id = -1;
		if (active_animation)
		{
			if (preview_reverse)
			{
				if (switch_alpha == 1.f)
				{
					preview_reverse = false;
					active_animation = false;
				}

				switch_alpha = math::clamp(switch_alpha + (4.f * ImGui::GetIO().DeltaTime), 0.01f, 1.f);
			}
			else
			{
				if (switch_alpha == 0.01f)
				{
					preview_reverse = true;
				}

				switch_alpha = math::clamp(switch_alpha - (4.f * ImGui::GetIO().DeltaTime), 0.01f, 1.f);
			}
		}
		else
			switch_alpha = math::clamp(switch_alpha + (4.f * ImGui::GetIO().DeltaTime), 0.0f, 1.f);


		// TODO: delete g_cfg.skin.enable and auto force it to TRUE
		g_cfg.skins.enable = true;

		ImGui::BeginChild("##SKINCHANGER__TAB", ImVec2(686 * dpi_scale, child_height * dpi_scale)); // first functional child
		{
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, (1.f - preview_alpha) * public_alpha * switch_alpha);
			child_title(current_profile == -1 ? "Skinchanger" : game_data::weapon_names[current_profile].name);
			tab_start();
			ImGui::BeginChild("##SKINCHANGER__CHILD", ImVec2(686 * dpi_scale, (child_height - 35)* dpi_scale));
			{
				// we need to count our items in 1 line
				int same_line_counter = 0;
				
				// if we didnt choose any weapon
				if (current_profile == -1)
				{
					for (auto i = 0; i < g_cfg.skins.skinChanger.size(); i++)
					{
						// do we need update our preview for some reasons?
						if (all_skins[i] == nullptr)
							all_skins[i] = get_skin_preview(get_wep(i, (i == 0 || i == 1) ? g_cfg.skins.skinChanger.at(i).definition_override_vector_index : -1, i == 0).c_str(), g_cfg.skins.skinChanger.at(i).skin_name, device);

						// we licked on weapon
						if (ImGui::ImageButton(all_skins[i], ImVec2(107 * dpi_scale, 76 * dpi_scale)))
						{
							next_id = i;
							active_animation = true;
						}

						// if our animation step is half from all - switch profile
						if (active_animation && preview_reverse)
						{
							ImGui::SetScrollY(0);
							current_profile = next_id;
						}

						
						if (same_line_counter < 4) { // continue push same-line
							ImGui::SameLine();
							same_line_counter++;
						}
						else { // we have maximum elements in 1 line
							same_line_counter = 0;
						}
					}
				}
				else
				{
					// update skin preview bool
					static bool need_update[36];

					// we pressed "Save & Close" button
					static bool leave;

					// update if we have nullptr texture or if we push force update
					if (all_skins[current_profile] == nullptr || need_update[current_profile])
					{
						all_skins[current_profile] = get_skin_preview(get_wep(current_profile, (current_profile == 0 || current_profile == 1) ? g_cfg.skins.skinChanger.at(current_profile).definition_override_vector_index : -1, current_profile == 0).c_str(), g_cfg.skins.skinChanger.at(current_profile).skin_name, device);
						need_update[current_profile] = false;
					}
					
					// get settings for selected weapon
					auto& selected_entry = g_cfg.skins.skinChanger[current_profile];
					selected_entry.itemIdIndex = current_profile;

					ImGui::BeginGroup();
					ImGui::PushItemWidth(260 * dpi_scale);

					// search input later
					static char search_skins[64] = "";
					static auto item_index = selected_entry.paint_kit_vector_index;

					if (!current_profile)
					{
						ImGui::Text("Knife");
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 5 * c_menu::get().dpi_scale);
						if (ImGui::Combo("##Knife_combo", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text)
						{
							*out_text = game_data::knife_names[idx].name;
							return true;
						}, nullptr, IM_ARRAYSIZE(game_data::knife_names)))
							need_update[current_profile] = true; // push force update
					}
					else if (current_profile == 1)
					{
						ImGui::Text("Gloves");
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 5 * c_menu::get().dpi_scale);
						if (ImGui::Combo("##Glove_combo", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text)
						{
							*out_text = game_data::glove_names[idx].name;
							return true;
						}, nullptr, IM_ARRAYSIZE(game_data::glove_names)))
						{
							item_index = 0; // set new generated paintkits element to 0;
							need_update[current_profile] = true; // push force update
						}
					}
					else
						selected_entry.definition_override_vector_index = 0;

					if (current_profile != 1)
					{
						ImGui::Text("Search");

						if (ImGui::InputText("##search", search_skins, sizeof(search_skins)))
							item_index = -1;
					}

					auto main_kits = current_profile == 1 ? SkinChanger::gloveKits : SkinChanger::skinKits;
					auto display_index = 0;

					SkinChanger::displayKits = main_kits;

					// we dont need custom gloves
					if (current_profile == 1)
					{
						for (auto i = 0; i < main_kits.size(); i++)
						{
							auto main_name = main_kits.at(i).name;

							for (auto i = 0; i < main_name.size(); i++)
								if (iswalpha((main_name.at(i))))
									main_name.at(i) = towlower(main_name.at(i));

							char search_name[64];

							if (!strcmp(game_data::glove_names[selected_entry.definition_override_vector_index].name, "Hydra"))
								strcpy_s(search_name, sizeof(search_name), "Bloodhound");
							else
								strcpy_s(search_name, sizeof(search_name), game_data::glove_names[selected_entry.definition_override_vector_index].name);

							for (auto i = 0; i < sizeof(search_name); i++)
								if (iswalpha(search_name[i]))
									search_name[i] = towlower(search_name[i]);

							if (main_name.find(search_name) != std::string::npos)
							{
								SkinChanger::displayKits.at(display_index) = main_kits.at(i);
								display_index++;
							}
						}

						SkinChanger::displayKits.erase(SkinChanger::displayKits.begin() + display_index, SkinChanger::displayKits.end());
					}
					else
					{
						// TODO: fix ru finding symbols ('Градиент' не будет найден по запросу 'градиент' etc)
						if (strcmp(search_skins, "")) //-V526
						{
							for (auto i = 0; i < main_kits.size(); i++)
							{
								auto main_name = main_kits.at(i).name;

								for (auto i = 0; i < main_name.size(); i++)
									if (iswalpha(main_name.at(i)))
										main_name.at(i) = towlower(main_name.at(i));

								char search_name[64];
								strcpy_s(search_name, sizeof(search_name), search_skins);

								for (auto i = 0; i < sizeof(search_name); i++)
									if (iswalpha(search_name[i]))
										search_name[i] = towlower(search_name[i]);

								if (main_name.find(search_name) != std::string::npos)
								{
									SkinChanger::displayKits.at(display_index) = main_kits.at(i);
									display_index++;
								}
							}

							SkinChanger::displayKits.erase(SkinChanger::displayKits.begin() + display_index, SkinChanger::displayKits.end());
						}
						else
							item_index = selected_entry.paint_kit_vector_index;
					}

					ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
					if (!SkinChanger::displayKits.empty())
					{
						if (ImGui::ListBox("##PAINTKITS", &item_index, [](void* data, int idx, const char** out_text)
						{
							while (SkinChanger::displayKits.at(idx).name.find("С‘") != std::string::npos) //-V807
								SkinChanger::displayKits.at(idx).name.replace(SkinChanger::displayKits.at(idx).name.find("С‘"), 2, "Рµ");

							*out_text = SkinChanger::displayKits.at(idx).name.c_str();
							return true;
						}, nullptr, SkinChanger::displayKits.size(), SkinChanger::displayKits.size() > 9 ? 9 : SkinChanger::displayKits.size()) || !all_skins[current_profile])
						{
							g_ctx.globals.force_skin_update = true;
							need_update[current_profile] = true;

							auto i = 0;

							while (i < main_kits.size())
							{
								if (main_kits.at(i).id == SkinChanger::displayKits.at(item_index).id)
								{
									selected_entry.paint_kit_vector_index = i;
									break;
								}

								i++;
							}

						}
					}
					ImGui::PopStyleVar();

					if (ImGui::InputInt("Seed", &selected_entry.seed, 1, 100))
						g_ctx.globals.force_skin_update = true;

					if (ImGui::InputInt("StatTrak", &selected_entry.stat_trak, 1, 15))
						g_ctx.globals.force_skin_update = true;

					if (ImGui::SliderFloat("Wear", &selected_entry.wear, 0.0f, 1.0f))
						drugs = true;
					else if (drugs)
					{
						g_ctx.globals.force_skin_update = true;
						drugs = false;
					}

					ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 6 * c_menu::get().dpi_scale);
					ImGui::Text("Quality");
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 5 * c_menu::get().dpi_scale);
					if (ImGui::Combo("##Quality_combo", &selected_entry.entity_quality_vector_index, [](void* data, int idx, const char** out_text)
					{
						*out_text = game_data::quality_names[idx].name;
						return true;
					}, nullptr, IM_ARRAYSIZE(game_data::quality_names)))
						g_ctx.globals.force_skin_update = true;

					if (current_profile != 1)
					{
						if (!g_cfg.skins.custom_name_tag[current_profile].empty())
							strcpy_s(selected_entry.custom_name, sizeof(selected_entry.custom_name), g_cfg.skins.custom_name_tag[current_profile].c_str());

						ImGui::Text("Name Tag");

						if (ImGui::InputText("##nametag", selected_entry.custom_name, sizeof(selected_entry.custom_name)))
						{
							g_cfg.skins.custom_name_tag[current_profile] = selected_entry.custom_name;
							g_ctx.globals.force_skin_update = true;
						}
					}
					
					ImGui::PopItemWidth();
		
					ImGui::EndGroup();

					ImGui::SameLine();
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 286 * dpi_scale - 200 * dpi_scale);

					ImGui::BeginGroup();
					if (ImGui::ImageButton(all_skins[current_profile], ImVec2(190 * dpi_scale, 155 * dpi_scale)))
					{
						// maybe i will do smth later where, who knows :/
					}

					if (ImGui::CustomButton("Save & Close", "##SAVE__CLOSE__SKING", ImVec2(198 * dpi_scale, 26 * dpi_scale)))
					{ 
						// start animation
						active_animation = true;
						next_id = -1;
						leave = true;
					}
					ImGui::EndGroup();

					// update element
					selected_entry.update();

					// we need to reset profile in the end to prevent render images with massive's index == -1
					if (leave && ((active_animation && preview_reverse) || !active_animation))
					{
						g_ctx.globals.force_skin_update = true;
						ImGui::SetScrollY(0);
						current_profile = next_id;
						leave = false;					
					}
					
				}
			}
			ImGui::EndChild();
			tab_end();

		}
		ImGui::EndChild();
	}
}



void c_menu::draw_tabs_players()
{
	const char* players_sections[3] = { "Enemy", "Team", "Local" };
	draw_semitabs(players_sections, 3, players_section, ImGui::GetStyle());
}

void c_menu::draw_players(int child)
{
	auto player = players_section;

	static std::vector <Player_list_data> players;

	if (!g_cfg.player_list.refreshing)
	{
		players.clear();

		for (auto player : g_cfg.player_list.players)
			players.emplace_back(player);
	}

	static auto current_player = 0;

	if (!child)
	{
		if (players_section < 3)
		{
			child_title("ESP");

			tab_start();
			ImGui::BeginChild("##ESP1_FIRST", ImVec2(317 * dpi_scale, (child_height - 35) * dpi_scale));
			{

				ImGui::Checkbox(crypt_str("Enable"), &g_cfg.player.enable);

				if (player == ENEMY)
				{
					ImGui::Checkbox(crypt_str("OOF arrows"), &g_cfg.player.arrows);
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("##arrowscolor"), &g_cfg.player.arrows_color, ALPHA);

					if (g_cfg.player.arrows)
					{
						ImGui::SliderInt(crypt_str("Arrows distance"), &g_cfg.player.distance, 1, 100);
						ImGui::SliderInt(crypt_str("Arrows size"), &g_cfg.player.size, 1, 100);
					}
				}

				ImGui::Checkbox(crypt_str("Bounding box"), &g_cfg.player.type[player].box);
				ImGui::SameLine();
				ImGui::ColorEdit(crypt_str("##boxcolor"), &g_cfg.player.type[player].box_color, ALPHA);

				ImGui::Checkbox(crypt_str("Name"), &g_cfg.player.type[player].name);
				ImGui::SameLine();
				ImGui::ColorEdit(crypt_str("##namecolor"), &g_cfg.player.type[player].name_color, ALPHA);

				ImGui::Checkbox(crypt_str("Health bar"), &g_cfg.player.type[player].health);
				ImGui::Checkbox(crypt_str("Health color"), &g_cfg.player.type[player].custom_health_color);
				ImGui::SameLine();
				ImGui::ColorEdit(crypt_str("##healthcolor"), &g_cfg.player.type[player].health_color, ALPHA);

				for (auto i = 0, j = 0; i < ARRAYSIZE(flags); i++)
				{
					if (g_cfg.player.type[player].flags[i])
					{
						if (j)
							preview += crypt_str(", ") + (std::string)flags[i];
						else
							preview = flags[i];

						j++;
					}
				}

				draw_multicombo("Flags", g_cfg.player.type[player].flags, flags, ARRAYSIZE(flags), preview);
				draw_multicombo("Weapon", g_cfg.player.type[player].weapon, weaponplayer, ARRAYSIZE(weaponplayer), preview);


				if (g_cfg.player.type[player].weapon[WEAPON_ICON] || g_cfg.player.type[player].weapon[WEAPON_TEXT])
				{
					ImGui::Text(crypt_str("Color "));
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("##weapcolor"), &g_cfg.player.type[player].weapon_color, ALPHA);
				}

				ImGui::Checkbox(crypt_str("Skeleton"), &g_cfg.player.type[player].skeleton);
				ImGui::SameLine();
				ImGui::ColorEdit(crypt_str("##skeletoncolor"), &g_cfg.player.type[player].skeleton_color, ALPHA);

				ImGui::Checkbox(crypt_str("Ammo bar"), &g_cfg.player.type[player].ammo);
				ImGui::SameLine();
				ImGui::ColorEdit(crypt_str("##ammocolor"), &g_cfg.player.type[player].ammobar_color, ALPHA);

				ImGui::Checkbox(crypt_str("Footsteps"), &g_cfg.player.type[player].footsteps);
				ImGui::SameLine();
				ImGui::ColorEdit(crypt_str("##footstepscolor"), &g_cfg.player.type[player].footsteps_color, ALPHA);

				if (g_cfg.player.type[player].footsteps)
				{
					ImGui::SliderInt(crypt_str("Thickness"), &g_cfg.player.type[player].thickness, 1, 10);
					ImGui::SliderInt(crypt_str("Radius"), &g_cfg.player.type[player].radius, 50, 500);
				}

				if (player == ENEMY || player == TEAM)
				{
					ImGui::Checkbox(crypt_str("Snap lines"), &g_cfg.player.type[player].snap_lines);
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("##snapcolor"), &g_cfg.player.type[player].snap_lines_color, ALPHA);

					if (player == ENEMY)
					{
						if (g_cfg.ragebot.enable)
						{
							ImGui::Checkbox(crypt_str("Aimbot points"), &g_cfg.player.show_multi_points);
							ImGui::SameLine();
							ImGui::ColorEdit(crypt_str("##showmultipointscolor"), &g_cfg.player.show_multi_points_color, ALPHA);
						}

						ImGui::Checkbox(crypt_str("Aimbot hitboxes"), &g_cfg.player.lag_hitbox);
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##lagcompcolor"), &g_cfg.player.lag_hitbox_color, ALPHA);
					}
				}
				else
				{
					draw_combo(crypt_str("Player model T"), g_cfg.player.player_model_t, player_model_t, ARRAYSIZE(player_model_t));
					padding(0, 3);
					draw_combo(crypt_str("Player model CT"), g_cfg.player.player_model_ct, player_model_ct, ARRAYSIZE(player_model_ct));
				}

			}
			ImGui::EndChild();
			tab_end();
		}
		else
		{
			child_title("Players");

			tab_start();
			ImGui::BeginChild("##ESP2_FIRST", ImVec2(317 * dpi_scale, (child_height - 35) * dpi_scale));
			{

				if (!players.empty())
				{
					std::vector <std::string> player_names;

					for (auto player : players)
						player_names.emplace_back(player.name);

					ImGui::PushItemWidth(291 * dpi_scale);
					ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
					ImGui::ListBoxConfigArray(crypt_str("##PLAYERLIST"), &current_player, player_names, 14);
					ImGui::PopStyleVar();
					ImGui::PopItemWidth();				
				}

			}
			ImGui::EndChild();
			tab_end();
		}
	}
	else
	{
		if (players_section > 2)
		{
			child_title("Settings");

			tab_start();
			ImGui::BeginChild("##ESP1_SECOND", ImVec2(317 * dpi_scale, (child_height - 35) * dpi_scale));
			{

				if (!players.empty())
				{
					if (current_player >= players.size())
						current_player = players.size() - 1; //-V103

					ImGui::Checkbox(crypt_str("White list"), &g_cfg.player_list.white_list[players.at(current_player).i]); //-V106 //-V807

					if (!g_cfg.player_list.white_list[players.at(current_player).i]) //-V106
					{
						// TODO: arty pidoras uncomment
						ImGui::Checkbox(crypt_str("High priority"), &g_cfg.player_list.high_priority[players.at(current_player).i]); //-V106
						//ImGui::Checkbox(crypt_str("Force safe points"), &g_cfg.player_list.force_safe_points[players.at(current_player).i]); //-V106
						ImGui::Checkbox(crypt_str("Force body aim"), &g_cfg.player_list.force_body_aim[players.at(current_player).i]); //-V106
						//ImGui::Checkbox(crypt_str("Low delta"), &g_cfg.player_list.low_delta[players.at(current_player).i]); //-V106
					}
				}

			}
			ImGui::EndChild();
			tab_end();
		}
		else
		{
			child_title("Model");

			tab_start();
			ImGui::BeginChild("##ESP2_SECOND", ImVec2(317 * dpi_scale, (child_height - 35) * dpi_scale));
			{
				ImGui::Spacing();
				if (player == LOCAL)
					draw_combo(crypt_str("Type"), g_cfg.player.local_chams_type, local_chams_type, ARRAYSIZE(local_chams_type));

				if (player != LOCAL || !g_cfg.player.local_chams_type)
					draw_multicombo("Chams", g_cfg.player.type[player].chams, g_cfg.player.type[player].chams[PLAYER_CHAMS_VISIBLE] ? chamsvisact : chamsvis, g_cfg.player.type[player].chams[PLAYER_CHAMS_VISIBLE] ? ARRAYSIZE(chamsvisact) : ARRAYSIZE(chamsvis), preview);

				if (g_cfg.player.type[player].chams[PLAYER_CHAMS_VISIBLE] || player == LOCAL && g_cfg.player.local_chams_type)
				{
					if (player == LOCAL && g_cfg.player.local_chams_type)
					{
						ImGui::Checkbox(crypt_str("Enable desync chams"), &g_cfg.player.fake_chams_enable);
						ImGui::Checkbox(crypt_str("Visualize lag"), &g_cfg.player.visualize_lag);
						ImGui::Checkbox(crypt_str("Layered"), &g_cfg.player.layered);

						draw_combo(crypt_str("Player chams material"), g_cfg.player.fake_chams_type, chamstype, ARRAYSIZE(chamstype));

						ImGui::Text(crypt_str("Color "));
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##fakechamscolor"), &g_cfg.player.fake_chams_color, ALPHA);

						if (g_cfg.player.fake_chams_type != 6)
						{
							ImGui::Checkbox(crypt_str("Double material "), &g_cfg.player.fake_double_material);
							ImGui::SameLine();
							ImGui::ColorEdit(crypt_str("##doublematerialcolor"), &g_cfg.player.fake_double_material_color, ALPHA);
						}

						ImGui::Checkbox(crypt_str("Animated material"), &g_cfg.player.fake_animated_material);
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##animcolormat"), &g_cfg.player.fake_animated_material_color, ALPHA);
					}
					else
					{
						draw_combo(crypt_str("Player chams material"), g_cfg.player.type[player].chams_type, chamstype, ARRAYSIZE(chamstype));

						if (g_cfg.player.type[player].chams[PLAYER_CHAMS_VISIBLE])
						{
							ImGui::Text(crypt_str("Visible "));
							ImGui::SameLine();
							ImGui::ColorEdit(crypt_str("##chamsvisible"), &g_cfg.player.type[player].chams_color, ALPHA);
						}

						if (g_cfg.player.type[player].chams[PLAYER_CHAMS_VISIBLE] && g_cfg.player.type[player].chams[PLAYER_CHAMS_INVISIBLE])
						{
							ImGui::Text(crypt_str("Invisible "));
							ImGui::SameLine();
							ImGui::ColorEdit(crypt_str("##chamsinvisible"), &g_cfg.player.type[player].xqz_color, ALPHA);
						}

						if (g_cfg.player.type[player].chams_type != 6)
						{
							ImGui::Checkbox(crypt_str("Double material "), &g_cfg.player.type[player].double_material);
							ImGui::SameLine();
							ImGui::ColorEdit(crypt_str("##doublematerialcolor"), &g_cfg.player.type[player].double_material_color, ALPHA);
						}

						ImGui::Checkbox(crypt_str("Animated material"), &g_cfg.player.type[player].animated_material);
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##animcolormat"), &g_cfg.player.type[player].animated_material_color, ALPHA);

						if (player == ENEMY)
						{
							ImGui::Checkbox(crypt_str("Backtrack chams"), &g_cfg.player.backtrack_chams);

							if (g_cfg.player.backtrack_chams)
							{
								draw_combo(crypt_str("Backtrack chams material"), g_cfg.player.backtrack_chams_material, chamstype, ARRAYSIZE(chamstype));

								ImGui::Text(crypt_str("Color "));
								ImGui::SameLine();
								ImGui::ColorEdit(crypt_str("##backtrackcolor"), &g_cfg.player.backtrack_chams_color, ALPHA);
							}
						}
					}
				}

				if (player == ENEMY || player == TEAM)
				{
					ImGui::Checkbox(crypt_str("Ragdoll chams"), &g_cfg.player.type[player].ragdoll_chams);

					if (g_cfg.player.type[player].ragdoll_chams)
					{
						draw_combo(crypt_str("Ragdoll chams material"), g_cfg.player.type[player].ragdoll_chams_material, chamstype, ARRAYSIZE(chamstype));

						ImGui::Text(crypt_str("Color "));
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("ragdollcolor"), &g_cfg.player.type[player].ragdoll_chams_color, ALPHA);
					}
				}
				else if (!g_cfg.player.local_chams_type)
				{
					ImGui::Checkbox(crypt_str("Transparency in scope"), &g_cfg.player.transparency_in_scope);

					if (g_cfg.player.transparency_in_scope)
						ImGui::SliderFloat(crypt_str("Alpha"), &g_cfg.player.transparency_in_scope_amount, 0.0f, 1.0f);
				}

				ImGui::Spacing();

				ImGui::Checkbox(crypt_str("Glow"), &g_cfg.player.type[player].glow);

				if (g_cfg.player.type[player].glow)
				{
					draw_combo(crypt_str("Glow type"), g_cfg.player.type[player].glow_type, glowtype, ARRAYSIZE(glowtype));
					ImGui::Text(crypt_str("Color "));
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("glowcolor"), &g_cfg.player.type[player].glow_color, ALPHA);
				}

			}
			ImGui::EndChild();
			tab_end();
		}
	}
}


void c_menu::draw_tabs_misc()
{
	const char* misc_sections[2] = { "Main", "Additives" };
	draw_semitabs(misc_sections, 2, misc_section, ImGui::GetStyle());
}

void c_menu::draw_misc(int child)
{
	if (!child)
	{
		if (!misc_section)
		{
			child_title("General");

			tab_start();
			ImGui::BeginChild("##MISC1_FIRST", ImVec2(317 * dpi_scale, (child_height - 35) * dpi_scale));
			{
				ImGui::Checkbox(crypt_str("Anti-untrusted"), &g_cfg.misc.anti_untrusted);
				ImGui::Checkbox(crypt_str("Rank reveal"), &g_cfg.misc.rank_reveal);
				ImGui::Checkbox(crypt_str("Unlock inventory access"), &g_cfg.misc.inventory_access);
				ImGui::Checkbox(crypt_str("Gravity ragdolls"), &g_cfg.misc.ragdolls);
				ImGui::Checkbox(crypt_str("Preserve killfeed"), &g_cfg.esp.preserve_killfeed);
				ImGui::Checkbox(crypt_str("Aspect ratio"), &g_cfg.misc.aspect_ratio);

				if (g_cfg.misc.aspect_ratio)
				{
					padding(0, -5);
					ImGui::SliderFloat(crypt_str("Amount"), &g_cfg.misc.aspect_ratio_amount, 1.0f, 2.0f);
				}

				ImGui::Checkbox(crypt_str("Fake lag"), &g_cfg.antiaim.fakelag);
				if (g_cfg.antiaim.fakelag)
				{
					draw_combo(crypt_str("Fake lag type"), g_cfg.antiaim.fakelag_type, fakelags, ARRAYSIZE(fakelags));
					ImGui::SliderInt(crypt_str("Limit"), &g_cfg.antiaim.fakelag_amount, 1, 16);

					draw_multicombo("Fake lag triggers", g_cfg.antiaim.fakelag_enablers, lagstrigger, ARRAYSIZE(lagstrigger), preview);

					auto enabled_fakelag_triggers = false;

					for (auto i = 0; i < ARRAYSIZE(lagstrigger); i++)
						if (g_cfg.antiaim.fakelag_enablers[i])
							enabled_fakelag_triggers = true;

					if (enabled_fakelag_triggers)
						ImGui::SliderInt(crypt_str("Triggers limit"), &g_cfg.antiaim.triggers_fakelag_amount, 1, 16);
				}

				draw_keybind("Teleport Exploit", &g_cfg.misc.teleport_exploit, "##TP_HOTKEY");

			}
			ImGui::EndChild();
			tab_end();
		}
		else
		{
			child_title("Information");

			tab_start();
			ImGui::BeginChild("##MISC1_SECOND", ImVec2(317 * dpi_scale, (child_height - 35) * dpi_scale));
			{

				ImGui::Checkbox(crypt_str("Watermark"), &g_cfg.menu.watermark);
				ImGui::Checkbox(crypt_str("Spectators list"), &g_cfg.misc.spectators_list);
				draw_combo(crypt_str("Hitsound"), g_cfg.esp.hitsound, sounds, ARRAYSIZE(sounds));
				ImGui::Checkbox(crypt_str("Killsound"), &g_cfg.esp.killsound);
				draw_multicombo(crypt_str("Logs"), g_cfg.misc.events_to_log, events, ARRAYSIZE(events), preview);
				padding(0, 3);
				draw_multicombo(crypt_str("Logs output"), g_cfg.misc.log_output, events_output, ARRAYSIZE(events_output), preview);			

				if (g_cfg.misc.events_to_log[EVENTLOG_HIT] || g_cfg.misc.events_to_log[EVENTLOG_ITEM_PURCHASES] || g_cfg.misc.events_to_log[EVENTLOG_BOMB])
				{
					ImGui::Text(crypt_str("Color "));
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("logcolor"), &g_cfg.misc.log_color, ALPHA);
				}

				ImGui::Checkbox(crypt_str("Show CS:GO logs"), &g_cfg.misc.show_default_log);

			}
			ImGui::EndChild();
			tab_end();
		}
		
	}
	else
	{
		if (!misc_section)
		{
			child_title("Movement");

			tab_start();
			ImGui::BeginChild("##MISC2_FIRST", ImVec2(317 * dpi_scale, (child_height - 35) * dpi_scale));
			{

				ImGui::Checkbox(crypt_str("Automatic jump"), &g_cfg.misc.bunnyhop);
				draw_combo(crypt_str("Automatic strafes"), g_cfg.misc.airstrafe, strafes, ARRAYSIZE(strafes));
				ImGui::Checkbox(crypt_str("Crouch in air"), &g_cfg.misc.crouch_in_air);
				ImGui::Checkbox(crypt_str("Fast stop"), &g_cfg.misc.fast_stop);
				ImGui::Checkbox(crypt_str("Slide walk"), &g_cfg.misc.slidewalk);
				ImGui::Checkbox(crypt_str("No duck cooldown"), &g_cfg.misc.noduck);

				if (g_cfg.misc.noduck)
					draw_keybind(crypt_str("Fakeduck"), &g_cfg.misc.fakeduck_key, crypt_str("##FAKEDUCK__HOTKEY"));

				draw_keybind(crypt_str("Slowwalk"), &g_cfg.misc.slowwalk_key, crypt_str("##SLOWWALK__HOTKEY"));

				draw_keybind(crypt_str("Auto peek"), &g_cfg.misc.automatic_peek, crypt_str("##AUTOPEEK__HOTKEY"));

				draw_keybind(crypt_str("Edge jump"), &g_cfg.misc.edge_jump, crypt_str("##EDGEJUMP__HOTKEY"));

			}
			ImGui::EndChild();
			tab_end();
		}
		else
		{
			child_title("Extra");

			tab_start();
			ImGui::BeginChild("##MISC2_SECOND", ImVec2(317 * dpi_scale, (child_height - 35)* dpi_scale));
			{

				ImGui::Checkbox(crypt_str("Anti-screenshot"), &g_cfg.misc.anti_screenshot);
				ImGui::Checkbox(crypt_str("Clantag"), &g_cfg.misc.clantag_spammer);
				ImGui::Checkbox(crypt_str("Chat spam"), &g_cfg.misc.chat);
				ImGui::Checkbox(crypt_str("Enable buybot"), &g_cfg.misc.buybot_enable);

				draw_combo(crypt_str("Snipers"), g_cfg.misc.buybot1, mainwep, ARRAYSIZE(mainwep));
				padding(0, 3);
				draw_combo(crypt_str("Pistols"), g_cfg.misc.buybot2, secwep, ARRAYSIZE(secwep));
				padding(0, 3);
				draw_multicombo("Other", g_cfg.misc.buybot3, grenades, ARRAYSIZE(grenades), preview);
			
#if !RELEASE

				ImGui::Text("DPI Scale");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 5 * dpi_scale);
				if (ImGui::DragFloat("##DPIScale", &dpi_scale, 0.005f, 1.f, 1.8f, "%.2f"))
					update_dpi = true;
#endif
			}
			ImGui::EndChild();
			tab_end();
		}
	}
}



void c_menu::draw_settings(int child)
{

	child_title("Configs");

	tab_start();
	ImGui::BeginChild("##CONFIGS__FIRST", ImVec2(686 * dpi_scale, (child_height - 35) * dpi_scale));
	{
		ImGui::BeginChild("##CONFIGS__FIRST_POSHELNAHUI", ImVec2(686 * dpi_scale, (child_height - 35) * dpi_scale));
		{
			ImGui::PushItemWidth(629 * c_menu::get().dpi_scale);

			static auto should_update = true;

			if (should_update)
			{
				should_update = false;

				cfg_manager->config_files();
				files = cfg_manager->files;

				for (auto& current : files)
					if (current.size() > 2)
						current.erase(current.size() - 3, 3);
			}

			if (ImGui::CustomButton("Open configs folder", "##CONFIG__FOLDER", ImVec2(629 * dpi_scale, 26 * dpi_scale)))
			{
				std::string folder;

				auto get_dir = [&folder]() -> void
				{
					static TCHAR path[MAX_PATH];

					if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, NULL, path)))
						folder = std::string(path) + crypt_str("\\Legendware\\Configs\\");

					CreateDirectory(folder.c_str(), NULL);
				};

				get_dir();
				ShellExecute(NULL, crypt_str("open"), folder.c_str(), NULL, NULL, SW_SHOWNORMAL);
			}


			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
			ImGui::ListBoxConfigArray(crypt_str("##CONFIGS"), &g_cfg.selected_config, files, 7);
			ImGui::PopStyleVar();


			if (ImGui::CustomButton("Refresh configs", "##CONFIG__REFRESH", ImVec2(629 * dpi_scale, 26 * dpi_scale)))
			{
				cfg_manager->config_files();
				files = cfg_manager->files;

				for (auto& current : files)
					if (current.size() > 2)
						current.erase(current.size() - 3, 3);
			}

			static char config_name[64] = "\0";

			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
			ImGui::InputText(crypt_str("##configname"), config_name, sizeof(config_name));
			ImGui::PopStyleVar();

			if (ImGui::CustomButton("Create config", "##CONFIG__CREATE", ImVec2(629 * dpi_scale, 26 * dpi_scale)))
			{
				g_cfg.new_config_name = config_name;
				add_config();
			}

			/* SAVE BUTTON */

			static bool next_save = false;
			static bool prenext_save = false;
			static bool clicked_sure = false;
			static auto save_time = m_globals()->m_realtime;
			static float save_alpha = 1.f;
			save_alpha = math::clamp(save_alpha + (4.f * ImGui::GetIO().DeltaTime * (!prenext_save ? 1.f : -1.f)), 0.01f, 1.f);

			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, save_alpha * public_alpha * (1.f - preview_alpha));
			if (!next_save)
			{
				clicked_sure = false;

				if (prenext_save && save_alpha <= 0.01f)
					next_save = true;

				if (ImGui::CustomButton("Save config", "##CONFIG__SAVE", ImVec2(629 * dpi_scale, 26 * dpi_scale)))
				{
					save_time = m_globals()->m_realtime;
					prenext_save = true;
				}
			}
			else
			{
				if (prenext_save && save_alpha <= 0.01f)
				{
					prenext_save = false;
					next_save = !clicked_sure;
				}

				if (ImGui::CustomButton("Are you sure?", "##AREYOUSURE__SAVE", ImVec2(629 * dpi_scale, 26 * dpi_scale)))
				{
					save_config();
					prenext_save = true;
					clicked_sure = true;
				}

				if (!clicked_sure && m_globals()->m_realtime > save_time + 3.f)
				{
					prenext_save = true;
					clicked_sure = true;
				}
			}
			ImGui::PopStyleVar();

			/*             */

			if (ImGui::CustomButton("Load config", "##CONFIG__LOAD", ImVec2(629 * dpi_scale, 26 * dpi_scale)))
				load_config();

			/* DELETE BUTTON */

			static bool next_delete = false;
			static bool prenext_delete = false;
			static bool clicked_sure_del = false;
			static auto delete_time = m_globals()->m_realtime;
			static float delete_alpha = 1.f;
			delete_alpha = math::clamp(delete_alpha + (4.f * ImGui::GetIO().DeltaTime * (!prenext_delete ? 1.f : -1.f)), 0.01f, 1.f);

			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, delete_alpha * public_alpha * (1.f - preview_alpha));
			if (!next_delete)
			{
				clicked_sure_del = false;

				if (prenext_delete && delete_alpha <= 0.01f)
					next_delete = true;

				if (ImGui::CustomButton("Remove config", "##CONFIG__delete", ImVec2(629 * dpi_scale, 26 * dpi_scale)))
				{
					delete_time = m_globals()->m_realtime;
					prenext_delete = true;
				}
			}
			else
			{
				if (prenext_delete && delete_alpha <= 0.01f)
				{
					prenext_delete = false;
					next_delete = !clicked_sure_del;
				}

				if (ImGui::CustomButton("Are you sure?", "##AREYOUSURE__delete", ImVec2(629 * dpi_scale, 26 * dpi_scale)))
				{
					remove_config();
					prenext_delete = true;
					clicked_sure_del = true;
				}

				if (!clicked_sure_del && m_globals()->m_realtime > delete_time + 3.f)
				{
					prenext_delete = true;
					clicked_sure_del = true;
				}
			}
			ImGui::PopStyleVar();

			/*             */

			ImGui::PopItemWidth();			
		}
		ImGui::EndChild();
		ImGui::SetWindowFontScale(c_menu::get().dpi_scale);

	}
	ImGui::EndChild();
	ImGui::PopStyleVar();

}

void c_menu::draw_lua(int child)
{
	if (!child)
	{
		child_title("Scripts");

		tab_start();
		ImGui::BeginChild("##LUA_FIRST", ImVec2(317 * dpi_scale, (child_height - 35) * dpi_scale));
		{

			ImGui::PushItemWidth(291 * c_menu::get().dpi_scale);

			static auto should_update = true;

			if (should_update)
			{
				should_update = false;
				scripts = c_lua::get().scripts;

				for (auto& current : scripts)
				{
					if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
						current.erase(current.size() - 5, 5);
					else if (current.size() >= 4)
						current.erase(current.size() - 4, 4);
				}
			}

			if (ImGui::CustomButton("Open scripts folder", "##LUAS__FOLDER", ImVec2(291 * dpi_scale, 26 * dpi_scale)))
			{
				std::string folder;

				auto get_dir = [&folder]() -> void
				{
					static TCHAR path[MAX_PATH];

					if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, NULL, path)))
						folder = std::string(path) + crypt_str("\\Legendware\\Scripts\\");

					CreateDirectory(folder.c_str(), NULL);
				};

				get_dir();
				ShellExecute(NULL, crypt_str("open"), folder.c_str(), NULL, NULL, SW_SHOWNORMAL);
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);

			if (scripts.empty())
				ImGui::ListBoxConfigArray(crypt_str("##LUAS"), &selected_script, scripts, 10);
			else
			{
				auto backup_scripts = scripts;

				for (auto& script : scripts)
				{
					auto script_id = c_lua::get().get_script_id(script + crypt_str(".lua"));

					if (script_id == -1)
						continue;

					if (c_lua::get().loaded.at(script_id))
						scripts.at(script_id) += crypt_str(" [loaded]");
				}

				ImGui::ListBoxConfigArray(crypt_str("##LUAS"), &selected_script, scripts, 10);

				scripts = std::move(backup_scripts);
			}

			ImGui::PopStyleVar();


			if (ImGui::CustomButton("Refresh scripts", "##LUA__REFRESH", ImVec2(291 * dpi_scale, 26 * dpi_scale)))
			{
				c_lua::get().refresh_scripts();
				scripts = c_lua::get().scripts;

				if (selected_script >= scripts.size())
					selected_script = scripts.size() - 1; //-V103

				for (auto& current : scripts)
				{
					if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
						current.erase(current.size() - 5, 5);
					else if (current.size() >= 4)
						current.erase(current.size() - 4, 4);
				}
			}	

			// TOOD: заставить кнопку работать
			if (ImGui::CustomButton("Edit script", "##LUA__EDIT", ImVec2(291 * dpi_scale, 26 * dpi_scale)))
			{

			}

			ImGui::PopItemWidth();

		}
		ImGui::EndChild();
		tab_end();
	}
	else
	{
		child_title("Script elements");

		tab_start();
		ImGui::BeginChild("##LUA_SECOND", ImVec2(317 * dpi_scale, (child_height - 35) * dpi_scale));
		{

			

		}
		ImGui::EndChild();
		tab_end();
	}
}


void c_menu::draw_radar(int child) 
{
	if (!child)
	{
		child_title("Hud radar");

		tab_start();
		ImGui::BeginChild("##RADAR_FIRST", ImVec2(317 * dpi_scale, (child_height - 35) * dpi_scale));
		{
			ImGui::Checkbox(crypt_str("Engine radar"), &g_cfg.misc.engine_radar);
			ImGui::Checkbox(crypt_str("Disable rendering"), &g_cfg.misc.disable_radar_render); // TODO: cl_drawhud 0
		}
		ImGui::EndChild();
		tab_end();
	}
	else
	{
		child_title("Custom radar");

		tab_start();
		ImGui::BeginChild("##RADAR_SECOND", ImVec2(317 * dpi_scale, (child_height - 35) * dpi_scale));
		{
			ImGui::Checkbox(crypt_str("Draw custom radar"), &g_cfg.misc.ingame_radar);
			ImGui::Checkbox(crypt_str("Render local"), &g_cfg.radar.render_local);
			ImGui::Checkbox(crypt_str("Render enemy"), &g_cfg.radar.render_enemy);
			ImGui::Checkbox(crypt_str("Render team"), &g_cfg.radar.render_team);
			ImGui::Checkbox(crypt_str("Show planted c4"), &g_cfg.radar.render_planted_c4);
			ImGui::Checkbox(crypt_str("Show dropped c4"), &g_cfg.radar.render_dropped_c4);
			ImGui::Checkbox(crypt_str("Show health"), &g_cfg.radar.render_health);
		}
		ImGui::EndChild();
		tab_end();
	}
}


void c_menu::draw_profile(int child)
{
	auto player = players_section;

	static std::vector <Player_list_data> players;

	if (!g_cfg.player_list.refreshing)
	{
		players.clear();

		for (auto player : g_cfg.player_list.players)
			players.emplace_back(player);
	}

	static auto current_player = 0;

	if (!child)
	{
		child_title("Players");

		tab_start();
		ImGui::BeginChild("##ESP2_FIRST", ImVec2(317 * dpi_scale, (child_height - 35) * dpi_scale));
		{

			if (!players.empty())
			{
				std::vector <std::string> player_names;

				for (auto player : players)
					player_names.emplace_back(player.name);

				ImGui::PushItemWidth(291 * dpi_scale);
				ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
				ImGui::ListBoxConfigArray(crypt_str("##PLAYERLIST"), &current_player, player_names, 14);
				ImGui::PopStyleVar();
				ImGui::PopItemWidth();
			}

		}
		ImGui::EndChild();
		tab_end();
	}
	else
	{
		child_title("Settings");

		tab_start();
		ImGui::BeginChild("##ESP1_SECOND", ImVec2(317 * dpi_scale, (child_height - 35) * dpi_scale));
		{

			if (!players.empty())
			{
				if (current_player >= players.size())
					current_player = players.size() - 1; //-V103

				ImGui::Checkbox(crypt_str("White list"), &g_cfg.player_list.white_list[players.at(current_player).i]); //-V106 //-V807

				if (!g_cfg.player_list.white_list[players.at(current_player).i]) //-V106
				{
					// TODO: arty pidoras uncomment
					ImGui::Checkbox(crypt_str("High priority"), &g_cfg.player_list.high_priority[players.at(current_player).i]); //-V106
					//ImGui::Checkbox(crypt_str("Force safe points"), &g_cfg.player_list.force_safe_points[players.at(current_player).i]); //-V106
					ImGui::Checkbox(crypt_str("Force body aim"), &g_cfg.player_list.force_body_aim[players.at(current_player).i]); //-V106
					//ImGui::Checkbox(crypt_str("Low delta"), &g_cfg.player_list.low_delta[players.at(current_player).i]); //-V106
				}
			}

		}
		ImGui::EndChild();
		tab_end();
	}
}

void c_menu::draw(bool is_open) {

	// animation related code
	static float m_alpha = 0.0002f;
	m_alpha = math::clamp(m_alpha + (3.f * ImGui::GetIO().DeltaTime * (is_open ? 1.f : -1.f)), 0.0001f, 1.f);

	// set alpha in class to use later in widgets
	public_alpha = m_alpha;

	if (m_alpha == 0.0001f)
		return;

	// set new alpha
	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, m_alpha);

	// setup colors and some styles
	if (!menu_setupped)
		menu_setup(ImGui::GetStyle());

	ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, ImVec4(ImGui::GetStyle().Colors[ImGuiCol_ScrollbarGrab].x, ImGui::GetStyle().Colors[ImGuiCol_ScrollbarGrab].y, ImGui::GetStyle().Colors[ImGuiCol_ScrollbarGrab].z, m_alpha));

	// default menu size
	const int x = 850, y = 560;

	// last active tab to switch effect & reverse alpha & preview alpha
	// IMPORTANT: DO TAB SWITCHING BY LAST_TAB!!!!!
	static int last_tab = active_tab;
	static bool preview_reverse = false;

	// preview for multicombos
	std::string preview = "None";

	// lua editor (test)
	// TODO: get name of script(not a path!!!!!)
	lua_edit("kon_utils");

	// start menu render
	ImGui::Begin("lw3", nullptr, ImGuiWindowFlags_NoDecoration);
	{
		auto p = ImGui::GetWindowPos();

		// main dpi update logic
		// KEYWORD : DPI_FIND
		if (update_dpi)
		{
			// prevent oversizing
			dpi_scale = min(1.8f, max(dpi_scale, 1.f));

			// font and window size setting
			ImGui::SetWindowFontScale(dpi_scale);
			ImGui::SetWindowSize(ImVec2(ImFloor(x * dpi_scale), ImFloor(y * dpi_scale)));

			// styles resizing
			dpi_resize(dpi_scale, ImGui::GetStyle());

			// setup new window sizes
			width = ImFloor(x * dpi_scale);
			height = ImFloor(y * dpi_scale);

			// end of dpi updating
			update_dpi = false;
		}

		// set padding to draw { tabs , main functional area } / close group to draw last bottom child at x-0 pos
		ImGui::SetCursorPos(ImVec2(10 * dpi_scale, 13 * dpi_scale));
		ImGui::BeginGroup();

		ImGui::BeginChild("##tabs_area", ImVec2(124 * dpi_scale, 509 * dpi_scale)); // there will be tabs
		{																			 // KEYWORD : TABS_FIND
			ImGui::PushFont(futura);
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.f * dpi_scale, 12.f * dpi_scale));

			// some extra pdding
			ImGui::Spacing();

			//  we need some more checks to prevent over-switching tabs
			if (ImGui::MainTab("Ragebot", ico_menu, "R", active_tab == 0, ImVec2(124 * dpi_scale, 0)) && last_tab == active_tab && !preview_reverse) active_tab = 0;
			if (ImGui::MainTab("Legit", ico_menu, "L", active_tab == 1, ImVec2(124 * dpi_scale, 0)) && last_tab == active_tab && !preview_reverse) active_tab = 1;
			if (ImGui::MainTab("Visuals", ico_menu, "V", active_tab == 2, ImVec2(124 * dpi_scale, 0)) && last_tab == active_tab && !preview_reverse) active_tab = 2;
			if (ImGui::MainTab("Players", ico_menu, "P", active_tab == 3, ImVec2(124 * dpi_scale, 0)) && last_tab == active_tab && !preview_reverse) active_tab = 3;
			if (ImGui::MainTab("Misc", ico_menu, "M", active_tab == 4, ImVec2(124 * dpi_scale, 0)) && last_tab == active_tab && !preview_reverse) active_tab = 4;
			if (ImGui::MainTab("Settings", ico_menu, "S", active_tab == 5, ImVec2(124 * dpi_scale, 0)) && last_tab == active_tab && !preview_reverse) active_tab = 5;

			// seperate our main-func tabs with additional tabs
			ImGui::Spacing();
			ImGui::Spacing();

			if (ImGui::MainTab("Player list", ico_bottom, "P", active_tab == 8, ImVec2(124 * dpi_scale, 0)) && last_tab == active_tab && !preview_reverse) active_tab = 8;
			if (ImGui::MainTab("Radar", ico_bottom, "R", active_tab == 7, ImVec2(124 * dpi_scale, 0)) && last_tab == active_tab && !preview_reverse) active_tab = 7;
			if (ImGui::MainTab("Lua", ico_bottom, "L", active_tab == 6, ImVec2(124 * dpi_scale, 0)) && last_tab == active_tab && !preview_reverse) active_tab = 6;		

			ImGui::PopStyleVar();
			ImGui::PopFont();
		}
		ImGui::EndChild();

		ImGui::SameLine();

		// vertical separator after tabs
		ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetCursorPos() + p + ImVec2(3 * dpi_scale, 0), ImGui::GetCursorPos() + p + ImVec2(4 * dpi_scale, 479 * dpi_scale), ImGui::ColorConvertFloat4ToU32(ImVec4(66 / 255.f, 68 / 255.f, 125 / 255.f, m_alpha * 0.3f)));


		/*     \\        let's set alpha of rect when we will switch tab        //      */
		//  ̶c̶h̶e̶c̶k̶ ̶E̶n̶d̶C̶h̶i̶l̶d̶()̶ ̶f̶o̶r̶ ̶m̶o̶r̶e̶ ̶i̶n̶f̶o̶r̶m̶a̶t̶i̶o̶n

		if (last_tab != active_tab || (last_tab == active_tab && preview_reverse))
		{
			if (!preview_reverse)
			{
				if (preview_alpha == 1.f)
					preview_reverse = true;

				preview_alpha = math::clamp(preview_alpha + (4.f * ImGui::GetIO().DeltaTime), 0.01f, 1.f);
			}
			else
			{
				last_tab = active_tab;
				if (preview_alpha == 0.01f)
				{
					preview_reverse = false;
				}

				preview_alpha = math::clamp(preview_alpha - (4.f * ImGui::GetIO().DeltaTime), 0.01f, 1.f);
			}
		}
		else
			preview_alpha = math::clamp(preview_alpha - (4.f * ImGui::GetIO().DeltaTime), 0.0f, 1.f);


		/*     \\--------------------------------------------------------------//      */


		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (12 * dpi_scale));

		ImGui::BeginChild("##content_area", ImVec2(686 * dpi_scale, 479 * dpi_scale)); // there wiil be main cheat functional
		{																			   // KEYWORD : FUNC_FIND
			ImGui::PushFont(futura);

			if (last_tab > 4)
				child_height = 471;
			else
				child_height = 435;

			// push alpha-channel adjustment
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, (1.f - preview_alpha) * m_alpha);

			// semitabs rendering
			switch (last_tab % 9)
			{
				case 0: draw_tabs_ragebot(); break;
				case 1: draw_tabs_legit(); break;
				case 2: draw_tabs_visuals(); break;
				case 3: draw_tabs_players(); break;
				case 4: draw_tabs_misc(); break;
				default: break;
			}			 

			ImGui::PopStyleVar();

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (6 * dpi_scale) + ((dpi_scale - 1.f) * 8.f));

			ImGui::BeginGroup();  // groupping two childs to prevent multipadding
			{
				ImGui::PushFont(futura_small);
				if (last_tab == 2 && visuals_section == 2)
				{
					draw_visuals(-1);
				}
				else if (last_tab == 5)
				{
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, (1.f - preview_alpha)* m_alpha);
					draw_settings(0);
				}
				else
				{
					ImGui::BeginChild("##content_first", ImVec2(339 * dpi_scale, child_height * dpi_scale)); // first functional child
					{
						ImGui::PushStyleVar(ImGuiStyleVar_Alpha, (1.f - preview_alpha) * m_alpha);

						switch (last_tab % 9)
						{
						case 0: draw_ragebot(0); break;
						case 1: draw_legit(0); break;
						case 2: draw_visuals(0); break;
						case 3: draw_players(0); break;
						case 4: draw_misc(0); break;
						case 6: draw_lua(0); break;
						case 7: draw_radar(0); break;
						case 8: draw_profile(0); break;
						default: break;
						}

					}
					ImGui::EndChild();

					ImGui::SameLine();

					ImGui::BeginChild("##content_second", ImVec2(339 * dpi_scale, child_height * dpi_scale)); // second functional child
					{
						ImGui::PushStyleVar(ImGuiStyleVar_Alpha, (1.f - preview_alpha) * m_alpha);

						switch (last_tab % 9)
						{
						case 0: draw_ragebot(1); break;
						case 1: draw_legit(1); break;
						case 2: draw_visuals(1); break;
						case 3: draw_players(1); break;
						case 4: draw_misc(1); break;
						case 6: draw_lua(1); break;
						case 7: draw_radar(1); break;
						case 8: draw_profile(1); break;
						default: break;
						}

					}
					ImGui::EndChild();
				}

				ImGui::PopFont();

			};
			ImGui::EndGroup();

			ImGui::PopFont();
		}
		ImGui::EndChild();

		ImGui::EndGroup();

		/* ------ BOTTOM ------ */

		// Fill bottom
		ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetCursorPos() + p, ImGui::GetCursorPos() + p + ImVec2(850 * dpi_scale, ((dpi_scale >= 1.04f && dpi_scale <= 1.07f) ? 2 : 0) + (29 * dpi_scale + (9.45f * (dpi_scale - 1.f)))), ImGui::ColorConvertFloat4ToU32(ImVec4(66 / 255.f, 68 / 255.f, 125 / 255.f, m_alpha)), 7.f * dpi_scale, ImDrawCornerFlags_Bot);

		ImGui::PushFont(gotham);

		ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(850 * dpi_scale - ImGui::CalcTextSize("legendawre.pw v3").x - (11 * dpi_scale), (16 * dpi_scale) - (ImGui::CalcTextSize("legendawre.pw v3").y / 2)));

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 1.f));
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, m_alpha);
		ImGui::Text("legendware.pw v3");
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();

		ImGui::PopFont();

	}
	ImGui::End();

	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
}