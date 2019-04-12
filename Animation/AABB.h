#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp> 
#include "Transform.h"

using namespace std;
using namespace glm;

class AABB
{
private:
	vec2 radii;
	Transform transform;
	vec2 direction;
public:

	AABB(vec2 radii, vec2& pos)
		: radii(radii)
	{
		direction = vec2(0, 1);
	}

	vec2 getPosition();
	vec2 getRadii();
};