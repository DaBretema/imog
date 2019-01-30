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
  // Param constructor w/o OBJ file
  Renderable(const std::shared_ptr<Shader>& shaderPath,
             const glm::vec3&               color = Colors::Magenta);

  // Param constructor w/ OBJ file, call basic constructor and link
  // render data of the OBJ file to EBO(indices) and
  // VBOs(vertices, normals)
  Renderable(const std::string&             objFilePath,
             const std::shared_ptr<Shader>& shaderPath,
             const glm::vec3&               color = Colors::Magenta);


  // Bind this Renderable VAO(m_vao) as active to auto attach VBO, EBO, ...
  void bind();

  // Unbind this Renderable VAO(m_vao) as active to avoid modify VBO, EBO, ...
  void unbind();


  // Getter for ID
  uint64_t ID() const;

  // G/Setter for shader
  std::shared_ptr<Shader> shader() const;
  void                    shader(const std::shared_ptr<Shader>& newShader);

  // G/Setter for color
  glm::vec3 color() const;
  void      color(glm::vec3 newColor);

  // G/Setter for model
  glm::mat4 model() const;
  void      model(glm::mat4 newModel);


  // Add a vertex attribute to this Renderable
  template <typename T>
  void addVBO(const std::vector<T>& data);

  // Store indices in the internal variable m_ebo
  void fillEBO(const std::vector<unsigned int>& indices);

  // Draw execute the Renderable in the viewport using its shader and vbos
  void draw();

  // Transform operations wrappers
  void translate(const glm::vec3& trans);
  void rotate(const glm::vec3& rot);
  void scale(const glm::vec3& scl);
};

} // namespace BRAVE
