#pragma once

#include <string>
#include <memory>

#include <Dac/Logger.hpp>

#include "wrap/Math.hpp"
#include "wrap/Paths.hpp"
#include "wrap/Colors.hpp"

#include "Renderable.hpp"
#include "Shader.hpp"

#define __p_h_WrapPrefab(fName)                  \
  fName(const glm::vec3& color = Colors::Yellow, \
        const glm::vec3& T     = glm::vec3{0},   \
        const glm::vec3& R     = glm::vec3{0},   \
        const glm::vec3& S     = glm::vec3{1})


namespace BRAVE {

using _PrefabBaseType = Renderable;
using _PrefabType     = std::shared_ptr<_PrefabBaseType>;

class Prefab {
private:
  static _PrefabType __get(const std::string& prefab,
                           const glm::vec3&   color = Colors::Yellow,
                           const glm::vec3&   T     = glm::vec3{0},
                           const glm::vec3&   R     = glm::vec3{0},
                           const glm::vec3&   S     = glm::vec3{1});

public:
  static _PrefabType __p_h_WrapPrefab(plane);
  static _PrefabType __p_h_WrapPrefab(sphere);
  static _PrefabType __p_h_WrapPrefab(cylynder);
};
} // namespace BRAVE
