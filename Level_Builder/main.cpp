#include <iostream>

#include "Render/RenderWindow.h"
#include <filesystem>
#include <functional>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <imgui/ImGuiFileDialog.h>

enum class Object_types
{
	BACKGROUND,
	TILES,
	BUMPERS,
	RAILS,
	TELEPORTERS,
	TURRETS
};

std::unordered_map<std::string, Object_types> transcriber
{
	{"BACKGROUND",	Object_types::BACKGROUND},
	{"TILES",		Object_types::TILES},
	{"BUMPERS",		Object_types::BUMPERS},
	{"RAILS",		Object_types::RAILS},
	{"TELEPORTERS",	Object_types::TELEPORTERS},
	{"TURRETS",		Object_types::TURRETS}
};

enum Type
{
	EMPTY,
	TILE,
	BUMPER,
	RAIL,
	TELEPORT,
	TURRET,
	DELETE
};

struct Tile
{
	Type		type		{};
	uint32		variation	{};
	uint32		stat		{};
	glm::vec2	pos			{};
	glm::ivec2	coord		{};
} temp_tile;

void framebuffer_size_callback(GLFWwindow*, int32, int32);
void window_size_callback(GLFWwindow*, int32, int32);
void key_callback(GLFWwindow*, int32, int32, int32, int32);
void process_input(GLFWwindow*);
void new_file();
void open_file();
void save_file();
void close_window(GLFWwindow* window);
void load_level(const char*);
void save_level(const char*);
void get_tile_params(const std::string&, std::vector<uint8>&);
void draw_tile(const Tile&);
void show_tile_props();
void show_rail_props();
void show_turret_props();

int32 WIN_WIDTH		{};
int32 WIN_HEIGHT	{};
constexpr auto TITLE	{"Level Builder"};

float32 delta_time	{};
float32 last_frame	{};
float32 max_period	{1.0f / 60.0f};

RenderWindow* render;
RenderWindow* viewport;

int32		button_num	{};
Tile		tile_cursor	{};
bool		hovered		{false};
bool		focused		{false};
bool		opened		{false};
bool		changed		{false};
bool		popup		{false};
bool		new_popup	{false};
bool		open_popup	{false};
bool		exit_popup	{false};
std::string lvl_name	{};
int32		background	{};
float32		r_window	{};
glm::vec2	w_pos		{};
std::array<Tile, 160> level {};


