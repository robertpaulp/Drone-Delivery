#include "T2object3D.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"
#include <iostream>

Mesh* T2object3D::CreateCube(const std::string& name, glm::vec3 originCorner, float height, glm::vec3 color)
{
	glm::vec3 corner = originCorner;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner + glm::vec3(0, 0, height), glm::vec3(0, 1, 1), color),
		VertexFormat(corner + glm::vec3(height, 0,  height), glm::vec3(0, 1, 1), color),
		VertexFormat(corner + glm::vec3(0, height, height), glm::vec3(0, 1, 1), color),
		VertexFormat(corner + glm::vec3(height, height, height), glm::vec3(0, 1, 1), color),
		VertexFormat(corner + glm::vec3(0, 0, 0), glm::vec3(0, 1, 1), color),
		VertexFormat(corner + glm::vec3(height, 0, 0), glm::vec3(0, 1, 1), color),
		VertexFormat(corner + glm::vec3(0, height, 0), glm::vec3(0, 1, 1), color),
		VertexFormat(corner + glm::vec3(height, height, 0), glm::vec3(0, 1, 1), color)
	};

	Mesh* square = new Mesh(name);


	std::vector<unsigned int> indices =
	{
		0, 1, 2,
		1, 3, 2,
		2, 3, 7,
		2, 7, 6,
		1, 7, 3,
		1, 5, 7,
		6, 7, 4,
		7, 5, 4,
		0, 4, 1,
		1, 4, 5,
		2, 6, 4,
		0, 2, 4
	};

	square->InitFromData(vertices, indices);
	return square;
}

Mesh* T2object3D::CreatePlane(const std::string& name, glm::vec3 originCorner, float length, glm::vec3 color)
{
	glm::vec3 corner = originCorner;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner + glm::vec3(0, 0, 0), glm::vec3(1, 0, 1), color),
		VertexFormat(corner + glm::vec3(length, 0, 0), glm::vec3(1, 0, 1), color),
		VertexFormat(corner + glm::vec3(0, 0, length), glm::vec3(1, 0, 1), color),
		VertexFormat(corner + glm::vec3(length, 0, length), glm::vec3(1, 0, 1), color)
	};

	Mesh* square = new Mesh(name);


	std::vector<unsigned int> indices =
	{
		0, 1, 2,
		1, 2, 3
	};

	square->InitFromData(vertices, indices);
	return square;
}

Mesh* T2object3D::CreateCilinder(const std::string& name, glm::vec3 originCorner, float height, float radius, glm::vec3 color)
{
	glm::vec3 corner = originCorner;

	std::vector<VertexFormat> vertices;
	std::vector<unsigned int> indices;

	int n = 100;
	float angle = 2 * M_PI / n;

	for (int i = 0; i < n; ++i) {
		float x = corner.x + radius * cos(i * angle);
		float z = corner.z + radius * sin(i * angle);
		vertices.emplace_back(glm::vec3(x, corner.y, z), glm::vec3(0, 1, 0), color);
		vertices.emplace_back(glm::vec3(x, corner.y + height, z), glm::vec3(0, 1, 0), color);
	}

	for (int i = 0; i < n; ++i) {
		indices.push_back(i * 2);
		indices.push_back(i * 2 + 1);
		indices.push_back((i * 2 + 2) % (2 * n));

		indices.push_back(i * 2 + 1);
		indices.push_back((i * 2 + 3) % (2 * n));
		indices.push_back((i * 2 + 2) % (2 * n));
	}

	Mesh* cilinder = new Mesh(name);
	cilinder->InitFromData(vertices, indices);
	return cilinder;
}

Mesh* T2object3D::CreateCone(const std::string& name, glm::vec3 originCorner, float height, float radius, glm::vec3 color)
{
	glm::vec3 corner = originCorner;

	std::vector<VertexFormat> vertices;
	std::vector<unsigned int> indices;

	int n = 100;
	float angle = 2 * M_PI / n;

	vertices.emplace_back(corner, glm::vec3(0, 1, 0), color);
	vertices.emplace_back(glm::vec3(corner.x, corner.y + height, corner.z), glm::vec3(0, 1, 0), color);

	for (int i = 0; i < n; ++i) {
		float x = corner.x + radius * cos(i * angle);
		float z = corner.z + radius * sin(i * angle);
		vertices.emplace_back(glm::vec3(x, corner.y, z), glm::vec3(0, 1, 0), color);
	}

	for (int i = 0; i < n; ++i) {
		indices.push_back(0);
		indices.push_back(i + 2);
		indices.push_back((i + 1) % n + 2);

		indices.push_back(1);
		indices.push_back((i + 1) % n + 2);
		indices.push_back(i + 2);
	}

	Mesh* cone = new Mesh(name);
	cone->InitFromData(vertices, indices);
	return cone;
}

