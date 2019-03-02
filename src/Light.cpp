#include "Light.hpp"
#include "helpers/Consts.hpp"

#include "Renderable.hpp"

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
      m_renderableID("Light_" + std::to_string(g_lastLightID++)) {

  Renderable::create(true,
                     m_renderableID,
                     Figures::mark,
                     "",
                     m_color,
                     Shader::getByName(Shaders::light),
                     false);
}

// ====================================================================== //
// ====================================================================== //
// G/Setter for pos
// ====================================================================== //

glm::vec3 Light::pos() const { return m_pos; }
void      Light::pos(const glm::vec3& newPos) {
  m_pos                                                = newPos;
  Renderable::getByName(m_renderableID)->transform.pos = m_pos;
}

// ====================================================================== //
// ====================================================================== //
// G/Setter for color
// ====================================================================== //

glm::vec3 Light::color() const { return m_color; }
void      Light::color(const glm::vec3& newColor) {
  m_color = newColor;
  Renderable::getByName(m_renderableID)->color(m_color);
}

// ====================================================================== //
// ====================================================================== //
// G/Setter for intensity
// ====================================================================== //

float Light::intensity() const { return m_intensity; }
void  Light::intensity(float newIntensity) {
  m_intensity = (newIntensity < 0.1f) ? 1.f : newIntensity;
}

} // namespace brave
