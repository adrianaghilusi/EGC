#include "lab_m1/Tema1/Tema1.h"
#include "components/simple_scene.h"

#include <vector>
#include <iostream>
#include <stdlib.h>
#include "lab_m1/Tema1/transform2D.h"
#include "lab_m1/Tema1/object2D.h"



#define headRad 2
#define eyeRad 1
#define hurdleSize 3
#define backgroundSide 50
#define enemySize 2
#define enemyArmSize 1
#define projectileSize 1
#define healthBarSize 3
#define eyeDist 0.5f
#define offset 10

using namespace object2d;

using namespace std;
using namespace m1;



Tema1::Tema1()
{
}


Tema1::~Tema1()
{
}

glm::mat3 Tema1::VisualizationTransf2D(const LogicSpace& logicSpace, const ViewportSpace& viewSpace)
{
	float sx, sy, tx, ty;
	sx = viewSpace.width / logicSpace.width;
	sy = viewSpace.height / logicSpace.height;
	tx = viewSpace.x - sx * logicSpace.x;
	ty = viewSpace.y - sy * logicSpace.y;

	return glm::transpose(glm::mat3(
		sx, 0.0f, tx,
		0.0f, sy, ty,
		0.0f, 0.0f, 1.0f));
}


glm::mat3 Tema1::VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace)
{
	float sx, sy, tx, ty, smin;
	sx = viewSpace.width / logicSpace.width;
	sy = viewSpace.height / logicSpace.height;
	if (sx < sy)
		smin = sx;
	else
		smin = sy;
	tx = viewSpace.x - smin * logicSpace.x + (viewSpace.width - smin * logicSpace.width) / 2;
	ty = viewSpace.y - smin * logicSpace.y + (viewSpace.height - smin * logicSpace.height) / 2;

	return glm::transpose(glm::mat3(
		smin, 0.0f, tx,
		0.0f, smin, ty,
		0.0f, 0.0f, 1.0f));
}


