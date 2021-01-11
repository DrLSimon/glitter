#define GLM_FORCE_RADIANS
#include "PA2Application.hpp"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "utils.hpp"

PA2Application::PA2Application(int windowWidth, int windowHeight)
    : Application(windowWidth, windowHeight, "Application for PA2"), m_vao(2), m_program("shaders/simple3d.v.glsl", "shaders/simple3d.f.glsl"), m_currentTime(0)
{
  makeA3dCube();
}

void PA2Application::makeA3dCube()
{
  std::vector<glm::vec2> positions = {{-0.5, -0.5}, {0.5, -0.5}, {0.5, 0.5}, {-0.5, 0.5}};
  std::vector<glm::vec3> colors = {{1, 1, 0}, {1, 1, 0}, {1, 1, 0}, {1, 1, 0}}; // a single yellow color
  std::vector<uint> ibo = {0, 1, 2, 0, 2, 3};
  m_vao.setVBO(0, positions);
  m_vao.setVBO(1, colors);
  m_vao.setIBO(ibo);
}

void PA2Application::renderFrame()
{
  glClearColor(1, 1, 1, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  m_program.bind();
  std::cerr << __PRETTY_FUNCTION__ << ": You must complete the implementation here (look at the documentation in the header)" << std::endl;
  glm::mat4 m(1);
  glm::mat4 v(1);
  glm::mat4 p(1);
  m_vao.draw(); // rendering first instance
  m_vao.draw(); // rendering second instance
  m_program.unbind();
}

void PA2Application::update()
{
  m_currentTime = glfwGetTime();
  m_program.bind();
  m_program.setUniform("time", m_currentTime);
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
}

void PA2Application::usage(std::string & shortDescritpion, std::string & synopsis, std::string & description)
{
  shortDescritpion = "Application for programming assignment 2";
  synopsis = "pa2";
  description = "  An application on GLSL uniform variables and true 3D meshes.\n";
}
