#include "lab_m1/Tema2/Tema2.h"
#include "lab_m1/Tema2/lab_camera.h"

#include <vector>
#include <string>
#include <iostream>
#include <stack>
#include <cmath>
#include <ctime>

using namespace std;
using namespace m1;

int mazeMat[h][w];
stack<pair<int, int>> s;


Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}

void Tema2::Init() {
	int x, y;
	init.first = h / 2;
	init.second = w / 2;
	x = init.first;
	y = init.second;
	s.push(make_pair(x, y));
	while (!s.empty()) {
		pair<int, int> next = s.top();
		s.pop();
		if (isValid(next)) {
			mazeMat[next.second][next.first] = 1;
			vector<pair<int, int>> neighbors = findNeighbors(next);
			addNodes(neighbors);
		}
	}
	camera = new t1::Camera();

	player.playerPos = glm::vec3(init.first, 0, init.second);
	player.playerRotation = glm::vec3(0, 1, 0);
	mazeMat[init.first][init.second] = 1;

	camera->position.x = init.first - 1;
	camera->position.z = init.second - 1;
	camera->position.y = 3;
	thirdPersonCameraLatestPosition = camera->position;

	nrOfEnemies = h + rand() % h + w;
	for (int i = 0; i < nrOfEnemies; i++) {
		bool canPlaceEnemy = false;
		do {
			int px = 1 + rand() % h - 1;
			int py = 1 + rand() % w - 1;
			if (mazeMat[px][py] == 1 && px != x && py != y) {
				canPlaceEnemy = true;
				mazeMat[px][py] = -1;
				Enemy e = { glm::vec3(px,0,py),false,glm::vec3(px ,0,py),0 };
				enemies.push_back(e);
			}

		} while (!canPlaceEnemy);

	}

	const string sourceTextureDir = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "textures");
	{

		{
			Texture2D* texture = new Texture2D();
			texture->Load2D(PATH_JOIN(sourceTextureDir, "wall.jpg").c_str(), GL_REPEAT);
			mapTextures["wall"] = texture;
		}

		{
			Texture2D* texture = new Texture2D();
			texture->Load2D(PATH_JOIN(sourceTextureDir, "ground.jpg").c_str(), GL_REPEAT);
			mapTextures["groundT"] = texture;
		}
		{
			Texture2D* texture = new Texture2D();
			texture->Load2D(PATH_JOIN(sourceTextureDir, "fire.jpg").c_str(), GL_REPEAT);
			mapTextures["fire"] = texture;
		}
		{
			Texture2D* texture = new Texture2D();
			texture->Load2D(PATH_JOIN(sourceTextureDir, "enemy.jpg").c_str(), GL_REPEAT);
			mapTextures["enemy"] = texture;
		}

	}


	{
		Mesh* mesh = new Mesh("box");
		mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("sphere");
		mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}
	{
		Mesh* mesh = new Mesh("hat");
		mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "hat.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}
	{
		Mesh* mesh = new Mesh("wand");
		mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "wand.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}



	{
		vector<VertexFormat> vertices
		{
			VertexFormat(glm::vec3(-1, -1,  1), glm::vec3(0, 1, 1)),
			VertexFormat(glm::vec3(1, -1,  1), glm::vec3(1, 0, 1)),
			VertexFormat(glm::vec3(-1,  1,  1), glm::vec3(1, 0, 0)),
			VertexFormat(glm::vec3(1,  1,  1), glm::vec3(0, 1, 0)),
			VertexFormat(glm::vec3(-1, -1, -1), glm::vec3(1, 1, 1)),
			VertexFormat(glm::vec3(1, -1, -1), glm::vec3(0, 1, 1)),
			VertexFormat(glm::vec3(-1,  1, -1), glm::vec3(1, 1, 0)),
			VertexFormat(glm::vec3(1,  1, -1), glm::vec3(0, 0, 1)),
		};

		vector<unsigned int> indices =
		{
			0, 1, 2,        1, 3, 2,
			2, 3, 7,        2, 7, 6,
			1, 7, 3,        1, 5, 7,
			6, 7, 4,        7, 5, 4,
			0, 4, 1,        1, 4, 5,
			2, 6, 4,        0, 2, 4,
		};

		CreateMesh("cube", vertices, indices);
	}
	{
		vector<unsigned int> sqInd =
		{
			1, 2, 3,
			0, 3, 1
		};
		vector<VertexFormat> sqVert =
		{
			VertexFormat(glm::vec3(0, 0, 0), glm::vec3(1.f, 1.f, 1.f)),
			VertexFormat(glm::vec3(0, 1, 0),  glm::vec3(1.f, 1.f, 1.f)),
			VertexFormat(glm::vec3(0, 1 , 1),  glm::vec3(1.f, 1.f, 1.f)),
			VertexFormat(glm::vec3(0, 0, 1),  glm::vec3(1.f, 1.f, 1.f))

		};
		CreateMesh("square", sqVert, sqInd);
	}
	Mesh* healthBarWire = Tema2::CreateRectangle("healthBarWire", corner, healthBarSize, glm::vec3(0.556863, 0.137255, 0.137255), false);
	AddMeshToList(healthBarWire);
	Mesh* healthBarFull = Tema2::CreateRectangle("healthBarFull", corner, healthBarSize, glm::vec3(0.32, 1.0, 0.498039), true);
	AddMeshToList(healthBarFull);
	Mesh* timeBarFull = Tema2::CreateRectangle("timeBarFull", corner, healthBarSize, glm::vec3(0.35, 0.35, 0.67), true);
	AddMeshToList(timeBarFull);

	{
		Shader* shader = new Shader("SimpleColorShader");
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;

		Shader* shaderTexture = new Shader("TextureShader");
		shaderTexture->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShaderT.glsl"), GL_VERTEX_SHADER);
		shaderTexture->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShaderT.glsl"), GL_FRAGMENT_SHADER);
		shaderTexture->CreateAndLink();
		shaders[shaderTexture->GetName()] = shaderTexture;

		Shader* shaderNoise = new Shader("NoiseShader");
		shaderNoise->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShaderDisp.glsl"), GL_VERTEX_SHADER);
		shaderNoise->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShaderDisp.glsl"), GL_FRAGMENT_SHADER);
		shaderNoise->CreateAndLink();
		shaders[shaderNoise->GetName()] = shaderNoise;

	}
	projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);
	logicSpace.x = 0;
	logicSpace.y = 0;
	logicSpace.width = 32;
	logicSpace.height = 32;

}

