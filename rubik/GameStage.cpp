#include "GameStage.hpp"
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

StartMenuStage::StartMenuStage()
{
  m_renderer.deform(true);
  int width, height;
  GLFWwindow * window = glfwGetCurrentContext();
  glfwGetFramebufferSize(window, &width, &height);
  m_text = std::unique_ptr<TextPrinter>(new TextPrinter(width, height));
  const glm::vec3 blue(0, 0, 1);
  m_text->printText("PRESS ENTER", 4, 12, 5, blue);
  m_text->printText(" TO START. ", 4, 13, 5, blue);
}

StartMenuStage::~StartMenuStage() {}

void StartMenuStage::renderFrame()
{
  m_renderer.renderFrame();
  m_text->draw();
}

void StartMenuStage::update()
{
  m_renderer.update();
}

void StartMenuStage::resize(GLFWwindow * window, int framebufferWidth, int framebufferHeight)
{
  m_renderer.resize(window, framebufferWidth, framebufferHeight);
  m_text->setWOverH(framebufferWidth / float(framebufferHeight));
  glViewport(0, 0, framebufferWidth, framebufferHeight);
}

void PlayingStage::turnClockwise(unsigned int faceID)
{
  if (m_renderer.isLocked()) {
    return;
  }
  RubikFace face = m_renderer.getFace(faceID);
  std::array<uint, 9> pieces = m_state.piecesOnFace(face);
  m_renderer.launchFaceRotation(face, pieces);
  m_state.applyFaceRotation(face);
}

std::unique_ptr<GameStage> StartMenuStage::nextStage() const
{
  return std::unique_ptr<GameStage>(new PlayingStage());
}

void PlayingStage::renderFrame()
{
  m_renderer.renderFrame();
  if (m_displayHelp) {
    m_helper.draw();
  }
}

void PlayingStage::update()
{
  m_renderer.update();
}

void PlayingStage::keyCallback(GLFWwindow * /*window*/, int key, int /*scancode*/, int action, int /*mods*/)
{
  if (action == GLFW_PRESS) {
    switch (key) {
    case '0':
      m_renderer.resetView();
      break;
    case 'H': {
      m_displayHelp = not m_displayHelp;
      break;
    }
    case GLFW_KEY_UP: {
      if (not m_renderer.isLocked()) {
        glm::vec3 right{1, 0, 0};
        m_renderer.launchViewRotation(right, 1);
      }
      break;
    }
    case GLFW_KEY_DOWN: {
      if (not m_renderer.isLocked()) {
        glm::vec3 left{-1, 0, 0};
        m_renderer.launchViewRotation(left, 1);
      }
      break;
    }
    case GLFW_KEY_RIGHT: {
      if (not m_renderer.isLocked()) {
        glm::vec3 up{0, 1, 0};
        m_renderer.launchViewRotation(up, 1);
      }
      break;
    }
    case GLFW_KEY_LEFT: {
      if (not m_renderer.isLocked()) {
        glm::vec3 down{0, -1, 0};
        m_renderer.launchViewRotation(down, 1);
      }
      break;
    }
    case '1':
      turnClockwise(0);
      break;
    case '2':
      turnClockwise(1);
      break;
    case '3':
      turnClockwise(2);
      break;
    }
  }
}

void PlayingStage::resize(GLFWwindow * window, int framebufferWidth, int framebufferHeight)
{
  m_renderer.resize(window, framebufferWidth, framebufferHeight);
  glViewport(0, 0, framebufferWidth, framebufferHeight);
}

std::unique_ptr<GameStage> PlayingStage::nextStage() const
{
  return std::unique_ptr<GameStage>(new GameOverStage());
}

GameOverStage::GameOverStage()
{
  glClearColor(1, 0.7, 0.4, 1);
  int width, height;
  GLFWwindow * window = glfwGetCurrentContext();
  glfwGetFramebufferSize(window, &width, &height);
  m_text = std::unique_ptr<TextPrinter>(new TextPrinter(width, height));
  m_text->printText("GAME OVER!", 0, 3, 10, glm::vec3(1, 0, 0));
}

void GameOverStage::renderFrame()
{
  glClear(GL_COLOR_BUFFER_BIT);
  m_text->draw();
}

void GameOverStage::resize(GLFWwindow * /*window*/, int framebufferWidth, int framebufferHeight)
{
  m_text->setWOverH(framebufferWidth / float(framebufferHeight));
  glViewport(0, 0, framebufferWidth, framebufferHeight);
}

std::unique_ptr<GameStage> GameOverStage::nextStage() const
{
  return std::unique_ptr<GameStage>(new StartMenuStage());
}
