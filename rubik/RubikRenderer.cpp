#include "RubikRenderer.hpp"
#include <GLFW/glfw3.h>
#include <functional>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <unordered_map>
#include "RubikLogic.hpp"

/// a simple struct for discrete linear range of the form {minVal, minVal+delta,...,minVal+length}
struct DiscreteLinRange {
  uint nbVals;  ///< the number of values in the range
  float minVal; ///< the minimum value of the range
  float length; ///< the lengh (max -min) of the range

  /// Constructor
  DiscreteLinRange(uint nbVals, float minVal, float length) : nbVals(nbVals), minVal(minVal), length(length) {}

  /// Returns the k^{th} value of the range
  float value(uint k) { return minVal + k * length / (nbVals - 1); }
};

std::shared_ptr<VAO> makeParamSurf(DiscreteLinRange rgPhi, DiscreteLinRange rgTheta, const std::function<glm::vec3(float, float)> & posFunc, bool isCyclicInPhi, bool isCyclicInTheta)
{
  std::vector<glm::vec3> positions;
  std::vector<glm::vec3> colors;
  std::vector<uint> ibo;
  for (uint kPhi = 0; kPhi < rgPhi.nbVals; kPhi++) {
    for (uint kTheta = 0; kTheta < rgTheta.nbVals; kTheta++) {
      float phi = rgPhi.value(kPhi);
      float theta = rgTheta.value(kTheta);
      glm::vec3 position = posFunc(phi, theta);
      positions.push_back(position);
      glm::vec3 color(0);
      const float eps = 0.01;
      if (0.5 - position.x < eps) {
        color = glm::vec3(1, 1, 1); // right face is white
      } else if (0.5 + position.x < eps) {
        color = glm::vec3(1, 1, 0); // left face is yellow
      } else if (0.5 - position.y < eps) {
        color = glm::vec3(0, 1, 0); // top face is green
      } else if (0.5 + position.y < eps) {
        color = glm::vec3(0, 0, 1); // down face is blue
      } else if (0.5 - position.z < eps) {
        color = glm::vec3(1, 0, 0); // front face is red
      } else if (0.5 + position.z < eps) {
        color = glm::vec3(1, 0.5, 0); // back face is orange
      }
      colors.push_back(color);
      if ((kPhi == rgPhi.nbVals - 1 and not isCyclicInPhi) or (kTheta == rgTheta.nbVals - 1 and not isCyclicInTheta)) {
        continue;
      }
      auto index = [&](uint i, uint j) { return (i % rgPhi.nbVals) * rgTheta.nbVals + (j % rgTheta.nbVals); };
      uint patchIndices[] = {
          index(kPhi, kTheta),     index(kPhi, kTheta + 1), index(kPhi + 1, kTheta),     // first triangle
          index(kPhi + 1, kTheta), index(kPhi, kTheta + 1), index(kPhi + 1, kTheta + 1), // second triangle
      };
      ibo.insert(ibo.end(), patchIndices, patchIndices + 6);
    }
  }

  std::shared_ptr<VAO> vao(new VAO(2));
  vao->setVBO(0, positions);
  vao->setVBO(1, colors);
  vao->setIBO(ibo);
  return vao;
}

std::shared_ptr<VAO> RubikRenderer::InstancedVAO::makeARoundedCube(unsigned int nbPhi, unsigned int nbTheta)
{
  const float exponent = 0.12;
  auto signPow = [](float x, float exp) {
    if (x < 0)
      return -pow(-x, exp);
    else
      return pow(x, exp);
  };
  auto posFunc = [&](float phi, float theta) { return 0.5f * glm::vec3(signPow(cos(phi) * sin(theta), exponent), signPow(sin(phi) * sin(theta), exponent), signPow(-cos(theta), exponent)); };

  const float pi = glm::pi<float>();
  return makeParamSurf(DiscreteLinRange(nbPhi, 0, 2 * pi), DiscreteLinRange(nbTheta, 0, pi), posFunc, true, false);
}

RubikRenderer::RubikRenderer() : m_program("rubik/rubik.v.glsl", "rubik/rubik.f.glsl"), m_view(1), m_currentTime(0), m_deltaTime(0)
{
  GLFWwindow * window = glfwGetCurrentContext();
  int windowWidth, windowHeight;
  glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
  resize(window, windowWidth, windowHeight);
  initGLState();
  createTheVAO();
}

void RubikRenderer::createTheVAO()
{
  m_vao = InstancedVAO::makeARoundedCube(50, 50);
  for (int x = -1; x <= 1; x++) {
    for (int y = -1; y <= 1; y++) {
      for (int z = -1; z <= 1; z++) {
        glm::mat4 mw(1);
        mw = glm::scale(mw, glm::vec3(2.f / 3.f / sqrt(3)));
        mw = glm::translate<float>(mw, {x, y, z});
        mw = glm::scale(mw, glm::vec3(1.025));
        unsigned int cnt = static_cast<unsigned int>(RubikPiece(x, y, z));
        if (x == 0 and y == 0 and z == 0) {
          m_vaos[cnt] = InstancedVAO::createInstance(nullptr, mw);
        }
        m_vaos[cnt] = InstancedVAO::createInstance(m_vao, mw);
      }
    }
  }
}

void RubikRenderer::initGLState() const
{
  glClearColor(1, 1, 1, 1);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
}

void RubikRenderer::deform(bool activate)
{
  m_program.bind();
  m_program.setUniform("deform", (uint)activate);
  m_program.unbind();
}