auto main(int32 argc, char** argv) -> int32
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // for MAC OS X
#endif // __APPLE__
	
	auto vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	WIN_WIDTH = static_cast<int32>(static_cast<float32>(vidmode->width) * 0.9f);
	WIN_HEIGHT = static_cast<int32>(static_cast<float32>(vidmode->height) * 0.9f);

	auto window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, TITLE, nullptr, nullptr);
	glfwMakeContextCurrent(window);

	if (!window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		std::cout << "Failed to load GLAD" << std::endl;
		return -1;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.Fonts->AddFontFromFileTTF("font.ttf", 36.0f);
	io.FontGlobalScale = 0.5f;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init((char*)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));
	

	glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetKeyCallback(window, key_callback);

	/*==============================*/

	Resources::load_shader("sprite", "Shaders/vSprite.glsl", "Shaders/fSprite.glsl");
	Resources::load_shader("screen", "Shaders/vScreen.glsl", "Shaders/fScreen.glsl");
	render = new RenderWindow(WIN_WIDTH, WIN_HEIGHT);
	viewport = new RenderWindow(WIN_WIDTH, WIN_HEIGHT);

	

	std::vector<std::pair<Texture*, std::function<void()>>> tiles_list
	{
		{new Texture("Textures/Tiles/delete.png", false), [&]{ tile_cursor.type = DELETE; }},
		{new Texture("Textures/Tiles/bumper.png", false), [&]{ tile_cursor.type = BUMPER; }},
		{new Texture("Textures/Tiles/teleport.png", false), [&]{ tile_cursor.type = TELEPORT; }},
		{new Texture("Textures/Tiles/rail-0.png", false),show_rail_props},
		{new Texture("Textures/Tiles/tile-0.png", false), show_tile_props},
		{new Texture("Textures/Tiles/turret.png", false), show_turret_props}
	};

	Resources::load_texture("interface", "Textures/interface.png", false);
	Resources::load_texture("bg0", "Textures/bg0.png", false);
	Resources::load_texture("bg1", "Textures/bg1.png", false);
	Resources::load_texture("bg2", "Textures/bg2.png", false);
	Resources::load_texture("bg3", "Textures/bg3.png", false);
	Resources::load_texture("bg4", "Textures/bg4.png", false);
	Resources::load_texture("bg5", "Textures/bg5.png", false);
	Resources::load_texture("bg6", "Textures/bg6.png", false);
	Resources::load_texture("tiles", "Textures/tiles.png", false);


	Resources::load_sprite("interface", new Sprite(Resources::get_texture("interface"), {0, 0, Resources::get_texture("interface")->width + 1, Resources::get_texture("interface")->height + 1}, {640, 400}, false));
	Resources::load_sprites("bg", {
		new Sprite(Resources::get_texture("bg0"), {0, 0, 641, 401}, {640, 400}, false),
		new Sprite(Resources::get_texture("bg1"), {0, 0, 641, 401}, {640, 400}, false),
		new Sprite(Resources::get_texture("bg2"), {0, 0, 641, 401}, {640, 400}, false),
		new Sprite(Resources::get_texture("bg3"), {0, 0, 641, 401}, {640, 400}, false),
		new Sprite(Resources::get_texture("bg4"), {0, 0, 641, 401}, {640, 400}, false),
		new Sprite(Resources::get_texture("bg5"), {0, 0, 641, 401}, {640, 400}, false),
		new Sprite(Resources::get_texture("bg6"), {0, 0, 641, 401}, {640, 400}, false)		});
	Resources::load_sprites("tile", {
		new Sprite(Resources::get_texture("tiles"), {0, 0, 32, 32}, {640, 400}),
		new Sprite(Resources::get_texture("tiles"), {33, 0, 65, 32}, {640, 400}),
		new Sprite(Resources::get_texture("tiles"), {66, 0, 98, 32}, {640, 400})});
	Resources::load_sprites("rail", {
		new Sprite(Resources::get_texture("tiles"), {0, 66, 32, 72}, {640, 400}),
		new Sprite(Resources::get_texture("tiles"), {33, 66, 39, 98}, {640, 400})});
	Resources::load_sprite("turret", new Sprite(Resources::get_texture("tiles"), {66, 33, 98, 65}, {640, 400}));
	Resources::load_sprite("bumper", new Sprite(Resources::get_texture("tiles"), {0, 33, 32, 65}, {640, 400}));
	Resources::load_sprite("teleport", new Sprite(Resources::get_texture("tiles"), {99, 33, 131, 65}, {640, 400}));
	Resources::load_sprite("delete", new Sprite(Resources::get_texture("tiles"), {65, 66, 97, 98}, {640, 400}));

	/*==============================*/

	ImGuiWindowFlags menu_bar = 
			ImGuiWindowFlags_NoResize				| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoCollapse			| ImGuiWindowFlags_NoSavedSettings
			| ImGuiWindowFlags_NoTitleBar			| ImGuiWindowFlags_NoScrollbar
			| ImGuiWindowFlags_NoScrollWithMouse	| ImGuiWindowFlags_MenuBar;
	
	/*==============================*/

	while (!glfwWindowShouldClose(window))
	{
		const auto currentFrame = static_cast<float32>(glfwGetTime());
		delta_time = currentFrame - last_frame;

		if (delta_time >= max_period)
		{
			last_frame = currentFrame;

			process_input(window);

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			render->clear();

			viewport->clear();
			
			Resources::get_sprites("bg")[background]->draw(Resources::get_shader("sprite"), {0, 0});
			Resources::get_sprites("interface")[0]->draw(Resources::get_shader("sprite"), {0, 400 - Resources::get_sprites("interface")[0]->getSize().y});

			for (const auto& tile : level)
				draw_tile(tile);

			if (hovered)
				draw_tile(tile_cursor);

			viewport->display();

			render->display();

			ImGui::SetNextWindowSize({static_cast<float32>(WIN_WIDTH), 0.0f});
			ImGui::SetNextWindowPos({0.0f, 0.0f});
			ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, {0.0f, 0.0f});
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.0f, 0.0f});
			ImGui::Begin("Level Builder", nullptr, menu_bar);
			if (ImGui::BeginMenuBar())
			{
				ImGui::PopStyleVar();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {10, 10});
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("New", "Ctrl+N"))
					{
						if (changed)
							popup = true;
						else
							new_file();
						new_popup = true;
						open_popup = false;
						exit_popup = false;
						opened = false;
					}
					if (ImGui::MenuItem("Open", "Ctrl+O"))
					{
						if (changed)
							popup = true;
						else
							open_file();
						new_popup = false;
						open_popup = true;
						exit_popup = false;
						opened = true;
					}
					if (ImGui::MenuItem("Save", "Ctrl+S"))
					{
						changed = false;
						if (opened)
							save_level(lvl_name.c_str());
						else
							save_file();
					}
					if (ImGui::MenuItem("Save as", "Ctrl+Shift+S"))
					{
						changed = false;
						save_file();
					}
					if (ImGui::MenuItem("Close", "Ctrl+W"))
					{
						if (changed)
							popup = true;
						else
							close_window(window);
						new_popup = false;
						open_popup = false;
						exit_popup = true;
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}
			ImGui::End();

			if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
			{
				if (ImGuiFileDialog::Instance()->IsOk())
				{
					lvl_name = ImGuiFileDialog::Instance()->GetFilePathName();
					load_level(lvl_name.c_str());
					opened = true;
				}

				ImGuiFileDialog::Instance()->Close();
			}
			if (ImGuiFileDialog::Instance()->Display("SaveFileDlgKey"))
			{
				if (ImGuiFileDialog::Instance()->IsOk())
				{
					lvl_name = ImGuiFileDialog::Instance()->GetFilePathName();
					save_level(lvl_name.c_str());
				}

				ImGuiFileDialog::Instance()->Close();
			}

			if (ImGui::Begin("Backgrounds", nullptr, ImGuiWindowFlags_NoCollapse))
			{
				r_window = ImGui::GetWindowWidth();
				w_pos.x = ImGui::GetWindowPos().x;
				w_pos.y = ImGui::GetWindowPos().y + ImGui::GetWindowHeight();
				ImGui::Combo("##", &background, "bg0.png\0bg1.png\0bg2.png\0bg3.png\0bg4.png\0bg5.png\0bg6.png\0\0");
				ImGui::End();
			}			

			ImGui::SetNextWindowSize({r_window, 0.0f});
			ImGui::SetNextWindowPos({w_pos.x, w_pos.y});
			if(ImGui::Begin("Tiles", nullptr, ImGuiWindowFlags_NoCollapse))
			{
				w_pos.y = ImGui::GetWindowPos().y + ImGui::GetWindowHeight();

				constexpr float32	button_size		{70.0f};
				constexpr int32		button_padd		{6};
				float32 width {ImGui::GetContentRegionAvail().x};
				int32 columns {glm::clamp(static_cast<int32>(width / button_size), 1, 64)};
				width -= static_cast<float32>(2 * columns * (button_padd + 2));
				columns = glm::clamp(static_cast<int32>(width / button_size), 1, 64);
				const int32 rows	{glm::clamp(static_cast<int32>(ceil(static_cast<float32>(tiles_list.size()) / static_cast<float32>(columns))), 1, 64)};
				ImGui::BeginTable("table", columns, ImGuiTableFlags_NoPadInnerX);
				for (int row = 0; row < rows; row++)
				{
					ImGui::TableNextRow();
					for (int column = 0; column < columns; column++)
					{
						ImGui::TableSetColumnIndex(column);
						if (row * columns + column < tiles_list.size())
							if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(tiles_list[row * columns + column].first->ID), {button_size, button_size}, {0.0f, 0.0f}, {1.0f, 1.0f}, button_padd))
							{
								button_num = row * columns + column;
							}
					}
				}
				ImGui::EndTable();
				ImGui::End();
			}

			ImGui::SetNextWindowSize({r_window, 0.0f});
			ImGui::SetNextWindowPos({w_pos.x, w_pos.y});
			if (ImGui::Begin("Tile properties", nullptr, ImGuiWindowFlags_NoCollapse))
			{
				w_pos.y = ImGui::GetWindowPos().y + ImGui::GetWindowHeight();
				ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
				tiles_list[button_num].second();
				ImGui::End();
			}

			ImGui::SetNextWindowSize({r_window, 0.0f});
			ImGui::SetNextWindowPos({w_pos.x, w_pos.y});
			if (ImGui::Begin("Tile info", nullptr, ImGuiWindowFlags_NoCollapse))
			{
				if (hovered)
					ImGui::Text("tile x: %d, y: %d", tile_cursor.coord.x, tile_cursor.coord.y);
				ImGui::End();
			}
			
			if (ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoCollapse | (hovered * ImGuiWindowFlags_NoMove)))
			{
				focused = ImGui::IsWindowFocused();

				double xpos, ypos;

				glfwGetCursorPos(window, &xpos, &ypos);
				tile_cursor.pos.x = (static_cast<float32>(xpos) - ImGui::GetCursorScreenPos().x - ImGui::GetScrollX()) * (640.0f / ImGui::GetContentRegionAvail().x);
				tile_cursor.pos.y = 400.0f - (static_cast<float32>(ypos) - ImGui::GetCursorScreenPos().y - ImGui::GetScrollY()) * (640.0f / ImGui::GetContentRegionAvail().x);

				hovered = tile_cursor.pos.x < 640.0f && tile_cursor.pos.x > 0.0f
					&& tile_cursor.pos.y < 336.0f && tile_cursor.pos.y > 80.0f;

				tile_cursor.pos.x = glm::clamp(floor(tile_cursor.pos.x / 32.0f) * 32.0f + 16.0f, 16.0f, 624.0f);
					tile_cursor.pos.y = glm::clamp(floor((tile_cursor.pos.y + 16.0f) / 32.0f) * 32.0f, 96.0f, 320.0f);

				tile_cursor.coord.x = static_cast<int32>(tile_cursor.pos.x - 16.0f) / 32;
				tile_cursor.coord.y = 7 - static_cast<int32>(tile_cursor.pos.y - 96.0f) / 32;

				if (tile_cursor.type == RAIL)
				{
					tile_cursor.pos.x += static_cast<float32>(tile_cursor.variation) * 0.5f
						* static_cast<float32>((tile_cursor.stat == 0) - (tile_cursor.stat == 2))
						* (Resources::get_sprites("rail")[tile_cursor.variation]->getSize().x - 32.0f);
					tile_cursor.pos.y += static_cast<float32>(!tile_cursor.variation) * 0.5f
						* static_cast<float32>((tile_cursor.stat == 2) - (tile_cursor.stat == 0))
						* (32.0f - Resources::get_sprites("rail")[tile_cursor.variation]->getSize().y);
				}				

				ImGui::GetWindowDrawList()->AddImage(
					reinterpret_cast<void*>(viewport->texture.ID), 
					ImVec2(ImGui::GetCursorScreenPos()),
					{ImGui::GetCursorScreenPos().x + ImGui::GetContentRegionAvail().x, 
					ImGui::GetCursorScreenPos().y + ImGui::GetContentRegionAvail().x * (400.0f / 640.0f)},
					{0, 0}, {1, 1});
				ImGui::End();
			}
			if (popup)
			{
				ImGui::OpenPopup("Unsaved changes!");
				popup = false;
			}

			if (ImGui::BeginPopupModal("Unsaved changes!", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("You will lost all unsaved changes!\n Do you want to continue?");

				if (ImGui::Button("Yes"))
				{
					if (new_popup)
					{
						new_file();
					}
					if (open_popup)
					{
						open_file();
					}
					if (exit_popup)
					{
						close_window(window);
					}
					ImGui::CloseCurrentPopup();
				}
				ImGui::SetItemDefaultFocus();
				ImGui::SameLine();
				if (ImGui::Button("No, let me save everything"))
					ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
			}

			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
			

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			glfwPollEvents();
			glfwSwapBuffers(window);
		}
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	Resources::clear();

	for (auto& [fst, scd] : tiles_list)
		delete fst;
	delete render;
	glfwTerminate();
	return 0;
}