Mesh* T2object3D::CreateParallelepiped(const std::string& name, glm::vec3 center, float length, float width, float height, glm::vec3 color)
{
	float halfLength = length / 2.0f;
	float halfWidth = width / 2.0f;
	float halfHeight = height / 2.0f;

	std::vector<VertexFormat> vertices =
	{
		// Bottom face
		VertexFormat(center + glm::vec3(-halfWidth, -halfLength, -halfHeight), glm::vec3(0, 1, 1), color),
		VertexFormat(center + glm::vec3(halfWidth, -halfLength, -halfHeight), glm::vec3(0, 1, 1), color),
		VertexFormat(center + glm::vec3(halfWidth, -halfLength, halfHeight), glm::vec3(0, 1, 1), color),
		VertexFormat(center + glm::vec3(-halfWidth, -halfLength, halfHeight), glm::vec3(0, 1, 1), color),

		// Top face
		VertexFormat(center + glm::vec3(-halfWidth, halfLength, -halfHeight), glm::vec3(0, 1, 1), color),
		VertexFormat(center + glm::vec3(halfWidth, halfLength, -halfHeight), glm::vec3(0, 1, 1), color),
		VertexFormat(center + glm::vec3(halfWidth, halfLength, halfHeight), glm::vec3(0, 1, 1), color),
		VertexFormat(center + glm::vec3(-halfWidth, halfLength, halfHeight), glm::vec3(0, 1, 1), color)
	};

	std::vector<unsigned int> indices =
	{
		// Bottom face
		0, 1, 2,
		0, 2, 3,

		// Top face
		4, 6, 5,
		4, 7, 6,

		// Front face
		0, 4, 5,
		0, 5, 1,

		// Back face
		3, 2, 6,
		3, 6, 7,

		// Left face
		0, 3, 7,
		0, 7, 4,

		// Right face
		1, 5, 6,
		1, 6, 2
	};

	Mesh* parallelepiped = new Mesh(name);
	parallelepiped->InitFromData(vertices, indices);
	return parallelepiped;
}

Mesh* T2object3D::CreateSphere(const std::string& name, glm::vec3 center, float radius, glm::vec3 color)
{
	std::vector<VertexFormat> vertices;
	std::vector<unsigned int> indices;

	int latitudeBands = 50;
	int longitudeBands = 50;

	for (int lat = 0; lat <= latitudeBands; ++lat) {
		float theta = lat * M_PI / latitudeBands;
		float sinTheta = sin(theta);
		float cosTheta = cos(theta);

		for (int lon = 0; lon <= longitudeBands; ++lon) {
			float phi = lon * 2 * M_PI / longitudeBands;
			float sinPhi = sin(phi);
			float cosPhi = cos(phi);

			float x = center.x + radius * cosPhi * sinTheta;
			float y = center.y + radius * cosTheta;
			float z = center.z + radius * sinPhi * sinTheta;

			glm::vec3 position = glm::vec3(x, y, z);
			glm::vec3 normal = glm::vec3(0, 1, 0);

			vertices.emplace_back(position, normal, color);
		}
	}

	for (int lat = 0; lat < latitudeBands; ++lat) {
		for (int lon = 0; lon < longitudeBands; ++lon) {
			int first = (lat * (longitudeBands + 1)) + lon;
			int second = first + longitudeBands + 1;

			indices.push_back(first);
			indices.push_back(second);
			indices.push_back(first + 1);

			indices.push_back(second);
			indices.push_back(second + 1);
			indices.push_back(first + 1);
		}
	}

	Mesh* sphere = new Mesh(name);
	sphere->InitFromData(vertices, indices);
	return sphere;
}

Mesh* T2object3D::CreateArrow(const std::string& name, glm::vec3 center, float size, glm::vec3 color)
{
	std::vector<VertexFormat> vertices =
	{
		VertexFormat(center + glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), color), // 0
		VertexFormat(center + glm::vec3(size, 0, 0), glm::vec3(0, 1, 0), color), // 1
		VertexFormat(center + glm::vec3(size, 0, 0), glm::vec3(0, 1, 0), color), // 2
		VertexFormat(center + glm::vec3(size * 0.8f, 0, size * 0.2f), glm::vec3(0, 1, 0), color), // 3
		VertexFormat(center + glm::vec3(size, 0, 0), glm::vec3(0, 1, 0), color), // 4
		VertexFormat(center + glm::vec3(size * 0.8f, 0, -size * 0.2f), glm::vec3(0, 1, 0), color), // 5
		VertexFormat(center + glm::vec3(size, 0, 0), glm::vec3(0, 1, 0), color), // 6
		VertexFormat(center + glm::vec3(size * 0.8f, size * 0.2f, 0), glm::vec3(0, 1, 0), color), // 7
		VertexFormat(center + glm::vec3(size, 0, 0), glm::vec3(0, 1, 0), color), // 8
		VertexFormat(center + glm::vec3(size * 0.8f, -size * 0.2f, 0), glm::vec3(0, 1, 0), color), // 9
	};

	std::vector<unsigned int> indices =
	{
		0, 1,
		2, 3,
		4, 5,
		6, 7,
		8, 9
	};

	Mesh* arrow = new Mesh(name);
	arrow->InitFromData(vertices, indices);
	return arrow;
}