void Tema1::SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor, bool clear)
{
	glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

	glEnable(GL_SCISSOR_TEST);
	glScissor(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

	glClearColor(colorColor.r, colorColor.g, colorColor.b, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);

	GetSceneCamera()->SetOrthographic((float)viewSpace.x, (float)(viewSpace.x + viewSpace.width), (float)viewSpace.y, (float)(viewSpace.y + viewSpace.height), 0.1f, 400);
	GetSceneCamera()->Update();
}

void Tema1::Init()
{
	glm::ivec2 resolution = window->GetResolution();
	auto camera = GetSceneCamera();
	camera->SetPosition(glm::vec3(0, 0, 50));
	camera->SetRotation(glm::vec3(0, 0, 0));
	camera->Update();
	GetCameraInput()->SetActive(false);

	glm::vec3 corner = glm::vec3(0, 0, 0);
	logicSpace.x = 0;
	logicSpace.y = 0;
	logicSpace.width = 32;
	logicSpace.height = 32;

	player.angularStep = AI_MATH_PI / 2 + eyeDist;
	player.angularStep2 = AI_MATH_PI / 2 - eyeDist;


	srand((unsigned)time(NULL));
	nrOfHurdles = 10 + rand() % 30;
	enemyNumber = 4 + rand() % 6;
	glm::ivec2 res = window->GetResolution();
	player.translateX = logicSpace.width / 2;
	player.translateY = logicSpace.height / 2;
	player.rX = player.translateX + headRad * cos(player.angularStep);
	player.rY = player.translateY + headRad * sin(player.angularStep);
	player.lX = player.translateX + headRad * cos(player.angularStep2);
	player.lY = player.translateY + headRad * sin(player.angularStep2);

	for (int i = 0; i < nrOfHurdles; i++) {

		int posx;
		int posy;
		int sx;
		int sy;
		do {
			posx = rand() % ((int)backgroundSide * 2 - 20);
			posy = rand() % ((int)backgroundSide - 5);
			sx = 1 + rand() % 2;
			sy = 1 + rand() % 2;
		} while (!avoidCenter(posx, posy, sx, sy, player.translateX, player.translateY));

		hurdlePosX.push_back(posx);
		hurdlePosY.push_back(posy);
		hurdleScalingFactorX.push_back(sx);
		hurdleScalingFactorY.push_back(sy);
	}
	for (int i = 0; i < enemyNumber; i++) {

		int posx;
		int posy;
		float speed;

		do {
			posx = rand() % ((int)backgroundSide * 2 - 20);
			posy = rand() % ((int)backgroundSide - 5);
			speed = 1 + rand() % 3;

		} while (!avoidCenter(posx, posy, 1, 1, player.translateX, player.translateY));
		Point right = { posx + enemySize / 4,posy + enemySize };
		Point left = { posx + enemySize / 4 ,posy - enemyArmSize };
		Enemy enemy = { posx,posy,false,speed,right,left };
		enemies.push_back(enemy);

	}



	Mesh* head = object2d::CreateCircle("head", 100, glm::vec3(0.50, 0.71, 0.49), true, headRad);
	AddMeshToList(head);
	Mesh* leftEye = object2d::CreateCircle("leftEye", 50, glm::vec3(0.95, 0.98, 0.94), true, eyeRad);
	AddMeshToList(leftEye);
	Mesh* rightEye = object2d::CreateCircle("rightEye", 50, glm::vec3(0.95, 0.98, 0.94), true, eyeRad);
	AddMeshToList(rightEye);
	Mesh* meshOchi = object2d::CreateCircle("meshOchi", 50, glm::vec3(0.38, 0.68, 0.20), false, eyeRad);
	AddMeshToList(meshOchi);
	Mesh* meshPupila = object2d::CreateCircle("meshPupila", 50, glm::vec3(0.06, 0.1, 0.06), false, eyeRad);
	AddMeshToList(meshPupila);
	Mesh* background = object2d::CreateRectangle("background", corner, backgroundSide, glm::vec3(0.48, 0.61, 0.78), false);
	AddMeshToList(background);
	Mesh* hurdle = object2d::CreateRectangle("hurdle", corner, hurdleSize, glm::vec3(0.03f, 0.4f, 0.53), true);
	AddMeshToList(hurdle);
	Mesh* projectile = object2d::CreateRectangle("projectile", corner, projectileSize, glm::vec3(1.0, 0.06, 0.33), true);
	AddMeshToList(projectile);
	Mesh* enemyBody = object2d::CreateSquare("enemyBody", corner, enemySize, glm::vec3(1.0, 0.6, 0.36), false);
	AddMeshToList(enemyBody);
	Mesh* enemyBodyMesh = object2d::CreateSquare("enemyBodyMesh", corner, enemySize, glm::vec3(1.0, 0.99, 0.60), true);
	AddMeshToList(enemyBodyMesh);
	Mesh* enemyArmR = object2d::CreateSquare("enemyArmR", corner, enemyArmSize, glm::vec3(0.26, 0.28, 0.45), true);
	AddMeshToList(enemyArmR);
	Mesh* enemyArmL = object2d::CreateSquare("enemyArmL", corner, enemyArmSize, glm::vec3(0.26, 0.28, 0.45), true);
	AddMeshToList(enemyArmL);
	Mesh* healthBarWire = object2d::CreateRectangle("healthBarWire", corner, healthBarSize, glm::vec3(0.556863, 0.137255, 0.137255), false);
	AddMeshToList(healthBarWire);
	Mesh* healthBarFull = object2d::CreateRectangle("healthBarFull", corner, healthBarSize, glm::vec3(0.32, 1.0, 0.498039), true);
	AddMeshToList(healthBarFull);
	AddMeshToList(head);
	Mesh* powerUpHealth = object2d::CreateCircle("powerUpHealth", 50, glm::vec3(0.32, 1.0, 0.498039), true, eyeRad);
	AddMeshToList(powerUpHealth);
	Mesh* powerUpScore = object2d::CreateCircle("powerUpScore", 50, glm::vec3(0.62, 0.55, 0.95), true, eyeRad);
	AddMeshToList(powerUpScore);



}





void Tema1::FrameStart()
{

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();

	glViewport(0, 0, resolution.x, resolution.y);
}

void Tema1::DrawScene(glm::mat3 visMatrix)
{
	modelMatrix = glm::mat3(1);
	float wireX = player.translateX + logicSpace.width / 2 + healthBarSize * 2;
	float wireY = player.translateY + logicSpace.height / 2 - healthBarSize;
	modelMatrix = visMatrix * transform2D::Translate(wireX, wireY);
	RenderMesh2D(meshes["healthBarWire"], shaders["VertexColor"], modelMatrix);
	modelMatrix = glm::mat3(1);
	modelMatrix = visMatrix * transform2D::Translate(wireX, wireY);
	modelMatrix *= transform2D::Scale(game.health / 100, 1);
	RenderMesh2D(meshes["healthBarFull"], shaders["VertexColor"], modelMatrix);
	//harta

	modelMatrix = visMatrix * transform2D::Translate(0, 0);
	RenderMesh2D(meshes["background"], shaders["VertexColor"], modelMatrix);

	//ochi stang
	glm::mat3 EyeModelMatrix = glm::mat3(1);
	EyeModelMatrix = visMatrix * transform2D::Translate(player.lX, player.lY);
	modelMatrix = EyeModelMatrix * transform2D::Scale(0.3, 0.3);
	RenderMesh2D(meshes["meshPupila"], shaders["VertexColor"], modelMatrix);
	RenderMesh2D(meshes["leftEye"], shaders["VertexColor"], EyeModelMatrix);
	modelMatrix = EyeModelMatrix * transform2D::Scale(1.15, 1.15);
	RenderMesh2D(meshes["meshOchi"], shaders["VertexColor"], modelMatrix);

	//ochi drept
	EyeModelMatrix = glm::mat3(1);
	EyeModelMatrix = visMatrix * transform2D::Translate(player.rX, player.rY);
	modelMatrix = EyeModelMatrix * transform2D::Scale(0.3, 0.3);
	RenderMesh2D(meshes["meshPupila"], shaders["VertexColor"], modelMatrix);
	RenderMesh2D(meshes["rightEye"], shaders["VertexColor"], EyeModelMatrix);
	modelMatrix = EyeModelMatrix * transform2D::Scale(1.15, 1.15);
	RenderMesh2D(meshes["meshOchi"], shaders["VertexColor"], modelMatrix);

	//cap
	modelMatrix = visMatrix * transform2D::Translate(player.translateX, player.translateY);
	RenderMesh2D(meshes["head"], shaders["VertexColor"], modelMatrix);

	//obstacole

	//proiectile
	if (game.openFire) {
		for (int i = 0; i < projectileNumber; i++) {
			if (!projectiles[i].outOfRange) {

				modelMatrix = glm::mat3(1);
				modelMatrix = visMatrix * transform2D::Translate(projectiles[i].projectileX, projectiles[i].projectileY);
				modelMatrix *= transform2D::Rotate(projectiles[i].angle);
				RenderMesh2D(meshes["projectile"], shaders["VertexColor"], modelMatrix);
			}

		}
	}
	for (int i = 0; i < enemyNumber; i++) {
		glm::mat3  BodmodelMatrix;
		if (!enemies[i].eliminated) {
			float px = enemies[i].x - player.translateX;
			float py = enemies[i].y - player.translateY;
			float ang2 = atan2(py, px);
			BodmodelMatrix = glm::mat3(1);

			BodmodelMatrix = visMatrix * transform2D::Translate(enemies[i].x, enemies[i].y);
			BodmodelMatrix *= transform2D::Translate(enemySize / 2, enemySize / 2);
			BodmodelMatrix *= transform2D::Rotate(ang2);
			BodmodelMatrix *= transform2D::Translate(-enemySize / 2, -enemySize / 2);
			RenderMesh2D(meshes["enemyBody"], shaders["VertexColor"], BodmodelMatrix);
			modelMatrix = glm::mat3(1);
			modelMatrix = BodmodelMatrix * transform2D::Scale(1, 1);
			RenderMesh2D(meshes["enemyBodyMesh"], shaders["VertexColor"], modelMatrix);

			modelMatrix = glm::mat3(1);
			modelMatrix = BodmodelMatrix * transform2D::Translate(enemySize / 4, enemySize);

			RenderMesh2D(meshes["enemyArmR"], shaders["VertexColor"], modelMatrix);
			modelMatrix = glm::mat3(1);
			modelMatrix = BodmodelMatrix * transform2D::Translate(enemySize / 4, -enemyArmSize);

			RenderMesh2D(meshes["enemyArmL"], shaders["VertexColor"], modelMatrix);
		}
	}
	for (int i = 0; i < nrOfHurdles; i++) {
		modelMatrix = glm::mat3(1);
		modelMatrix = visMatrix * transform2D::Translate(hurdlePosX[i], hurdlePosY[i]);
		modelMatrix *= transform2D::Scale(hurdleScalingFactorX[i], hurdleScalingFactorY[i]);
		RenderMesh2D(meshes["hurdle"], shaders["VertexColor"], modelMatrix);
	}

	if (game.powerUpHealth.x > 0 && game.powerUpHealth.y > 0) {
		modelMatrix = glm::mat3(1);
		modelMatrix = visMatrix * transform2D::Translate(game.powerUpHealth.x, game.powerUpHealth.y);
		RenderMesh2D(meshes["powerUpHealth"], shaders["VertexColor"], modelMatrix);
	}

	if (game.powerUpScore.x > 0 && game.powerUpScore.y > 0) {
		modelMatrix = glm::mat3(1);
		modelMatrix = visMatrix * transform2D::Translate(game.powerUpScore.x, game.powerUpScore.y);
		RenderMesh2D(meshes["powerUpScore"], shaders["VertexColor"], modelMatrix);
	}



}


void Tema1::Update(float deltaTimeSeconds)
{
	glLineWidth(3);
	glPointSize(5);
	glm::ivec2 res = window->GetResolution();
	game.secondsElapsedShoot += deltaTimeSeconds;
	game.secondsElapsedEnemyWave += deltaTimeSeconds;
	viewSpace = ViewportSpace(0, 0, res.x, res.y);
	SetViewportArea(viewSpace, glm::vec3(0.62f, 0.73f, 0.80f), true);
	if (game.health > 0) {

		visMatrix = glm::mat3(1);
		visMatrix *= VisualizationTransf2DUnif(logicSpace, viewSpace);
		DrawScene(visMatrix);
		FlyProjectiles(deltaTimeSeconds);
		if (game.secondsElapsedEnemyWave > 10) {
			game.secondsElapsedEnemyWave = 0;
			float newWave = 1 + rand() % 10;
			for (int i = enemyNumber; i < enemyNumber + newWave; i++) {

				int posx;
				int posy;
				float speed;

				do {
					posx = rand() % ((int)backgroundSide * 2 - 20);
					posy = rand() % ((int)backgroundSide - 5);
					speed = 1 + rand() % 3;

				} while (!avoidCenter(posx, posy, 1, 1, player.translateX + offset, player.translateY + offset));
				Point right = { posx + enemySize / 2,posy + enemySize / 2 };
				Point left = { posx + enemySize / 2,posy };
				Enemy enemy = { posx,posy,false,speed,right,left,0 };
				enemies.push_back(enemy);
				float px, py;
				do {
					px = rand() % ((int)backgroundSide * 2 - 20);
					py = rand() % ((int)backgroundSide - 5);

				} while (!notObstacle(px, py));
				game.powerUpHealth = { px,py };
				do {
					px = rand() % ((int)backgroundSide * 2 - 20);
					py = rand() % ((int)backgroundSide - 5);

				} while (!notObstacle(px, py));
				game.powerUpScore = { px,py };

			}
			enemyNumber += newWave;
		}


		MoveEnemies(deltaTimeSeconds);
		EnemyCollision(player.translateX, player.translateY);
		if (powerUpCollision(player.translateX, player.translateY, game.powerUpHealth.x, game.powerUpHealth.y)) {
			if (game.health < 100) {
				game.health += 10;
			}
			game.powerUpHealth = { 0,0 };
		}
		if (powerUpCollision(player.translateX, player.translateY, game.powerUpScore.x, game.powerUpScore.y)) {

			game.score += 500;
			cout << "Score: " << game.score << endl;
			game.powerUpScore = { 0,0 };
		}
	}
	else {
		if (!game.gameOver) {
			game.gameOver = true;
			cout << "Congrats!You got killed by a bunch of squares..." << endl;
			cout << "GAME OVER!" << endl;
			exit(0);
		}


		enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
			[&](Enemy const& enemy) {return enemy.eliminated; }), enemies.end());

		projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(),
			[&](Projectile const& proj) {return proj.outOfRange; }), projectiles.end());

	}

}

