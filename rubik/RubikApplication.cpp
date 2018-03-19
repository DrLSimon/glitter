#include "RubikApplication.hpp"
#include <GLFW/glfw3.h>

RubikApplication::RubikApplication() : Application(800, 600, "Rubik's cube"), m_stage(new StartMenuStage()) {}

void RubikApplication::renderFrame()
{
  glClear(GL_COLOR_BUFFER_BIT);
  glClear(GL_DEPTH_BUFFER_BIT);
  if (m_stage) {
    m_stage->renderFrame();
  }
}

void RubikApplication::update()
{
  m_stage->update();
}

void RubikApplication::nextStage()
{
  m_stage = m_stage->nextStage();
}

void RubikApplication::resize(GLFWwindow * window, int framebufferWidth, int framebufferHeight)
{
  RubikApplication & app = *static_cast<RubikApplication *>(glfwGetWindowUserPointer(window));
  app.m_stage->resize(window, framebufferWidth, framebufferHeight);
}

void RubikApplication::keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
  RubikApplication & app = *static_cast<RubikApplication *>(glfwGetWindowUserPointer(window));
  if (action == GLFW_PRESS) {
    switch (key) {
    case GLFW_KEY_ENTER:
      app.nextStage();
      return;
    }
  }
  app.m_stage->keyCallback(window, key, scancode, action, mods);
}

void RubikApplication::setCallbacks()
{
  GLFWwindow * window = glfwGetCurrentContext();
  glfwSetFramebufferSizeCallback(window, RubikApplication::resize);
  glfwSetKeyCallback(window, RubikApplication::keyCallback);
}
