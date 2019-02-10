#include "Skeleton.hpp"
#include "LoaderBVH.hpp"

namespace brave {

unsigned int Skeleton::g_lastSkeletonID{0u};

Skeleton::Skeleton(const std::string& bvhFilePath)
    : m_id(g_lastSkeletonID++),
      m_show(true),
      m_play(true),
      m_initFrame(0),
      m_currFrame(m_initFrame),
      m_frameDespl(0.f),
      m_initPos() {
  std::tie(m_joints, m_frameTime) = loader::BVH(bvhFilePath);
}

} // namespace brave