auto WinMain(int32 argc, char** argv) -> int
{
	main(argc, argv);
}

void framebuffer_size_callback(GLFWwindow* window, int32 width, int32 height)
{
	glViewport(0, 0, width, height);
}

void window_size_callback(GLFWwindow* window, int32 width, int32 height)
{
	WIN_WIDTH = width;
	WIN_HEIGHT = height;

	delete render;
	delete viewport;

	viewport = new RenderWindow(WIN_WIDTH, WIN_HEIGHT);
	render = new RenderWindow(WIN_WIDTH, WIN_HEIGHT);
}

void process_input(GLFWwindow* window)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS
		&& hovered && focused)
	{
		changed = true;
	    level[tile_cursor.coord.y * 20 + tile_cursor.coord.x] = tile_cursor;
		level[tile_cursor.coord.y * 20 + tile_cursor.coord.x].type = static_cast<Type>(EMPTY * (tile_cursor.type == DELETE) + tile_cursor.type * (tile_cursor.type != DELETE));
	}
}

void key_callback(GLFWwindow* window, int32 key, int32 scancode, int32 action, int32 mods)
{
	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
	if (mods == GLFW_MOD_CONTROL && key == GLFW_KEY_N && action == GLFW_PRESS)
	{
		if (changed)
			popup = true;
		else
			new_file();
		new_popup = true;
		open_popup = false;
		exit_popup = false;
		opened = false;
	}

	if (mods == GLFW_MOD_CONTROL && key == GLFW_KEY_O && action == GLFW_PRESS)
	{
		if (changed)
			popup = true;
		else
			open_file();
		new_popup = false;
		open_popup = true;
		exit_popup = false;
		opened = true;
	}

	if (mods == GLFW_MOD_CONTROL && key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		changed = false;
		if (opened)
			save_level(lvl_name.c_str());
		else
			save_file();
	}		

	if ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT))
		&& (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL))
		&& key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		changed = false;
		save_file();
	}

	if (mods == GLFW_MOD_CONTROL && key == GLFW_KEY_W && action == GLFW_PRESS)
	{
		if (changed)
			popup = true;
		else
			close_window(window);
		new_popup = false;
		open_popup = false;
		exit_popup = true;
	}
}

