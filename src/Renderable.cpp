#include "Renderable.hpp"

#include <mutex>
#include <sstream>

#include <dac/Logger.hpp>

#include "Core.hpp"
#include "Loader.hpp"
#include "Settings.hpp"

#include "helpers/Consts.hpp"
#include "helpers/GLAssert.hpp"



namespace brave {

// ====================================================================== //
// ====================================================================== //
// Global Renderable objects counter
// ====================================================================== //

unsigned int Renderable::g_RenderablesLastID{0u};

// ====================================================================== //
// ====================================================================== //
// Global pool for renderables
// ====================================================================== //

std::vector<std::shared_ptr<Renderable>>      Renderable::pool{};
std::unordered_map<std::string, unsigned int> Renderable::poolIndices{};


// ====================================================================== //
// ====================================================================== //
// Param constructor w/o OBJ file
// ====================================================================== //

Renderable::Renderable(bool                           allowGlobalDraw,
                       const std::string&             name,
                       const std::string&             objFilePath,
                       const std::string&             texturePath,
                       const glm::vec3&               color,
                       const std::shared_ptr<Shader>& shader,
                       bool                           culling)
    : m_allowGlobalDraw(allowGlobalDraw),
      m_ID(g_RenderablesLastID++),
      m_name(name),
      m_meshPath(objFilePath),
      m_shader(shader),
      m_texture(Texture::create(texturePath)),
      m_culling(culling),
      m_color(color),
      // m_model(glm::mat4(1.f)),
      m_vao(0),
      m_loc(0),
      m_eboSize(0) {

  GL_ASSERT(glGenVertexArrays(1, &m_vao));
  if (!m_shader) { m_shader = Shader::getByName(Shaders::base); }
  if (m_name.empty()) { m_name = std::string("R_" + std::to_string(m_ID)); }

  if (!objFilePath.empty()) {
    Renderable::data renderData = loader::OBJ(objFilePath);
    this->fillEBO(renderData.indices); // No location, just internal data.
    this->addVBO(renderData.vertices); // Location = 0
    this->addVBO(renderData.normals);  // Location = 1
    this->addVBO(renderData.uvs);      // Location = 2
  }

  pool.push_back(std::shared_ptr<Renderable>(this));
}

// ====================================================================== //
// ====================================================================== //
// Destructor
// ====================================================================== //

Renderable::~Renderable() {
  if (!Settings::quiet) dInfo("Destroyed @ {}.{}", m_ID, m_name);
}


// ====================================================================== //
// ====================================================================== //
// Get a shared ptr to Renderable obj from global pool
// by mixed data of Renderable, like id or objFilepath
// ====================================================================== //

std::shared_ptr<Renderable> Renderable::get(const std::string& dataMix) {
  if (poolIndices.count(dataMix) > 0) { return pool[poolIndices[dataMix]]; }
  return nullptr;
}

// ====================================================================== //
// ====================================================================== //
// Get a shared ptr to Renderable obj from global pool
// by name
// ====================================================================== //

std::shared_ptr<Renderable> Renderable::getByName(const std::string& name) {
  if (poolIndices.count(name) > 0) { return pool[poolIndices[name]]; }
  return nullptr;
}

// ====================================================================== //
// ====================================================================== //
// Create a new Renderable if it isn't on the gloabl pool
// ====================================================================== //

std::shared_ptr<Renderable>
    Renderable::create(bool                           allowGlobalDraw,
                       const std::string&             name,
                       const std::string&             objFilePath,
                       const std::string&             texturePath,
                       const glm::vec3&               color,
                       const std::shared_ptr<Shader>& shader,
                       bool                           culling) {
  std::string key = name + objFilePath + texturePath + glm::to_string(color);
  if (shader) { key += shader->name(); }
  if (auto R = get(key)) { return R; }

  pool.push_back(std::make_shared<Renderable>(
      allowGlobalDraw, name, objFilePath, texturePath, color, shader, culling));

  auto idx          = pool.size() - 1;
  poolIndices[name] = idx;
  poolIndices[key]  = idx;
  return pool.at(idx);
}


// ====================================================================== //
// ====================================================================== //
// Bind this Renderable VAO(m_vao) as active to auto attach VBO, EBO, ...
// ====================================================================== //

void Renderable::bind() { GL_ASSERT(glBindVertexArray(m_vao)); }

// ====================================================================== //
// ====================================================================== //
// Unbind this Renderable VAO(m_vao) as active to avoid modify VBO, EBO, ...
// ====================================================================== //

void Renderable::unbind() {
  GL_ASSERT(glBindVertexArray(0));
  GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, 0));
  GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}


// ====================================================================== //
// ====================================================================== //
// Getter for global draw privileges
// ====================================================================== //

bool Renderable::allowGlobalDraw() const { return m_allowGlobalDraw; }

// ====================================================================== //
// ====================================================================== //
// Getter for ID
// ====================================================================== //

unsigned int Renderable::ID() const { return m_ID; }

// ====================================================================== //
// ====================================================================== //
// Getter for name
// ====================================================================== //

std::string Renderable::name() const { return m_name; }

// ====================================================================== //
// ====================================================================== //
// G/Setter for shader
// ====================================================================== //

std::shared_ptr<Shader> Renderable::shader() const { return m_shader; }
void Renderable::shader(const std::shared_ptr<Shader>& newShader) {
  m_shader = newShader;
}