// Template specialization of std::hash
namespace std
{
template <> struct hash<glm::ivec3> {
public:
  std::size_t operator()(const glm::ivec3 & v) const
  {
    std::size_t hashes[] = {std::hash<float>()(v.x), std::hash<float>()(v.y), std::hash<float>()(v.z)};
    std::size_t seed = 0;
    for (std::size_t h : hashes) {
      // from boost::hash_combine
      seed ^= h + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
  }
};
} // namespace std

RubikFace RubikRenderer::getFace(unsigned int face) const
{
  const glm::ivec3 normals[3] = {{0, 0, -1}, {1, 0, 0}, {0, 1, 0}};
  assert(face < 3);
  glm::ivec3 normal = normals[face];
  normal = glm::ivec3(glm::round(glm::mat3(glm::transpose(m_view)) * glm::vec3(normal)));
  const std::unordered_map<glm::ivec3, RubikFaceName> names = {
      {{0, 0, -1}, RubikFaceName::F}, //
      {{1, 0, 0}, RubikFaceName::R},  //
      {{0, 1, 0}, RubikFaceName::T},  //
      {{0, 0, 1}, RubikFaceName::B},  //
      {{-1, 0, 0}, RubikFaceName::L}, //
      {{0, -1, 0}, RubikFaceName::D}, //
  };
  return RubikFace(names.at(normal));
}

void RubikRenderer::renderFrame()
{
  m_program.bind();
  glm::mat4 view(1);
  const float pi = glm::pi<float>();
  view = glm::rotate(glm::mat4(1), pi / 7, {0, 1, 0});
  view = glm::rotate(glm::mat4(1), -pi / 4, {1, 0, 0}) * view * m_view;
  for (const auto & vao : m_vaos) {
    vao->updateProgram(m_program, m_proj, view);
    vao->draw();
  }
  m_program.unbind();
}

bool RubikRenderer::isLocked() const
{
  for (const auto & vao : m_vaos) {
    if (vao->isLocked())
      return true;
  }
  return m_viewAnim.isLocked();
}

void RubikRenderer::update()
{
  float prevTime = m_currentTime;
  m_currentTime = glfwGetTime();
  m_deltaTime = m_currentTime - prevTime;
  m_program.bind();
  m_program.setUniform("time", m_currentTime);
  m_program.unbind();
  m_viewAnim.update(m_deltaTime);
  for (auto & vao : m_vaos) {
    vao->update(m_deltaTime);
  }
}

void RubikRenderer::launchViewRotation(const glm::vec3 & axis, float angle)
{
  const float pi = glm::pi<float>();
  m_viewAnim.startAnimation(m_view, axis, angle * pi / 2.);
}

void RubikRenderer::resetView()
{
  m_view = glm::mat4(1);
}

void RubikRenderer::launchFaceRotation(const RubikFace & face, const std::array<uint, 9> & pieces)
{
  const float pi = glm::pi<float>();
  glm::vec3 axis = -face.n;
  for (uint piece : pieces) {
    auto & vao = m_vaos[piece];
    vao->launchRotation(axis, pi / 2);
  }
}

void RubikRenderer::resize(GLFWwindow * /*window*/, int framebufferWidth, int framebufferHeight)
{
  float aspect = framebufferHeight / float(framebufferWidth);
  if (aspect > 1) {
    m_proj = glm::ortho(-1.f, 1.f, -aspect, aspect);
  } else {
    m_proj = glm::ortho(-1 / aspect, 1 / aspect, -1.f, 1.f);
  }
}

RubikRenderer::InstancedVAO::InstancedVAO(const std::shared_ptr<VAO> & vao, const glm::mat4 & modelWorld) : m_vao(vao), m_mw(modelWorld) {}

std::shared_ptr<RubikRenderer::InstancedVAO> RubikRenderer::InstancedVAO::createInstance(const std::shared_ptr<VAO> & vao, const glm::mat4 & modelWorld)
{
  return std::shared_ptr<InstancedVAO>(new InstancedVAO(vao, modelWorld));
}

void RubikRenderer::InstancedVAO::draw(GLenum mode) const
{
  if (m_vao) {
    m_vao->draw(mode);
  }
}

void RubikRenderer::InstancedVAO::updateProgram(Program & prog, const glm::mat4 & proj, const glm::mat4 & view) const
{
  prog.setUniform("MVP", proj * view * m_mw);
}

void RubikRenderer::InstancedVAO::launchRotation(const glm::vec3 & axis, float angle)
{
  m_anim.startAnimation(m_mw, axis, angle);
}

void RubikRenderer::InstancedVAO::update(float deltaTime)
{
  m_anim.update(deltaTime);
}

bool RubikRenderer::InstancedVAO::isLocked() const
{
  return m_anim.isLocked();
}

RubikRenderer::RotateAnimation::RotateAnimation() : m_locked(false) {}

void RubikRenderer::RotateAnimation::startAnimation(glm::mat4 & target, const glm::vec3 & axis, float angle)
{
  m_rotAxis = axis;
  m_rotAngle = angle;
  m_locked = true;
  m_target = &target;
  m_finalTarget = glm::rotate(glm::mat4(1), angle, axis) * target;
}

void RubikRenderer::RotateAnimation::update(float deltaTime)
{
  glm::mat4 & target = *m_target;
  if (m_locked) {
    float oldAngle = m_rotAngle;
    float angle = deltaTime * 5;
    m_rotAngle -= angle;
    if (oldAngle * m_rotAngle < 0) {
      m_locked = false;
      angle = oldAngle;
      target = m_finalTarget;
    } else {
      target = glm::rotate(glm::mat4(1), angle, m_rotAxis) * target;
    }
  }
}

bool RubikRenderer::RotateAnimation::isLocked() const
{
  return m_locked;
}