void new_file()
{
	for (auto& tile : level)
		tile.type = EMPTY;
	new_popup = false;
}

void open_file()
{
	ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".lvl", ".");
	open_popup = false;
}

void save_file()
{
	ImGuiFileDialog::Instance()->OpenDialog("SaveFileDlgKey","Choose a File", ".lvl", ".", "", 1, nullptr, ImGuiFileDialogFlags_ConfirmOverwrite);
}

void close_window(GLFWwindow* window)
{
	glfwSetWindowShouldClose(window, true);
}

void load_level(const char* file)
{
	std::ifstream lvl_file;
	std::string lvl_str;
	bool read_m	{false};
	Object_types mode{};

	for (auto& tile : level)
		tile.type = EMPTY;

	lvl_file.open(file, std::ios::in);
	while (std::getline(lvl_file, lvl_str))
	{
		if (const auto it = lvl_str.find("//"); it != std::string::npos)
			lvl_str.erase(it);
		lvl_str.erase(
			std::remove_if(lvl_str.begin(), lvl_str.end(), 
				isspace), lvl_str.end());
		lvl_str.erase(std::remove_if(lvl_str.begin(), lvl_str.end(), 
			[&read_m](uint8 x) -> bool { return read_m = (x == '[' || x == ']'); }), lvl_str.end());
		if (lvl_str.empty())
			continue;
		if (read_m)
		{
			mode = transcriber.at(lvl_str);
			continue;	
		}

		std::vector<uint8> tile_params;
		get_tile_params(lvl_str, tile_params);

		switch (mode)
		{
		case Object_types::BACKGROUND:
			tile_params[0] %= 7;
			background = tile_params[0];
			break;

		case Object_types::TILES:
			tile_params[0] %= 20;
			tile_params[1] %= 8;

			level[tile_params[1] * 20 + tile_params[0]].coord = {tile_params[0], tile_params[1]};

			level[tile_params[1] * 20 + tile_params[0]].pos = {
				32.0f * (static_cast<float32>(tile_params[0]) + 0.5f), 
				static_cast<float32>(400) - 32.0f * (static_cast<float32>(tile_params[1]) + 2.5f)};

			level[tile_params[1] * 20 + tile_params[0]].type = TILE;
			level[tile_params[1] * 20 + tile_params[0]].stat = tile_params[2];
			level[tile_params[1] * 20 + tile_params[0]].variation = tile_params[3];
			break;

		case Object_types::BUMPERS:
			tile_params[0] %= 20;
			tile_params[1] %= 8;

			level[tile_params[1] * 20 + tile_params[0]].coord = {tile_params[0], tile_params[1]};

			level[tile_params[1] * 20 + tile_params[0]].pos = {
				32.0f * (static_cast<float32>(tile_params[0]) + 0.5f), 
				static_cast<float32>(400) - 32.0f * (static_cast<float32>(tile_params[1]) + 2.5f)};

			level[tile_params[1] * 20 + tile_params[0]].type = BUMPER;
			break;

		case Object_types::RAILS:
			tile_params[0] %= 20;
			tile_params[1] %= 8;

			level[tile_params[1] * 20 + tile_params[0]].coord = {tile_params[0], tile_params[1]};

			level[tile_params[1] * 20 + tile_params[0]].pos = {
				32.0f * (static_cast<float32>(tile_params[0]) + 0.5f) 
				+ static_cast<float32>(tile_params[3]) * 0.5f
				* static_cast<float32>((tile_params[2] == 0) - (tile_params[2] == 2))
				* (Resources::get_sprites("rail")[tile_params[3]]->getSize().x - 32.0f),
				static_cast<float32>(400) -
				32.0f * (static_cast<float32>(tile_params[1]) + 2.5f)
				+ static_cast<float32>(!tile_params[3]) * 0.5f
				* static_cast<float32>((tile_params[2] == 2) - (tile_params[2] == 0))
				* (32.0f - Resources::get_sprites("rail")[tile_params[3]]->getSize().y)};

			level[tile_params[1] * 20 + tile_params[0]].type = RAIL;
			level[tile_params[1] * 20 + tile_params[0]].stat = tile_params[2];
			level[tile_params[1] * 20 + tile_params[0]].variation = tile_params[3];
			break;

		case Object_types::TELEPORTERS:
			tile_params[0] %= 20;
			tile_params[1] %= 8;

			level[tile_params[1] * 20 + tile_params[0]].coord = {tile_params[0], tile_params[1]};

			level[tile_params[1] * 20 + tile_params[0]].pos = {
				32.0f * (static_cast<float32>(tile_params[0]) + 0.5f), 
				static_cast<float32>(400) - 32.0f * (static_cast<float32>(tile_params[1]) + 2.5f)};

			level[tile_params[1] * 20 + tile_params[0]].type = TELEPORT;
			break;

		case Object_types::TURRETS:
			tile_params[0] %= 20;
			tile_params[1] %= 8;

			level[tile_params[1] * 20 + tile_params[0]].coord = {tile_params[0], tile_params[1]};

			level[tile_params[1] * 20 + tile_params[0]].pos = {
				32.0f * (static_cast<float32>(tile_params[0]) + 0.5f), 
				static_cast<float32>(400) - 32.0f * (static_cast<float32>(tile_params[1]) + 2.5f)};

			level[tile_params[1] * 20 + tile_params[0]].type = TURRET;
			level[tile_params[1] * 20 + tile_params[0]].stat = tile_params[2];
			break;
		}
		tile_params.clear();
	}
	lvl_file.close();
}

