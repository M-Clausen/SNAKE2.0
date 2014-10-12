/******************************************
 * COPYRIGHT (C) 2014 MADS HAUPT CLAUSEN
 ******************************************/

#ifndef __POLYGON_H_INCLUDED__
#define __POLYGON_H_INCLUDED__

#include <vector>

#include "math/vec2.hpp"

namespace video
{
	class Polygon
	{
		private:
			std::vector<math::vec2f> vertices;

		public:
			Polygon() {};

			void add_vertex(math::vec2f vert)
			{
				this->vertices.push_back(vert);
			}

			void add_vertex(float x, float y)
			{
				this->vertices.push_back(math::vec2f(x, y));
			}

			math::vec2f size()
			{
				/* find lowest and highest x and y */
				float low_x = 0, low_y = 0;
				float high_x = 0, high_y = 0;
				for(int i = 0; i < this->vertices.size(); ++i)
				{
					if(i == 0) { low_x = this->vertices[i].x; low_y = this->vertices[i].y; }

					if(this->vertices[i].x < low_x) low_x = this->vertices[i].x;
					if(this->vertices[i].x > high_x) high_x = this->vertices[i].x;

					if(this->vertices[i].y < low_y) low_y = this->vertices[i].y;
					if(this->vertices[i].y > high_y) high_y = this->vertices[i].y;
				}

				return math::vec2f(high_x - low_x, high_y - low_y);
			}

			math::vec2f pos()
			{
				/* find lowest x and y */
				float low_x = 0, low_y = 0;
				for(int i = 0; i < this->vertices.size(); ++i)
				{
					if(i == 0) { low_x = this->vertices[i].x; low_y = this->vertices[i].y; }

					if(this->vertices[i].x < low_x) low_x = this->vertices[i].x;

					if(this->vertices[i].y < low_y) low_y = this->vertices[i].y;
				}

				return math::vec2f(low_x, low_y);
			}

			std::vector<math::vec2f> &get_vertices()
			{
				return this->vertices;
			}

			int num_vertices()
			{
				return this->vertices.size();
			}
	};
}

#endif