//enemies follow player
void Tema1::MoveEnemies(float deltaTimeSeconds) {
	for (int i = 0; i < enemyNumber; i++) {
		Point b1, dir;
		dir.x = player.translateX - enemies[i].x;
		dir.y = player.translateY - enemies[i].y;
		float ip = sqrt(dir.x * dir.x + dir.y * dir.y);
		dir.x /= ip;
		dir.y /= ip;
		b1 = { enemies[i].x + dir.x * enemies[i].speed * deltaTimeSeconds, enemies[i].y + dir.y * enemies[i].speed * deltaTimeSeconds };
		if (((b1.x > 2 * backgroundSide || b1.y > backgroundSide) && !enemies[i].eliminated || (b1.x < 0) || (b1.y < 0)) && (!enemies[i].eliminated)) {
			enemies[i].eliminated = true;
		}
		else {
			if ((!enemies[i].eliminated)) {
				if (checkProjectileEnemyCollision(b1, i)) {
					enemies[i].eliminated = true;
					game.score += 100;
					cout << "Score: " << game.score << endl;
				}
				else {
					enemies[i].x = b1.x;
					enemies[i].y = b1.y;

				}
			}
		}
	}
}
//projectiles follow trajectory
void Tema1::FlyProjectiles(float deltaTimeSeconds) {
	int k = 0;
	for (int i = 0; i < projectileNumber; i++) {
		Point b1, t1;
		b1 = { projectiles[i].projectileX + deltaTimeSeconds / 100 + cos(projectiles[i].angle), projectiles[i].projectileY + deltaTimeSeconds / 100 + sin(projectiles[i].angle) };
		t1 = { projectiles[i].projectileX + deltaTimeSeconds / 100 + cos(projectiles[i].angle) + projectileSize * 2, projectiles[i].projectileY + deltaTimeSeconds / 100 + sin(projectiles[i].angle) + projectileSize };
		if (((b1.x > 2 * backgroundSide || b1.y > backgroundSide) && !projectiles[i].outOfRange || (b1.x < 0) || (b1.y < 0)) && (!projectiles[i].outOfRange)
			|| checkProjectileWallCollision(b1, t1) && !projectiles[i].outOfRange) {
			projectiles[i].outOfRange = true;
		}
		else {
			if (((abs(b1.x - projectiles[i].projectileStart.x) > game.maxProjDist || abs(b1.y - projectiles[i].projectileStart.y) > game.maxProjDist)) && !projectiles[i].outOfRange
				) {
				projectiles[i].outOfRange = true;
			}
			else {
				if (!projectiles[i].outOfRange) {
					projectiles[i].projectileX += deltaTimeSeconds / 100 + cos(projectiles[i].angle);
					projectiles[i].projectileY += deltaTimeSeconds / 100 + sin(projectiles[i].angle);
				}
			}

		}

	}
}
//projectile - enemy collision
bool Tema1::checkProjectileEnemyCollision(Point b1, int j) {
	for (int i = 0; i < projectileNumber; i++) {

		if ((((b1.x + projectileSize / 2 <= projectiles[i].projectileX + projectileSize * 3) &&
			(b1.x + enemySize - projectileSize / 2 >= projectiles[i].projectileX)) &&
			((b1.y + enemySize - projectileSize / 2 >= projectiles[i].projectileY) &&
				(b1.y + projectileSize / 2 <= projectiles[i].projectileY + 3.5 * projectileSize)))
			&& !projectiles[i].outOfRange) {
			projectiles[i].outOfRange = true;
			enemies[j].eliminated = true;

			return true;
		}


	}return false;
}
//can generate obstacle?
bool Tema1::avoidCenter(int x, int y, int sx, int sy, int c1, int c2) {

	int px = max(x, min(c1, x + hurdleSize * 2 * sx));
	int py = max(y, min(c2, y + hurdleSize * sy));
	int dx = px - c1;
	int dy = py - c2;
	if (dx * dx + dy * dy <= headRad * headRad) {
		return false;
	}

	return true;

}
//player - power-up collision
bool Tema1::powerUpCollision(float cx, float cy, float x, float y) {
	float dx = cx + headRad - x - eyeRad;
	float dy = cy + headRad - y - eyeRad;
	float d = sqrt(dx * dx + dy * dy);
	return d < headRad + eyeRad;
}
//player - hurdle collsion
bool Tema1::notObstacle(int x, int y) {
	for (int i = 0; i < nrOfHurdles; i++) {
		int px = max(hurdlePosX[i], min(x, hurdlePosX[i] + hurdleSize * 2 * hurdleScalingFactorX[i]));
		int py = max(hurdlePosY[i], min(y, hurdlePosY[i] + hurdleSize * hurdleScalingFactorY[i]));
		int dx = px - x;
		int dy = py - y;
		if (dx * dx + dy * dy < headRad * headRad) {
			return false;
		}
	}
	return true;

}
//enemy - player collision
void Tema1::EnemyCollision(float x, float y) {
	for (int i = 0; i < enemyNumber; i++) {
		if (!enemies[i].eliminated) {
			int px = max(enemies[i].x, min(x, enemies[i].x + (float)enemySize));
			int py = max(enemies[i].y, min(y, enemies[i].y + (float)enemySize));
			int dx = px - x;
			int dy = py - y;
			if (dx * dx + dy * dy < headRad * headRad) {
				game.health -= 10;
				enemies[i].eliminated = true;
			}
		}

	}

}
//projectile - wall collision
bool Tema1::checkProjectileWallCollision(Point b1, Point t1) {
	for (int i = 0; i < nrOfHurdles; i++) {

		if (((b1.x + projectileSize / 2 < hurdlePosX[i] + hurdleSize * 2 * hurdleScalingFactorX[i] && b1.x - projectileSize / 2 > hurdlePosX[i]) &&
			b1.y - projectileSize / 2 > hurdlePosY[i] && b1.y + projectileSize / 2 < hurdlePosY[i] + hurdleSize * hurdleScalingFactorY[i])) {
			return true;
		}


	}return false;
}


