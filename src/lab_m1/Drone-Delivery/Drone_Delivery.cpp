#include <string>
#include <iostream>
#include <random>
#include <set>

#include "lab_m1/Drone-Delivery/Drone_Delivery.h"
#include "lab_m1/Drone-Delivery/T2transform3D.h"
#include "lab_m1/Drone-Delivery/T2object3D.h"


using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Drone_Delivery::Drone_Delivery()
{
}


Drone_Delivery::~Drone_Delivery()
{
}


void Drone_Delivery::Init()
{
	polygonMode = GL_FILL;

	// --------------- Camera Vars -------------------
	renderCameraTarget = false;
	camera = new implemented::T2Camera();
	camera->Set(glm::vec3(0, 2, 3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
	projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);


	// -------------- Package Vars ------------------
	{
		playerScore = 0;

		packageHeight = 0.5f;
		pickupRadius = 1.0f;
		deliveryRadius = 1.0f;
		isPackageCollected = false;
		packagePos = make_tuple(15, 0.1f, 0);
		Mesh* cube = T2object3D::CreateCube("package", glm::vec3(0, 0, 0), packageHeight, glm::vec3(1, 0.7, 0));
		AddMeshToList(cube);
	}
	{
		startZonePos = glm::vec3(15, 0, 0);
		endZonePos = glm::vec3(-15, 0, 0);

		Mesh* start = T2object3D::CreateCilinder("start", glm::vec3(0, 0, 0), 10, 0.1, glm::vec3(0.25, 0.8, 0));
		AddMeshToList(start);

		cubeHeight = 2.0f;
		Mesh* cube_start = T2object3D::CreateCube("cube_start", glm::vec3(0, 0, 0), cubeHeight, glm::vec3(0.25, 0.8, 0));
		AddMeshToList(cube_start);

		Mesh* end = T2object3D::CreateCilinder("end", glm::vec3(0, 0, 0), 10, 0.1, glm::vec3(0.8, 0.25, 0));
		AddMeshToList(end);

		cubeHeight = 2.0f;
		Mesh* cube_end = T2object3D::CreateCube("cube_end", glm::vec3(0, 0, 0), cubeHeight, glm::vec3(0.8, 0.25, 0));
		AddMeshToList(cube_end);
	}

	// --------------- Terrain Vars -------------------
	{
		Shader* shader = new Shader("TerrainShader");
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Drone-Delivery", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Drone-Delivery", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{
		cubeHeight = 0.1f;
		Mesh* cube = T2object3D::CreateCube("cube_1", glm::vec3(0, 0, 0), cubeHeight, glm::vec3(0.25, 0.25, 0.25));
		AddMeshToList(cube);
	}

	{
		planeLength = 5;
		Mesh* plane = T2object3D::CreatePlane("plane_1", glm::vec3(0, 0, 0), planeLength, glm::vec3(1, 1, 1));
		AddMeshToList(plane);
	}


	// --------------- Tree & Block -------------------
	{
		ciliHeight = 0.5;
		ciliRadius = 0.2;
		Mesh* trunk = T2object3D::CreateCilinder("tree_trunk", glm::vec3(0, 0, 0), ciliHeight, ciliRadius, glm::vec3(0.21, 0.14, 0));
		AddMeshToList(trunk);

		coneHeight = 2;
		coneRadius = 0.5;
		Mesh* treeLeaves = T2object3D::CreateCone("tree_leaves", glm::vec3(0, 0, 0), coneHeight, coneRadius, glm::vec3(0.26, 0.5, 0.2));
		AddMeshToList(treeLeaves);

		coneHeight = 1.75;
		coneRadius = 0.45;
		Mesh* treeSnow = T2object3D::CreateCone("tree_snow", glm::vec3(0, 0, 0), coneHeight, coneRadius, glm::vec3(0.8, 0.9, 0.9));
		AddMeshToList(treeSnow);

		cubeHeight = 1.0f;
		Mesh* cube = T2object3D::CreateCube("block", glm::vec3(0, 0, 0), cubeHeight, glm::vec3(0.7, 0.7, 0.7));
		AddMeshToList(cube);
		cubeHeight = 0.1f;

		float gridSize = 18.0f;
		float spacing = 3.0f;
		float treeProbability = 0.5;
		bool objTurn = false;

		random_device rd;
		mt19937 gen(rd());
		uniform_real_distribution<> dis(0, 1);

		for (float x = -gridSize; x <= gridSize; x += spacing) {
			for (float z = -gridSize; z <= gridSize; z += spacing) {
				if (dis(gen) < treeProbability) {
					float offsetX = (dis(gen)) * spacing;
					float offsetZ = (dis(gen)) * spacing;

					float valX = x + offsetX;
					float valZ = z + offsetZ;

					if ((valX > 1.5f || valX < -1.5f) && (valZ > 1.5f || valZ < -1.5f)) {
						if (objTurn) {
							blocksPos.push_back(make_tuple(valX, 0, valZ));
							objTurn = false;
						}
						else {
							treesPos.push_back(make_tuple(valX, 0, valZ));
							objTurn = true;
						}
					}

				}
			}
		}
	}

	// --------------- Drone Vars -------------------
	{
		dronePos = make_tuple(0, 1, 0);
		dronepropellerSpeed = 0;
		droneRotationAngle = 0.0f;
		paralellipipedHeight = 0.1f;
		paralellipipedLength = 0.1f;
		paralellipipedWidth = 1.0f;

		propellerLength = 0.025f;
		propellerWidth = 0.025f;
		propellerHeight = 0.25f;

		glm::vec3 black = glm::vec3(0, 0, 0);
		Mesh* propeller1 = T2object3D::CreateParallelepiped("propeller1", glm::vec3(0, 0, 0), propellerLength, propellerWidth, propellerHeight, black);
		AddMeshToList(propeller1);

		Mesh* propeller2 = T2object3D::CreateParallelepiped("propeller2", glm::vec3(0, 0, 0), propellerLength, propellerWidth, propellerHeight, black);
		AddMeshToList(propeller2);

		Mesh* body1 = T2object3D::CreateParallelepiped("body1", glm::vec3(0, 0, 0), paralellipipedLength, paralellipipedWidth, paralellipipedHeight, glm::vec3(0.5, 0.5, 0.5));
		AddMeshToList(body1);

		Mesh* body2 = T2object3D::CreateParallelepiped("body2", glm::vec3(0, 0, 0), paralellipipedLength, paralellipipedWidth, paralellipipedHeight, glm::vec3(0.5, 0.5, 0.5));
		AddMeshToList(body2);
	}

	// -------------- Stars & Clouds & Snow -------------
	{
		starRadius = 0.1f;
		Mesh* sphere = T2object3D::CreateSphere("sphere", glm::vec3(0, 0, 0), starRadius, glm::vec3(1, 1, 0));
		AddMeshToList(sphere);

		float gridSize = 18.0f;
		float spacing = 3.0f;
		float starProbability = 0.7;

		random_device rd;
		mt19937 gen(rd());
		uniform_real_distribution<> dis(0, 1);

		for (float x = -gridSize; x <= gridSize; x += spacing) {
			for (float z = -gridSize; z <= gridSize; z += spacing) {
				if (dis(gen) < starProbability) {
					float offsetX = (dis(gen)) * spacing;
					float offsetZ = (dis(gen)) * spacing;

					int gridX = round((x + offsetX) / spacing);
					int gridZ = round((z + offsetZ) / spacing);

					starsPos.push_back(make_tuple(x + offsetX, 10, z + offsetZ));
				}
			}
		}
	}

	{
		float cloudSingleSphereRadius = 0.5f;
		Mesh* sphere = T2object3D::CreateSphere("cloud_sphere", glm::vec3(0, 0, 0), cloudSingleSphereRadius, glm::vec3(1, 1, 1));
		AddMeshToList(sphere);

		random_device rd;
		mt19937 gen(rd());
		uniform_real_distribution<> dis(0, 1);

		for (auto starPos : starsPos) {

			int randomDirection = dis(gen) < 0.5 ? -1 : 1;

			cloudsPos.push_back(make_tuple(get<0>(starPos) + randomDirection, get<1>(starPos) - 2.0f, get<2>(starPos) + randomDirection));
		}
	}

	{
		int snowPerCloud = 20;

		for (const auto& cloudPos : cloudsPos) {
			for (int i = 0; i < snowPerCloud; ++i) {
				float offsetX = ((rand() % 100) / 100.0f - 0.5f);
				float offsetZ = ((rand() % 100) / 100.0f - 0.5f);
				snowParticles.emplace_back(
					get<0>(cloudPos) + offsetX,
					get<1>(cloudPos),
					get<2>(cloudPos) + offsetZ
				);
				snowVelocities.push_back(0.5f + ((rand() % 100) / 200.0f));
			}
		}

		float snowRadius = 0.01f;
		Mesh* sphere = T2object3D::CreateSphere("snow", glm::vec3(0, 0, 0), snowRadius, glm::vec3(1, 1, 1));
		AddMeshToList(sphere);
	}

	// -------------- Arrow Var -------------------
	{
		float arrowSize = 2.0f;
		Mesh* arrow = T2object3D::CreateArrow("arrow", glm::vec3(0, 0, 0), arrowSize, glm::vec3(1, 0, 0));
		AddMeshToList(arrow);
	}

}

void Drone_Delivery::FrameStart()
{
	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	glViewport(0, 0, resolution.x, resolution.y);
}

void Drone_Delivery::GenerateTree(tuple<float, float, float> treePos) {
	modelMatrix = glm::mat4(1);
	modelMatrix *= transform3D::Translate(get<0>(treePos), get<1>(treePos), get<2>(treePos));
	RenderMesh(meshes["tree_trunk"], shaders["VertexNormal"], modelMatrix);

	modelMatrix = glm::mat4(1);
	modelMatrix *= transform3D::Translate(get<0>(treePos), get<1>(treePos) + ciliHeight, get<2>(treePos));
	RenderMesh(meshes["tree_leaves"], shaders["VertexNormal"], modelMatrix);

	modelMatrix = glm::mat4(1);
	modelMatrix *= transform3D::Translate(get<0>(treePos), get<1>(treePos) + ciliHeight + 0.25f, get<2>(treePos));
	RenderMesh(meshes["tree_snow"], shaders["VertexNormal"], modelMatrix);
}

void Drone_Delivery::GenerateTrees() {

	for (tuple<float, float, float> treePos : treesPos) {
		GenerateTree(treePos);
	}
}

void Drone_Delivery::GenerateBlock(tuple<float, float, float> blockPos) {
	modelMatrix = glm::mat4(1);
	modelMatrix *= transform3D::Translate(get<0>(blockPos), get<1>(blockPos), get<2>(blockPos));
	RenderMesh(meshes["block"], shaders["VertexNormal"], modelMatrix);
}

void Drone_Delivery::GenerateBlocks() {
	for (tuple<float, float, float> blockPos : blocksPos) {
		GenerateBlock(blockPos);
	}
}

void Drone_Delivery::GenerateDrone(tuple<float, float, float> dronePos, float propellerSpeed)
{
	modelMatrix = glm::mat4(1);
	modelMatrix *= transform3D::Translate(get<0>(dronePos), get<1>(dronePos), get<2>(dronePos));
	modelMatrix *= transform3D::RotateOY(droneRotationAngle);

	// Body
	glm::mat4 bodyMatrix = modelMatrix;
	bodyMatrix *= transform3D::RotateOY(M_PI / 4);
	RenderMesh(meshes["body1"], shaders["VertexNormal"], bodyMatrix);

	bodyMatrix = modelMatrix;
	bodyMatrix *= transform3D::RotateOY(-M_PI / 4);
	RenderMesh(meshes["body2"], shaders["VertexNormal"], bodyMatrix);

	float armAngles[4] = {
		M_PI / 4,        // 45 degrees
		M_PI / 4 + M_PI / 2,  // 45 + 90 = 135 degrees
		M_PI / 4 + M_PI,    // 45 + 180 = 225 degrees
		M_PI / 4 + 3 * M_PI / 2 // 45 + 270 = 315 degrees
	};

	for (int i = 0; i < 4; i++) {
		float angle = armAngles[i];

		// CUBE
		glm::mat4 cubeMatrix = modelMatrix;
		cubeMatrix *= transform3D::RotateOY(angle);
		cubeMatrix *= transform3D::Translate(paralellipipedWidth / 2.0f, 0, -0.05f);
		RenderMesh(meshes["cube_1"], shaders["VertexNormal"], cubeMatrix);

		// PROPELLERS
		glm::mat4 propellerMatrix = modelMatrix;
		propellerMatrix *= transform3D::RotateOY(angle);
		propellerMatrix *= transform3D::Translate(paralellipipedWidth / 2.0f + cubeHeight / 2, cubeHeight + propellerWidth / 2, 0);
		propellerMatrix *= transform3D::RotateOY(propellerSpeed);
		RenderMesh(meshes["propeller1"], shaders["VertexNormal"], propellerMatrix);

		propellerMatrix = modelMatrix;
		propellerMatrix *= transform3D::RotateOY(angle);
		propellerMatrix *= transform3D::Translate(paralellipipedWidth / 2.0f + cubeHeight / 2, cubeHeight + propellerWidth / 2, 0);
		propellerMatrix *= transform3D::RotateOY(M_PI / 2);
		propellerMatrix *= transform3D::RotateOY(propellerSpeed);
		RenderMesh(meshes["propeller1"], shaders["VertexNormal"], propellerMatrix);
	}
}


void Drone_Delivery::GenerateTerrain() {
	int gridSizeX = 5; // Number of planes in the X direction
	int gridSizeZ = 5; // Number of planes in the Z direction

	for (int i = 0; i < gridSizeX; i++) {
		for (int j = 0; j < gridSizeZ; j++) {
			modelMatrix = glm::mat4(1);
			modelMatrix *= transform3D::Translate(i * planeLength, 0, j * planeLength);
			RenderMesh(meshes["plane_1"], shaders["TerrainShader"], modelMatrix);

			modelMatrix = glm::mat4(1);
			modelMatrix *= transform3D::Translate(i * planeLength, 0, -j * planeLength);
			RenderMesh(meshes["plane_1"], shaders["TerrainShader"], modelMatrix);

			modelMatrix = glm::mat4(1);
			modelMatrix *= transform3D::Translate(-i * planeLength, 0, j * planeLength);
			RenderMesh(meshes["plane_1"], shaders["TerrainShader"], modelMatrix);

			modelMatrix = glm::mat4(1);
			modelMatrix *= transform3D::Translate(-i * planeLength, 0, -j * planeLength);
			RenderMesh(meshes["plane_1"], shaders["TerrainShader"], modelMatrix);
		}
	}
}

void Drone_Delivery::CheckCollision(tuple<float, float, float>* dronePos, vector<tuple<float, float, float>> blocksPos, vector<tuple<float, float, float>> treesPos) {

	float dx = get<0>(*dronePos);
	float dy = get<1>(*dronePos);
	float dz = get<2>(*dronePos);

	float droneRadius = 0.5f;

	// Check ground collision
	if (dy < 0.0f) {
		camera->Set(glm::vec3(0, 2, 3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
		*dronePos = make_tuple(0, 1, 0);
	}

	// Check collision with blocks
	for (auto& blockPos : blocksPos) {
		// Add 0.5 to go to the center
		float bx = get<0>(blockPos) + 0.5f;
		float by = get<1>(blockPos) + 0.5f;
		float bz = get<2>(blockPos) + 0.5f;

		float blockRadius = 0.5f;

		float distX = dx - bx;
		float distY = dy - by;
		float distZ = dz - bz;
		float distance = sqrt(distX * distX + distY * distY + distZ * distZ);

		if (distance < (droneRadius + blockRadius)) {
			cout << "Collision with block at " << bx << " " << by << " " << bz << endl;
			camera->Set(glm::vec3(0, 2, 3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
			*dronePos = make_tuple(0, 1, 0);
		}
	}

	// Check collision with trees
	for (auto& treePos : treesPos) {
		float tx = get<0>(treePos);
		float ty = get<1>(treePos) + 1.0f;
		float tz = get<2>(treePos);

		float treeRadius = 0.5f;

		float distX = dx - tx;
		float distY = dy - ty;
		float distZ = dz - tz;
		float distance = sqrt(distX * distX + distY * distY + distZ * distZ);

		if (distance < (droneRadius + treeRadius)) {
			cout << "Collision with tree at " << tx << " " << ty << " " << tz << endl;
			camera->Set(glm::vec3(0, 2, 3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
			*dronePos = make_tuple(0, 1, 0);
		}
	}
}

void Drone_Delivery::GenerateStar(tuple<float, float, float> starPos) {
	modelMatrix = glm::mat4(1);
	modelMatrix *= transform3D::Translate(get<0>(starPos), get<1>(starPos), get<2>(starPos));
	RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
}

void Drone_Delivery::GenerateStars() {
	for (tuple<float, float, float> starPos : starsPos) {
		GenerateStar(starPos);
	}
}

void Drone_Delivery::GenerateCloud(tuple<float, float, float> cloudPos) {
	float sphereSpacing = 1.0f;

	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 2; ++j) {
			modelMatrix = glm::mat4(1);
			modelMatrix *= transform3D::Translate(
				get<0>(cloudPos) + i * sphereSpacing,
				get<1>(cloudPos),
				get<2>(cloudPos) + j * sphereSpacing);
			RenderMesh(meshes["cloud_sphere"], shaders["VertexNormal"], modelMatrix);
		}
	}

	modelMatrix = glm::mat4(1);
	modelMatrix *= transform3D::Translate(
		get<0>(cloudPos) + sphereSpacing / 2,
		get<1>(cloudPos) + sphereSpacing / 6,
		get<2>(cloudPos) + sphereSpacing / 2);
	RenderMesh(meshes["cloud_sphere"], shaders["VertexNormal"], modelMatrix);
}

void Drone_Delivery::GenerateClouds() {
	for (tuple<float, float, float> cloudPos : cloudsPos) {
		GenerateCloud(cloudPos);
	}
}

void Drone_Delivery::GenerateSnow(float deltaTimeSeconds) {
	float terrainY = 0.0f;
	vector<tuple<float, float, float>> newSnowParticles;
	vector<float> newSnowVelocities;

	for (size_t i = 0; i < snowParticles.size(); ++i) {
		auto& particle = snowParticles[i];
		float velocity = snowVelocities[i];

		float x = get<0>(particle);
		float y = get<1>(particle) - velocity * deltaTimeSeconds;
		float z = get<2>(particle);

		if (y <= terrainY) {
			int randomCloudIndex = rand() % cloudsPos.size();
			auto& cloudPos = cloudsPos[randomCloudIndex];

			float offsetX = ((rand() % 100) / 100.0f - 0.5f);
			float offsetZ = ((rand() % 100) / 100.0f - 0.5f);
			newSnowParticles.emplace_back(
				get<0>(cloudPos) + offsetX,
				get<1>(cloudPos),
				get<2>(cloudPos) + offsetZ
			);
			newSnowVelocities.push_back(velocity);
		}
		else {
			newSnowParticles.emplace_back(x, y, z);
			newSnowVelocities.push_back(velocity);
		}

		modelMatrix = glm::mat4(1);
		modelMatrix *= transform3D::Translate(x, y, z);
		RenderMesh(meshes["snow"], shaders["VertexNormal"], modelMatrix);
	}

	snowParticles = newSnowParticles;
	snowVelocities = newSnowVelocities;
}

void Drone_Delivery::GeneratePackage(tuple<float, float, float>* packagePos) {
	if (isPackageCollected) {
		*packagePos = make_tuple(get<0>(dronePos), get<1>(dronePos) - 0.75f, get<2>(dronePos));
	}

	modelMatrix = glm::mat4(1);
	modelMatrix *= transform3D::Translate(get<0>(*packagePos), get<1>(*packagePos), get<2>(*packagePos));
	RenderMesh(meshes["package"], shaders["VertexNormal"], modelMatrix);
}

bool Drone_Delivery::HasFreeSpace(glm::vec3 position, float radius)
{
	for (auto& treePos : treesPos) {
		float distance = glm::distance(glm::vec3(get<0>(treePos), get<1>(treePos), get<2>(treePos)), position);
		if (distance < radius) return false;
	}

	for (auto& blockPos : blocksPos) {
		float distance = glm::distance(glm::vec3(get<0>(blockPos), get<1>(blockPos), get<2>(blockPos)), position);
		if (distance < radius) return false;
	}

	return true;
}

void Drone_Delivery::RandomizeStartEndPositions() {
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> dis(-18.0f, 18.0f);

	glm::vec3 newStartPos, newEndPos;

	do {
		newStartPos = glm::vec3(dis(gen), 0.0f, dis(gen));
	} while (!HasFreeSpace(newStartPos, 3.0f));

	do {
		newEndPos = glm::vec3(dis(gen), 0.0f, dis(gen));
	} while (!HasFreeSpace(newEndPos, 3.0f) || glm::distance(newStartPos, newEndPos) < 10.0f);

	startZonePos = newStartPos;
	endZonePos = newEndPos;
	packagePos = make_tuple(newStartPos.x, 0.2f, newStartPos.z);
}


void Drone_Delivery::GenerateStart() {
	modelMatrix = glm::mat4(1);
	modelMatrix *= transform3D::Translate(startZonePos.x, startZonePos.y, startZonePos.z);
	RenderMesh(meshes["start"], shaders["VertexNormal"], modelMatrix);

	modelMatrix = glm::mat4(1);
	modelMatrix *= transform3D::Translate(startZonePos.x, 17.0f, startZonePos.z);
	RenderMesh(meshes["cube_start"], shaders["VertexNormal"], modelMatrix);
}

void Drone_Delivery::GenerateEnd() {
	modelMatrix = glm::mat4(1);
	modelMatrix *= transform3D::Translate(endZonePos.x, endZonePos.y, endZonePos.z);
	RenderMesh(meshes["end"], shaders["VertexNormal"], modelMatrix);

	modelMatrix = glm::mat4(1);
	modelMatrix *= transform3D::Translate(endZonePos.x, 17.0f, endZonePos.z);
	RenderMesh(meshes["cube_end"], shaders["VertexNormal"], modelMatrix);
}


void Drone_Delivery::CheckPackageDelivery() {

	if (!isPackageCollected) {
		float dx = get<0>(packagePos) - endZonePos.x;
		float dz = get<2>(packagePos) - endZonePos.z;
		float distance = sqrt(dx * dx + dz * dz);

		if (distance <= deliveryRadius) {
			playerScore += 1;
			packagePos = make_tuple(15.0f, 0.1f, 0.0f);

			cout << "Package delivered! Score: " << playerScore << endl;

			RandomizeStartEndPositions();
		}
	}
}

void Drone_Delivery::GenerateArrow() {
	glm::vec3 dronePosition = glm::vec3(get<0>(dronePos), get<1>(dronePos), get<2>(dronePos));
	glm::vec3 directionToEnd = glm::normalize(endZonePos - dronePosition);

	// Position of the arrow
	glm::vec3 arrowPosition = dronePosition;
	float angle = atan2(directionToEnd.z, directionToEnd.x);

	modelMatrix = glm::mat4(1);
	modelMatrix *= transform3D::Translate(arrowPosition.x, arrowPosition.y + 0.5f, arrowPosition.z);
	modelMatrix *= transform3D::RotateOY(-angle);
	RenderMesh(meshes["arrow"], shaders["VertexNormal"], modelMatrix);
}

void Drone_Delivery::RenderScene(float deltaTimeSeconds) {

	// Sky
	GenerateSnow(deltaTimeSeconds);
	GenerateClouds();
	GenerateStars();

	// Terrain
	GenerateTerrain();
	GenerateBlocks();
	GenerateTrees();

	// Drone
	GenerateDrone(dronePos, dronepropellerSpeed);
	CheckCollision(&dronePos, blocksPos, treesPos);
	dronepropellerSpeed += 0.1f;

	// Package
	GeneratePackage(&packagePos);
	GenerateStart();
	GenerateEnd();

	// Arrow
	GenerateArrow();

	CheckPackageDelivery();
}

void Drone_Delivery::RenderMinimap(float deltaTimeSeconds)
{
	int minimapWidth = 200;
	int minimapHeight = 200;
	glm::ivec2 resolution = window->GetResolution();
	glViewport(resolution.x - minimapWidth - 10, resolution.y - minimapHeight - 10, minimapWidth, minimapHeight);

	glClear(GL_DEPTH_BUFFER_BIT);

	// Camera pentru minimap
	glm::vec3 minimapCameraPos = glm::vec3(0, 20, 0);
	glm::vec3 minimapTarget = glm::vec3(0, 0, 0);
	glm::vec3 minimapUp = glm::vec3(0, 0, -1);
	camera->Set(minimapCameraPos, minimapTarget, minimapUp);

	// Proiecție ortografica
	float orthoSize = 20.0f;
	projectionMatrix = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, 0.01f, 100.0f);

	RenderScene(deltaTimeSeconds);
}


void Drone_Delivery::Update(float deltaTimeSeconds)
{
	projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);
	RenderScene(deltaTimeSeconds);

	RenderMinimap(deltaTimeSeconds);
}

void Drone_Delivery::FrameEnd()
{
	//DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}

void Drone_Delivery::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
	if (!mesh || !shader || !shader->program)
		return;

	// Render an object using the specified shader and the specified position
	shader->Use();
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	mesh->Render();
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Drone_Delivery::OnInputUpdate(float deltaTime, int mods)
{
	float droneSpeed = 10.0f * deltaTime;
	float rotationSpeed = 2.0f * deltaTime;
	glm::vec3 droneForward = glm::vec3(sin(droneRotationAngle), 0, cos(droneRotationAngle));
	glm::vec3 droneRight = glm::vec3(cos(droneRotationAngle), 0, -sin(droneRotationAngle));


	if (window->KeyHold(GLFW_KEY_W)) {
		dronePos = make_tuple(
			get<0>(dronePos) + droneForward.x * droneSpeed,
			get<1>(dronePos),
			get<2>(dronePos) + droneForward.z * droneSpeed);
	}
	if (window->KeyHold(GLFW_KEY_S)) {
		dronePos = make_tuple(
			get<0>(dronePos) - droneForward.x * droneSpeed,
			get<1>(dronePos),
			get<2>(dronePos) - droneForward.z * droneSpeed);
	}
	if (window->KeyHold(GLFW_KEY_A)) {
		dronePos = make_tuple(
			get<0>(dronePos) + droneRight.x * droneSpeed,
			get<1>(dronePos),
			get<2>(dronePos) + droneRight.z * droneSpeed);
	}
	if (window->KeyHold(GLFW_KEY_D)) {
		dronePos = make_tuple(
			get<0>(dronePos) - droneRight.x * droneSpeed,
			get<1>(dronePos),
			get<2>(dronePos) - droneRight.z * droneSpeed);
	}
	if (window->KeyHold(GLFW_KEY_Q)) {
		droneRotationAngle += rotationSpeed;
	}
	if (window->KeyHold(GLFW_KEY_E)) {
		droneRotationAngle -= rotationSpeed;
	}
	if (window->KeyHold(GLFW_KEY_R)) {
		dronePos = make_tuple(get<0>(dronePos), get<1>(dronePos) + droneSpeed, get<2>(dronePos));
	}
	if (window->KeyHold(GLFW_KEY_F)) {
		dronePos = make_tuple(get<0>(dronePos), get<1>(dronePos) - droneSpeed, get<2>(dronePos));
	}

	// Update camera position
	droneForward = glm::normalize(droneForward);
	glm::vec3 dronePosVec(get<0>(dronePos), get<1>(dronePos), get<2>(dronePos));
	float cameraDistance = 3.5f;
	float cameraHeight = 2.0f;
	glm::vec3 cameraPosition = dronePosVec + glm::vec3(0, cameraHeight, 0) - droneForward * cameraDistance;
	glm::vec3 cameraTarget = dronePosVec + glm::vec3(0, 1, 0);

	camera->Set(cameraPosition, cameraTarget, glm::vec3(0, 1, 0));
}


void Drone_Delivery::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_SPACE) {
		float dx = get<0>(dronePos) - get<0>(packagePos);
		float dz = get<2>(dronePos) - get<2>(packagePos);
		float distance = sqrt(dx * dx + dz * dz);

		if (distance <= pickupRadius && !isPackageCollected) {
			isPackageCollected = true;
		}
	}
	if (key == GLFW_KEY_M) {
		if (isPackageCollected == true) {
			isPackageCollected = false;
			packagePos = make_tuple(get<0>(dronePos), 0.1f, get<2>(dronePos));
		}
	}
}


void Drone_Delivery::OnKeyRelease(int key, int mods)
{
	// Add key release event
}


void Drone_Delivery::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// Add mouse move event
}


void Drone_Delivery::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// Add mouse button press event
}


void Drone_Delivery::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// Add mouse button release event
}


void Drone_Delivery::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Drone_Delivery::OnWindowResize(int width, int height)
{
}
