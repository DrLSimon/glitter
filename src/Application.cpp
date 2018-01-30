#include "Application.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "utils.hpp"

Application::Application(int windowWidth, int windowHeight, const char * title)
{
  initOGLContext(windowWidth, windowHeight, title);
}

Application::~Application()
{
  shutDown(0);
}

void Application::mainLoop()
{
  GLFWwindow * window = glfwGetCurrentContext();
  while (!glfwWindowShouldClose(window)) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS or glfwGetKey(window, 'Q') == GLFW_PRESS) {
      break;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
      update();
    }
    renderFrame();
    // swap back and front buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}

void Application::initOGLContext(int windowWidth, int windowHeight, const char * title)
{
  if (!glfwInit()) {
    shutDown(1);
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
#ifdef __APPLE__
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#else
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#endif

  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  GLFWwindow * window = glfwCreateWindow(windowWidth, windowHeight, title, NULL, NULL);
  if (!window) {
    std::cerr << "Could not open a window" << std::endl;
    shutDown(1);
  }
  glfwMakeContextCurrent(window);
  glfwSetWindowUserPointer(window, this);

  /* GLEW Initialization */
  // Do not forget to use glewExperimental so that glewInit works with core
  // profile: http://www.opengl.org/wiki/OpenGL_Loading_Library#GLEW
  glewExperimental = GL_TRUE;
  GLenum GlewInitResult = glewInit();
  std::cerr << "Here, we should expect to get a GL_INVALID_ENUM (that's a known bug), and indeed:" << std::endl;
  checkGLerror();
  if (GlewInitResult != GLEW_OK) {
    std::cerr << "ERROR: " << glewGetErrorString(GlewInitResult) << std::endl;
    shutDown(1);
  }

  std::cout << "Seems we made it " << std::endl;
  std::cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
  std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

void Application::shutDown(int return_code)
{
  glfwTerminate();
  exit(return_code);
}
