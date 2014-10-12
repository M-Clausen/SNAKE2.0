/******************************************
 * COPYRIGHT (C) 2014 MADS HAUPT CLAUSEN
 ******************************************/

#include "events.h"

std::vector<events::Game_Event> queue;

namespace events
{
	void fire(Game_Event ev)
	{
		queue.push_back(ev);
	}

	char poll(Game_Event *ev)
	{
		if(queue.size() > 0)
		{
			*ev = queue[0];
			queue.erase(queue.begin());
			return 1;
		}

		return 0;
	}
};