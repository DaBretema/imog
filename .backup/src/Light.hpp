#pragma once

#include <memory>
#include "Math.hpp"

namespace brave {

class Light {

  static unsigned int g_lastLightID;

private:
  glm::vec3   m_pos{0.1f};
  glm::vec3   m_color{0.5f};
  float       m_intensity;
  std::string m_renderableID;

public:
  // Param constructor
  Light(const glm::vec3& pos, const glm::vec3& color, float intensity = 1.f);

  // G/Setter for pos
  glm::vec3 pos() const;
  void      pos(const glm::vec3& newPos);

  // G/Setter for color
  glm::vec3 color() const;
  void      color(const glm::vec3& newColor);

  // G/Setter for intensity
  float intensity() const;
  void  intensity(float newIntensity);
};

} // namespace brave
