#include "Light.hpp"
#include "helpers/Consts.hpp"

namespace brave {

unsigned int Light::g_lastLightID{0u};

// ====================================================================== //
// ====================================================================== //
// Param constructor
// ====================================================================== //

Light::Light(const glm::vec3& pos, const glm::vec3& color, float intensity)
    : m_pos(pos),
      m_color(color),
      m_intensity(intensity),
      m_renderable(Renderable::create(
          std::string("Light_" + std::to_string(g_lastLightID++)),
          Figures::cube,
          "",
          m_color,
          Shader::getByName(Shaders::light),
          false)) {}

// ====================================================================== //
// ====================================================================== //
// G/Setter for pos
// ====================================================================== //

glm::vec3 Light::pos() const { return m_pos; }
void      Light::pos(const glm::vec3& newPos) {
  m_pos = newPos;
  m_renderable->pos(m_pos);
}

// ====================================================================== //
// ====================================================================== //
// G/Setter for color
// ====================================================================== //

glm::vec3 Light::color() const { return m_color; }
void      Light::color(const glm::vec3& newColor) {
  m_color = newColor;
  m_renderable->color(m_color * m_intensity);
}

// ====================================================================== //
// ====================================================================== //
// G/Setter for color
// ====================================================================== //

float Light::intensity() const { return m_intensity; }
void  Light::intensity(float newIntensity) {
  m_intensity = (newIntensity < 0.1f) ? 1.f : newIntensity;
  m_renderable->color(m_color * m_intensity * 0.05f);
}

} // namespace brave
