#include "../incl/Light.hpp"

namespace BRAVE {

// Ctor
Light::Light(const glm::vec3& t_pos, const glm::vec3& t_color)
    : m_pos(t_pos), m_color(t_color) {}

// Pos
glm::vec3 Light::pos() const { return m_pos; }
void      Light::pos(const glm::vec3& newPos) { m_pos = newPos; }

// Color
glm::vec3 Light::color() const { return m_color; }
void      Light::color(const glm::vec3& newColor) { m_color = newColor; }

} // namespace BRAVE