// ====================================================================== //
// ====================================================================== //
// G/Setter for color
// ====================================================================== //

glm::vec3 Renderable::color() const { return m_color; }
void      Renderable::color(const glm::vec3& newColor) { m_color = newColor; }

// ====================================================================== //
// ====================================================================== //
// G/Setter for model
// ====================================================================== //

// glm::mat4 Renderable::model() const { return m_model; }
// void      Renderable::model(const glm::mat4& newModel) { m_model = newModel; }
// void      Renderable::updateModel() {
//   m_model = glm::mat4(1.f);
//   Math::translate(m_model, m_pos);
//   Math::rotateXYZ(m_model, m_rot);
//   Math::scale(m_model, m_scl);
// }

// // ====================================================================== //
// // ====================================================================== //
// // G/Setter for pos
// // ====================================================================== //

// glm::vec3 Renderable::pos() const { return m_pos; }
// void      Renderable::pos(const glm::vec3& newPos) {
//   m_pos = newPos;
//   updateModel();
// }
// void Renderable::pos(float x, float y, float z) { pos(glm::vec3{x, y, z}); }
// void Renderable::accumPos(const glm::vec3& addPos) { pos(m_pos + addPos); }
// void Renderable::accumPos(float x, float y, float z) {
//   accumPos(glm::vec3{x, y, z});
// }

// // ====================================================================== //
// // ====================================================================== //
// // G/Setter for rot
// // ====================================================================== //

// glm::vec3 Renderable::rot() const { return m_rot; }
// void      Renderable::rot(const glm::vec3& newRot) {
//   m_rot = newRot;
//   updateModel();
// }
// void Renderable::rot(float x, float y, float z) { rot(glm::vec3{x, y, z}); }
// void Renderable::accumRot(const glm::vec3& addRot) { rot(m_rot + addRot); }
// void Renderable::accumRot(float x, float y, float z) {
//   accumRot(glm::vec3{x, y, z});
// }

// // ====================================================================== //
// // ====================================================================== //
// // G/Setter for scl
// // ====================================================================== //

// glm::vec3 Renderable::scl() const { return m_scl; }
// void      Renderable::scl(const glm::vec3& newScl) {
//   m_scl = newScl;
//   updateModel();
// }
// void Renderable::scl(float x, float y, float z) { scl(glm::vec3{x, y, z}); }
// void Renderable::accumScl(const glm::vec3& addScl) { scl(m_scl + addScl); }
// void Renderable::accumScl(float x, float y, float z) {
//   accumScl(glm::vec3{x, y, z});
// }

// ====================================================================== //
// ====================================================================== //
// Add a vertex attribute to this Renderable
// ====================================================================== //

template <typename T>
void Renderable::addVBO(const std::vector<T>& data) {
  this->bind();
  {
    // Upload data to OpenGL
    unsigned int vbo;
    GL_ASSERT(glGenBuffers(1, &vbo));
    GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    GL_ASSERT(glBufferData(
        GL_ARRAY_BUFFER, data.size() * sizeof(T), &data[0], GL_DYNAMIC_DRAW));
    // Link data to a vertex attribute (in order of entry)
    GL_ASSERT(glEnableVertexAttribArray(m_loc));
    GL_ASSERT(glVertexAttribPointer(
        m_loc, data[0].length(), GL_FLOAT, GL_FALSE, 0, 0));
    // Increment location index for vertex attribs
    // [!] Check that order in your shader is the order in which you add VBOs
    ++m_loc;
  }
  this->unbind();
}


// ====================================================================== //
// ====================================================================== //
// Store indices in the internal variable m_ebo
// ====================================================================== //

void Renderable::fillEBO(const std::vector<unsigned int>& indices) {
  this->bind();
  // Store indices count
  m_eboSize = indices.size();
  // Upload indices to OpenGL
  unsigned int ebo;
  GL_ASSERT(glGenBuffers(1, &ebo));
  GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
  GL_ASSERT(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         m_eboSize * sizeof(unsigned int),
                         &indices[0],
                         GL_DYNAMIC_DRAW));
  this->unbind();
}

// ====================================================================== //
// ====================================================================== //
// Draw execute the Renderable in the viewport using its shader and vbos
// ====================================================================== //

void Renderable::draw() {
  this->bind();
  m_shader->bind();
  (m_texture) ? m_texture->bind(m_shader)
              : m_shader->uInt1("u_texture", 99); // "Disable" texture

  m_shader->uFloat3("u_color", m_color);

  auto currModel = this->transform.model();

  glm::mat4 matMV = Core::camera->view() * currModel;
  m_shader->uMat4("u_matMV", matMV);
  m_shader->uMat4("u_matN", glm::transpose(glm::inverse(matMV)));

  m_shader->uMat4("u_matM", currModel);
  m_shader->uMat4("u_matMVP", Core::camera->viewproj() * currModel);

  if (!m_culling) { glDisable(GL_CULL_FACE); }
  GL_ASSERT(glDrawElements(GL_TRIANGLES, m_eboSize, GL_UNSIGNED_INT, 0));
  if (!m_culling) { glEnable(GL_CULL_FACE); }

  if (m_texture) m_texture->unbind();
  m_shader->unbind();
  this->unbind();
}

} // namespace brave