Mesh* Tema2::CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices)
{
	unsigned int VAO = 0;
	// Create the VAO and bind it
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Create the VBO and bind it
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Send vertices data into the VBO buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// Create the IBO and bind it
	unsigned int IBO;
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	// Send indices data into the IBO buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);


	// Set vertex position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

	// Set vertex normal attribute
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

	// Set texture coordinate attribute
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

	// Set vertex color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));
	// ========================================================================

	// Unbind the VAO
	glBindVertexArray(0);

	// Check for OpenGL errors
	CheckOpenGLError();

	// Mesh information is saved into a Mesh object
	meshes[name] = new Mesh(name);
	meshes[name]->InitFromBuffer(VAO, static_cast<unsigned int>(indices.size()));
	meshes[name]->vertices = vertices;
	meshes[name]->indices = indices;
	return meshes[name];
}

void Tema2::FrameStart()
{
	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// Sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Tema2::Update(float deltaTimeSeconds)
{
	auto res = window->GetResolution();
	auto camera2D = GetSceneCamera();
	camera2D->SetPosition(glm::vec3(0, 0, 50));
	camera2D->SetRotation(glm::vec3(0, 0, 0));
	camera2D->Update();
	viewSpace = ViewportSpace(0, 0, res.x, res.y);

	GetCameraInput()->SetActive(false);
	SetViewportArea(viewSpace, glm::vec3(0.62f, 0.73f, 0.80f), true);
	visMatrix = glm::mat3(1);
	visMatrix *= VisualizationTransf2DUnif(logicSpace, viewSpace);

	auto modelMatrix = glm::mat3(1);
	float wireX = logicSpace.width;
	float wireY = logicSpace.height - healthBarSize;
	modelMatrix = visMatrix * Tema2::Translate(wireX, wireY);
	modelMatrix *= Tema2::Scale(2, 0.5);
	RenderMesh2D(meshes["healthBarWire"], shaders["VertexColor"], modelMatrix);

	modelMatrix = glm::mat3(1);
	modelMatrix = visMatrix * Tema2::Translate(wireX, wireY);
	modelMatrix *= Tema2::Scale(game.health / 100 * 2, 0.5);
	RenderMesh2D(meshes["healthBarFull"], shaders["VertexColor"], modelMatrix);

	modelMatrix = glm::mat3(1);
	wireX = logicSpace.width;
	wireY = logicSpace.height - 2 * healthBarSize;
	modelMatrix = visMatrix * Tema2::Translate(wireX, wireY);
	modelMatrix *= Tema2::Scale(2, 0.5);
	RenderMesh2D(meshes["healthBarWire"], shaders["VertexColor"], modelMatrix);

	modelMatrix = glm::mat3(1);
	modelMatrix = visMatrix * Tema2::Translate(wireX, wireY);
	modelMatrix *= Tema2::Scale((timeToEscape - game.time) / timeToEscape * 2, 0.5);
	RenderMesh2D(meshes["timeBarFull"], shaders["VertexColor"], modelMatrix);

	GetCameraInput()->SetActive(true);


	{
		RemoveOutOfRangeProjectiles();
		RemoveEliminatedEnemies();
		DrawScene(deltaTimeSeconds);

		if (thirdPersonView) {
			camera->Set(camera->position, player.playerPos, glm::vec3(0, 1, 0));
			thirdPersonCameraLatestPosition = camera->position;
		}

		FlyProjectiles(deltaTimeSeconds);
		MoveEnemies(deltaTimeSeconds);

		game.time = Engine::GetElapsedTime();
		if (game.health <= 0 || game.time > timeToEscape) {
			game.gameOver = true;
		}
		if (game.gameOver) {
			cout << "GAME OVER!" << endl;
			exit(0);
		}
		if (player.playerPos.x >= w || player.playerPos.z >= h || player.playerPos.x < 0 || player.playerPos.z < 0) {
			cout << "YOU ESCAPED!" << endl;
			exit(0);
		}

	}

}

void Tema2::DrawScene(float deltaTimeSeconds) {

	GenerateMaze();
	GeneratePlayer();
	GenerateEnemies(deltaTimeSeconds);
	if (game.openFire) {
		GenerateProjectiles();
	}

}

void Tema2::GenerateMaze() {
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(i, -1, j));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(1.f, 1.f, 1.f));
			RenderSimpleMesh(meshes["box"], shaders["TextureShader"], modelMatrix, mapTextures["groundT"]);
			if (!mazeMat[i][j]) {
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(i, 0, j));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(1.f, 1.f, 1.f));
				RenderSimpleMesh(meshes["box"], shaders["TextureShader"], modelMatrix, mapTextures["wall"]);
				Wall wall;
				wall.x = i;
				wall.y = 0;
				wall.z = j;
				walls.push_back(wall);
				nrOfWalls++;
			}

		}
	}
}

