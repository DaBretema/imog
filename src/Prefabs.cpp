#include "Prefabs.hpp"


#define __p_c_WrapPrefab(fName)           \
  fName(const glm::vec3& color,           \
        const glm::vec3& T,               \
        const glm::vec3& R,               \
        const glm::vec3& S) {             \
    return __get(#fName, color, T, R, S); \
  }


namespace BRAVE {

// ====================================================================== //
// ====================================================================== //
// Apply default settings to load an figure
// and apply transform operations to put it where user desired
// ====================================================================== //

_PrefabType Prefab::__get(const std::string& prefab,
                          const glm::vec3&   color,
                          const glm::vec3&   T,
                          const glm::vec3&   R,
                          const glm::vec3&   S) {

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

// ====================================================================== //
// ====================================================================== //
// Wrap to PLANE prefab
// ====================================================================== //

_PrefabType Prefab::__p_c_WrapPrefab(plane);

// ====================================================================== //
// ====================================================================== //
// Wrap to SPHERE prefab
// ====================================================================== //

_PrefabType Prefab::__p_c_WrapPrefab(sphere);

// ====================================================================== //
// ====================================================================== //
// Wrap to CYLYNDER prefab
// ====================================================================== //

_PrefabType Prefab::__p_c_WrapPrefab(cylynder);


} // namespace BRAVE
