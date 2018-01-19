#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include "PA3Application.hpp"
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "utils.hpp"

PA3Application::PA3Application(int windowWidth, int windowHeight)
    : Application(windowWidth, windowHeight, "Application for PA3"), m_program("shaders/simple3d.v.glsl", "shaders/simple3d.f.glsl"), m_proj(1)
{
  makeASphere(100, 100);
  makeATorus(100, 100, 0.25);
}

void PA3Application::makeASphere(unsigned int nbPhi, unsigned int nbTheta)
{
  std::shared_ptr<VAO> vao(new VAO(1));
  std::cerr << __PRETTY_FUNCTION__ << ": You must complete the implementation here (look at the documentation in the header)" << std::endl;

  glm::mat4 mv(1);
  m_vaos.push_back(InstancedVAO::createInstance(vao, mv, m_proj));
}

void PA3Application::makeATorus(unsigned int nbPhi, unsigned int nbTheta, float smallRadius)
{
  std::shared_ptr<VAO> vao(new VAO(1));
  std::cerr << __PRETTY_FUNCTION__ << ": You must complete the implementation here (look at the documentation in the header)" << std::endl;

  glm::mat4 mv(1);
  mv = glm::translate(mv, {0.5, 0, 0});
  mv = glm::rotate(mv, glm::pi<float>() / 4, {0, 0, 1});
  m_vaos.push_back(InstancedVAO::createInstance(vao, mv, m_proj));
}

void PA3Application::renderFrame()
{
  glClearColor(1, 1, 1, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  m_program.bind();
  for (const auto & vao : m_vaos) {
    vao->updateProgram(m_program);
    vao->draw();
  }
  m_program.unbind();
}

void PA3Application::update()
{
  float current_time = glfwGetTime();
  m_program.bind();
  m_program.setUniform("time", current_time);
  m_program.unbind();
}

void PA3Application::resize(GLFWwindow *, int framebufferWidth, int framebufferHeight)
{
  glViewport(0, 0, framebufferWidth, framebufferHeight);
}

void PA3Application::setCallbacks()
{
  GLFWwindow * window = glfwGetCurrentContext();
  glfwSetFramebufferSizeCallback(window, PA3Application::resize);
  glfwSetWindowUserPointer(window, this);
}

void PA3Application::usage(std::string & shortDescritpion, std::string & synopsis, std::string & description)
{
  shortDescritpion = "Application for programming assignment 3";
  synopsis = "pa3";
  description = "  An application for rendering parametric surfaces (sphere, torus, ...).\n";
}

PA3Application::InstancedVAO::InstancedVAO(const std::shared_ptr<VAO> & vao, const glm::mat4 & modelView, const glm::mat4 & proj) : m_vao(vao), m_mvp(proj * modelView) {}

std::shared_ptr<PA3Application::InstancedVAO> PA3Application::InstancedVAO::createInstance(const std::shared_ptr<VAO> & vao, const glm::mat4 & modelView, const glm::mat4 & proj)
{
  return std::shared_ptr<InstancedVAO>(new InstancedVAO(vao, modelView, proj));
}

void PA3Application::InstancedVAO::draw() const
{
  m_vao->draw();
}

void PA3Application::InstancedVAO::updateProgram(Program & prog) const
{
  prog.setUniform("MVP", m_mvp);
}