void Tema1::FrameEnd()
{
}


void Tema1::OnInputUpdate(float deltaTime, int mods)
{
	auto camera = GetSceneCamera();
	if (window->KeyHold(GLFW_KEY_W)) {
		if ((player.translateY + deltaTime * 2.5f < backgroundSide - headRad - eyeRad / 2) && notObstacle(player.translateX, player.translateY + deltaTime * 2.5)) {

			player.translateY += deltaTime * 2.5f;
			logicSpace.y += deltaTime * 2.5f;
			player.rY += deltaTime * 2.5f;
			player.lY += deltaTime * 2.5f;
		}
	}
	if (window->KeyHold(GLFW_KEY_A)) {
		if ((player.translateX - deltaTime * 2.5f > 0 + headRad + eyeRad / 2) && notObstacle(player.translateX - deltaTime * 2.5, player.translateY)) {
			player.translateX -= deltaTime * 2.5f;
			logicSpace.x -= deltaTime * 2.5f;
			player.rX -= deltaTime * 2.5f;
			player.lX -= deltaTime * 2.5f;

		}
	}

	if (window->KeyHold(GLFW_KEY_S)) {
		if ((player.translateY - deltaTime * 2.5f > 0 + headRad + eyeRad / 2) && notObstacle(player.translateX, player.translateY - deltaTime * 2.5)) {

			player.translateY -= deltaTime * 2.5f;
			logicSpace.y -= deltaTime * 2.5f;
			player.rY -= deltaTime * 2.5f;
			player.lY -= deltaTime * 2.5f;
		}

	}
	if (window->KeyHold(GLFW_KEY_D)) {
		if ((player.translateX + deltaTime * 2.5f < backgroundSide * 2 - headRad - eyeRad / 2) && notObstacle(player.translateX + deltaTime * 2.5, player.translateY)) {

			player.translateX += deltaTime * 2.5f;
			logicSpace.x += deltaTime * 2.5;
			player.rX += deltaTime * 2.5f;
			player.lX += deltaTime * 2.5f;

		}
	}
	if (window->KeyHold(GLFW_KEY_Z)) {
		logicSpace.width += deltaTime * 5;
		logicSpace.height += deltaTime * 5;
	}
	if (window->KeyHold(GLFW_KEY_X)) {
		logicSpace.width -= deltaTime * 5;
		logicSpace.height -= deltaTime * 5;

	}


}


