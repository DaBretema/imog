#pragma once

#include <string>
#include <memory>
#include <vector>

#include "Math.hpp"
#include "helpers/Consts.hpp"

namespace brave {

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
  std::string                         name;
  std::vector<std::shared_ptr<Joint>> joints;
  std::vector<Frame>                  frames;
  float                               timeStep;

  // Clean any motion to get a smoother loop
  void clean(loopMode lm);

  // Mix any motion with other and get a new animation
  // that conect both smoothly
  std::shared_ptr<Motion> mix(const std::shared_ptr<Motion>& m);
};

} // namespace brave
