/******************************************
 * COPYRIGHT (C) 2014 MADS HAUPT CLAUSEN
 ******************************************/

#ifndef __KEYBOARD_H_INCLUDED__
#define __KEYBOARD_H_INCLUDED__

#include <iostream>
#include <string>
#include <algorithm>

#include <SDL2/SDL.h>

namespace io
{
	enum IO_KEY
	{
		IO_KEY_BACKSPACE = 	8,
		IO_KEY_ESCAPE 	 =	27,
		IO_KEY_SPACE 	 =	' ',
		IO_KEY_TAB 		 = 	'\t',
		IO_KEY_RETURN 	 = 	'\n',
		IO_KEY_SHIFT 	 =	15,
		IO_KEY_NULL 	 =	0,
		IO_KEY_UP 		 =	17,
		IO_KEY_RIGHT 	 =	18,
		IO_KEY_DOWN 	 =	19,
		IO_KEY_LEFT 	 =	20
	};

	enum IO_KEYEVENT
	{
		IO_KEYEVENT_UP = 0,
		IO_KEYEVENT_DOWN = 1
	};

	void init();
	void destroy();

	void keyboard_update(SDL_Event &event);
	char keystate(char key);
	void add_keyhandler(void (*handler) (IO_KEYEVENT, char));
};

#endif