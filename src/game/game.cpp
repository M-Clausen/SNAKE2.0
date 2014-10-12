/******************************************
 * COPYRIGHT (C) 2014 MADS HAUPT CLAUSEN
 ******************************************/

#include "game.h"

void Game::render()
{
	math::vec4f bg_color(66.0f / 255.0f, 100.0f / 255.0f, 127.0f / 255.0f, 1.0f);
	math::mat2f background(0.0f, 0.0f, WINDOW_WIDTH, WINDOW_HEIGHT);
	video::draw::rectangle(background, bg_color);
	this->current_map->render_base();
	this->current_map->render_food();

	this->current_snake->render();

	this->current_map->render_portals();
	
	this->current_map->render_grid();
}

bool Game::update()
{
    if(this->snake_timer.get_ticks() > 1000 - (this->snake_speed * 2 + 800) + 1 + 30)
    {
    	this->current_snake->move();
    	this->snake_timer.start();
    }
    // std::cout << "snake_timer!" << std::endl;

    int i = 1;
    while(events::poll(&(this->event)))
    {
    	// std::cout << "poll #" << i++ << std::endl;

    	if(this->event.type == events::EVENT_TYPE_SNAKE_HIT_WALL)
    	{
    		/* loop around */
    		if(this->snake_loop)
    		{
    			int wall_x = event.dat1_i;
    			int wall_y = event.dat2_i;
    			switch(this->current_snake->get_direction())
    			{
    				case SNAKE_DIRECTION_UP:
    					if(wall_y < 0)
    					{
    						this->current_snake->set_next_pos(wall_x, this->current_map->get_height() - 1, this->current_map->game_list_index);
    						return true;
    					}
    					break;

    				case SNAKE_DIRECTION_RIGHT:
    					if(wall_x >= this->current_map->get_width())
    					{
    						this->current_snake->set_next_pos(0, wall_y, this->current_map->game_list_index);
    						return true;
    					}
    					break;

    				case SNAKE_DIRECTION_DOWN:
    					if(wall_y >= this->current_map->get_height())
    					{
    						this->current_snake->set_next_pos(wall_x, 0, this->current_map->game_list_index);
    						return true;
    					}
    					break;

    				case SNAKE_DIRECTION_LEFT:
    					if(wall_x < 0)
    					{
    						this->current_snake->set_next_pos(this->current_map->get_width() - 1, wall_y, this->current_map->game_list_index);
    						return true;
    					}
    					break;

    				default:
    					break;
    			}
    		}

    		std::cout << "You hit a wall!" << std::endl;
    		return false;
    	}

    	if(this->event.type == events::EVENT_TYPE_SNAKE_HIT_SELF)
    	{
    		std::cout << "You hit yourself!" << std::endl;
    		return false;
    	}

    	if(this->event.type == events::EVENT_TYPE_SNAKE_HIT_FOOD)
    	{
    		std::cout << "You ate a fruit!" << std::endl;
    		this->current_map->set_food_tile(this->event.dat1_i, this->event.dat2_i, 0); /* destroy the fruit */
    		while(!this->current_map->set_food_tile(rand() % (this->current_map->get_width() - 1), rand() % (this->current_map->get_height() - 1), 1)) {};
    		this->current_snake->add_element();
    	}

    	if(this->event.type == events::EVENT_TYPE_SNAKE_HIT_PORTAL)
    	{
    		Portal portal = *((Portal *) event.dat1_u);

    		printf("portal partner map: %p\n", portal.partner->parent_map);
    		if(portal.parent_map != portal.partner->parent_map)
    		{
    			this->current_map = this->maps[portal.partner->parent_map->game_list_index];
    			printf("current_map: %p\n", this->current_map);
    			this->current_snake->set_map(this->current_map);
    		}
    		
    		switch(this->current_snake->get_direction())
			{
				case SNAKE_DIRECTION_UP:
					this->current_snake->set_next_pos(portal.partner->mapx, portal.partner->mapy - 1, portal.partner->parent_map->game_list_index);
					break;

				case SNAKE_DIRECTION_RIGHT:
					this->current_snake->set_next_pos(portal.partner->mapx + 1, portal.partner->mapy, portal.partner->parent_map->game_list_index);
					break;

				case SNAKE_DIRECTION_DOWN:
					this->current_snake->set_next_pos(portal.partner->mapx, portal.partner->mapy + 1, portal.partner->parent_map->game_list_index);
					break;

				case SNAKE_DIRECTION_LEFT:
					this->current_snake->set_next_pos(portal.partner->mapx - 1, portal.partner->mapy, portal.partner->parent_map->game_list_index);
					break;

				default:
					break;
			}

			this->current_snake->can_change_direction = 0;
			this->current_snake->can_teleport = 1;
    	}
    }
    // std::cout << "done update" << std::endl;

    return true;
}

void Game::key_handler(io::IO_KEYEVENT ev, char key)
{
	if(ev == io::IO_KEYEVENT_DOWN)
	{
		switch(key)
		{
			case io::IO_KEY_UP:
				this->current_snake->set_direction(SNAKE_DIRECTION_UP);
				break;
			case io::IO_KEY_RIGHT:
				this->current_snake->set_direction(SNAKE_DIRECTION_RIGHT);
				break;
			case io::IO_KEY_DOWN:
				this->current_snake->set_direction(SNAKE_DIRECTION_DOWN);
				break;
			case io::IO_KEY_LEFT:
				this->current_snake->set_direction(SNAKE_DIRECTION_LEFT);
				break;
			case io::IO_KEY_SPACE:
				this->snake_speed = 80;
			default:
				break;
		}
	}

	if(ev == io::IO_KEYEVENT_UP)
	{
		switch(key)
		{
			case io::IO_KEY_SPACE:
				this->snake_speed = 70;
			default:
				break;
		}
	}
}