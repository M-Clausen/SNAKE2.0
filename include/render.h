/******************************************
 * COPYRIGHT (C) 2014 MADS HAUPT CLAUSEN
 ******************************************/

#ifndef __DRAW_H_INCLUDED__
#define __DRAW_H_INCLUDED__

#include <iostream>
#include <stdlib.h>
#include <string>
#include <cmath>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>

#include "polygon.h"
#include "shader.h"
#include "constants.h"

#include "math/math.hpp"
#include "math/mat2.hpp"
#include "math/mat4.hpp"
#include "math/vec4.hpp"

namespace graphics
{
	struct Shadow_Map
	{
		math::mat4f *projection_matrix, *translation_matrix;
		GLuint fbo, cubemap_texture;
		Shader *shader;
		GLuint projection_uniform_id, translation_uniform_id;

		GLuint posx_id, negx_id, posy_id, negy_id, posz_id, negz_id;

		void init();
		void upload_rotation_matrices();
	};

	struct Light
	{
		math::vec3f position, color;
		float range;
		Shadow_Map *shadowmap;

		/* init shadowmap and stuff */
		void init(Shader *shad, math::mat4f *proj_matrix, math::mat4f *transl_matrix);
	};

	struct Rectangle
	{
		math::mat2f rect;
		math::vec4f color;
		float height;
		char render_in_shadowmap;

		Rectangle()
		{
			render_in_shadowmap = 1;
		}
	};

	void init_shadowmap_rotation_matrices();

	void set_window_title(std::string title);
	void set_window_size(int w, int h);

	/**
	 * Create SDL window & context.
	 *
	 * @return 0 on success, otherwise -1 or other error code
	 */
	int init_sdl();

	/**
	 * Set up OpenGL.
	 *
	 * @return 0 on success, otherwise -1 or other error code
	 */
	int init_gl();

	void clear();
	void render();
	void destroy();

	namespace draw
	{
		void square(math::vec2f pos, float size, math::vec4f color);

		void add_rectangle(Rectangle *rect);
		void clear_rectangles();

		void polygon(Polygon &poly, math::vec4f &color, float z_index = 0.2f);
		void line(math::vec2f p1, math::vec2f p2, math::vec4f &color, float z_index = 0.2f);
		void vertical_line(float x, float y, float height, math::vec4f &color);
		void outline(Polygon &poly, math::vec4f &color, float z_index = 0.2f);
	};

	void add_light(Light *l, char gen_framebuffer = 0);
	void add_light_block_rect(math::mat2f *rect);
	void clear_light_block_rects();
};

#endif