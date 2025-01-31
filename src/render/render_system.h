#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include <stdlib.h>
#include <DirectXMath.h>

#include "mesh.h"
#include "font.h"
#include "vertices.h"
#include "render_api.h"
#include "shader_manager.h"
#include "../libs/color.h"
#include "../libs/math/matrix.h"
#include "../libs/math/vector.h"
#include "../win32/win_helpers.h"

struct Primitive_2D {
	//Vars is set by Render_2D
	u32 vertex_offset = 0;
	u32 index_offset = 0;

	Array<Vertex_X2UV> vertices;
	Array<u32> indices;

	void add_point(const Vector2 &point, const Vector2 &uv = Vector2(0.0f, 0.0f)) { vertices.push(Vertex_X2UV(point, uv)); }
	void add_rounded_points(float x, float y, float width, float height, Rect_Side rect_side, u32 rounding);
	void add_rounded_points(float x, float y, float width, float height, Rect_Side rect_side, float x_rounding, float y_rounding);

	void make_triangle_polygon();
	void make_outline_triangle_polygons();
};

struct Render_Primitive_2D {
	Primitive_2D *primitive = NULL;
	Color color;
	Rect_s32 clip_rect;
	Texture2D texture;
	Matrix4 transform_matrix;
};

struct Render_2D;

struct Render_Font {
	Texture2D font_atlas;
	Hash_Table<u8, Primitive_2D *> lookup_table;

	void init(Render_2D *render_2d, Font *font);
	void make_font_atlas(Font *font, Hash_Table<char, Rect_f32> *font_uvs);
};

const u32 ROUND_TOP_LEFT_RECT = 0x1;
const u32 ROUND_TOP_RIGHT_RECT = 0x2;
const u32 ROUND_BOTTOM_LEFT_RECT = 0x4;
const u32 ROUND_BOTTOM_RIGHT_RECT = 0x8;
const u32 ROUND_RIGHT_RECT = ROUND_TOP_RIGHT_RECT | ROUND_BOTTOM_RIGHT_RECT;
const u32 ROUND_LEFT_RECT = ROUND_TOP_LEFT_RECT | ROUND_BOTTOM_LEFT_RECT;
const u32 ROUND_TOP_RECT = ROUND_TOP_LEFT_RECT | ROUND_TOP_RIGHT_RECT;
const u32 ROUND_BOTTOM_RECT = ROUND_BOTTOM_LEFT_RECT | ROUND_BOTTOM_RIGHT_RECT;
const u32 ROUND_RECT = ROUND_TOP_RECT | ROUND_BOTTOM_RECT;

struct Circle_Range {
	u32 start;
	u32 end;
};

struct Render_Primitive_List {
	//@Note: default construcor should be deleted. 
	Render_Primitive_List() {}
	Render_Primitive_List(Render_2D *render, Font *font, Render_Font *render_font);
#ifdef _DEBUG
	String name;
#endif
	Render_2D *render_2d = NULL;
	Font *font = NULL;
	Render_Font *render_font = NULL;

	Array<Rect_s32> clip_rects;
	Array<Render_Primitive_2D> render_primitives;

	void push_clip_rect(Rect_s32 *rect);
	void pop_clip_rect();
	void get_clip_rect(Rect_s32 *rect);

	void add_outlines(int x, int y, int width, int height, const Color &color, float outline_width = 1.0f, u32 rounding = 0, u32 flags = ROUND_RECT);

	void add_text(Rect_s32 *rect, const char *text, Text_Alignment text_alignment = ALIGN_TEXT_BY_MAX_SYMBOL_IN_TEXT);
	void add_text(int x, int y, const char *text, Text_Alignment text_alignment = ALIGN_TEXT_BY_MAX_SYMBOL_IN_TEXT);

	void add_rect(Rect_s32 *rect, const Color &color, u32 rounding = 0, u32 flags = ROUND_RECT);
	void add_rect(s32 x, s32 y, s32 width, s32 height, const Color &color, u32 rounding = 0, u32 flags = ROUND_RECT);
	void add_rect(float x, float y, float width, float height, const Color &color, u32 rounding = 0, u32 flags = ROUND_RECT);

	void add_texture(Rect_s32 *rect, Texture2D *resource);
	void add_texture(int x, int y, int width, int height, Texture2D *resource);

	void add_line(const Point_s32 &first_point, const Point_s32 &second_point, const Color &color, float thicknesss = 0.5f);

	void add_circle(int x, int y, u32 radius, const Color &color, const Circle_Range &circle_range = { 0, 360 } );
	void add_outline_circle(int x, int y, u32 radius, float thickness, const Color &color);

