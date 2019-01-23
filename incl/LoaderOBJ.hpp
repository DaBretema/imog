#pragma once

#include <vector>

#include "wrap/Math.hpp"

namespace BRAVE {

/// Basic data needed to render
struct RenderData {
  std::vector<glm::vec3>    vertices;
  std::vector<glm::vec3>    normals;
  std::vector<unsigned int> indices;
};

/// loadOBJ return a struct with data needed to rendering from a OBJ file
RenderData loadOBJ(const std::string& filePath);

} // namespace BRAVE
