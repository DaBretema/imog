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
  static std::vector<std::shared_ptr<Skeleton>>        pool;
  static std::unordered_map<std::string, unsigned int> poolIndices;

private:
  unsigned int m_id;
  std::string  m_path;
  bool         m_show;
  bool         m_play;

  std::once_flag m_animFlag;
  bool           m_animThread;

  glm::vec3 m_initPos;
  int       m_initFrame;
  int       m_currFrame;
  glm::vec3 m_frameDespl;

  std::vector<std::shared_ptr<Joint>> m_joints;
  int                                 m_frames;
  float                               m_frameTime;

  float     m_scale;
  glm::mat4 m_model;


public:
  // Constructor, init default variables and load from bvh file
  Skeleton(const std::string& bvhFilePath, float scale = 1.f);

  // Destructor, stops animation and kills its threads
  ~Skeleton();

  // Get a shared ptr to Skeleton obj from global pool
  // by bvh file of Skeleton
  static std::shared_ptr<Skeleton> getByPath(const std::string& bvhFilePath);

  // Create a new Renderable if it isn't on the gloabl pool
  static std::shared_ptr<Skeleton> create(const std::string& bvhFilePath,
                                          float              scale = 1.f);


  // Getter for frame-time
  float frameTime() const;

  // Getter for root pos
  glm::vec3 rootPos() const;

  // G/Setter for model
  glm::mat4 model() const;
  void      model(const glm::mat4& newModel);


  // Plays skeleton animation
  void play(int atFrame = -1, bool showIfIsHidden = true);

  // Stops skeleton animation
  void stop(bool hideIfIsShowed = false);

  // Launch a detached thread where is computed the animation loop
  // That loop have a delay between each iteration, equal to m_frameTime
  void animate();

  // While user input, step over animation, when user input stops,
  // plays idle animation or stop

  // Draw the skelenton joints
  // Should be called inside the render loop
  void draw();
};

} // namespace brave
