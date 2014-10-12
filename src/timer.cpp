/******************************************
 * COPYRIGHT (C) 2014 MADS HAUPT CLAUSEN
 ******************************************/

#include "timer.h"

Timer::Timer()
{
    //Initialize the variables
    start_ticks = 0;
    paused_ticks = 0;

    paused = false;
    started = false;
}

void Timer::start()
{
    //Start the timer
    started = true;

    //Unpause the timer
    paused = false;

    //Get the current clock time
    start_ticks = SDL_GetTicks();
	paused_ticks = 0;
}

void Timer::stop()
{
    //Stop the timer
    started = false;

    //Unpause the timer
    paused = false;

	//Clear tick variables
	start_ticks = 0;
	paused_ticks = 0;
}

void Timer::pause()
{
    //If the timer is running and isn't already paused
    if( started && !paused )
    {
        //Pause the timer
        paused = true;

        //Calculate the paused ticks
        paused_ticks = SDL_GetTicks() - start_ticks;
		start_ticks = 0;
    }
}

void Timer::unpause()
{
    //If the timer is running and paused
    if( started && paused )
    {
        //Unpause the timer
        paused = false;

        //Reset the starting ticks
        start_ticks = SDL_GetTicks() - paused_ticks;

        //Reset the paused ticks
        paused_ticks = 0;
    }
}

Uint32 Timer::get_ticks()
{
	Uint32 time = 0;

    if(started)
    {
        if(paused)
        {
            time = paused_ticks;
        }
        else
        {
            time = SDL_GetTicks() - start_ticks;
        }
    }

    return time;
}

bool Timer::is_started()
{
	//Timer is running and paused or unpaused
    return started;
}

bool Timer::is_paused()
{
	//Timer is running and paused
    return paused && started;
}