void save_level(const char*)
{
	std::ofstream lvl_file;
	lvl_file.open(lvl_name, std::ios::out);

	lvl_file << "[BACKGROUND]\n";
	lvl_file << background << "\n\n";

	lvl_file << "[TILES]\n";
	for (const auto& tile : level)
		if (tile.type == TILE)
			lvl_file << tile.coord.x << ", " << tile.coord.y << ", " << tile.stat << ", " << tile.variation << "\n";
	lvl_file << "\n";

	lvl_file << "[BUMPERS]\n";
	for (const auto& tile : level)
		if (tile.type == BUMPER)
			lvl_file << tile.coord.x << ", " << tile.coord.y << "\n";
	lvl_file << "\n";

	lvl_file << "[RAILS]\n";
	for (const auto& tile : level)
		if (tile.type == RAIL)
			lvl_file << tile.coord.x << ", " << tile.coord.y << ", " << tile.stat << ", " << tile.variation << "\n";
	lvl_file << "\n";

	lvl_file << "[TELEPORTERS]\n";
	for (const auto& tile : level)
		if (tile.type == TELEPORT)
			lvl_file << tile.coord.x << ", " << tile.coord.y << "\n";
	lvl_file << "\n";

	lvl_file << "[TURRETS]\n";
	for (const auto& tile : level)
		if (tile.type == TURRET)
			lvl_file << tile.coord.x << ", " << tile.coord.y << ", " << tile.stat << "\n";

	lvl_file.close();
}


