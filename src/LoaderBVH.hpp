#pragma once

#include <tuple>

#include "Joint.hpp"


namespace brave {
namespace loader {

  // Return a tuple with data needed to create a Skeleton from a BVH file
  std::tuple<std::vector<std::shared_ptr<Joint>>, float>
      BVH(const std::string& bvhFilePath);
} // namespace loader
} // namespace brave
