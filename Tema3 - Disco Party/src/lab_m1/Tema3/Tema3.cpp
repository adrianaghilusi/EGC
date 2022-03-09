#include "lab_m1/Tema3/Tema3.h"

#include <vector>
#include <string>
#include <iostream>
#include<windows.h>
#include <mmsystem.h>
#include<conio.h>

using namespace std;
using namespace m1;
#define n 8
glm::vec3 dancefloor[n][n];



Tema3::Tema3()
{
}


Tema3::~Tema3()
{
}


void Tema3::Init()
{
	char filename[] = "rick-astley-never-gonna-give-you-up.wav";
	char fullFilename[MAX_PATH];

	GetFullPathName(filename, MAX_PATH, fullFilename, nullptr);

	//PlaySound(fullFilename, NULL, SND_FILENAME | SND_ASYNC);
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
		Mesh* mesh = new Mesh("plane");
		mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "plane50.obj");
		meshes[mesh->GetMeshID()] = mesh;

		Mesh* disk = CreateCircle("disk", 100, glm::vec3(0.50, 0.71, 0.49), true, 1);
		AddMeshToList(disk);
	}

	// Create a shader program for drawing face polygon with the color of the normal
	{
		Shader* shader = new Shader("LabShader");
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
	{
		Shader* shader = new Shader("ColorShader");
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "shaders", "VSColor.glsl"), GL_VERTEX_SHADER);
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "shaders", "FSColor.glsl"), GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
	{
		Shader* shader = new Shader("ObjectShader");
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "shaders", "VSObject.glsl"), GL_VERTEX_SHADER);
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "shaders", "FSObject.glsl"), GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
	{
		Shader* shader = new Shader("TextureShader");
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "shaders", "VSTexture.glsl"), GL_VERTEX_SHADER);
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "shaders", "FSTexture.glsl"), GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
	{
		mapTextures["random"] = CreateRandomTexture(16, 16);
	}
	// Light & material properties
	{
		dancerPosition = glm::vec3(3, 0.5, 3);
		dancerPosition2 = glm::vec3(5, 0.5, 1);
		lightDirection = glm::vec3(0, 1, 0);
		materialShininess = 30;
		materialKd = 0.5;
		materialKs = 0.5;

	}

	GetSceneCamera()->SetPosition(glm::vec3(4, 3, -1));


	//randomly set floor colors
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {

			auto r = ((double)rand() / (RAND_MAX));
			auto  g = ((double)rand() / (RAND_MAX));
			auto b = ((double)rand() / (RAND_MAX));
			dancefloor[i][j] = glm::vec3(r, g, b);

		}
	}

}
void Tema3::FrameStart()
{
	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// Sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}
void Tema3::Update(float deltaTimeSeconds)
{


	{
		dancerPosition = RandomMovement(dancerPosition, deltaTimeSeconds);
		dancerPosition2 = RandomMovement(dancerPosition2, deltaTimeSeconds);
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, dancerPosition);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5, 2, 0.5));
		GetLightsAffectingDancer(dancerPosition);
		RenderSimpleMesh2(meshes["box"], shaders["ObjectShader"], modelMatrix, glm::vec3(0, 0.5, 0));
	}
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, dancerPosition2);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5, 2, 0.5));
		GetLightsAffectingDancer(dancerPosition2);
		RenderSimpleMesh2(meshes["box"], shaders["ObjectShader"], modelMatrix, glm::vec3(0, 0.5, 0));
	}


	DrawDanceFloor();

	DrawWalls();

	MoveSpotlights(deltaTimeSeconds);

	DrawCeiling();

	DrawDiscoBall();

	DrawLightCones();

}
void Tema3::DrawDiscoBall() {
	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, ballPos);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 1, 1));
	RenderTextureMesh(meshes["sphere"], shaders["TextureShader"], modelMatrix, mapTextures["random"]);
}
void Tema3::DrawCeiling() {
	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(4, 5, 4));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(9, 1, 9));
	GetLightsAffectingDancer(glm::vec3(4, 5, 4));
	RenderSimpleMesh2(meshes["box"], shaders["ObjectShader"], modelMatrix, glm::vec3(1, 1, 1));
}
void::Tema3::DrawLightCones() {
	// se vor desena doar fatetele fata
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	// aceasta directiva este folosita pentru nu se scrie in depth buffer
	glDepthMask(GL_FALSE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// desenare conuri
	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(spotLightPos1.x, 0, spotLightPos1.z));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(tan(cutoffangle), 1, tan(cutoffangle)) * glm::vec3(5));
	RenderMesh2(meshes["disk"], shaders["ColorShader"], modelMatrix, glm::vec4(1, 0, 0, 1));
	modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(spotLightPos2.x, 0, spotLightPos2.z));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(tan(cutoffangle), 1, tan(cutoffangle)) * glm::vec3(5));
	RenderMesh2(meshes["disk"], shaders["ColorShader"], modelMatrix, glm::vec4(0, 1, 0, 1));
	modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(spotLightPos3.x, 0, spotLightPos3.z));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(tan(cutoffangle), 1, tan(cutoffangle)) * glm::vec3(5));
	RenderMesh2(meshes["disk"], shaders["ColorShader"], modelMatrix, glm::vec4(0, 0, 1, 1));
	modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(spotLightPos4.x, 0, spotLightPos4.z));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(tan(cutoffangle), 1, tan(cutoffangle)) * glm::vec3(5));
	RenderMesh2(meshes["disk"], shaders["ColorShader"], modelMatrix, glm::vec4(1, 1, 0, 1));
	modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(spotLightPos5.x, 0, spotLightPos5.z));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(tan(cutoffangle), 1, tan(cutoffangle)) * glm::vec3(5));
	RenderMesh2(meshes["disk"], shaders["ColorShader"], modelMatrix, glm::vec4(0, 1, 1, 1));

	// se dezactiveaza actiunile tuturor directivelor apelate anterior
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
}
void Tema3::DrawWalls() {
	for (int i = 0; i <= 8; i++) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-1, 2.5, i));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 5, 1));
		GetLightsAffectingDancer(glm::vec3(-1, 0, i));
		RenderSimpleMesh2(meshes["box"], shaders["ObjectShader"], modelMatrix, dancefloor[0][i]);

		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(8, 2.5, i));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 5, 1));
		GetLightsAffectingDancer(glm::vec3(8, 0, i));
		RenderSimpleMesh2(meshes["box"], shaders["ObjectShader"], modelMatrix, dancefloor[7][i]);
		modelMatrix = glm::mat4(1);

		modelMatrix = glm::translate(modelMatrix, glm::vec3(i, 2.5, 8));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 5, 1));
		GetLightsAffectingDancer(glm::vec3(i, 0, 8));
		RenderSimpleMesh2(meshes["box"], shaders["ObjectShader"], modelMatrix, dancefloor[i][7]);


	}

}
void Tema3::DrawDanceFloor() {
	srand(time(0));
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(i, 0, j));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.02f));
			discoLightPos = glm::vec3(i, 0, j);
			discoLightColor = dancefloor[i][j];
			RenderSimpleMesh(meshes["plane"], shaders["LabShader"], modelMatrix, dancefloor[i][j]);
		}
	}





}
glm::vec3 Tema3::RandomMovement(glm::vec3 pos, float t) {
	glm::vec3 nextPos;
	float speed = 0.7;
	int horizontalMov = rand() % 3 + (-1);
	if (round(pos.x) <= 1) {
		horizontalMov = 1;
	}
	if (round(pos.x) >= 7) {
		horizontalMov = -1;
	}
	nextPos.x = pos.x + horizontalMov * t * speed;
	int verticalMov = rand() % 3 + (-1);
	if (round(pos.z) <= 1) {
		verticalMov = 1;
	}
	if (round(pos.z) >= 7) {
		verticalMov = -1;
	}
	nextPos.z = pos.z + verticalMov * t * speed;

	nextPos.y = 0.25;


	return nextPos;
}
void Tema3::MoveSpotlights(float deltaTimeSeconds) {
	static int table[] = { -1,1, -AI_MATH_PI, AI_MATH_PI, AI_MATH_HALF_PI, -AI_MATH_HALF_PI };
	int index = rand() % (sizeof table / sizeof * table);
	float s = sin(deltaTimeSeconds * table[index]);
	float c = cos(deltaTimeSeconds * table[index]);

	glm::vec2 pivot1 = glm::vec2(2, 6);
	spotLightPos1.x -= pivot1.x;
	spotLightPos1.z -= pivot1.y;

	float xnew = spotLightPos1.x * c - spotLightPos1.z * s;
	float ynew = spotLightPos1.x * s + spotLightPos1.z * c;

	spotLightPos1.x = xnew + pivot1.x;
	spotLightPos1.z = ynew  + pivot1.y;

	glm::vec2 pivot2 = glm::vec2(2, 2);
	spotLightPos2.x -= pivot2.x;
	spotLightPos2.z -= pivot2.y;

	xnew = spotLightPos2.x * c - spotLightPos2.z * s;
	ynew = spotLightPos2.x * s + spotLightPos2.z * c;

	spotLightPos2.x = xnew + pivot2.x;
	spotLightPos2.z = ynew + pivot2.y;

	glm::vec2 pivot3 = glm::vec2(6, 3);
	spotLightPos3.x -= pivot3.x;
	spotLightPos3.z -= pivot3.y;

	xnew = spotLightPos3.x * c - spotLightPos3.z * s;
	ynew = spotLightPos3.x * s + spotLightPos3.z * c;

	spotLightPos3.x = xnew + pivot3.x;
	spotLightPos3.z = ynew + pivot3.y;


	glm::vec2 pivot4 = glm::vec2(6, 5);
	spotLightPos4.x -= pivot4.x;
	spotLightPos4.z -= pivot4.y;

	xnew = spotLightPos4.x * c - spotLightPos4.z * s;
	ynew = spotLightPos4.x * s + spotLightPos4.z * c;

	spotLightPos4.x = xnew + pivot4.x;
	spotLightPos4.z = ynew + pivot4.y;



	glm::vec2 pivot5 = glm::vec2(4, 5);
	spotLightPos5.x -= pivot5.x;
	spotLightPos5.z -= pivot5.y;

	xnew = spotLightPos5.x * c - spotLightPos5.z * s;
	ynew = spotLightPos5.x * s + spotLightPos5.z * c;

	spotLightPos5.x = xnew + pivot5.x;
	spotLightPos5.z = ynew + pivot5.y;



}
void Tema3::GetLightsAffectingDancer(glm::vec3 dancerPos) {
	static glm::vec3 lights[9];
	int px = round(dancerPos.x);
	int pz = round(dancerPos.z);

	pointLightPos[0] = glm::vec3(px, 0, pz);
	pointLightColor[0] = dancefloor[px][pz];
	pointLightPos[1] = glm::vec3(px + 1, 0, pz);
	pointLightColor[1] = dancefloor[px + 1][pz];
	pointLightPos[2] = glm::vec3(px - 1, 0, pz);
	pointLightColor[2] = dancefloor[px - 1][pz];
	pointLightPos[3] = glm::vec3(px, 0, pz + 1);
	pointLightColor[3] = dancefloor[px][pz + 1];
	pointLightPos[4] = glm::vec3(px, 0, pz - 1);
	pointLightColor[4] = dancefloor[px][pz - 1];
	pointLightPos[5] = glm::vec3(px + 1, 0, pz + 1);
	pointLightColor[5] = dancefloor[px + 1][pz + 1];
	pointLightPos[6] = glm::vec3(px - 1, 0, pz - 1);
	pointLightColor[6] = dancefloor[px - 1][pz - 1];
	pointLightPos[7] = glm::vec3(px + 1, 0, pz - 1);
	pointLightColor[7] = dancefloor[px + 1][pz - 1];
	pointLightPos[8] = glm::vec3(px - 1, 0, pz + 1);
	pointLightColor[8] = dancefloor[px - 1][pz + 1];
	if (px == 0) {
		pointLightColor[2] = glm::vec3(1);
		pointLightColor[8] = glm::vec3(1);
	}
	if (px == 7) {
		pointLightColor[1] = glm::vec3(1);
		pointLightColor[7] = glm::vec3(1);
	}
	if (pz == 0) {
		pointLightColor[4] = glm::vec3(1);
		pointLightColor[7] = glm::vec3(1);
	}
	if (pz == 7) {
		pointLightColor[3] = glm::vec3(1);
		pointLightColor[8] = glm::vec3(1);
	}

}
Texture2D* Tema3::CreateRandomTexture(unsigned int width, unsigned int height)
{
	GLuint textureID = 0;
	unsigned int channels = 3;
	unsigned int size = width * height * channels;
	unsigned char* data = new unsigned char[size];

	// TODO(student): Generate random texture data
	srand(time(0));

	for (int i = 0; i < size; i++) {
		data[i] = rand() % (UINT8_MAX + 1);

	}
	// TODO(student): Generate and bind the new texture ID
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	if (GLEW_EXT_texture_filter_anisotropic) {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
	}
	// TODO(student): Set the texture parameters (MIN_FILTER, MAG_FILTER and WRAPPING MODE) using glTexParameteri
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //optiunea care ofera cea mai buna calitate, tranzitie smooth intre mipmaps 
	//cu MIPMAP_NEAREST tranzitia este sharp intre mipmaps
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //cu gl_nearest imaginea apare mai sharp, delimitarile sunt mai clare
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//GL LINEAR
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	CheckOpenGLError();

	// Use glTexImage2D to set the texture data
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	// TODO(student): Generate texture mip-maps
	glGenerateMipmap(GL_TEXTURE_2D);

	CheckOpenGLError();

	// Save the texture into a wrapper Texture2D class for using easier later during rendering phase
	Texture2D* texture = new Texture2D();
	texture->Init(textureID, width, height, channels);

	SAFE_FREE_ARRAY(data);
	return texture;
}
Mesh* Tema3::CreateCircle(
	const std::string& name2,
	float length,
	glm::vec3 color,
	bool fill,
	int rad)
{

	std::vector<VertexFormat> vertices;
	std::vector<unsigned int> indices;
	float theta;
	int triangles = 50;

	vertices.emplace_back(glm::vec3(0, 0.5, 0), color);
	for (int i = 0; i < triangles; i++)
	{
		theta = AI_MATH_TWO_PI * i / triangles;

		vertices.emplace_back(glm::vec3(rad * cos(theta), 0, rad * sin(theta)), color);
		indices.push_back(i);
	}

	Mesh* circle = new Mesh(name2);


	indices.push_back(triangles);
	indices.push_back(1);
	circle->SetDrawMode(GL_TRIANGLE_FAN);

	circle->InitFromData(vertices, indices);
	return circle;
}
void Tema3::RenderTextureMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture1)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// Render an object using the specified shader and the specified position
	glUseProgram(shader->program);
	int light_pos = glGetUniformLocation(shader->program, "light_pos");
	glUniform3f(light_pos, ballPos.x, ballPos.y, ballPos.z);
	int type = glGetUniformLocation(shader->program, "type_of_light");
	glUniform1i(type, typeoflight);
	int t = glGetUniformLocation(shader->program, "time");
	timeEng = Engine::GetElapsedTime()/100;
	glUniform1f(t, timeEng);
	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	if (texture1)
	{
		// TODO(student): Do these:
		// - activate texture location 0
		glActiveTexture(GL_TEXTURE0);
		// - bind the texture1 ID
		glBindTexture(GL_TEXTURE_2D, texture1->GetTextureID());
		// - send theuniform value
		glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);

	}


	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->m_VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}