void get_tile_params(const std::string& object_str, std::vector<uint8>& tile_params)
{
	std::istringstream lvl_stream(object_str);
	std::string tile_param_str;
	while (std::getline(lvl_stream, tile_param_str,','))
		tile_params.push_back(static_cast<uint8>(std::stoi(tile_param_str)));
}

void draw_tile(const Tile& tile)
{
	switch (tile.type)
	{
	case EMPTY:
		break;

	case TILE:
		Resources::get_sprites("tile")[tile.variation]->draw(Resources::get_shader("sprite"), tile.pos);
		break;

	case RAIL:
		Resources::get_sprites("rail")[tile.variation]->draw(Resources::get_shader("sprite"), tile.pos);
		break;

	case BUMPER:
		Resources::get_sprites("bumper")[0]->draw(Resources::get_shader("sprite"), tile.pos);
		break;

	case TELEPORT:
		Resources::get_sprites("teleport")[0]->draw(Resources::get_shader("sprite"), tile.pos);
		break;

	case TURRET:
		Resources::get_sprites("turret")[0]->draw(Resources::get_shader("sprite"), tile.pos);
		break;

	case DELETE:
		Resources::get_sprites("delete")[0]->draw(Resources::get_shader("sprite"), tile.pos);
		break;
	}
}

