#include "engine.h"
#include "../gui/gui.h"
#include "../win32/test.h"
#include "../win32/win_time.h"
#include "../libs/os/file.h"

#define DRAW_TEST_GUI 0

static const u32 FONT_SIZE = 11;
static Engine *engine = NULL;

void Engine::init(Win32_Info *_win32_info)
{
	engine = this;
	
	win32_info = *_win32_info;
	init_os_path();

	font.init(FONT_SIZE);

	render_sys.init(&win32_info, &font);
	gui::init_gui(&render_sys.render_2d, &win32_info, &font);

	game_world.init();
	render_world.init();
	
	String path;
	build_full_path_to_map_file("temp_map.bmap", path);
	if (file_exists(path)) {
		init_from_map();
	}

	editor.init();

	engine->is_initialized = true;
}

void Engine::init_from_map()
{
	game_world.init_from_file();

	Geometry_Entity *geometry_entity = NULL;
	For(game_world.geometry_entities, geometry_entity) {
		
		char *mesh_name = NULL;
		Mesh_Idx mesh_idx;
		Triangle_Mesh triangle_mesh;
		if (geometry_entity->geometry_type == GEOMETRY_TYPE_BOX) {
			make_box_mesh(&geometry_entity->box, &triangle_mesh);
			mesh_name = format("Box", geometry_entity->box.width, geometry_entity->box.height, geometry_entity->box.depth);
		
		} else if (geometry_entity->geometry_type == GEOMETRY_TYPE_SPHERE) {
			make_sphere_mesh(&geometry_entity->sphere, &triangle_mesh);
			mesh_name = format("Sphere", geometry_entity->sphere.radius, geometry_entity->sphere.slice_count, geometry_entity->sphere.stack_count);
		
		} else if (geometry_entity->geometry_type == GEOMETRY_TYPE_GRID) {
			make_grid_mesh(&geometry_entity->grid, &triangle_mesh);
			mesh_name = format("Grid", geometry_entity->grid.width, geometry_entity->grid.depth, geometry_entity->grid.rows_count, geometry_entity->grid.columns_count);
		
		} else {
			print("Engine::init_from_file: Unknown geometry type.");
		}
		render_world.add_mesh(mesh_name, &triangle_mesh, &mesh_idx);
		render_world.make_render_entity(get_entity_id(geometry_entity), mesh_idx);
		
		free_string(mesh_name);
	}
}

void Engine::frame()
{
	s64 start_time = milliseconds_counter();
	s64 ticks_counter = cpu_ticks_counter();

	pump_events();
	run_event_loop();

	render_world.update();

	render_sys.new_frame();

	render_world.render();

#if DRAW_TEST_GUI
	gui::draw_test_gui();
#else
	editor.render();
#endif

	display_performace_info();

	render_sys.end_frame();

	clear_event_queue();

	fps = cpu_ticks_per_second() / (cpu_ticks_counter() - ticks_counter);
	frame_time = milliseconds_counter() - start_time;
}

void Engine::save_to_file()
{
	game_world.save_to_file();
}

void Engine::display_performace_info()
{
	static auto render_list = Render_Primitive_List(&render_sys.render_2d);
	
	char *test = format("Fps", fps);
	char *test2 = format("Frame time {} ms", frame_time);
	u32 text_width = font.get_text_width(test2);
	
	s32 x = win32_info.window_width - text_width - 10;

	render_list.add_text(x, 20, test);
	render_list.add_text(x, 40, test2);
	free_string(test);
	free_string(test2);

	render_sys.render_2d.add_render_primitive_list(&render_list);
}

void Engine::shutdown()
{
	gui::shutdown();
	save_to_file();
}

bool Engine::initialized()
{
	if (engine) {
		return engine->is_initialized;
	}
	return false;
}

void Engine::resize_window(u32 window_width, u32 window_height)
{
	engine->render_sys.resize(window_width, window_height);
}

Engine *Engine::get_instance()
{
	return engine;
}

Font *Engine::get_font()
{
	return &engine->font;
}

Game_World *Engine::get_game_world()
{
	return &engine->game_world;
}

Render_World *Engine::get_render_world()
{
	return &engine->render_world;
}

Render_System *Engine::get_render_system()
{
	return &engine->render_sys;
}

Mesh_Loader *Engine::get_mesh_loader()
{
	return &engine->mesh_loader;
}

u32 get_window_width()
{
	return engine->win32_info.window_width;
}

u32 get_window_height()
{
	return engine->win32_info.window_height;
}