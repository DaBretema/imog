#pragma once

#include <vector>

#include "Math.hpp"

// Stuff with data types used for loaders
#include "Skeleton.hpp"
#include "Renderable.hpp"

namespace brave {
namespace loader {

  // Return a struct with data needed to rendering from a OBJ file
  Renderable::data OBJ(const std::string& filePath);

  // Return data needed to create a Skeleton from a BVH file
  using bvh_t = std::shared_ptr<Skeleton::Motion>;
  bvh_t BVH(const std::string& bvhFilePath);

} // namespace loader
} // namespace brave
