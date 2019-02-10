#pragma once
#include <vector>
#include <mutex>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>

#include "Joint.hpp"
#include "Math.hpp"

namespace brave {


class Skeleton {

  static unsigned int g_lastSkeletonID;

public:
  static std::vector<std::shared_ptr<Skeleton>> pool;
  static std::unordered_map<std::string,unsigned int> poolIndices;

private:
  unsigned int m_id;
  bool         m_show;
  bool         m_play;
  bool         m_attendDrawCallback;

  std::once_flag m_animFlag;

  glm::vec3 m_initPos;
  int       m_initFrame;
  int       m_currFrame;
  glm::vec3 m_frameDespl;

  std::vector<std::shared_ptr<Joint>> m_joints;
  int                                 m_frames;
  float                               m_frameTime;


public:
  Skeleton() = default;
  Skeleton(const std::string& bvhFilePath);
  static std::shared_ptr<Skeleton> Ptr(const std::string& bvhFilePath);

  // Getter for frame-time
  float frameTime() const;

  // Root renderer
  //   std::shared_ptr<Renderer> rootRenderer() const;

  // Plays skeleton animation
  void play(int atFrame = -1, bool showIfIsHidden = true);

  // Stops skeleton animation
  void stop(bool hideIfIsShowed = true);

  // Launch a detached thread where is computed the animation loop
  // That loop have a delay between each iteration, equal to m_frameTime
  void animate();

  // Should be called inside the render loop.
  void draw();
};

} // namespace brave
