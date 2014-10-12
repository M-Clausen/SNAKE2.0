/******************************************
 * COPYRIGHT (C) 2014 MADS HAUPT CLAUSEN
 ******************************************/

#ifndef __MAP_H_INCLUDED__
#define __MAP_H_INCLUDED__

#include <string>
#include <sstream>
#include <fstream>
#include <stdlib.h>

#include "draw.h"
#include "constants.h"

#include "math/vec2.hpp"
#include "math/vec4.hpp"

class Game;

class Map;

struct Portal
{
	Portal *partner;
	int mapx, mapy;
	Map *parent_map = 0;
	char render = 0;
	math::vec4f color;

	Portal()
	{
		color = math::vec4f(1.0f, 1.0f, 0.0f, 1.0f);
	}
};

class Map
{
	private:
		char *base_map, *food_map;
		int width, height;
		std::vector<Portal *> portals;
		Game *parent_game;

		/* for internal use upon normal loading failure */
		void init()
		{
			this->width = WINDOW_WIDTH / MAP_TILE_SIZE;
			this->height = WINDOW_HEIGHT / MAP_TILE_SIZE;

			this->base_map = new char[this->width * this->height];
			memset(this->base_map, 0, this->width * this->height);

			this->food_map = new char[this->width * this->height];
			memset(this->food_map, 0, this->width * this->height);

			this->food_color = math::vec4f(1.0f, 1.0f, 1.0f, 1.0f);
		}

	public:
		int game_list_index;
		math::vec4f food_color;

		/* use window width/height for width and height of map */
		Map()
		{
			this->init();
		}

		Map(int w, int h)
		{
			this->width = w;
			this->height = h;

			this->base_map = new char[this->width * this->height];
			memset(this->base_map, 0, this->width * this->height);

			this->food_map = new char[this->width * this->height];
			memset(this->food_map, 0, this->width * this->height);
			
			this->food_color = math::vec4f(1.0f, 1.0f, 1.0f, 1.0f);
		}

		Map(std::string path);

		std::vector<Portal *> &get_portals()
		{
			return this->portals;
		}

		void set_parent_game(Game *game)
		{
			this->parent_game = game;
		}

		void set_base_tile(int x, int y, int t)
		{
			if(x < this->width && y < this->height && x > 0 && y > 0)
				this->base_map[x + y * this->width] = t;
		}

		int get_base_tile(int x, int y)
		{
			if(x < this->width && y < this->height && x >= 0 && y >= 0)
				return this->base_map[x + y * this->width];

			return 1;
		}

		bool set_food_tile(int x, int y, int t)
		{
			if(this->base_map[x + y * this->width] != 0)
				return false;

			if(x < this->width && y < this->height && x > 0 && y > 0)
			{
				this->food_map[x + y * this->width] = t;
				return true;
			}

			return false;
		}

		int get_food_tile(int x, int y)
		{
			if(x < this->width && y < this->height && x >= 0 && y >= 0)
				return this->food_map[x + y * this->width];

			return 1;
		}

		void add_portals(Portal p1, Portal p2, char same_map)
		{
			Portal *p1_p = new Portal(p1);
			Portal *p2_p = new Portal(p2);

			/* map pointers expected if not in same map */
			if(same_map)
			{
				p1_p->parent_map = p2_p->parent_map = this;
				p1_p->render = p2_p->render = 1;
			}
			else
			{
				if(p1_p->parent_map == this)
					p1_p->render = 1;

				if(p2_p->parent_map == this)
					p2_p->render = 1;
			}

			p1_p->partner = p2_p;
			p2_p->partner = p1_p;
			portals.push_back(p1_p);
			portals.push_back(p2_p);
		}

		void render_base();
		void render_food();
		void render_portals();
		void render_grid();

		int get_width()
		{
			return this->width;
		}

		int get_height()
		{
			return this->height;
		}

		void destroy()
		{
			for(int i = 0; i < portals.size(); ++i)
			{
				delete(portals[i]);
			}

			delete this->base_map;
			delete this->food_map;
		}
};

#endif