void Tema1::OnKeyPress(int key, int mods)
{

}


void Tema1::OnKeyRelease(int key, int mods)
{

}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{

	glm::ivec2 res = window->GetResolution();
	float px = mouseX - res.x / 2;
	float py = res.y / 2 - mouseY;
	float ang2 = atan2(py, px) - AI_MATH_PI / 2;

	player.angularStep = AI_MATH_PI / 2 + eyeDist + ang2;
	player.angularStep2 = AI_MATH_PI / 2 - eyeDist + ang2;
	player.rX = player.translateX + headRad * cos(player.angularStep);
	player.rY = player.translateY + headRad * sin(player.angularStep);
	player.lX = player.translateX + headRad * cos(player.angularStep2);
	player.lY = player.translateY + headRad * sin(player.angularStep2);

}



void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{

	if (window->MouseHold(GLFW_MOUSE_BUTTON_LEFT)) {
		if (game.secondsElapsedShoot > game.fireRate) {
			game.openFire = true;
			game.secondsElapsedShoot = 0;

			glm::ivec2 res = window->GetResolution();
			float px = mouseX - res.x / 2;
			float py = res.y / 2 - mouseY;
			float ang2 = atan2(py, px);
			Point start = { player.translateX ,player.translateY };
			Projectile projectile = { start.x  ,start, start.y,
			false, ang2 };
			projectiles.push_back(projectile);
			projectileNumber++;
		}

	}




}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema1::OnWindowResize(int width, int height)
{
}