void Tema2::GeneratePlayer() {
	glm::vec3 pos = player.playerPos;
	glm::mat4 modelMatrix = glm::mat4(1);
	auto  modelMatrix2 = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(pos.x, pos.y, pos.z));
	modelMatrix = glm::rotate(modelMatrix, RADIANS(direction), player.playerRotation);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1, 0.15, 0.1));


	//pantaloni
	modelMatrix2 = glm::translate(modelMatrix, glm::vec3(-2, 0, 0));
	glm::vec4 color = glm::vec4(0.137255, 0.419608, 0.556863, 1);
	RenderMesh(meshes["cube"], shaders["SimpleColorShader"], modelMatrix2, color);

	modelMatrix2 = glm::mat4(1);
	modelMatrix2 = glm::translate(modelMatrix, glm::vec3(1, 0, 0));
	color = glm::vec4(0.137255, 0.419608, 0.556863, 1);
	RenderMesh(meshes["cube"], shaders["SimpleColorShader"], modelMatrix2, color);

	//torso
	modelMatrix2 = glm::mat4(1);
	modelMatrix2 = glm::translate(modelMatrix, glm::vec3(-0.5, 2.75, 0));
	modelMatrix2 = glm::scale(modelMatrix2, glm::vec3(2.5, 1.5, 1));
	color = glm::vec4(0.372549, 0.372549, 0.623529, 1);
	RenderMesh(meshes["cube"], shaders["SimpleColorShader"], modelMatrix2, color);

	//maneci
	modelMatrix2 = glm::mat4(1);
	modelMatrix2 = glm::translate(modelMatrix, glm::vec3(-3.75, 3.25, 0));
	modelMatrix2 = glm::scale(modelMatrix2, glm::vec3(0.5, 1, 1));
	color = glm::vec4(0.372549, 0.372549, 0.623529, 1);
	RenderMesh(meshes["cube"], shaders["SimpleColorShader"], modelMatrix2, color);

	modelMatrix2 = glm::mat4(1);
	modelMatrix2 = glm::translate(modelMatrix, glm::vec3(2.75, 3.25, 0));
	modelMatrix2 = glm::scale(modelMatrix2, glm::vec3(0.5, 1, 1));
	color = glm::vec4(0.372549, 0.372549, 0.623529, 1);
	RenderMesh(meshes["cube"], shaders["SimpleColorShader"], modelMatrix2, color);

	//manute
	modelMatrix2 = glm::mat4(1);
	modelMatrix2 = glm::translate(modelMatrix, glm::vec3(-3.75, 1.5, 0));
	modelMatrix2 = glm::scale(modelMatrix2, glm::vec3(0.5, 0.5, 1));
	color = glm::vec4(0.858824, 0.576471, 0.439216, 1);
	RenderMesh(meshes["cube"], shaders["SimpleColorShader"], modelMatrix2, color);

	modelMatrix2 = glm::mat4(1);
	modelMatrix2 = glm::translate(modelMatrix, glm::vec3(2.75, 1.5, 0));
	modelMatrix2 = glm::scale(modelMatrix2, glm::vec3(0.5, 0.5, 1));
	color = glm::vec4(0.858824, 0.576471, 0.439216, 1);
	RenderMesh(meshes["cube"], shaders["SimpleColorShader"], modelMatrix2, color);

	//cap
	modelMatrix2 = glm::mat4(1);
	modelMatrix2 = glm::translate(modelMatrix, glm::vec3(-0.5, 5.25, 0));
	modelMatrix2 = glm::scale(modelMatrix2, glm::vec3(1, 0.75, 0.75));
	color = glm::vec4(0.858824, 0.576471, 0.439216, 1);
	RenderMesh(meshes["cube"], shaders["SimpleColorShader"], modelMatrix2, color);

	//lil wizard hat
	color = glm::vec4(0.372549, 0.372549, 0.623529, 1);
	modelMatrix2 = glm::mat4(1);
	modelMatrix2 = glm::translate(modelMatrix, glm::vec3(0.25, 8, -1.5));
	modelMatrix2 = glm::rotate(modelMatrix2, RADIANS(270), glm::vec3(abs(pos.x), 0, 0));
	modelMatrix2 = glm::scale(modelMatrix2, glm::vec3(0.1f));
	RenderMesh(meshes["hat"], shaders["SimpleColorShader"], modelMatrix2, color);

	//lil wizard wand
	color = glm::vec4(0.55, 0.47, 0.14, 1);
	modelMatrix2 = glm::mat4(1);
	modelMatrix2 = glm::translate(modelMatrix, glm::vec3(-3.25, 1.5, 0));
	modelMatrix2 = glm::rotate(modelMatrix2, RADIANS(180), glm::vec3(abs(pos.x), 0, 0));
	modelMatrix2 = glm::scale(modelMatrix2, glm::vec3(0.25f));
	RenderMesh(meshes["wand"], shaders["SimpleColorShader"], modelMatrix2, color);
}
bool Tema2::checkPlayerWallCollision(glm::vec3 futurePos) {
	for (int i = 0; i < nrOfWalls; i++) {

		if (futurePos.x <= walls[i].x + wallCollisionOffset && futurePos.x + wallCollisionOffset > walls[i].x &&
			futurePos.z < walls[i].z + wallCollisionOffset && futurePos.z + wallCollisionOffset > walls[i].z) {
			return true;
		}


	}return false;
}
bool Tema2::checkPlayerEnemyCollision(glm::vec3 futurePos) {
	for (int i = 0; i < nrOfEnemies; i++) {
		if (!enemies[i].eliminated) {
			if (futurePos.x <= enemies[i].startPos.x + 2 * enemyCollisionOffset && futurePos.x + 2 * enemyCollisionOffset >= enemies[i].startPos.x &&
				futurePos.z <= enemies[i].startPos.z + 2 * enemyCollisionOffset && futurePos.z + 2 * enemyCollisionOffset >= enemies[i].startPos.z) {
				return true;
			}
		}

	}return false;
}

