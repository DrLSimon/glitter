#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "glApi.hpp"
#include "utils.hpp"

Buffer::Buffer(GLenum target) : m_location(0), m_target(target), m_attributeSize(0)
{
  FAIL_BECAUSE_INCOMPLETE;
}

Buffer::~Buffer()
{
  FAIL_BECAUSE_INCOMPLETE;
}

void Buffer::bind() const
{
  FAIL_BECAUSE_INCOMPLETE;
}

void Buffer::unbind() const
{
  FAIL_BECAUSE_INCOMPLETE;
}

uint Buffer::attributeCount() const
{
  return m_attributeCount;
}

GLenum Buffer::attributeType() const
{
  return m_attributeType;
}

uint Buffer::attributeSize() const
{
  return m_attributeSize;
}

VAO::VAO(uint nbVBO) : m_location(0), m_vbos(nbVBO), m_ibo(GL_ELEMENT_ARRAY_BUFFER)
{
  for (auto & vbo : m_vbos) {
    vbo = std::shared_ptr<Buffer>(new Buffer(GL_ARRAY_BUFFER));
  }
  assert(nbVBO <= 16); // You may want to replace 16 by the real hardware limitation
  FAIL_BECAUSE_INCOMPLETE;
}

VAO::~VAO()
{
  FAIL_BECAUSE_INCOMPLETE;
}

void VAO::bind() const
{
  FAIL_BECAUSE_INCOMPLETE;
}

void VAO::unbind() const
{
  FAIL_BECAUSE_INCOMPLETE;
}

void VAO::encapsulateVBO(unsigned int attributeIndex) const
{
  FAIL_BECAUSE_INCOMPLETE;
}

std::shared_ptr<VAO> VAO::makeSlaveVAO() const
{
  unsigned int nbVBO = m_vbos.size();
  std::shared_ptr<VAO> slave(new VAO(nbVBO));
  slave->m_vbos = m_vbos;
  slave->bind();
  for (unsigned int attributeIndex = 0; attributeIndex < nbVBO; attributeIndex++) {
    slave->encapsulateVBO(attributeIndex);
  }
  slave->unbind();
  return slave;
}

void VAO::draw(GLenum mode) const
{
  FAIL_BECAUSE_INCOMPLETE;
}

Shader::Shader(GLenum type, const std::string & filename) : m_location(0)
{
  FAIL_BECAUSE_INCOMPLETE;
}

Shader::~Shader()
{
  FAIL_BECAUSE_INCOMPLETE;
}

uint Shader::location() const
{
  return m_location;
}

Program::Program(const std::string & vname, const std::string & fname) : m_location(0), m_vshader(GL_VERTEX_SHADER, vname), m_fshader(GL_FRAGMENT_SHADER, fname)
{
  FAIL_BECAUSE_INCOMPLETE;
}

Program::~Program()
{
  FAIL_BECAUSE_INCOMPLETE;
}

void Program::bind() const
{
  FAIL_BECAUSE_INCOMPLETE;
}

void Program::unbind() const
{
  FAIL_BECAUSE_INCOMPLETE;
}

bool Program::getUniformLocation(const std::string & name, int & location) const
{
  FAIL_BECAUSE_INCOMPLETE;
  return false;
}

template <> void Program::uniformDispatcher(int location, const int & val)
{
  FAIL_BECAUSE_INCOMPLETE;
}

template <> void Program::uniformDispatcher(int location, const uint & val)
{
  FAIL_BECAUSE_INCOMPLETE;
}

template <> void Program::uniformDispatcher(int location, const float & val)
{
  FAIL_BECAUSE_INCOMPLETE;
}

template <> void Program::uniformDispatcher(int location, const glm::vec2 & val)
{
  FAIL_BECAUSE_INCOMPLETE;
}

template <> void Program::uniformDispatcher(int location, const glm::vec3 & val)
{
  FAIL_BECAUSE_INCOMPLETE;
}

template <> void Program::uniformDispatcher(int location, const glm::vec4 & val)
{
  FAIL_BECAUSE_INCOMPLETE;
}

template <> void Program::uniformDispatcher(int location, const glm::mat2 & val)
{
  FAIL_BECAUSE_INCOMPLETE;
}

template <> void Program::uniformDispatcher(int location, const glm::mat3 & val)
{
  FAIL_BECAUSE_INCOMPLETE;
}

template <> void Program::uniformDispatcher(int location, const glm::mat4 & val)
{
  FAIL_BECAUSE_INCOMPLETE;
}

bool Program::bound() const
{
  int currentProgram;
  glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
  return m_location == (GLuint)currentProgram;
}

Texture::Texture(GLenum target) : m_location(0), m_target(target)
{
  FAIL_BECAUSE_INCOMPLETE;
}

Texture::~Texture()
{
  FAIL_BECAUSE_INCOMPLETE;
}

void Texture::bind() const
{
  FAIL_BECAUSE_INCOMPLETE;
}

void Texture::unbind() const
{
  FAIL_BECAUSE_INCOMPLETE;
}

template <> void Texture::setData<GLubyte>(const Image<GLubyte> & image, bool mipmaps) const
{
  FAIL_BECAUSE_INCOMPLETE;
}

Sampler::Sampler(int texUnit) : m_location(0), m_texUnit(texUnit)
{
  FAIL_BECAUSE_INCOMPLETE;
}

Sampler::~Sampler()
{
  FAIL_BECAUSE_INCOMPLETE;
}

void Sampler::bind() const
{
  FAIL_BECAUSE_INCOMPLETE;
}

void Sampler::unbind() const
{
  FAIL_BECAUSE_INCOMPLETE;
}

void Sampler::attachToProgram(const Program & prog, const std::string & samplerName, BindOption bindOption) const
{
  FAIL_BECAUSE_INCOMPLETE;
}

void Sampler::attachTexture(const Texture & texture) const
{
  FAIL_BECAUSE_INCOMPLETE;
}

template <> void Sampler::setParameter<int>(GLenum paramName, const int & value) const
{
  FAIL_BECAUSE_INCOMPLETE;
}

template <> void Sampler::setParameter<float>(GLenum paramName, const float & value) const
{
  FAIL_BECAUSE_INCOMPLETE;
}

void Sampler::enableAnisotropicFiltering() const
{
  FAIL_BECAUSE_INCOMPLETE;
}