	Primitive_2D *make_or_find_primitive(Matrix4 &transform_matx, Texture2D *texture, const Color &color, String &primitve_hash);
};

struct Render_System;

struct Render_2D {
	~Render_2D();

	bool initialized = false;

	Texture2D default_texture;

	Gpu_Buffer constant_buffer;
	Gpu_Buffer vertex_buffer;
	Gpu_Buffer index_buffer;

	Rasterizer_State rasterizer_state;
	Blend_State blend_state;
	Depth_Stencil_State depth_stencil_state;

	Extend_Shader *render_2d = NULL;

	Gpu_Device *gpu_device = NULL;
	Render_Pipeline *render_pipeline = NULL;
	Render_System *render_system = NULL;

	u32 total_vertex_count = 0;
	u32 total_index_count = 0;

	Array<Primitive_2D *> primitives;
	Array<Render_Primitive_List *> draw_list;
	Hash_Table<String, Primitive_2D *> lookup_table;
	Hash_Table<String, Render_Font *> render_fonts;

	void init(Render_System *render_sys, Shader_Manager *shader_manager);
	void add_primitive(Primitive_2D *primitive);
	void add_render_primitive_list(Render_Primitive_List *render_primitive_list);
	Render_Font *get_render_font(Font *font);

	void new_frame();
	void render_frame(); // @Clean up change name 
};

struct View {
	float ratio;
	float fov;
	float near_plane;
	float far_plane;

	Matrix4 perspective_matrix;
	Matrix4 orthogonal_matrix;

	void update_projection_matries(u32 fov_in_degrees, u32 width, u32 height, float _near_plane, float _far_plane);
};

struct Projection_Plane {
	u32 width;
	u32 height;
	float ratio;
	float fov_y_ratio;
	float near_plane;
	float far_plane;
};

struct Projection_Matries {
	Matrix4 perspective_matrix;
	Matrix4 orthogonal_matrix;

	void update();
};

struct Render_Pipeline_States {
	Blend_State default_blend_state;
	Blend_State disabled_blend_state;
	Blend_State transparent_blend_state;
	Sampler_State point_sampling;
	Sampler_State linear_sampling;
	Rasterizer_State default_rasterizer_state;
	Rasterizer_State disabled_multisampling_state;
	Depth_Stencil_State default_depth_stencil_state;
	Depth_Stencil_State disabled_depth_test;
	Depth_Stencil_State outlining_depth_stencil_state;
	Depth_Stencil_State pre_outlining_depth_stencil_state;

	void init(Gpu_Device *gpu_device);
};

struct Render_3D {
	struct Draw_Info {
		Color color;
		Matrix4 world_matrix;
	};

	u32 index_count = 0;
	u32 index_offset = 0;
	u32 vertex_offset = 0;

	Shader *draw_vertices = NULL;
	Gpu_Device *gpu_device = NULL;
	Render_Pipeline *render_pipeline = NULL;
	Render_System *render_system = NULL;

	Gpu_Buffer vertex_buffer;
	Gpu_Buffer index_buffer;
	Gpu_Buffer draw_info_cbuffer;

	void init(Render_System *_render_system, Shader_Manager *shader_manager);
	void shutdown();
	void set_mesh(Vertex_Mesh *mesh);
	void reset_mesh();
	
	void draw(const Vector3 &position, const Color &mesh_color);
	void draw_lines(const Vector3 &position, const Color &mesh_color);
	void draw_triangles(const Vector3 &position, const Color &mesh_color);
};

struct Render_System {
	static u32 screen_width;
	static u32 screen_height;

	struct Input_Layouts {
		Input_Layout vertex_P2UV2;
		Input_Layout vertex_P3;
	} input_layouts;

	//@Note: Why I need to have this var here ?
	View view;
	//@Note: Why I need to have this var here ?
	Render_2D render_2d;
	Render_3D render_3d;

	Texture2D silhouette_buffer;
	Texture2D silhouette_depth_stencil_buffer;
	Texture2D back_buffer_texture;

	Texture2D multisampling_back_buffer_texture;
	Texture2D multisampling_depth_stencil_texture;
	Texture2D voxel_render_target;

	Swap_Chain swap_chain;
	Gpu_Device gpu_device;
	Render_Pipeline render_pipeline;
	Render_Pipeline_States render_pipeline_states;

	void init(Win32_Window *window);
	void init_render_targets(u32 window_width, u32 window_height);
	void init_input_layouts(Shader_Manager *shader_manager);

	void resize(u32 window_width, u32 window_height);

	void new_frame();
	void end_frame();
};
#endif
