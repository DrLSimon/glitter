#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "glApi.hpp"
#include "utils.hpp"

Buffer::Buffer(GLenum target) : m_location(0), m_target(target), m_attributeSize(0)
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must allocate GPU memory for this instance" << std::endl;
  assert(false);
}

Buffer::~Buffer()
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must release GPU memory for this instance" << std::endl;
  assert(false);
}

void Buffer::bind() const
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must bind the underlying GPU object to the openGL state" << std::endl;
  assert(false);
}

void Buffer::unbind() const
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must unbind the underlying GPU object from the openGL state" << std::endl;
  assert(false);
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
  std::cerr << __PRETTY_FUNCTION__ << ": You must allocate GPU memory for this instance" << std::endl;
  assert(false);
}

VAO::~VAO()
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must release GPU memory for this instance" << std::endl;
  assert(false);
}

void VAO::bind() const
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must bind the underlying GPU object to the openGL state" << std::endl;
  assert(false);
}

void VAO::unbind() const
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must unbind the underlying GPU object from the openGL state" << std::endl;
  assert(false);
}

void VAO::encapsulateVBO(unsigned int attributeIndex) const
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must complete the implementation (look at the documentation in the header)" << std::endl;
  assert(false);
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
  std::cerr << __PRETTY_FUNCTION__ << ": You must complete the implementation (look at the documentation in the header)" << std::endl;
  assert(false);
}

Shader::Shader(GLenum type, const std::string & filename) : m_location(0)
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must complete the implementation (look at the documentation in the header)" << std::endl;
  assert(false);
}

Shader::~Shader()
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must complete the implementation (look at the documentation in the header)" << std::endl;
  assert(false);
}

uint Shader::location() const
{
  return m_location;
}

Program::Program(const std::string & vname, const std::string & fname) : m_location(0), m_vshader(GL_VERTEX_SHADER, vname), m_fshader(GL_FRAGMENT_SHADER, fname)
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must complete the implementation (look at the documentation in the header)" << std::endl;
  assert(false);
}

Program::~Program()
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must complete the implementation (look at the documentation in the header)" << std::endl;
  assert(false);
}

void Program::bind() const
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must bind the underlying GPU object to the openGL state" << std::endl;
  assert(false);
}

void Program::unbind() const
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must unbind the underlying GPU object from the openGL state" << std::endl;
  assert(false);
}

bool Program::getUniformLocation(const std::string & name, int & location) const
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must update location to point to the target uniform and return true if valid" << std::endl;
  assert(false);
  return false;
}

template <> void Program::uniformDispatcher(int location, const int & val)
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must send the value to the uniform GPU location" << std::endl;
  assert(false);
}

template <> void Program::uniformDispatcher(int location, const uint & val)
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must send the value to the uniform GPU location" << std::endl;
  assert(false);
}

template <> void Program::uniformDispatcher(int location, const float & val)
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must send the value to the uniform GPU location" << std::endl;
  assert(false);
}

template <> void Program::uniformDispatcher(int location, const glm::vec2 & val)
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must send the values to the uniform GPU location" << std::endl;
  assert(false);
}

template <> void Program::uniformDispatcher(int location, const glm::vec3 & val)
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must send the values to the uniform GPU location" << std::endl;
  assert(false);
}

template <> void Program::uniformDispatcher(int location, const glm::vec4 & val)
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must send the values to the uniform GPU location" << std::endl;
  assert(false);
}

template <> void Program::uniformDispatcher(int location, const glm::mat2 & val)
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must send the values to the uniform GPU location" << std::endl;
  assert(false);
}

template <> void Program::uniformDispatcher(int location, const glm::mat3 & val)
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must send the values to the uniform GPU location" << std::endl;
  assert(false);
}

template <> void Program::uniformDispatcher(int location, const glm::mat4 & val)
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must send the values to the uniform GPU location" << std::endl;
  assert(false);
}

bool Program::bound() const
{
  int currentProgram;
  glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
  return m_location == (GLuint)currentProgram;
}

Texture::Texture(GLenum target) : m_location(0), m_target(target)
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must allocate GPU memory for this instance" << std::endl;
  assert(false);
}

Texture::~Texture()
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must release GPU memory for this instance" << std::endl;
  assert(false);
}

void Texture::bind() const
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must bind the underlying GPU object to the openGL state" << std::endl;
  assert(false);
}

void Texture::unbind() const
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must unbind the underlying GPU object from the openGL state" << std::endl;
  assert(false);
}

template <> void Texture::setData<GLubyte>(const Image<GLubyte> & image, bool mipmaps) const
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must complete the implementation (look at the documentation in the header)" << std::endl;
  assert(false);
}

Sampler::Sampler(int texUnit) : m_location(0), m_texUnit(texUnit)
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must allocate GPU memory for this instance" << std::endl;
  assert(false);
}

Sampler::~Sampler()
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must release GPU memory for this instance" << std::endl;
  assert(false);
}

void Sampler::bind() const
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must bind the underlying GPU object to the openGL state" << std::endl;
  assert(false);
}

void Sampler::unbind() const
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must unbind the underlying GPU object from the openGL state" << std::endl;
  assert(false);
}

void Sampler::attachToProgram(const Program & prog, const std::string & samplerName, BindOption bindOption) const
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must complete the implementation (look at the documentation in the header)" << std::endl;
  assert(false);
}

void Sampler::attachTexture(const Texture & texture) const
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must complete the implementation (look at the documentation in the header)" << std::endl;
  assert(false);
}

template <> void Sampler::setParameter<int>(GLenum paramName, const int & value) const
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must set up the target sampler parameter with the value" << std::endl;
  assert(false);
}

template <> void Sampler::setParameter<float>(GLenum paramName, const float & value) const
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must set up the target sampler parameter with the value" << std::endl;
  assert(false);
}

void Sampler::enableAnisotropicFiltering() const
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must complete the implementation (look at the documentation in the header)" << std::endl;
  assert(false);
}
