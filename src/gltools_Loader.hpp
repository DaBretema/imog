#pragma once
#include <vector>
#include "gltools_Math.hpp"

// Stuff with data types used for loaders
#include "mgtools_Motion.hpp"
#include "gltools_Renderable.hpp"

namespace imog {
namespace loader {

  // Return a struct with data needed to rendering from a OBJ file
  Renderable::data OBJ(const std::string& filePath);

  // Return data needed to create a Skeleton from a BVH file
  std::shared_ptr<Motion> BVH(const std::string& bvhFilePath);

} // namespace loader
} // namespace imog
