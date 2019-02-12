#pragma once

#include <memory>
#include <vector>

#include "Math.hpp"
#include "helpers/Colors.hpp"

#include "Shader.hpp"
#include "Texture.hpp"


namespace brave {

class Renderable {

  static unsigned int g_RenderablesLastID;

public:
  // Global pool for renderables
  static std::vector<std::shared_ptr<Renderable>>      pool;
  static std::unordered_map<std::string, unsigned int> poolIndices;


private:
  bool m_allowGlobalDraw;

  unsigned int m_ID;
  std::string  m_name;
  std::string  m_meshPath;

  std::shared_ptr<Shader>  m_shader;
  std::shared_ptr<Texture> m_texture;

  bool m_culling;

  glm::vec3 m_color;
  glm::mat4 m_model;

  unsigned int m_vao;
  unsigned int m_loc;
  unsigned int m_eboSize;

  glm::vec3 m_pos;
  glm::vec3 m_rot;
  glm::vec3 m_scl;

  void updateModel();

public:
  // Param constructor w/o OBJ file
  Renderable(bool                           allowGlobalDraw = true,
             const std::string&             name            = "",
             const std::string&             objFilePath     = "",
             const std::string&             texturePath     = "",
             const glm::vec3&               color           = Colors::magenta,
             const std::shared_ptr<Shader>& shader          = nullptr,
             bool                           culling         = true);

  // Destructor
  ~Renderable();


  // Get a shared ptr to Renderable obj from global pool
  // by mixed data of Renderable, like id or objFilepath
  static std::shared_ptr<Renderable> get(const std::string& dataMix);

  // Get a shared ptr to Renderable obj from global pool
  // by name
  static std::shared_ptr<Renderable> getByName(const std::string& name);

  // Create a new Renderable if it isn't on the gloabl pool
  static std::shared_ptr<Renderable>
      create(bool                           allowGlobalDraw = true,
             const std::string&             name            = "",
             const std::string&             objFilePath     = "",
             const std::string&             texturePath     = "",
             const glm::vec3&               color           = Colors::magenta,
             const std::shared_ptr<Shader>& shader          = nullptr,
             bool                           culling         = true);


  // Bind this Renderable VAO(m_vao) as active to auto attach VBO, EBO, ...
  void bind();

  // Unbind this Renderable VAO(m_vao) as active to avoid modify VBO, EBO, ...
  void unbind();


  // Getter for global draw privileges
  bool allowGlobalDraw() const;

  // Getter for ID
  unsigned int ID() const;

  // Getter for name
  std::string name() const;

  // G/Setter for shader
  std::shared_ptr<Shader> shader() const;
  void                    shader(const std::shared_ptr<Shader>& newShader);

  // G/Setter for color
  glm::vec3 color() const;
  void      color(const glm::vec3& newColor);

  // G/Setter for model
  glm::mat4 model() const;
  void      model(const glm::mat4& newModel);

  // G/Setter for pos
  glm::vec3 pos() const;
  void      pos(const glm::vec3& newPos);
  void      pos(float x, float y, float z);
  void      accumPos(const glm::vec3& addPos);
  void      accumPos(float x, float y, float z);

  // G/Setter for rot
  glm::vec3 rot() const;
  void      rot(const glm::vec3& newRot);
  void      rot(float x, float y, float z);
  void      accumRot(const glm::vec3& addRot);
  void      accumRot(float x, float y, float z);

  // G/Setter for scl
  glm::vec3 scl() const;
  void      scl(const glm::vec3& newScl);
  void      scl(float x, float y, float z);
  void      accumScl(const glm::vec3& addScl);
  void      accumScl(float x, float y, float z);


  // Add a vertex attribute to this Renderable
  template <typename T>
  void addVBO(const std::vector<T>& data);

  // Store indices in the internal variable m_ebo
  void fillEBO(const std::vector<unsigned int>& indices);

  // Draw execute the Renderable in the viewport using its shader and vbos
  void draw();
};

} // namespace brave
