#include "AABB.h"
#include <iostream>

using namespace std;

vec2 AABB::getPosition()
{
	return transform.getPosition();
}

vec2 AABB::getRadii()
{
	return radii;
}


