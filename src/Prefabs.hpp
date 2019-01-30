#pragma once

#include <string>
#include <memory>

#include <dac/Logger.hpp>

#include "helpers/Math.hpp"
#include "helpers/Paths.hpp"
#include "helpers/Colors.hpp"

#include "Renderable.hpp"
#include "Shader.hpp"

namespace BRAVE {

using _PrefabBaseType = Renderable;
using _PrefabType     = std::shared_ptr<_PrefabBaseType>;

class Prefab {

#define __p_h_WrapPrefab(fName)                  \
  fName(const glm::vec3& color = Colors::Yellow, \
        const glm::vec3& T     = glm::vec3{0},   \
        const glm::vec3& R     = glm::vec3{0},   \
        const glm::vec3& S     = glm::vec3{1})

private:
  // Apply default settings to load an figure
  // and apply transform operations to put it where user desired
  static _PrefabType __get(const std::string& prefab,
                           const glm::vec3&   color = Colors::Yellow,
                           const glm::vec3&   T     = glm::vec3{0},
                           const glm::vec3&   R     = glm::vec3{0},
                           const glm::vec3&   S     = glm::vec3{1});

public:
  // Wrap to PLANE prefab
  static _PrefabType __p_h_WrapPrefab(plane);

  // Wrap to SPHERE prefab
  static _PrefabType __p_h_WrapPrefab(sphere);

  // Wrap to CYLYNDER prefab
  static _PrefabType __p_h_WrapPrefab(cylynder);
};

} // namespace BRAVE
