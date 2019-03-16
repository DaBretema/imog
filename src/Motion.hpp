#pragma once

#include <string>
#include <memory>
#include <vector>

#include "Math.hpp"

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
};

class Motion {
public:
  std::string                         name;
  std::vector<std::shared_ptr<Joint>> joints;
  std::vector<Frame>                  frames;
  float                               timeStep;

  void clean();
  void mix(const std::shared_ptr<Motion>& m);
};

}