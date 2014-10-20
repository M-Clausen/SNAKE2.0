/******************************************
 * COPYRIGHT (C) 2014 MADS HAUPT CLAUSEN
 ******************************************/

#ifndef __DRAW_H_INCLUDED__
#define __DRAW_H_INCLUDED__

#include <iostream>
#include <stdlib.h>
#include <string>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>

#include "polygon.h"
#include "shader.h"

#include "math/math.hpp"
#include "math/mat2.hpp"
#include "math/mat4.hpp"
#include "math/vec4.hpp"


namespace graphics
{
	struct Light
	{
		math::vec3f position, color;
		float range;
	};

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
		void rectangle(math::mat2f &rect, math::vec4f &color, float z_index = 0.2f);
		void polygon(Polygon &poly, math::vec4f &color, float z_index = 0.2f);
		void line(math::vec2f p1, math::vec2f p2, math::vec4f &color, float z_index = 0.2f);
		void vertical_line(float x, float y, float height, math::vec4f &color);
		void outline(Polygon &poly, math::vec4f &color, float z_index = 0.2f);
	};

	void add_light(Light *l);
	void add_light_block_rect(math::mat2f *rect);
	void clear_light_block_rects();
};

#endif