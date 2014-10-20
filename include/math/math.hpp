/******************************************
 * COPYRIGHT (C) 2014 MADS HAUPT CLAUSEN
 ******************************************/

#ifndef __MATH_HPP_INCLUDED__
#define __MATH_HPP_INCLUDED__

#include "math/vec2.hpp"
#include "math/vec3.hpp"
#include "math/vec4.hpp"
#include "math/mat2.hpp"
#include "math/mat3.hpp"
#include "math/mat4.hpp"

namespace math
{
	char line_intersects_line(math::vec2f l1p1, math::vec2f l1p2, math::vec2f l2p1, math::vec2f l2p2);
	char rect_contains(math::mat2f rect, math::vec2f p);
	char line_intersects_rect(math::vec2f p1, math::vec2f p2, math::mat2f rect);
};

#endif