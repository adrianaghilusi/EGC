#pragma once

#include "components/simple_scene.h"
#include "lab_m1/Tema2/lab_camera.h"
#include <stack>
#include <vector>
#include <string>
#include <iostream>

using namespace std;
#define h 40
#define w 40
#define timeToEscape 120
#define wallCollisionOffset 0.9
#define enemyCollisionOffset 0.2
#define animDuration 5
#define projSpeed 2
#define enemyMaxRad 0.1
#define collisionCooldown 0.25
#define healthBarSize 3

namespace m1
{
	class Tema2 : public gfxc::SimpleScene
	{
	public:
		Tema2();
		~Tema2();

		void Init() override;

	private:
		struct Point {
			float x, y;
		};

		struct Player {
			glm::vec3 playerPos;
			glm::vec3 playerRotation = glm::vec3(0, 0, 0);
		};
		struct Projectile {

			bool outOfRange;
			glm::vec3 start;
			glm::vec3 aimDir;
			glm::vec3 pos;
		};

		struct Game {
			float health = 100;
			int maxProjDist = 5;
			float time = 0;
			bool openFire = false;
			bool gameOver = false;
			float cooldown = 0;

		};
		struct Wall {
			float x;
			float y;
			float z;
		};
		struct Enemy {
			glm::vec3 pos;
			bool eliminated = false;
			glm::vec3 startPos;
			float animationDuration = 0;
		};
		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;
		void DrawScene(float deltaTimeSeconds);
		void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec4& color);
		void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture);
		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
		void OnWindowResize(int width, int height) override;
		void GenerateHealthBarAndTimeLeft();  //asta nuj
		void addNodes(vector<pair<int, int>> nodes);
		void GenerateMaze();
		void GenerateProjectiles();
		void GeneratePlayer();
		void GenerateEnemies(float deltaTimeSeconds);
		void FlyProjectiles(float deltaTimeSeconds);
		void RemoveOutOfRangeProjectiles();
		void MoveEnemies(float deltaTimeSeconds);
		void RenderMeshAnimation(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture);
		void RemoveEliminatedEnemies();
		bool notCorner(pair<int, int> next, int x, int y);
		bool inside(int x, int y);
		bool notNode(pair<int, int> next, int x, int y);
		bool isValid(pair<int, int> node);
		bool checkPlayerWallCollision(glm::vec3 futurePos);
		bool ProjectileBoxCollision(glm::vec3 c);
		bool ProjectileEnemyCollision(glm::vec3 c, glm::vec3 enemyPos);
		bool checkPlayerEnemyCollision(glm::vec3 futurePos);
		bool Ttl(Projectile p);
		vector<pair<int, int>> findNeighbors(pair<int, int> next);
		Mesh* CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices);

	public:
		t1::Camera* camera;
		Game game;
		stack<pair<int, int>> s;
		Player player;
		std::vector<Projectile> projectiles;
		std::vector<Wall> walls;
		std::vector<Enemy> enemies;
		std::unordered_map<std::string, Texture2D*> mapTextures;
		glm::mat4 projectionMatrix;
		glm::vec3 thirdPersonCameraLatestPosition;
		pair<int, int> init;
		bool thirdPersonView = true;
		int projectileNumber = 0;
		int nrOfWalls = 0;
		int nrOfEnemies = 0;
		int enemySpeed = 2;
		float l = 0, r = 10, b = 0, u = 10, fov = 60, zNear = 0.01, zFar = 200;
		float direction = 0.f;

		struct ViewportSpace
		{
			ViewportSpace() : x(0), y(0), width(1), height(1) {}
			ViewportSpace(int x, int y, int width, int height)
				: x(x), y(y), width(width), height(height) {}
			int x;
			int y;
			int width;
			int height;
		};

		struct LogicSpace
		{
			LogicSpace() : x(0), y(0), width(1), height(1) {}
			LogicSpace(float x, float y, float width, float height)
				: x(x), y(y), width(width), height(height) {}
			float x;
			float y;
			float width;
			float height;
		};

		glm::mat3 VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace);
		glm::vec3 corner = glm::vec3(0, 0, 0);
		void SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor, bool clear);
		Mesh* CreateRectangle(
			const std::string& name,
			glm::vec3 leftBottomCorner,
			float length,
			glm::vec3 color,
			bool fill);

		glm::mat3 Translate(float translateX, float translateY);
		glm::mat3 Scale(float scaleX, float scaleY);

		ViewportSpace viewSpace;
		LogicSpace logicSpace;
		glm::mat3  visMatrix;

	};
}   // namespace m1
