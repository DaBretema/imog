#pragma once

#include <string>
#include <memory>
#include <vector>

#include "Math.hpp"
#include "helpers/Consts.hpp"

namespace brave {
using uint = unsigned int;

struct Joint {
  std::string            name{""};
  glm::vec3              offset{0.f};
  glm::mat4              transformAsMatrix{1.f};
  std::shared_ptr<Joint> parent;
  std::shared_ptr<Joint> endsite;
  Joint(const std::string& name, std::shared_ptr<Joint> parent)
      : name(name), parent(parent) {}
};

struct Frame {
  std::vector<glm::vec3> rotations;
  glm::vec3              translation;

  // Sum rotations of frame
  glm::vec3 sumRots() const;
};

class Motion {
public:
  static std::shared_ptr<Motion> create(const std::string& name,
                                        const std::string& filepath,
                                        loopMode           lm,
                                        uint               steps);

  std::string                         name; // If contains _ is a mix
  std::vector<std::shared_ptr<Joint>> joints;
  std::vector<Frame>                  frames;
  float                               timeStep;

  // Only for mixes
  uint frameA = 0u;
  uint frameB = 0u;

  // If its name contains _ is a mix
  bool        isMix();
  static bool isMix(const std::string& str);

  // Clean any motion to get a smoother loop
  void clean(loopMode lm, uint steps = 0);

  // Mix any motion with other and get a new animation
  // that conect both smoothly
  std::shared_ptr<Motion> mix(const std::shared_ptr<Motion>& m);
};

} // namespace brave
