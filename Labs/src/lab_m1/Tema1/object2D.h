#pragma once

#include <string>
#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace object2d
{

	Mesh* CreateSquare(const std::string& name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill);
	Mesh* CreateCircle(const std::string& name2, float length, glm::vec3 color, bool fill, int rad);
	Mesh* CreateRectangle(const std::string& name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill);
}
