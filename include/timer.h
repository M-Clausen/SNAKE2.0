/******************************************
 * COPYRIGHT (C) 2014 MADS HAUPT CLAUSEN
 ******************************************/

#ifndef __TIMER_H_INCLUDED__
 #define __TIMER_H_INCLUDED__
#include <SDL2/SDL.h>

//The application time based timer
class Timer
{
    public:
		//Initializes variables
		Timer();

		//The various clock actions
		void start();
		void stop();
		void pause();
		void unpause();

		//Gets the timer's time
		Uint32 get_ticks();

		//Checks the status of the timer
		bool is_started();
		bool is_paused();

    private:
		//The clock time when the timer started
		Uint32 start_ticks;

		//The ticks stored when the timer was paused
		Uint32 paused_ticks;

		//The timer status
		bool paused;
		bool started;
};

#endif