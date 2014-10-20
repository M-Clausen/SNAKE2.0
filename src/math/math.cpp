/******************************************
 * COPyRIGHT (C) 2014 MADS HAUPT CLAUSEN
 ******************************************/

#include "math/math.hpp"

namespace math
{
	char line_intersects_line(math::vec2f l1p1, math::vec2f l1p2, math::vec2f l2p1, math::vec2f l2p2)
	{
		float q = (l1p1.y - l2p1.y) * (l2p2.x - l2p1.x) - (l1p1.x - l2p1.x) * (l2p2.y - l2p1.y);
        float d = (l1p2.x - l1p1.x) * (l2p2.y - l2p1.y) - (l1p2.y - l1p1.y) * (l2p2.x - l2p1.x);

        if( d == 0 )
        {
            return 0;
        }

        float r = q / d;

        q = (l1p1.y - l2p1.y) * (l1p2.x - l1p1.x) - (l1p1.x - l2p1.x) * (l1p2.y - l1p1.y);
        float s = q / d;

        if( r < 0 || r > 1 || s < 0 || s > 1 )
        {
            return 0;
        }

        return 1;
	}

	char rect_contains(math::mat2f rect, math::vec2f p)
	{
		float rx = rect[0];
		float ry = rect[3];
		float rw = rect[1];
		float rh = rect[4];

		return (p.x >= rx && p.x <= rx + rw && p.y >= ry && p.y <= ry + rh) ? 1 : 0;
	}

	char line_intersects_rect(math::vec2f p1, math::vec2f p2, math::mat2f rect)
	{
		float rx = rect[0];
		float ry = rect[3];
		float rw = rect[1];
		float rh = rect[4];

		bool intersect = 
				line_intersects_line(p1, p2, math::vec2f(rx, ry), 			math::vec2f(rx + rw, ry + rh)) ||
               	line_intersects_line(p1, p2, math::vec2f(rx + rw, ry), 		math::vec2f(rx + rw, ry + rh)) ||
               	line_intersects_line(p1, p2, math::vec2f(rx + rw, ry + rh), math::vec2f(rx, ry + rh)) ||
               	line_intersects_line(p1, p2, math::vec2f(rx, ry + rh), 		math::vec2f(rx, ry)) ||
               	(rect_contains(rect, p1) && rect_contains(rect, p2));

        return intersect ? 1 : 0;
	}
};