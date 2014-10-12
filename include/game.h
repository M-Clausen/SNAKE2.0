/******************************************
 * COPYRIGHT (C) 2014 MADS HAUPT CLAUSEN
 ******************************************/

#ifndef __GAME_H_INCLUDED__
#define __GAME_H_INCLUDED__

#include <vector>

#include "constants.h"
#include "map.h"
#include "snake.h"
#include "timer.h"
#include "keyboard.h"
#include "events.h"

#include "math/mat2.hpp"
#include "math/vec4.hpp"

class Game
{
	private:
		Timer snake_timer;
		int snake_speed; /* 0 - 100 */
		char playing, snake_loop;
		Map *current_map;
		Snake *current_snake;
		std::vector<Map *> maps;
		std::vector<Snake *> snakes;
		events::Game_Event event;

	public:
		Game() : playing(1), snake_speed(70), snake_loop(1) {};

		void init()
		{
			this->snake_timer.start();
		}

		void add_map(Map *map, char cur)
		{
			maps.push_back(map);
			map->game_list_index = maps.size() - 1;

			if(cur)
				current_map = maps[maps.size() - 1];
		}

		void add_snake(Snake *snake, char cur)
		{
			snake->set_map(current_map);
			snakes.push_back(snake);

			if(cur)
				current_snake = snakes[snakes.size() - 1];
		}

		void key_handler(io::IO_KEYEVENT ev, char key);

		bool update();
		void render();
};

#endif