void Tema2::GenerateEnemies(float deltaTimeSeconds) {
	for (int i = 0; i < nrOfEnemies; i++) {
		if (!enemies[i].eliminated) {
			if (projectileNumber == 0) {
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, enemies[i].pos);
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f, 0.3f, 0.3f));
				RenderSimpleMesh(meshes["box"], shaders["TextureShader"], modelMatrix, mapTextures["enemy"]);
			}
			else {
				for (int p = 0; p < projectileNumber; p++) {
					if (!projectiles[p].outOfRange) {
						if (ProjectileEnemyCollision(projectiles[p].pos, enemies[i].pos)) {
							projectiles[p].outOfRange = true;
							enemies[i].eliminated = true;
							glm::mat4 modelMatrix = glm::mat4(1);
							modelMatrix = glm::translate(modelMatrix, enemies[i].pos);
							modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f, 0.3f, 0.3f));
							enemies[i].animationDuration += deltaTimeSeconds;
							RenderMeshAnimation(meshes["box"], shaders["NoiseShader"], modelMatrix, mapTextures["enemy"]);
						}
						else {
							glm::mat4 modelMatrix = glm::mat4(1);
							modelMatrix = glm::translate(modelMatrix, enemies[i].startPos);
							modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f, 0.3f, 0.3f));
							RenderSimpleMesh(meshes["box"], shaders["TextureShader"], modelMatrix, mapTextures["enemy"]);
						}
					}
				}
			}
		}
		else if (enemies[i].eliminated && enemies[i].animationDuration < 5) {
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, enemies[i].pos);
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f, 0.3f, 0.3f));
			enemies[i].animationDuration += deltaTimeSeconds;
			RenderMeshAnimation(meshes["box"], shaders["NoiseShader"], modelMatrix, mapTextures["enemy"]);
		}
	}
}
void Tema2::MoveEnemies(float deltaTimeSeconds) {
	for (int i = 0; i < nrOfEnemies; i++) {

		float s = sin(deltaTimeSeconds * enemySpeed * enemySpeed);
		float c = cos(deltaTimeSeconds * enemySpeed * enemySpeed);

		enemies[i].pos.x -= enemies[i].startPos.x + enemyMaxRad;
		enemies[i].pos.z -= enemies[i].startPos.z + enemyMaxRad;

		float xnew = enemies[i].pos.x * c - enemies[i].pos.z * s;
		float ynew = enemies[i].pos.x * s + enemies[i].pos.z * c;

		enemies[i].pos.x = xnew + enemies[i].startPos.x + enemyMaxRad;
		enemies[i].pos.z = ynew + enemies[i].startPos.z + enemyMaxRad;
	}
}
void Tema2::RemoveEliminatedEnemies() {
	enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
		[&](Enemy const& e) {return e.eliminated && e.animationDuration >= animDuration; }), enemies.end());
	nrOfEnemies = enemies.size();
}

