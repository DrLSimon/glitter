#define GLM_FORCE_RADIANS
#include "PA2Application.hpp"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "utils.hpp"

PA2Application::PA2Application(int windowWidth, int windowHeight)
    : Application(windowWidth, windowHeight, "Application for PA2"), m_vao(1), m_program("shaders/simple3d.v.glsl", "shaders/simple3d.f.glsl")
{
  makeA3dCube();
}

void PA2Application::makeA3dCube()
{
  std::vector<glm::vec2> positions = {{-0.5, -0.5}, {0.5, -0.5}, {0.5, 0.5}, {-0.5, 0.5}};
  std::vector<uint> ibo = {0, 1, 2, 0, 2, 3};
  m_vao.setVBO(0, positions);
  m_vao.setIBO(ibo);
}

void PA2Application::renderFrame()
{
  glClearColor(1, 1, 1, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  m_program.bind();
  glm::mat4 mvp(1);
  mvp = glm::rotate(mvp, glm::pi<float>() / 4, {0, 0, 1});
  std::cerr << __PRETTY_FUNCTION__ << ": You must complete the implementation here (look at the documentation in the header)" << std::endl;

  m_vao.draw();
  m_program.unbind();
}

void PA2Application::update()
{
  float current_time = glfwGetTime();
  m_program.bind();
  m_program.setUniform("time", current_time);
  m_program.unbind();
}

void PA2Application::resize(GLFWwindow *, int framebufferWidth, int framebufferHeight)
{
  glViewport(0, 0, framebufferWidth, framebufferHeight);
}

void PA2Application::setCallbacks()
{
  GLFWwindow * window = glfwGetCurrentContext();
  glfwSetFramebufferSizeCallback(window, PA2Application::resize);
  glfwSetWindowUserPointer(window, this);
}

void PA2Application::usage(std::string & shortDescritpion, std::string & synopsis, std::string & description)
{
  shortDescritpion = "Application for programming assignment 2";
  synopsis = "pa2";
  description = "  An application on GLSL uniform variables and true 3D meshes.\n";
}
