/******************************************
 * COPYRIGHT (C) 2014 MADS HAUPT CLAUSEN
 ******************************************/

#include "keyboard.h"

char *keystates;

std::vector<void (*)(io::IO_KEYEVENT, char)> key_handlers;

namespace io
{
	void init()
	{
		keystates = new char[128]; /* ascii and shit */
		memset(keystates, 0, 128);
	}

	void destroy()
	{
		delete keystates;
	}

	void keyboard_update(SDL_Event &event)
	{
		if(event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
		{
			int keysym = event.key.keysym.sym;
			std::string key_str(SDL_GetKeyName(keysym));
			std::transform(key_str.begin(), key_str.end(), key_str.begin(), ::tolower);

			if(keysym == SDLK_BACKSPACE) 	key_str[0] = IO_KEY_BACKSPACE;
			else if(keysym == SDLK_ESCAPE)	key_str[0] = IO_KEY_ESCAPE;
			else if(keysym == SDLK_LSHIFT 
				|| keysym == SDLK_RSHIFT)	key_str[0] = IO_KEY_SHIFT;
			else if(keysym == SDLK_SPACE) 	key_str[0] = IO_KEY_SPACE;
			else if(keysym == SDLK_RETURN) 	key_str[0] = IO_KEY_RETURN;
			else if(keysym == SDLK_TAB) 	key_str[0] = IO_KEY_TAB;
			else if(keysym == SDLK_UP) 		key_str[0] = IO_KEY_UP;
			else if(keysym == SDLK_RIGHT) 	key_str[0] = IO_KEY_RIGHT;
			else if(keysym == SDLK_DOWN) 	key_str[0] = IO_KEY_DOWN;
			else if(keysym == SDLK_LEFT) 	key_str[0] = IO_KEY_LEFT;
			else if(key_str.length() > 1) 	key_str[0] = IO_KEY_NULL;

			char key = key_str[0];

			if(event.type == SDL_KEYDOWN) 	keystates[key] = 1;
			else 							keystates[key] = 0;

			for(int i = 0; i < key_handlers.size(); ++i)
			{
				(key_handlers[i])((event.type == SDL_KEYDOWN) ? IO_KEYEVENT_DOWN : IO_KEYEVENT_UP, key);
			}
		}
	}

	char keystate(char key)
	{
		if(key > 127) return IO_KEY_NULL;
		return keystates[key];
	}

	void add_keyhandler(void (*handler) (IO_KEYEVENT, char))
	{
		key_handlers.push_back(handler);
	}
};