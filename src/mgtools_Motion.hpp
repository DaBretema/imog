#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

#include "cpptools_Logger.hpp"
#include "gltools_Math.hpp"
#include "helpers/Consts.hpp"


namespace imog {
using uint = unsigned int;

struct Joint {
  std::string            name{""};
  glm::vec3              offset{0.f};
  glm::mat4              matrix{1.f};
  std::shared_ptr<Joint> parent;
  std::shared_ptr<Joint> endsite;
  Joint(const std::string& name, std::shared_ptr<Joint> parent)
      : name(name), parent(parent) {}
};

struct Frame {
  std::vector<glm::vec3> rotations;
  glm::vec3              translation;
  glm::vec3                  value() const;

  Frame lerpOne(const Frame& f2, float alpha, bool alsoLerpRoot = true) const;
  std::vector<Frame> lerpTransition(const Frame& f2,
                                    uint         steps,
                                    bool         alsoLerpRoot = true) const;
};

class Motion {

  float m_maxStep;

public:
  static std::string plotFolder();

  using mixMap =
      std::unordered_map<uint, std::pair<int, std::shared_ptr<Motion>>>;

  static std::shared_ptr<Motion> create(const std::string& name,
                                        const std::string& filepath,
                                        loopMode           lm,
                                        uint               steps        = 0u,
                                        bool               alsoLerpRoot = true);

  std::string                         name;
  std::vector<std::shared_ptr<Joint>> joints;
  std::vector<Frame>                  frames;
  float                               timeStep;

  // Acces to private vars
  float maxStep() const;

  // Linked motion
  std::shared_ptr<Motion> linked = nullptr;
  Frame                   linkedFrame(uint frameIdx, float alpha) const;

  // Type
  bool isMix();

  // Mix any motion with other and get a new animation
  // that conect both smoothly
  mixMap mix(const std::shared_ptr<Motion>& m2);
};

} // namespace imog