void show_tile_props()
{
	tile_cursor.type = TILE;
	tile_cursor.variation %= Resources::get_sprites("tile").size();
	tile_cursor.stat = glm::clamp(tile_cursor.stat, 1u, 64u);
	ImGui::Text("Tile hp");
	ImGui::InputInt("##1", reinterpret_cast<int32*>(&tile_cursor.stat));
	ImGui::Separator();
	ImGui::Text("Tile variation");
	ImGui::SliderInt("##2", reinterpret_cast<int32*>(&tile_cursor.variation), 0, 2);
}

void show_rail_props()
{
	tile_cursor.type = RAIL;
	tile_cursor.variation %= Resources::get_sprites("rail").size();
	ImGui::Text("Rail displacement");
	ImGui::SliderInt("##1", reinterpret_cast<int32*>(&tile_cursor.stat), 0, 2);
	ImGui::Separator();
	ImGui::Text("Rail is vertical?");
	ImGui::Checkbox("##2", reinterpret_cast<bool*>(&tile_cursor.variation));
}

void show_turret_props()
{
	tile_cursor.type = TURRET;
	tile_cursor.stat = glm::clamp(tile_cursor.stat, 1u, 100u);
	ImGui::Text("Bullet count");
	ImGui::InputInt("##", reinterpret_cast<int32*>(&tile_cursor.stat));
}
