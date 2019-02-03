#include "Renderable.hpp"

#include <mutex>
#include <sstream>

#include <dac/Logger.hpp>

#include "Core.hpp"
#include "LoaderOBJ.hpp"
#include "Settings.hpp"

#include "helpers/Paths.hpp"
#include "helpers/GLAssert.hpp"



namespace BRAVE {

// ====================================================================== //
// ====================================================================== //
// Global Renderable objects counter
// ====================================================================== //

unsigned int Renderable::g_RenderablesLastID{0u};

// ====================================================================== //
// ====================================================================== //
// Global pool for shaders
// ====================================================================== //

std::vector<std::shared_ptr<Renderable>>      Renderable::pool{};
std::unordered_map<std::string, unsigned int> Renderable::poolIndices{};


// ====================================================================== //
// ====================================================================== //
// Param constructor w/o OBJ file
// ====================================================================== //

Renderable::Renderable(const std::string&             objFilePath,
                       const std::string&             texturePath,
                       const glm::vec3&               color,
                       const std::shared_ptr<Shader>& shader)
    : m_ID(g_RenderablesLastID++),
      m_shader(shader),
      m_texture(Texture::create(texturePath)),
      m_color(color),
      m_model(glm::mat4(1.f)),
      m_vao(0),
      m_loc(0),
      m_eboSize(0) {

  GL_ASSERT(glGenVertexArrays(1, &m_vao));

  if (objFilePath != "") {
    RenderData renderData = loadOBJ(objFilePath);
    this->fillEBO(renderData.indices); // No location, just internal data.
    this->addVBO(renderData.vertices); // Location = 0
    this->addVBO(renderData.normals);  // Location = 1
    this->addVBO(renderData.uvs);      // Location = 2
  }

  if (m_shader == nullptr) {
    std::string sV = Paths::Shaders + "base.vert";
    std::string sG = Paths::Shaders + "base.geom";
    std::string sF = Paths::Shaders + "base.frag";
    dInfo("SET SHADER");
    m_shader = Shader::create("BASE", sV, sG, sF);
    dInfo("/ SET SHADER");
  }

  pool.push_back(std::shared_ptr<Renderable>(this));
}


// ====================================================================== //
// ====================================================================== //
// Get a shared ptr to Renderable obj from global pool
// by mixed data of Renderable, like id or objFilepath
// ====================================================================== //

std::shared_ptr<Renderable> Renderable::get(const std::string dataMix) {
  if (poolIndices.count(dataMix) > 0) { return pool[poolIndices[dataMix]]; }
  return nullptr;
}

// ====================================================================== //
// ====================================================================== //
// Create a new Renderable if it isn't on the gloabl pool
// ====================================================================== //

std::shared_ptr<Renderable>
    Renderable::create(const std::string&             objFilePath,
                       const std::string&             texturePath,
                       const glm::vec3&               color,
                       const std::shared_ptr<Shader>& shader) {
  std::string key = objFilePath + texturePath;
  key += glm::to_string(color);
  if (shader != nullptr) { key += shader->name(); }
  if (auto R = get(key); R != nullptr) { return R; }

  pool.push_back(
      std::make_shared<Renderable>(objFilePath, texturePath, color, shader));

  auto idx         = pool.size() - 1;
  poolIndices[key] = idx;
  return pool.at(idx);
}


// ====================================================================== //
// ====================================================================== //
// Destructor
// ====================================================================== //

Renderable::~Renderable() {
  if (!Settings::quiet) dInfo("ID: {}, Destroyed!", m_ID);
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
// Getter for ID
// ====================================================================== //

unsigned int Renderable::ID() const { return m_ID; }

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
void      Renderable::color(glm::vec3 newColor) { m_color = newColor; }

// ====================================================================== //
// ====================================================================== //
// G/Setter for model
// ====================================================================== //

glm::mat4 Renderable::model() const { return m_model; }
void      Renderable::model(glm::mat4 newModel) { m_model = newModel; }


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
  m_texture->bind();


  m_shader->uFloat3("u_color", m_color);
  m_shader->uMat4("u_matM", m_model);
  m_shader->uMat4("u_matMVP", BRAVE::Core::camera->viewproj() * m_model);

  glm::mat4 matMV = BRAVE::Core::camera->view() * m_model;
  m_shader->uMat4("u_matMV", matMV);
  m_shader->uMat4("u_matN", glm::transpose(glm::inverse(matMV)));

  GL_ASSERT(glDrawElements(GL_TRIANGLES, m_eboSize, GL_UNSIGNED_INT, 0));

  m_texture->unbind();
  m_shader->unbind();
  this->unbind();
}

// ====================================================================== //
// ====================================================================== //
// Transform operations wrappers
// ====================================================================== //

void Renderable::translate(const glm::vec3& T) { Math::translate(m_model, T); }
void Renderable::translate(float x, float y, float z) {
  this->translate(glm::vec3{x, y, z});
}

void Renderable::rotate(const glm::vec3& R) { Math::rotate(m_model, R); }
void Renderable::rotate(float x, float y, float z) {
  this->rotate(glm::vec3{x, y, z});
}

void Renderable::scale(const glm::vec3& S) { Math::scale(m_model, S); }
void Renderable::scale(float x, float y, float z) {
  this->scale(glm::vec3{x, y, z});
}

} // namespace BRAVE
