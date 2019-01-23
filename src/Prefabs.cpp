#include "../incl/Prefabs.hpp"

#pragma once

#include <string>

#include <Dac/Logger.hpp>

#include "wrap/Math.hpp"
#include "wrap/Paths.hpp"
#include "wrap/Colors.hpp"

#include "Renderable.hpp"


#define __p_c_WrapPrefab(fName)    \
  fName(const glm::vec3& color,    \
        const glm::vec3& T,        \
        const glm::vec3& R,        \
        const glm::vec3& S) {      \
    __get(#fName, color, T, R, S); \
  }



namespace BRAVE {

_PrefabType Prefab::__p_c_WrapPrefab(plane);
_PrefabType Prefab::__p_c_WrapPrefab(sphere);
_PrefabType Prefab::__p_c_WrapPrefab(cylynder);


_PrefabType __get(const std::string& prefab,
                  const glm::vec3&   color = Colors::Yellow,
                  const glm::vec3&   T     = glm::vec3{0},
                  const glm::vec3&   R     = glm::vec3{0},
                  const glm::vec3&   S     = glm::vec3{1}) {

  std::string sV     = Paths::Shaders + "base.vert";
  std::string sG     = Paths::Shaders + "base.geom";
  std::string sF     = Paths::Shaders + "base.frag";
  auto        shader = std::make_shared<Shader>("BASE", sV, sG, sF);

  std::string figure = Paths::Figures + prefab + ".obj";
  _PrefabType out    = std::make_shared<_PrefabBaseType>(figure, shader, color);

  // Apply rotation
  out->rotate(R);
  out->translate(T);
  out->scale(S);

  // Return the renderer
  return out;
}


} // namespace BRAVE
