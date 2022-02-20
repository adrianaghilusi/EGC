#include "object2D.h"
#include <vector>
#include "core/engine.h"
#include "utils/gl_utils.h"



	Mesh* object2d::CreateSquare(
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
			VertexFormat(corner + glm::vec3(length, 0, 0), color),
			VertexFormat(corner + glm::vec3(length, length, 0), color),
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

	Mesh* object2d::CreateCircle(
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

		vertices.emplace_back(glm::vec3(0, 0, 0), color);
		for (int i = 0; i < triangles; i++)
		{
			theta = AI_MATH_TWO_PI * i / triangles;

			vertices.emplace_back(glm::vec3(rad * cos(theta), rad * sin(theta), 0), color);
			indices.push_back(i);
		}

		Mesh* circle = new Mesh(name2);


		indices.push_back(triangles);
		indices.push_back(1);
		circle->SetDrawMode(GL_TRIANGLE_FAN);

		circle->InitFromData(vertices, indices);
		return circle;
	}

	Mesh* object2d::CreateRectangle(
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