void Tema3::RenderSimpleMesh2(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// Render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// Set shader uniforms for light properties
	int light_position = glGetUniformLocation(shader->program, "light_position");
	glUniform3fv(light_position, 9, glm::value_ptr(pointLightPos[0]));

	int light_color = glGetUniformLocation(shader->program, "light_color");
	glUniform3fv(light_color, 9, glm::value_ptr(pointLightColor[0]));

	int light_direction = glGetUniformLocation(shader->program, "light_direction");
	glUniform3f(light_direction, lightDirection.x, lightDirection.y, lightDirection.z);

	// Set eye position (camera position) uniform
	glm::vec3 eyePosition = GetSceneCamera()->m_transform->GetWorldPosition();
	int eye_position = glGetUniformLocation(shader->program, "eye_position");
	glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

	// Set material property uniforms (shininess, kd, ks, object color) 
	int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
	glUniform1i(material_shininess, materialShininess);

	int material_kd = glGetUniformLocation(shader->program, "material_kd");
	glUniform1f(material_kd, materialKd);

	int material_ks = glGetUniformLocation(shader->program, "material_ks");
	glUniform1f(material_ks, materialKs);

	int object_color = glGetUniformLocation(shader->program, "object_color");
	glUniform3f(object_color, color.r, color.g, color.b);

	// TODO(student): Set any other shader uniforms that you need
	int type = glGetUniformLocation(shader->program, "type_of_light");
	glUniform1i(type, typeoflight);
	int t = glGetUniformLocation(shader->program, "time");
	timeEng = Engine::GetElapsedTime()/100;
	glUniform1f(t, timeEng);

	int cut_off_angle = glGetUniformLocation(shader->program, "cut_off_angle");
	glUniform1f(cut_off_angle, cutoffangle);
	int spot_position1 = glGetUniformLocation(shader->program, "spot_position1");
	glUniform3f(spot_position1, spotLightPos1.x, spotLightPos1.y, spotLightPos1.z);


	int spot_position2 = glGetUniformLocation(shader->program, "spot_position2");
	glUniform3f(spot_position2, spotLightPos2.x, spotLightPos2.y, spotLightPos2.z);

	int spot_position3 = glGetUniformLocation(shader->program, "spot_position3");
	glUniform3f(spot_position3, spotLightPos3.x, spotLightPos3.y, spotLightPos3.z);

	int spot_position4 = glGetUniformLocation(shader->program, "spot_position4");
	glUniform3f(spot_position4, spotLightPos4.x, spotLightPos4.y, spotLightPos4.z);

	int spot_position5 = glGetUniformLocation(shader->program, "spot_position5");
	glUniform3f(spot_position5, spotLightPos5.x, spotLightPos5.y, spotLightPos5.z);
	int spot_dir = glGetUniformLocation(shader->program, "spot_direction");
	glUniform3f(spot_dir, spotLightDir.x, spotLightDir.y, spotLightDir.z);

	// TODO(student): Do these:
	   // - activate texture location 0
	glActiveTexture(GL_TEXTURE0);
	// - bind the texture1 ID
	auto texture1 = mapTextures["random"];
	glBindTexture(GL_TEXTURE_2D, texture1->GetTextureID());
	// - send theuniform value
	glUniform1i(glGetUniformLocation(shader->program, "texture1"), 0);

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->m_VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}
void Tema3::RenderMesh2(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec4& color)
{
	if (!mesh || !shader || !shader->program)
		return;

	glUseProgram(shader->program);

	int location = glGetUniformLocation(shader->GetProgramID(), "Model");

	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	int location2 = glGetUniformLocation(shader->GetProgramID(), "View");
	glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
	glUniformMatrix4fv(location2, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	int location3 = glGetUniformLocation(shader->GetProgramID(), "Projection");
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	glUniformMatrix4fv(location3, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	int col = glGetUniformLocation(shader->GetProgramID(), "Color");
	glUniform4fv(col, 1, glm::value_ptr(color));

	int type = glGetUniformLocation(shader->program, "type_of_light");
	glUniform1i(type, typeoflight);

	glBindVertexArray(mesh->GetBuffers()->m_VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);


}
void Tema3::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// Render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// Set shader uniforms for light properties

	int light_position = glGetUniformLocation(shader->program, "light_position");
	glUniform3f(light_position, discoLightPos.x, discoLightPos.y, discoLightPos.z);

	int light_color = glGetUniformLocation(shader->program, "c");
	glUniform3f(light_color, discoLightColor.x, discoLightColor.y, discoLightColor.z);


	int light_direction = glGetUniformLocation(shader->program, "light_direction");
	glUniform3f(light_direction, lightDirection.x, lightDirection.y, lightDirection.z);

	// Set eye position (camera position) uniform
	glm::vec3 eyePosition = GetSceneCamera()->m_transform->GetWorldPosition();
	int eye_position = glGetUniformLocation(shader->program, "eye_position");
	glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

	// Set material property uniforms (shininess, kd, ks, object color) 
	int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
	glUniform1i(material_shininess, materialShininess);

	int material_kd = glGetUniformLocation(shader->program, "material_kd");
	glUniform1f(material_kd, materialKd);

	int material_ks = glGetUniformLocation(shader->program, "material_ks");
	glUniform1f(material_ks, materialKs);

	int object_color = glGetUniformLocation(shader->program, "object_color");
	glUniform3f(object_color, color.r, color.g, color.b);

	// TODO(student): Set any other shader uniforms that you need
	int type = glGetUniformLocation(shader->program, "type_of_light");
	glUniform1i(type, typeoflight);

	int t = glGetUniformLocation(shader->program, "time");
	timeEng = Engine::GetElapsedTime()/100;
	glUniform1f(t, timeEng);
	//cout << timeEng << endl;

	int cut_off_angle = glGetUniformLocation(shader->program, "cut_off_angle");
	glUniform1f(cut_off_angle, cutoffangle);

	int spot_position1 = glGetUniformLocation(shader->program, "spot_position1");
	glUniform3f(spot_position1, spotLightPos1.x, spotLightPos1.y, spotLightPos1.z);


	int spot_position2 = glGetUniformLocation(shader->program, "spot_position2");
	glUniform3f(spot_position2, spotLightPos2.x, spotLightPos2.y, spotLightPos2.z);

	int spot_position3 = glGetUniformLocation(shader->program, "spot_position3");
	glUniform3f(spot_position3, spotLightPos3.x, spotLightPos3.y, spotLightPos3.z);

	int spot_position4 = glGetUniformLocation(shader->program, "spot_position4");
	glUniform3f(spot_position4, spotLightPos4.x, spotLightPos4.y, spotLightPos4.z);

	int spot_position5 = glGetUniformLocation(shader->program, "spot_position5");
	glUniform3f(spot_position5, spotLightPos5.x, spotLightPos5.y, spotLightPos5.z);
	int spot_dir = glGetUniformLocation(shader->program, "spot_direction");
	glUniform3f(spot_dir, spotLightDir.x, spotLightDir.y, spotLightDir.z);

	glActiveTexture(GL_TEXTURE0);
	// - bind the texture1 ID
	auto texture1 = mapTextures["random"];
	glBindTexture(GL_TEXTURE_2D, texture1->GetTextureID());
	// - send theuniform value
	glUniform1i(glGetUniformLocation(shader->program, "texture"), 0);
	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->m_VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}
void Tema3::FrameEnd()
{
}

void Tema3::OnInputUpdate(float deltaTime, int mods)
{
	float speed = 2;

	if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		glm::vec3 up = glm::vec3(0, 1, 0);
		glm::vec3 right = GetSceneCamera()->m_transform->GetLocalOXVector();
		glm::vec3 forward = GetSceneCamera()->m_transform->GetLocalOZVector();
		forward = glm::normalize(glm::vec3(forward.x, 0, forward.z));


	}
}


void Tema3::OnKeyPress(int key, int mods)
{
	// Add key press event

	// TODO(student): Set keys that you might need
	if (key == GLFW_KEY_F)
	{
		typeoflight++;
		if (typeoflight == 0) {
			typeoflight++;
		}
		if (typeoflight == 3) {
			typeoflight = -1;
		}
	}


}


void Tema3::OnKeyRelease(int key, int mods)
{
	// Add key release event
}


void Tema3::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// Add mouse move event
}


void Tema3::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// Add mouse button press event
}


void Tema3::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// Add mouse button release event
}


void Tema3::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema3::OnWindowResize(int width, int height)
{
}
