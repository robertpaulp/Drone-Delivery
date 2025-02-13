#pragma once

#include <tuple>
#include <vector>

#include "components/simple_scene.h"
#include "lab_m1/Drone-Delivery/T2camera.h"


namespace m1
{
	class Drone_Delivery : public gfxc::SimpleScene
	{
	public:
		struct ViewportArea
		{
			ViewportArea() : x(0), y(0), width(1), height(1) {}
			ViewportArea(int x, int y, int width, int height)
				: x(x), y(y), width(width), height(height) {}
			int x;
			int y;
			int width;
			int height;
		};

		Drone_Delivery();
		~Drone_Delivery();

		void Init() override;

	private:
		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
		void OnWindowResize(int width, int height) override;
		void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix) override;

		void RenderScene(float deltaTimeSeconds);
		void CheckCollision(std::tuple<float, float, float>* dronePos, std::vector<std::tuple<float, float, float>> blocksPos, std::vector<std::tuple<float, float, float>> treesPos);
		void GenerateTerrain();
		void GenerateTree(std::tuple<float, float, float> treePos);
		void GenerateDrone(std::tuple<float, float, float> dronePos, float eliceSpeed);
		void GenerateTrees();
		void GenerateBlock(std::tuple<float, float, float> blockPos);
		void GenerateBlocks();
		void GenerateStar(std::tuple<float, float, float> starPos);
		void GenerateStars();
		void GenerateCloud(std::tuple<float, float, float> cloudPos);
		void GenerateClouds();
		void GenerateSnow(float deltaTimeSeconds);
		void GeneratePackage(std::tuple<float, float, float>* packagePos);
		void GenerateStart();
		void GenerateEnd();
		void CheckPackageDelivery();
		void RenderMinimap(float deltaTimeSeconds);
		bool HasFreeSpace(glm::vec3 position, float radius);
		void RandomizeStartEndPositions();
		void GenerateArrow();
	protected:
		implemented::T2Camera* camera;
		glm::mat4 projectionMatrix;
		bool renderCameraTarget;

		implemented::T2Camera* miniMapCamera;
		glm::mat4 miniMapprojectionMatrix;

		glm::mat4 modelMatrix;
		GLenum polygonMode;
		ViewportArea miniViewportArea;

		float cubeHeight, planeLength;
		float ciliHeight, ciliRadius;
		float coneHeight, coneRadius;

		// --------------- Block Vars -------------------
		std::vector<std::tuple<float, float, float>> blocksPos;

		// --------------- Tree Vars -------------------
		std::vector<std::tuple<float, float, float>> treesPos;
		float treeScaleX, treeScaleY, treeScaleZ;
		float treeHeight, treeWidth;

		// --------------- Drone Vars -------------------
		std::tuple<float, float, float> dronePos;

		float paralellipipedLength, paralellipipedWidth, paralellipipedHeight;
		float propellerLength, propellerWidth, propellerHeight;

		float dronepropellerSpeed, droneRotationAngle;

		// --------------- Stars & Clouds & Snow --------
		std::vector<std::tuple<float, float, float>> starsPos;
		std::vector<std::tuple<float, float, float>> cloudsPos;
		std::vector<std::tuple<float, float, float>> snowParticles;
		std::vector<float> snowVelocities;
		float starRadius;

		// --------------- Package Vars -----------------
		std::tuple<float, float, float> packagePos;
		glm::vec3 startZonePos;
		glm::vec3 endZonePos;

		float packageHeight;
		bool isPackageCollected;
		float pickupRadius;
		float deliveryRadius;

		// --------------- Player Vars -------------
		int playerScore;
	};
}   // namespace m1

