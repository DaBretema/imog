#pragma once

#include <memory>
#include <vector>

#include "wrap/Math.hpp"
#include "wrap/Colors.hpp"

#include "Shader.hpp"


namespace BRAVE {

class Renderable {

  static uint64_t g_ID;

private:
  uint64_t                m_ID      = 0;
  std::shared_ptr<Shader> m_shader  = nullptr;
  glm::vec3               m_color   = glm::vec3(0);
  glm::mat4               m_model   = glm::mat4(1.f);
  unsigned int            m_vao     = 0;
  unsigned int            m_loc     = 0;
  unsigned int            m_eboSize = 0;

public:
  // ID
  uint64_t ID() const;
  // Shader
  std::shared_ptr<Shader> shader() const;
  void                    shader(const std::shared_ptr<Shader>& newShader);
  // Color
  glm::vec3 color() const;
  void      color(glm::vec3 newColor);
  // Model
  glm::mat4 model() const;
  void      model(glm::mat4 newModel);

  // Ctors
  Renderable(const std::shared_ptr<Shader>& shaderPath,
             const glm::vec3&               color = Colors::Magenta);

  Renderable(const std::string&             objFilePath,
             const std::shared_ptr<Shader>& shaderPath,
             const glm::vec3&               color = Colors::Magenta);

  /// Bind this Renderable VAO(m_vao) as active to auto attach VBO, EBO, ...
  void bind();

  /// unbind this Renderable VAO(m_vao) as active to avoid modify VBO, EBO, ...
  void unbind();

  /// @param data, the type of vector should be glm::vecX
  /// addVBO add a vertex attribute to this Renderable
  template <typename T>
  void addVBO(const std::vector<T>& data);

  /// @param indices, the indices to draw this figure
  /// fillEBO store indices in the internal variable m_ebo
  void fillEBO(const std::vector<unsigned int>& indices);

  /// draw execute the Renderable in the viewport using its shader and vbos
  void draw();

  /// @param trans, vector whose values apply on translation
  /// translate is a wrap over glm translate
  void translate(const glm::vec3& trans);

  /// @param rot, vector whose values apply on rotation
  /// rotate is a wrap over glm roration
  void rotate(const glm::vec3& rot);

  /// @param scl, vector whose values apply on scale
  /// scale is a wrap over glm scale
  void scale(const glm::vec3& scl);
};

} // namespace BRAVE