void Tema2::GenerateProjectiles() {
	for (int i = 0; i < projectileNumber; i++) {
		if (!projectiles[i].outOfRange) {

			auto modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, projectiles[i].pos);
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2, 0.2, 0.2));
			RenderSimpleMesh(meshes["sphere"], shaders["TextureShader"], modelMatrix, mapTextures["fire"]);

		}

	}
}
void Tema2::FlyProjectiles(float deltaTimeSeconds) {
	for (int i = 0; i < projectileNumber; i++) {
		if (ProjectileBoxCollision(projectiles[i].pos) || !Ttl(projectiles[i])) {
			projectiles[i].outOfRange = true;
		}
		else {
			glm::vec3 deltaproj = deltaTimeSeconds * projSpeed * projectiles[i].aimDir;
			projectiles[i].pos += deltaproj;
		}

	}
}
bool Tema2::Ttl(Projectile p) {
	float dist = sqrt(pow(p.start.x - p.pos.x, 2) + pow(p.start.y - p.pos.y, 2) + pow(p.start.z - p.pos.z, 2));
	if (dist >= game.maxProjDist) {
		return false;
	}
	return true;
}
bool Tema2::ProjectileBoxCollision(glm::vec3 c) {
	for (int i = 0; i < nrOfWalls; i++) {

		if (c.x + enemyCollisionOffset >= walls[i].x && c.x + enemyCollisionOffset <= walls[i].x + 1 &&
			c.y + enemyCollisionOffset >= walls[i].y && c.y + enemyCollisionOffset <= walls[i].y + 1 &&
			c.z + enemyCollisionOffset >= walls[i].z && c.z + enemyCollisionOffset <= walls[i].z + 1) {
			return true;
		}

	}return false;

}
bool Tema2::ProjectileEnemyCollision(glm::vec3 c, glm::vec3 enemyPos) {


	if (c.x + enemyCollisionOffset >= enemyPos.x && c.x + enemyCollisionOffset <= enemyPos.x + 1 &&
		c.y + enemyCollisionOffset >= enemyPos.y && c.y + enemyCollisionOffset <= enemyPos.y + 1 &&
		c.z + enemyCollisionOffset >= enemyPos.z && c.z + enemyCollisionOffset <= enemyPos.z + 1) {
		return true;
	}

	return false;

}
void Tema2::RemoveOutOfRangeProjectiles() {
	projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(),
		[&](Projectile const& proj) {return proj.outOfRange; }), projectiles.end());
	projectileNumber = projectiles.size();
}


