#pragma once

#include "utils/glm_utils.h"


namespace transform3D
{
	// Translate matrix
	inline glm::mat4 Translate(float translateX, float translateY, float translateZ)
	{
		return glm::transpose(glm::mat4(
			1.0f, 0.0f, 0.0f, translateX,
			0.0f, 1.0f, 0.0f, translateY,
			0.0f, 0.0f, 1.0f, translateZ,
			0.0f, 0.0f, 0.0f, 1.0f));

	}

	// Scale matrix
	inline glm::mat4 Scale(float scaleX, float scaleY, float scaleZ)
	{
		return glm::transpose(glm::mat4(
			scaleX, 0.0f, 0.0f, 0.0f,
			0.0f, scaleY, 0.0f, 0.0f,
			0.0f, 0.0f, scaleZ, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f));

	}

	// Rotate matrix relative to the OZ axis
	inline glm::mat4 RotateOZ(float radians)
	{
		return glm::transpose(glm::mat4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, cos(radians), -sin(radians), 0.0f,
			0.0f, sin(radians), cos(radians), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f));

	}

	// Rotate matrix relative to the OY axis
	inline glm::mat4 RotateOY(float radians)
	{
		return glm::transpose(glm::mat4(
			cos(radians), 0.0f, sin(radians), 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			-sin(radians), 0.0f, cos(radians), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f));

	}

	// Rotate matrix relative to the OX axis
	inline glm::mat4 RotateOX(float radians)
	{
		return glm::transpose(glm::mat4(
			cos(radians), -sin(radians), 0.0f, 0.0f,
			sin(radians), cos(radians), 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f));

	}
}   // namespace transform3D
