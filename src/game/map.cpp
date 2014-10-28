/******************************************
 * COPYRIGHT (C) 2014 MADS HAUPT CLAUSEN
 ******************************************/

#include "map.h"
void split_string(const std::string &s, char delim, std::vector<std::string> &elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

Map::Map(std::string path)
{
	std::ifstream file(path.c_str());

	if(!file.is_open())
	{
		this->init();
		return;
	}

	int w = 0, h = 0, i = -1;
	for(std::string line; getline(file, line); ++i)
	{
		// std::cout << "LINE: " << line.c_str() << std::endl;
		std::vector<std::string> splitted;
		split_string(line, ' ', splitted);

		if(i == -1)
		{
			if(splitted.size() < 2)
			{
				this->init();
				break;
			}

			w = std::stoi(splitted[0], 0, 10);
			h = std::stoi(splitted[1], 0, 10);
			this->width = w;
			this->height = h;
			this->base_map = new Map_Tile[this->width * this->height];

			this->food_map = new Map_Tile[this->width * this->height];

			this->food_color = math::vec4f(1.0f, 1.0f, 1.0f, 1.0f);
			continue;
		}

		for(int x = 0; x < splitted.size(); ++x)
		{
			this->base_map[x + i * this->width].data = (char) (splitted[x][0] - '0'); /* convert to number */

			if(this->base_map[x + i * this->width].data != 0)
			{
				this->base_map[x + i * this->width].solid = this->base_map[x + i * this->width].blocks_light = 1;
			}
		}
	}

	this->basetile_outside.solid = 1;
	this->basetile_outside.data = 1;

	file.close();

	this->register_all_light_blocks();
}

void Map::register_all_light_blocks()
{
	for(int x = 0; x < this->width; ++x)
	{
		for(int y = 0; y < this->height; ++y)
		{
			if(this->base_map[x + this->width * y].data == 1 && this->base_map[x + this->width * y].registered_as_light_block == 0)
			{
				graphics::add_light_block_rect(&(this->base_map[x + this->width * y].rect.rect));
				this->base_map[x + this->width * y].registered_as_light_block = 1;
			}
		}
	}
}

void Map::render_base(char add)
{
	/* 255 227 3 */
	math::vec4f tile_color(26.0f / 255.0f, 60.0f / 255.0f, 87.0f / 255.0f, 1.0f);

	/* draw tiles */
	for(int x = 0; x < this->width; ++x)
	{
		for(int y = 0; y < this->height; ++y)
		{
			if(this->base_map[x + this->width * y].solid == 1)
			{
				this->base_map[x + this->width * y].rect.rect = math::mat2f(x * MAP_TILE_SIZE, y * MAP_TILE_SIZE, MAP_TILE_SIZE, MAP_TILE_SIZE);
				this->base_map[x + this->width * y].rect.color = tile_color;
				this->base_map[x + this->width * y].rect.height = 0.05f;
				if(add == 1 && this->base_map[x + this->width * y].solid) graphics::draw::add_rectangle(&this->base_map[x + this->width * y].rect);
			}
		}
	}
}

void Map::render_food(char add)
{
	/* draw tiles */
	for(int x = 0; x < this->width; ++x)
	{
		for(int y = 0; y < this->height; ++y)
		{
			if(this->food_map[x + this->width * y].data == 1)
			{
				this->food_map[x + this->width * y].rect.rect = math::mat2f(x * MAP_TILE_SIZE, y * MAP_TILE_SIZE, MAP_TILE_SIZE, MAP_TILE_SIZE);
				this->food_map[x + this->width * y].rect.color = this->food_color;
				this->food_map[x + this->width * y].rect.height = 0.05f;
				if(add == 1) graphics::draw::add_rectangle(&this->food_map[x + this->width * y].rect);
			}
		}
	}
}

void Map::render_portals()
{
	/* draw tiles */
	for(int i = 0; i < this->portals.size(); ++i)
	{
		if(this->portals[i]->render == 1)
		{
			math::mat2f tile(this->portals[i]->mapx * MAP_TILE_SIZE, this->portals[i]->mapy * MAP_TILE_SIZE, MAP_TILE_SIZE, MAP_TILE_SIZE);
			// graphics::draw::rectangle(tile, this->portals[i]->color, 0.05f);
		}
	}
}

void Map::render_grid()
{
	math::vec4f color(0.2f, 0.05f, 0.05f, 1.0f);

	for(int x = 0; x < this->width; ++x)
	{
		for(int y = 0; y < this->height; ++y)
		{
			graphics::draw::line(math::vec2f(x * MAP_TILE_SIZE, y * MAP_TILE_SIZE), math::vec2f(x * MAP_TILE_SIZE, y * MAP_TILE_SIZE + MAP_TILE_SIZE), color);
			graphics::draw::line(math::vec2f(x * MAP_TILE_SIZE, y * MAP_TILE_SIZE), math::vec2f(x * MAP_TILE_SIZE + MAP_TILE_SIZE, y * MAP_TILE_SIZE), color);
		}	
	}
}