/******************************************
 * COPYRIGHT (C) 2014 MADS HAUPT CLAUSEN
 ******************************************/

#ifndef __EVENTS_H_INCLUDED__
#define __EVENTS_H_INCLUDED__

#include <vector>
#include <string>

namespace events
{
	enum EVENT_TYPE
	{
		EVENT_TYPE_SNAKE_HIT_WALL,
		EVENT_TYPE_SNAKE_HIT_SELF,
		EVENT_TYPE_SNAKE_HIT_FOOD,
		EVENT_TYPE_SNAKE_HIT_PORTAL
	};

	struct Game_Event
	{
		EVENT_TYPE type;
		int dat1_i, dat2_i, dat3_i, dat4_i;
		uint64_t dat1_u;
		float dat1_f, dat2_f, dat3_f, dat4_f;
		std::string dat1_str;
		char dat1_c, dat2_c;
	};

	void fire(Game_Event ev);
	char poll(Game_Event *ev);
};

#endif