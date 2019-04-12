#include "Transform.h"

Transform::Transform()
{
	transformMatrix = mat3(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f);
}						//   x     y     z

void Transform::Translate(vec2& translation)
{
	mat3 translationMatrix = mat3(1.0f, 0.0f, 0,
		0.0f, 1.0f, 0,
		translation.x, translation.y, 1);

	transformMatrix = translationMatrix * transformMatrix;
}

void Transform::Scale(vec2& scaling)
{
	mat3 scalingMatrix = mat3x2(scaling.x, 0.0f, 0.0f,
		0.0f, scaling.y, 0.0f);

	transformMatrix = scalingMatrix * transformMatrix;
}

void Transform::Rotate(float angle)
{
	mat3 rotateMatrix = mat3x2(cos(angle), sin(angle), 0.0f,
		-sin(angle), cos(angle), 0.0f);

	transformMatrix = rotateMatrix * transformMatrix;
}

vec2 Transform::getPosition()
{
	return column(transformMatrix, 2);
}

vec2 Transform::getScale()
{
	float sx = length(column(transformMatrix, 0));  // local x axis length
	float sy = length(column(transformMatrix, 1));	// local y azis length
	return vec2(sx, sy);
}

vec2 Transform::getOrientation()
{
	vec3 col0 = normalize(column(transformMatrix, 0));
	return vec2(col0.x, col0.y);
}

vec2 Transform::getUpDir()
{
	vec3 col1 = column(transformMatrix, 1);
	return vec2(col1.x, col1.y);					// y Axis
}

vec2 Transform::getLeftDir()
{
	vec3 col0 = column(transformMatrix, 0);
	return vec2(col0.x, col0.y);					// x Axis
}

mat3 Transform::localToWorldMatrix()
{
	return transformMatrix;							// transforms local coordinates into world coordinates (if we are not using any hierarchical data structure)
}

mat3 Transform::worldToLocalMatrix()
{
	return inverse(transformMatrix);
}
