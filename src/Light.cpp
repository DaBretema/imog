
#include "../incl/Light.hpp"

namespace BRAVE {

// ====================================================================== //
// ====================================================================== //
// Param constructor
// ====================================================================== //

Light::Light(const glm::vec3& pos, const glm::vec3& color)
    : m_pos(pos), m_color(color) {}

// ====================================================================== //
// ====================================================================== //
// G/Setter for pos
// ====================================================================== //

glm::vec3 Light::pos() const { return m_pos; }
void      Light::pos(const glm::vec3& newPos) { m_pos = newPos; }

// ====================================================================== //
// ====================================================================== //
// G/Setter for color
// ====================================================================== //

glm::vec3 Light::color() const { return m_color; }
void      Light::color(const glm::vec3& newColor) { m_color = newColor; }

} // namespace BRAVE
