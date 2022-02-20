#pragma once

#include "components/simple_scene.h"
#include "components/transform.h"


namespace m1
{
	class Tema3 : public gfxc::SimpleScene
	{
	public:
		Tema3();
		~Tema3();

		void Init() override;

		glm::vec3 RandomMovement(glm::vec3 pos, float t);

	private:
		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		void DrawWalls();
		void GetLightsAffectingDancer(glm::vec3 dancerPos);
		void RenderMesh2(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec4& color);
		void FrameEnd() override;

		void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color = glm::vec3(1));

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
		void OnWindowResize(int width, int height) override;
		void DrawDanceFloor();
		void DrawLightCones();
		Mesh* CreateCircle(const std::string& name2, float length, glm::vec3 color, bool fill, int rad);
		void MoveSpotlights(float deltaTimeSeconds);
		void GetLightsAffectingWall(glm::vec3 dancerPos);
		void RenderSimpleMesh2(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color);
		Texture2D* CreateRandomTexture(unsigned int width, unsigned int height);
		void RenderTextureMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture1);
		void DrawCeiling();
		void DrawDiscoBall();
		glm::vec3 dancerPosition;
		glm::vec3 dancerPosition2;
		glm::vec3 lightDirection = glm::vec3(0, 1, 0);
		unsigned int materialShininess;
		float materialKd;
		float materialKs;
		float cutoffangle;
		int typeoflight = -1;
		glm::vec3 pointLightPos[64];
		glm::vec3 pointLightColor[64];
		glm::vec3 lightColor;
		glm::vec3 discoLightColor;
		glm::vec3 discoLightPos;
		glm::vec3 spotLightPos1 = glm::vec3(1, 1, 6);
		glm::vec3 spotLightPos2 = glm::vec3(1, 1, 2);
		glm::vec3 spotLightPos3 = glm::vec3(6, 1, 2);
		glm::vec3 spotLightPos4 = glm::vec3(6, 1, 6);
		glm::vec3 spotLightPos5 = glm::vec3(4, 1, 4);
		glm::vec3 spotLightDir = glm::vec3(0, -1, 0);
		std::unordered_map<std::string, Texture2D*> mapTextures;
		glm::vec3 ballPos = glm::vec3(4, 4, 4);
		float timeEng = 0;

	};
}   // namespace m1
