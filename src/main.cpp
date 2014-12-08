/******************************************
 * COPYRIGHT (C) 2014 MADS HAUPT CLAUSEN
 ******************************************/

#include <iostream>
#include <stdlib.h>
#include <time.h>

#include <SDL2/SDL.h>

#include "constants.h"
#include "render.h"
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

Timer fps_timer, cap_timer, light_timer;
int counted_frames = 0;

Snake snake(2, 2);

Portal portal1;
Portal portal2;

void key_handler(io::IO_KEYEVENT ev, char key)
{
	game.key_handler(ev, key);
}

int main(int argc, char **argv)
{
	std::cout << "Starting SNAKE 2.0" << std::endl;

	srand (time(NULL));

	graphics::init_shadowmap_rotation_matrices();
	graphics::set_window_title(WINDOW_TITLE);
	graphics::set_window_size(WINDOW_WIDTH, WINDOW_HEIGHT);
	graphics::init_sdl();
	if(graphics::init_gl() == -1)
	{
		graphics::destroy();
    	return -1;
	}

	graphics::Light light, light2;
	light.position = math::vec3f(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, -0.2f);
	light2.position = math::vec3f(WINDOW_WIDTH / 2 + 200, WINDOW_HEIGHT / 2 - 200, -0.2f);
	light.color = math::vec3f(0.0f, 1.0f, 1.0f);
	light2.color = math::vec3f(1.0f, 0.2f, 0.5f);
	light.range = 3;
	light2.range = 3;

	graphics::add_light(&light, 1);
	graphics::add_light(&light2, 1);

	io::init();
	io::add_keyhandler(key_handler);

	fps_timer.start();

	Map map("levels/1/maps/1.map");
	Map map2("levels/1/maps/2.map");

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
	//map.add_portals(portal1, portal2, 0);
	//map2.add_portals(portal1, portal2, 0);

	snake.add_element();
	game.add_snake(&snake, 1);
	game.init();

	light_timer.start();

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
	    // std::cout << "fps: " << avg_fps << std::endl;


	    if(!quit)
	    {
	       	graphics::clear();

	       	quit = !game.update();
	       	// std::cout << "updated!" << std::endl;
	     	game.render();
	     	// std::cout << "rendered" << std::endl;

	    	graphics::render();
	    	// std::cout << "flipped" << std::endl;
	    }

    	++counted_frames;
    	
    	int frame_ticks = cap_timer.get_ticks();
	    
	    if(frame_ticks < TICKS_PER_FRAME)
	    {
	        SDL_Delay(TICKS_PER_FRAME - frame_ticks);
	    }

	    if(light_timer.get_ticks() > 1000)
	    {
	    	float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	    	float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	    	float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	    	light.color = math::vec3f(r, g, b);
	    	light_timer.start();
	    }
    }

    SDL_Delay(1000);

    map.destroy();

    graphics::destroy();
    io::destroy();

	return 0;
}