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
			this->base_map = new char[this->width * this->height];
			memset(this->base_map, 0, this->width * this->height);

			this->food_map = new char[this->width * this->height];
			memset(this->food_map, 0, this->width * this->height);

			this->food_color = math::vec4f(1.0f, 1.0f, 1.0f, 1.0f);
			continue;
		}

		for(int x = 0; x < splitted.size(); ++x)
		{
			this->base_map[x + i * this->width] = (char) (splitted[x][0] - '0'); /* convert to number */
		}
	}

	file.close();
}

void Map::render_base()
{
	/* 255 227 3 */
	math::vec4f tile_color(26.0f / 255.0f, 60.0f / 255.0f, 87.0f / 255.0f, 1.0f);

	/* draw tiles */
	for(int x = 0; x < this->width; ++x)
	{
		for(int y = 0; y < this->height; ++y)
		{
			if(this->get_base_tile(x, y) == 1)
			{
				math::mat2f tile(x * MAP_TILE_SIZE, y * MAP_TILE_SIZE, MAP_TILE_SIZE, MAP_TILE_SIZE);
				video::draw::rectangle(tile, tile_color);
			}
		}
	}
}

void Map::render_food()
{
	/* draw tiles */
	for(int x = 0; x < this->width; ++x)
	{
		for(int y = 0; y < this->height; ++y)
		{
			if(this->get_food_tile(x, y) == 1)
			{
				math::mat2f tile(x * MAP_TILE_SIZE, y * MAP_TILE_SIZE, MAP_TILE_SIZE, MAP_TILE_SIZE);
				video::draw::rectangle(tile, this->food_color);
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
			video::draw::rectangle(tile, this->portals[i]->color);
		}
	}
}

void Map::render_grid()
{
	math::vec4f color(0.2f, 0.2f, 0.2f, 1.0f);

	/* draw grid */
	for(int x = 0; x < this->width; ++x)
	{
		video::draw::line(math::vec2f(x * MAP_TILE_SIZE, 0.0f), math::vec2f(x * MAP_TILE_SIZE, WINDOW_HEIGHT), color);
	}

	for(int y = 0; y < this->height; ++y)
	{
		video::draw::line(math::vec2f(0, y * MAP_TILE_SIZE), math::vec2f(WINDOW_WIDTH, y * MAP_TILE_SIZE), color);
	}
}