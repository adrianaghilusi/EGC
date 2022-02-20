#pragma once

#include <string>
#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"
#include "components/simple_scene.h"


namespace m1
{
	class Tema1 : public gfxc::SimpleScene
	{
	public:
		Tema1();
		~Tema1();
		struct Point {
			float x, y;
		};

		struct Game {
			float score = 0;
			float health = 100;
			int maxProjDist = 15;
			float secondsElapsedShoot = 0;
			float secondsElapsedEnemyWave = 0;
			bool openFire = false;
			float fireRate = 0.5f;
			bool gameOver = false;
			Point powerUpHealth = { 0,0 };
			Point powerUpScore = { 0,0 };
		};
		struct Projectile {
			float projectileX;
			Point projectileStart;
			float projectileY;
			bool outOfRange;
			float angle;
		};
		struct Enemy {
			float x, y;
			bool eliminated = false;
			float speed;
			Point armR, armL;
			float angle;
		};
		struct Player {
			float translateX, translateY;
			float angularStep;
			float angularStep2;
			float rX, rY, lX, lY;
		};

		void Init() override;

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
	private:
		bool Tema1::notObstacle(int x, int y);
		void EnemyCollision(float x, float y);
		bool Tema1::avoidCenter(int x, int y, int sx, int sy, int c1, int c2);
		bool powerUpCollision(float cx, float cy, float x, float y);
		bool checkProjectileWallCollision(Point b, Point t);
		void FrameStart() override;
		void DrawScene(glm::mat3 visMatrix);
		void Update(float deltaTimeSeconds) override;
		void MoveEnemies(float deltaTimeSeconds);
		void FlyProjectiles(float deltaTimeSeconds);
		bool checkProjectileEnemyCollision(Point b1, int pos);
		void FrameEnd() override;
		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
		void OnWindowResize(int width, int height) override;
		glm::mat3 VisualizationTransf2D(const LogicSpace& logicSpace, const ViewportSpace& viewSpace);
		glm::mat3 VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace);
		void SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor = glm::vec3(0), bool clear = true);

	protected:
		float length;
		ViewportSpace viewSpace;
		LogicSpace logicSpace;
		glm::mat3  visMatrix;



	protected:

		glm::mat3 modelMatrix;

		//player
		Player player;


		//hurdles
		int nrOfHurdles = 0;
		std::vector<int> hurdlePosX;
		std::vector<int> hurdlePosY;
		std::vector<int> hurdleScalingFactorX;
		std::vector<int> hurdleScalingFactorY;

		//game

		Game game;

		int enemyNumber = 0;
		std::vector<Enemy> enemies;

		int projectileNumber = 0;
		std::vector<Projectile> projectiles;



	};
}   // namespace m1