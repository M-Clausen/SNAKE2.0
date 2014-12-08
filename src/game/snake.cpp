/******************************************
 * COPYRIGHT (C) 2014 MADS HAUPT CLAUSEN
 ******************************************/

#include "snake.h"

void Snake::move()
{
	this->can_change_direction = 1;
	events::Game_Event ev;
	
	char hit = 0;
	for(Snake_Elem *elem = this->head.last; elem->next != 0; elem = elem->next)
	{
		if(this->next.mapx == elem->mapx && this->next.mapy == elem->mapy && this->next.mapz == elem->mapz)
		{
			hit = 1;
			ev.type = events::EVENT_TYPE_SNAKE_HIT_SELF;
			events::fire(ev);
		}
	}


	Map_Tile *pointer = this->parent_map->get_base_tile(this->next.mapx, this->next.mapy);
	// printf("checking collision, wall pointer = %lx\n", (uint64_t) pointer);

	// printf("checking if wall is null\n");
	if((uint64_t) pointer != 0)
	{
		// printf("wat\n");
		if(pointer->solid != 0)
		{
			hit = 1;
			ev.dat1_i = this->next.mapx;
			ev.dat2_i = this->next.mapy;
			ev.dat3_i = this->next.mapz;
			ev.type = events::EVENT_TYPE_SNAKE_HIT_WALL;
			events::fire(ev);
		}
	}

	if(hit == 0)
	{
		int i = 1;
		for(Snake_Elem *elem = this->head.last; elem->next != 0; elem = elem->next)
		{
			elem->visible = 1;
			elem->mapx = elem->next->mapx;
			elem->mapy = elem->next->mapy;
			elem->mapz = elem->next->mapz;
			elem->direction = elem->next->direction;
		}

		this->head.mapx = this->next.mapx;
		this->head.mapy = this->next.mapy;
		this->head.mapz = this->next.mapz;

		switch(this->direction)
		{
			case SNAKE_DIRECTION_UP:
				this->next.mapy -= 1;
				break;
			case SNAKE_DIRECTION_RIGHT:
				this->next.mapx += 1;
				break;
			case SNAKE_DIRECTION_DOWN:
				this->next.mapy += 1;
				break;
			case SNAKE_DIRECTION_LEFT:
				this->next.mapx -= 1;
				break;
		}


		/* check for portal collision */
		std::vector<Portal *> portals = this->parent_map->get_portals();
		for(int i = 0; i < portals.size(); ++i)
		{
			// printf("(%i, %i) vs. (%i, %i)\n", this->head.mapx, this->head.mapy, portals[i]->mapx, portals[i]->mapy);
			if(this->head.mapx == portals[i]->mapx && this->head.mapy == portals[i]->mapy
				&& this->head.mapz == portals[i]->parent_map->game_list_index)
			{
				if(this->can_teleport > 0)
				{
					ev.type = events::EVENT_TYPE_SNAKE_HIT_PORTAL;
					ev.dat1_u = (uint64_t) portals[i];
					events::fire(ev);
					this->can_teleport = 0;
					break;
				}
			}
		}

		if(this->parent_map->get_food_tile(this->head.mapx, this->head.mapy)->solid != 0 && hit == 0)
		{
			ev.dat1_i = this->head.mapx;
			ev.dat2_i = this->head.mapy;
			printf("hit food at (%i, %i)\n", ev.dat1_i, ev.dat2_i);
			ev.type = events::EVENT_TYPE_SNAKE_HIT_FOOD;
			events::fire(ev);
		}
	}
	else
	{
		this->next.mapx = this->head.mapx;
		this->next.mapy = this->head.mapy;
		switch(this->direction)
		{
			case SNAKE_DIRECTION_UP:
				this->next.mapy -= 1;
				break;
			case SNAKE_DIRECTION_RIGHT:
				this->next.mapx += 1;
				break;
			case SNAKE_DIRECTION_DOWN:
				this->next.mapy += 1;
				break;
			case SNAKE_DIRECTION_LEFT:
				this->next.mapx -= 1;
				break;
		}
	}
}

void Snake::add_element()
{
	Snake_Elem *elem = new Snake_Elem;
	elem->next = this->head.last;
	elem->prev = 0;
	elem->visible = 0;


	switch(this->direction)
	{
		case SNAKE_DIRECTION_UP:
			elem->mapx = elem->next->mapx;
			elem->mapy = elem->next->mapy + 1;
			break;
		case SNAKE_DIRECTION_RIGHT:
			elem->mapx = elem->next->mapx - 1;
			elem->mapy = elem->next->mapy;
			break;
		case SNAKE_DIRECTION_DOWN:
			elem->mapx = elem->next->mapx;
			elem->mapy = elem->next->mapy - 1;
			break;
		case SNAKE_DIRECTION_LEFT:
			elem->mapx = elem->next->mapx;
			elem->mapy = elem->next->mapy + 1;
			break;
	}

	this->head.last = elem;
}

void Snake::set_direction(char dir)
{
	if(this->can_change_direction == 1)
	{
		this->next.mapx = this->head.mapx;
		this->next.mapy = this->head.mapy;

		/* make sure the snake can't turn back into itself */
		switch(this->direction)
		{
			case SNAKE_DIRECTION_UP:
				if(dir != SNAKE_DIRECTION_DOWN) this->direction = dir;
				break;
			case SNAKE_DIRECTION_RIGHT:
				if(dir != SNAKE_DIRECTION_LEFT) this->direction = dir;
				break;
			case SNAKE_DIRECTION_DOWN:
				if(dir != SNAKE_DIRECTION_UP) this->direction = dir;
				break;
			case SNAKE_DIRECTION_LEFT:
				if(dir != SNAKE_DIRECTION_RIGHT) this->direction = dir;
				break;
		}

		switch(this->direction)
		{
			case SNAKE_DIRECTION_UP:
				this->next.mapy -= 1;
				break;
			case SNAKE_DIRECTION_RIGHT:
				this->next.mapx += 1;
				break;
			case SNAKE_DIRECTION_DOWN:
				this->next.mapy += 1;
				break;
			case SNAKE_DIRECTION_LEFT:
				this->next.mapx -= 1;
				break;
		}
		
		this->head.direction = this->direction;
		this->can_change_direction = 0;
	}
}

void Snake::render(char add)
{
	this->head.rect.rect = math::mat2f(this->head.mapx * MAP_TILE_SIZE, this->head.mapy * MAP_TILE_SIZE, MAP_TILE_SIZE, MAP_TILE_SIZE);
	this->head.rect.color = this->color;
	this->head.rect.height = 0.03f;
	this->head.rect.render_in_shadowmap = 0;
	if(add == 1) graphics::draw::add_rectangle(&this->head.rect);

	for(Snake_Elem *elem = this->head.last; elem->next != 0; elem = elem->next)
	{
		if(elem->visible == 1 && elem->mapz == this->head.mapz)
		{
			elem->rect.rect = math::mat2f(elem->mapx * MAP_TILE_SIZE, elem->mapy * MAP_TILE_SIZE, MAP_TILE_SIZE, MAP_TILE_SIZE);
			elem->rect.color = this->color;
			elem->rect.height = 0.03f;
			elem->rect.render_in_shadowmap = 0;
			if(add == 1) graphics::draw::add_rectangle(&elem->rect);

			if(elem != this->head.last) graphics::add_light_block(elem->mapx, elem->mapy, 2);
		}
	}

	graphics::add_light_block(this->head.mapx, this->head.mapy, 2);
}