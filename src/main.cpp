/******************************************
 * COPYRIGHT (C) 2014 MADS HAUPT CLAUSEN
 ******************************************/

#include <iostream>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "constants.h"
#include "draw.h"
#include "polygon.h"
#include "game.h"
#include "map.h"
#include "keyboard.h"
#include "timer.h"
#include "snake.h"

#include "math/vec4.hpp"
#include "math/mat2.hpp"

SDL_Event sdl_event;

Game game;


Timer fps_timer;
Timer cap_timer;
int counted_frames = 0;

Snake snake(2, 2);
Map map("levels/1/maps/1.map");
Map map2("levels/1/maps/2.map");

Portal portal1;
Portal portal2;

void key_handler(io::IO_KEYEVENT ev, char key)
{
	game.key_handler(ev, key);
}

int main(int argc, char **argv)
{
	std::cout << "Starting SNAKE 2.0" << std::endl;

	video::set_window_title(WINDOW_TITLE);
	video::set_window_size(WINDOW_WIDTH, WINDOW_HEIGHT);
	video::init_sdl();
	video::init_gl();

	io::init();
	io::add_keyhandler(key_handler);

	fps_timer.start();

	while(!map.set_food_tile(rand() % (map.get_width() - 1), rand() % (map.get_height() - 1), 1)) {};
	game.add_map(&map, 1);
	game.add_map(&map2, 0);

	portal1.mapx = 13;
	portal1.mapy = 5;

	portal2.mapx = 32;
	portal2.mapy = 17;

	portal1.parent_map = &map;
	portal2.parent_map = &map2;


	printf("&map2: %p\n", &map2);
	map.add_portals(portal1, portal2, 0);
	map2.add_portals(portal1, portal2, 0);

	snake.add_element();
	game.add_snake(&snake, 1);
	game.init();

	bool quit = false;
	while(!quit)
    {
    	cap_timer.start();

        while(SDL_PollEvent(&sdl_event) != 0)
        {
        	io::keyboard_update(sdl_event);

      		if(sdl_event.type == SDL_QUIT)
       		{
				quit = true;
       		}
       	}

       	int fps_ticks = fps_timer.get_ticks();
       	float avg_fps = counted_frames / (fps_ticks / 1000.f);
	    if(avg_fps > 2000000) avg_fps = 0;

	    if(!quit)
	    {
	       	video::clear();

	       	quit = !game.update();
	       	// std::cout << "updated!" << std::endl;
	     	game.render();
	     	// std::cout << "rendered" << std::endl;

	    	video::render();
	    	// std::cout << "flipped" << std::endl;
	    }

    	++counted_frames;
    	
    	int frame_ticks = cap_timer.get_ticks();
	    
	    if(frame_ticks < TICKS_PER_FRAME)
	    {
	        SDL_Delay(TICKS_PER_FRAME - frame_ticks);
	    }
    }

    SDL_Delay(1000);

    map.destroy();

    video::destroy();
    io::destroy();

	return 0;
}