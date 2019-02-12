#include "Skeleton.hpp"
#include "LoaderBVH.hpp"

#include <dac/Async.hpp>
#include <dac/Logger.hpp>

namespace brave {

// ====================================================================== //
// ====================================================================== //
// Global skeletons counter
// ====================================================================== //

unsigned int Skeleton::g_lastSkeletonID{0u};

// ====================================================================== //
// ====================================================================== //
// Global pool for skeletons
// ====================================================================== //

std::vector<std::shared_ptr<Skeleton>>        Skeleton::pool;
std::unordered_map<std::string, unsigned int> Skeleton::poolIndices;


// ====================================================================== //
// ====================================================================== //
// Constructor, init default variables and load from bvh file
// ====================================================================== //

Skeleton::Skeleton(const std::string& bvhFilePath)
    : m_id(g_lastSkeletonID++),
      m_path(bvhFilePath),
      m_show(true),
      m_play(false),
      m_animThread(true),
      m_initPos(0.f),
      m_initFrame(0),
      m_currFrame(m_initFrame),
      m_frameDespl(0.f) {

  std::tie(m_joints, m_frameTime) = loader::BVH(bvhFilePath);
  this->animate();
}

// ====================================================================== //
// ====================================================================== //
// Destructor, stops animation and kills its threads
// ====================================================================== //

Skeleton::~Skeleton() {
  dInfo("Destroyed @ {}", m_path);
  stop();
  m_animThread = false;
}


// ====================================================================== //
// ====================================================================== //
// Get a shared ptr to Skeleton obj from global pool
// by bvh file of Skeleton
// ====================================================================== //

std::shared_ptr<Skeleton> Skeleton::get(const std::string& path) {
  if (poolIndices.count(path) > 0) { return pool[poolIndices[path]]; }
  return nullptr;
}

// ====================================================================== //
// ====================================================================== //
// Create a new Skeleton if it isn't on the gloabl pool
// ====================================================================== //

std::shared_ptr<Skeleton> Skeleton::create(const std::string& bvhFilePath) {
  if (auto Sk = get(bvhFilePath)) { return Sk; }

  pool.push_back(std::make_shared<Skeleton>(bvhFilePath));

  auto idx                 = pool.size() - 1;
  poolIndices[bvhFilePath] = idx;
  return pool.at(idx);
}


// ====================================================================== //
// ====================================================================== //
// Getter for frame-time
// ====================================================================== //

float Skeleton::frameTime() const { return m_frameTime; }

// ====================================================================== //
// ====================================================================== //
// Getter for root pos
// ====================================================================== //

glm::vec3 Skeleton::rootPos() const { return m_joints.at(0)->currPos(); }


// ====================================================================== //
// ====================================================================== //
// Plays sekeleton animation
// ====================================================================== //

void Skeleton::play(int atFrame, bool showIfIsHidden) {
  m_play      = true;
  m_show      = (showIfIsHidden) ? true : m_show;
  m_currFrame = (atFrame < 0) ? m_currFrame : atFrame;
}

// ====================================================================== //
// ====================================================================== //
// Stops skeleton animation
// ====================================================================== //

void Skeleton::stop(bool hideIfIsShowed) {
  m_play = false;
  m_show = (hideIfIsShowed) ? false : m_show;
}

// ====================================================================== //
// ====================================================================== //
// Launch a detached thread where is computed the animation loop
// That loop have a delay between each iteration, equal to m_frameTime
// ====================================================================== //

void Skeleton::animate() {
  if (m_joints.size() <= 0) {
    dErr("No joints in skeleton of file {}", m_path);
    return;
  }

  // genLoop(m_joints);
  m_frames = m_joints.at(0)->frames();
  if (m_frames != -1) {

    std::call_once(m_animFlag, [&]() {
      dac::Async::periodic(m_frameTime, &m_animThread, [&]() {
        // Check thats playing, currframe dont exceed the frames number and
        // that joint zero is the root
        if (m_play && m_currFrame < m_frames &&
            m_joints.at(0)->name() == "Root") {

          // Translate
          m_joints.at(0)->updateTrans(m_currFrame, m_frameDespl);

          // Rotate
          for (auto& J : m_joints) { J->updateRot(m_currFrame); }

          // On init frame, store current pos
          if (m_currFrame == m_initFrame) {
            m_initPos = m_joints.at(0)->currPos();
          }

          // On last frame, reset currFrame and displacement between frames
          if (m_currFrame >= m_frames - 1) {
            m_frameDespl += m_joints.at(0)->currPos() - m_initPos;
            m_currFrame = m_initFrame - 1;
          }

          // Increment value of current frame
          ++m_currFrame;
        }
      });
    });
  }
}

// ====================================================================== //
// ====================================================================== //
// Draw the skelenton joints
// Should be called inside the render loop
// ====================================================================== //

void Skeleton::draw() {
  if (m_show)
    for (const auto& J : m_joints) J->draw(m_currFrame);
}

} // namespace brave
