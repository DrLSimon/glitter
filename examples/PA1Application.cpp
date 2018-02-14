#include "PA1Application.hpp"
#include <GLFW/glfw3.h>
#include <iostream>
#include "utils.hpp"

PA1Application::PA1Application(int windowWidth, int windowHeight, unsigned int part)
    : Application(windowWidth, windowHeight, "Application for PA1"), m_part(part), m_vbo(GL_ARRAY_BUFFER), m_fakevao(1), m_vao(1), m_program("shaders/simple2d.v.glsl", "shaders/simple2d.f.glsl")
{
  if (m_part == 1) {
    makeA2DSquareWithFakeVAO();
  } else {
    makeA2DSquareWithRealVAO();
  }
}

void PA1Application::makeA2DSquareWithFakeVAO()
{
  std::vector<glm::vec2> positions = {{-0.5, -0.5}, {0.5, -0.5}, {0.5, 0.5}, {-0.5, -0.5}, {0.5, 0.5}, {-0.5, 0.5}};
  m_vbo.setData(positions);
  m_fakevao.bind();
  m_vbo.bind();
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, m_vbo.attributeSize(), m_vbo.attributeType(), GL_FALSE, 0, 0);
  m_vbo.unbind();
  m_fakevao.unbind();
}

void PA1Application::makeA2DSquareWithRealVAO()
{
  std::vector<glm::vec2> positions = {{-0.5, -0.5}, {0.5, -0.5}, {0.5, 0.5}, {-0.5, 0.5}};
  std::vector<uint> ibo = {0, 1, 2, 0, 2, 3};
  m_vao.setVBO(0, positions);
  m_vao.setIBO(ibo);
  m_vbo.setData(positions);
}

void PA1Application::renderFrame()
{
  glClearColor(1, 1, 1, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  m_program.bind();
  if (m_part == 1) {
    m_fakevao.bind();
    glDrawArrays(GL_TRIANGLES, 0, m_vbo.attributeCount());
    m_fakevao.unbind();
  } else {
    m_vao.draw();
  }
  m_program.unbind();
}

void PA1Application::update() {}

void PA1Application::resize(GLFWwindow *, int framebufferWidth, int framebufferHeight)
{
  glViewport(0, 0, framebufferWidth, framebufferHeight);
}

void PA1Application::setCallbacks()
{
  GLFWwindow * window = glfwGetCurrentContext();
  glfwSetFramebufferSizeCallback(window, PA1Application::resize);
}

void PA1Application::usage(std::string & shortDescritpion, std::string & synopsis, std::string & description)
{
  shortDescritpion = "Application for programming assignment 1";
  synopsis = "pa1 <part>";
  description = "  A minimal application displaying a yellow rectangle on white background.\n"
                "  Two parts are available in this tutorial, and a command argument (<part>)\n"
                "  can be used to specify which one to launch.\n";
}