void Tema2::FrameEnd()
{
}

//pt culoare
void Tema2::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec4& color)
{
	if (!mesh || !shader || !shader->program)
		return;

	glUseProgram(shader->program);

	int location = glGetUniformLocation(shader->GetProgramID(), "Model");

	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	int location2 = glGetUniformLocation(shader->GetProgramID(), "View");
	glm::mat4 viewMatrix = camera->GetViewMatrix();
	glUniformMatrix4fv(location2, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	int location3 = glGetUniformLocation(shader->GetProgramID(), "Projection");
	glm::mat4 projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, zNear, zFar);
	glUniformMatrix4fv(location3, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	int col = glGetUniformLocation(shader->GetProgramID(), "Color");
	glUniform4fv(col, 1, glm::value_ptr(color));

	glBindVertexArray(mesh->GetBuffers()->m_VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);


}
//pt textura
void Tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	glUseProgram(shader->program);

	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	glm::mat4 viewMatrix = camera->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	glm::mat4 projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, zNear, zFar);
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	if (texture)
	{

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
		glUniform1i(glGetUniformLocation(shader->program, "texture"), 0);

	}

	glBindVertexArray(mesh->GetBuffers()->m_VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}
//pt vertex displacement
void Tema2::RenderMeshAnimation(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture)
{
	if (!mesh || !shader || !shader->program)
		return;

	glUseProgram(shader->program);
	int location = glGetUniformLocation(shader->GetProgramID(), "Model");

	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	int location2 = glGetUniformLocation(shader->GetProgramID(), "View");
	glm::mat4 viewMatrix = camera->GetViewMatrix();
	glUniformMatrix4fv(location2, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	int location3 = glGetUniformLocation(shader->GetProgramID(), "Projection");
	glm::mat4 projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, zNear, zFar);
	glUniformMatrix4fv(location3, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	if (texture)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
		glUniform1i(glGetUniformLocation(shader->program, "texture"), 0);

	}

	glUniform1f(
		glGetUniformLocation(shader->program, "time"),
		(GLfloat)Engine::GetElapsedTime()
	);

	glBindVertexArray(mesh->GetBuffers()->m_VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);


}


