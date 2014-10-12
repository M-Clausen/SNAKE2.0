/******************************************
 * COPYRIGHT (C) 2014 MADS HAUPT CLAUSEN
 ******************************************/

#ifndef __SNAKE_H_INCLUDED__
#define __SNAKE_H_INCLUDED__

#include <stdlib.h>

#include "constants.h"
#include "draw.h"
#include "map.h"
#include "events.h"

#include "math/vec2.hpp"
#include "math/vec4.hpp"
#include "math/mat2.hpp"

struct Snake_Elem
{
	int mapx, mapy, mapz;
	char is_head, visible, direction;
	Snake_Elem *next, *prev, *last;
};

class Snake
{
	private:
		char direction; /* 0 - up, 1 - right, 2 - down, 3 - left */
		Snake_Elem head, next;
		Map *parent_map;

	public:
		char can_change_direction, can_teleport;
		math::vec4f color;

		Snake()
		{
			this->head.mapx = this->head.mapy = 0;
			this->head.is_head = 1;
			this->head.last = &(this->head);
			this->next.mapx = 1;
			this->next.mapy = 0;
			this->next.is_head = this->next.visible = 0;
			this->direction = SNAKE_DIRECTION_DOWN;
			this->can_change_direction = this->can_teleport = 1;

			this->color = math::vec4f(1.0f, 1.0f, 1.0f, 1.0f);

			this->head.mapz = this->next.mapz = 0;
		};

		Snake(int x, int y)
		{
			this->head.mapx = x;
			this->head.mapy = y;
			this->head.is_head = 1;
			this->head.last = &(this->head);
			printf("init: head: %p             last = %p\n", &(this->head), this->head.last);
			this->next.mapx = x + 1;
			this->next.mapy = y;
			this->next.is_head = this->next.visible = 0;
			this->direction = SNAKE_DIRECTION_DOWN;
			this->can_change_direction = this->can_teleport = 1;

			this->color = math::vec4f(1.0f, 1.0f, 1.0f, 1.0f);

			this->head.mapz = this->next.mapz = 0;
		};

		void set_direction(char dir);

		char get_direction()
		{
			return this->direction;
		}

		void set_map(Map *map)
		{
			this->parent_map = map;
		}

		void set_next_pos(int x, int y, int z = 0)
		{
			this->next.mapx = x;
			this->next.mapy = y;
			this->next.mapz = z;
		}

		void add_element();

		void move();
		void render();
};

#endif