#pragma once

#include <vector>

#include "Math.hpp"

namespace brave {

// Basic data needed to render
struct RenderData {
  std::vector<glm::vec3>    vertices;
  std::vector<glm::vec3>    normals;
  std::vector<glm::vec2>    uvs;
  std::vector<unsigned int> indices;
};

namespace loader {

  // Return a struct with data needed to rendering from a OBJ file
  RenderData OBJ(const std::string& filePath);

} // namespace loader
} // namespace brave