void Tema2::OnInputUpdate(float deltaTime, int mods)
{
	float cameraSpeed = 2.0f;
	if (thirdPersonView) {
		if (window->KeyHold(GLFW_KEY_W)) {

			if (!checkPlayerWallCollision(glm::vec3(player.playerPos.x, player.playerPos.y, player.playerPos.z + cameraSpeed * deltaTime))) {
				player.playerPos.z += cameraSpeed * deltaTime;
				camera->position.z += cameraSpeed * deltaTime;
			}
			direction = 0;
		}



		if (window->KeyHold(GLFW_KEY_A)) {

			if (!checkPlayerWallCollision(glm::vec3(player.playerPos.x + cameraSpeed * deltaTime, player.playerPos.y, player.playerPos.z))) {

				player.playerPos.x += cameraSpeed * deltaTime;
				camera->position.x += cameraSpeed * deltaTime;
			}
			direction = 90;

		}

		if (window->KeyHold(GLFW_KEY_S)) {

			if (!checkPlayerWallCollision(glm::vec3(player.playerPos.x, player.playerPos.y, player.playerPos.z - cameraSpeed * deltaTime))) {

				player.playerPos.z -= cameraSpeed * deltaTime;
				camera->position.z -= cameraSpeed * deltaTime;
			}

			direction = 180;
		}

		if (window->KeyHold(GLFW_KEY_D)) {

			if (!checkPlayerWallCollision(glm::vec3(player.playerPos.x - cameraSpeed * deltaTime, player.playerPos.y, player.playerPos.z))) {

				player.playerPos.x -= cameraSpeed * deltaTime;
				camera->position.x -= cameraSpeed * deltaTime;
			}
			direction = 270;
		}
	}
	if (checkPlayerEnemyCollision(player.playerPos)) {
		if (game.cooldown == 0) {
			game.health -= 20;
			game.cooldown = collisionCooldown;
		}

	}
	else {
		if (game.cooldown < 0) {
			game.cooldown = 0;
		}
		else {
			game.cooldown -= deltaTime;
		}
	}


}


void Tema2::OnKeyPress(int key, int mods)
{
	// Add key press event
	if (key == GLFW_KEY_LEFT_CONTROL) {
		if (thirdPersonView) {
			thirdPersonView = false;
			glm::vec3 target = camera->GetTargetPosition();
			glm::mat4 modelMatrix = glm::mat4(1);
			if (direction == 0) {
				camera->Set(glm::vec3(player.playerPos.x, player.playerPos.y + 0.8, player.playerPos.z + 0.2), glm::vec3(player.playerPos.x, player.playerPos.y, player.playerPos.z + 5), glm::vec3(0, 1, 0));
			}
			if (direction == 180) {
				camera->Set(glm::vec3(player.playerPos.x, player.playerPos.y + 0.8, player.playerPos.z - 0.2), glm::vec3(player.playerPos.x, player.playerPos.y, player.playerPos.z - 5), glm::vec3(0, 1, 0));

			}
			if (direction == 90) {
				camera->Set(glm::vec3(player.playerPos.x + 0.2, player.playerPos.y + 0.8, player.playerPos.z), glm::vec3(player.playerPos.x + 5, player.playerPos.y, player.playerPos.z), glm::vec3(0, 1, 0));

			}
			if (direction == 270) {
				camera->Set(glm::vec3(player.playerPos.x - 0.2, player.playerPos.y + 0.8, player.playerPos.z), glm::vec3(player.playerPos.x - 5, player.playerPos.y, player.playerPos.z), glm::vec3(0, 1, 0));

			}

		}
		else {
			thirdPersonView = true;
			camera->Set(thirdPersonCameraLatestPosition, player.playerPos, glm::vec3(0, 1, 0));

		}

	}


}


void Tema2::OnKeyRelease(int key, int mods)
{

}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{


	float sensivityOX = 0.003f;
	float sensivityOY = 0.003f;

	if (window->GetSpecialKeyState() == 0 && !thirdPersonView) {

		camera->RotateFirstPerson_OX(sensivityOX * (-deltaY));
		camera->RotateFirstPerson_OY(sensivityOY * (-deltaX));

	}


}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{

	if (window->MouseHold(GLFW_MOUSE_BUTTON_LEFT) && !thirdPersonView) {
		game.openFire = true;

		Projectile projectile = { false,player.playerPos,
		glm::vec3(camera->forward.x,camera->forward.y + 0.3,camera->forward.z),player.playerPos };
		projectiles.push_back(projectile);
		projectileNumber++;

	}

}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{

}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}


