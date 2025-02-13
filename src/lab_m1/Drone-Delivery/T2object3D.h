#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace T2object3D
{
	Mesh* CreateCube(const std::string& name, glm::vec3 originCorner, float height, glm::vec3 color);
	Mesh* CreatePlane(const std::string& name, glm::vec3 originCorner, float length, glm::vec3 color);
	Mesh* CreateCilinder(const std::string& name, glm::vec3 originCorner, float height, float radius, glm::vec3 color);
	Mesh* CreateCone(const std::string& name, glm::vec3 originCorner, float height, float radius, glm::vec3 color);
	Mesh* CreateParallelepiped(const std::string& name, glm::vec3 center, float length, float width, float height, glm::vec3 color);
	Mesh* CreateSphere(const std::string& name, glm::vec3 center, float radius, glm::vec3 color);
	Mesh* CreateArrow(const std::string& name, glm::vec3 center, float size, glm::vec3 color);
}
