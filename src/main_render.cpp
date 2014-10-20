/******************************************
 * COPYRIGHT (C) 2014 MADS HAUPT CLAUSEN
 ******************************************/

#include <iostream>
#include <stdlib.h>

#include <SDL2/SDL.h>

#include "constants.h"
#include "render.h"
#include "timer.h"
#include "keyboard.h"

#include "math/vec4.hpp"
#include "math/mat2.hpp"

SDL_Event sdl_event;

Timer fps_timer;
Timer cap_timer;
int counted_frames = 0;

void key_handler(io::IO_KEYEVENT ev, char key)
{

}

int main(int argc, char **argv)
{
	std::cout << "Starting SNAKE 2.0" << std::endl;

	graphics::set_window_title(WINDOW_TITLE);
	graphics::set_window_size(WINDOW_WIDTH, WINDOW_HEIGHT);
	graphics::init_sdl();
	graphics::init_gl();

	io::init();
	io::add_keyhandler(key_handler);

	fps_timer.start();

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
	       	graphics::clear();

	       	graphics::draw::square(math::vec2f(100.0f, 100.0f), 28.0f, math::vec4f(1.0f, 0.0f, 0.0f, 1.0f));

	    	graphics::render();
	    }

    	++counted_frames;
    	
    	int frame_ticks = cap_timer.get_ticks();
	    
	    if(frame_ticks < TICKS_PER_FRAME)
	    {
	        SDL_Delay(TICKS_PER_FRAME - frame_ticks);
	    }
    }

    SDL_Delay(1000);

    graphics::destroy();
    io::destroy();

	return 0;
}