bool Tema2::isValid(pair<int, int> node) {
	int ones = 0;
	for (int y = node.second - 1; y < node.second + 2; y++) {
		for (int x = node.first - 1; x < node.first + 2; x++) {
			if (inside(x, y) && notNode(node, x, y) && mazeMat[y][x] == 1) {
				ones++;
			}
		}
	}
	return (ones < 3) && mazeMat[node.second][node.first] != 1;
}
void Tema2::addNodes(vector<pair<int, int>> nodes) {
	int targetIndex;
	while (!nodes.empty()) {
		targetIndex = rand() % (nodes.size());
		s.push(nodes[targetIndex]);
		nodes.erase(nodes.begin() + targetIndex);
	}
}
vector<pair<int, int>> Tema2::findNeighbors(pair<int, int> next) {
	vector<pair<int, int>> neighbors;
	for (int y = next.second - 1; y < next.second + 2; y++) {
		for (int x = next.first - 1; x < next.first + 2; x++) {
			if (inside(x, y) && notCorner(next, x, y)
				&& notNode(next, x, y)) {
				neighbors.emplace_back(x, y);
			}
		}
	}
	return neighbors;
}
bool Tema2::notCorner(pair<int, int> next, int x, int y) {
	return (x == next.first || y == next.second);
}
bool Tema2::inside(int x, int y) {
	return x >= 0 && y >= 0 && x < h&& y < w;
}
bool Tema2::notNode(pair<int, int> next, int x, int y) {
	return !(x == next.first && y == next.second);
}

void Tema2::SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor, bool clear)
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
glm::mat3 Tema2::VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace)
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
void Tema2::GenerateHealthBarAndTimeLeft() {
	ViewportSpace viewSpace;
	LogicSpace logicSpace;
	glClear(GL_DEPTH_BUFFER_BIT);
	auto res = window->GetResolution();
	viewSpace = ViewportSpace(res.x / 7, 0, 0, 0);
	SetViewportArea(viewSpace, glm::vec3(0.62f, 0.73f, 0.80f), true);
	logicSpace.x = 0;
	logicSpace.y = 0;
	logicSpace.width = 6;
	logicSpace.height = 6;
	glm::mat3 visMatrix = glm::mat3(1);
	visMatrix *= VisualizationTransf2DUnif(logicSpace, viewSpace);
	glm::mat3 modelMatrix = glm::mat3(1);
	float wireX = 1;
	float wireY = 1;
	modelMatrix = visMatrix * Translate(wireX, wireY);
	RenderMesh2D(meshes["healthBarWire"], shaders["VertexColor"], modelMatrix);
	modelMatrix = glm::mat3(1);
	modelMatrix = visMatrix * Translate(0, 0);
	modelMatrix *= Scale(game.health / 100, 1);
	RenderMesh2D(meshes["healthBarFull"], shaders["VertexColor"], modelMatrix);
	RenderMesh(meshes["square"], shaders["SimpleColorShader"], modelMatrix, glm::vec4(0.32, 1.0, 0.498039, 1));
}
Mesh* Tema2::CreateRectangle(
	const std::string& name,
	glm::vec3 leftBottomCorner,
	float length,
	glm::vec3 color,
	bool fill)
{
	glm::vec3 corner = leftBottomCorner;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(2 * length, 0, 0), color),
		VertexFormat(corner + glm::vec3(2 * length, length, 0), color),
		VertexFormat(corner + glm::vec3(0, length, 0), color)
	};

	Mesh* square = new Mesh(name);
	std::vector<unsigned int> indices = { 0, 1, 2, 3 };

	if (!fill) {
		square->SetDrawMode(GL_LINE_LOOP);
	}
	else {
		indices.push_back(0);
		indices.push_back(2);
	}


	square->InitFromData(vertices, indices);
	return square;
}
glm::mat3 Tema2::Translate(float translateX, float translateY)
{
	return glm::transpose(
		glm::mat3(1, 0, translateX,
			0, 1, translateY,
			0, 0, 1));

}
glm::mat3 Tema2::Scale(float scaleX, float scaleY)
{
	return glm::transpose(
		glm::mat3(scaleX, 0, 0,
			0, scaleY, 0,
			0, 0, 1));

}