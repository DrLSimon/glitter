#ifndef __RUBIK_APPLICATION_H__
#define __RUBIK_APPLICATION_H__

#include "Application.hpp"
#include "GameStage.hpp"

/// An application for a Rubik's cube game
class RubikApplication : public Application {
public:
  /// Default constructor
  RubikApplication();

  /// Sets the callbacks (key, resize)
  void setCallbacks() override;

private:
  /// Renders a single frame
  void renderFrame() override;

  /// Updates the time related members
  void update() override;

  /// Window resize callback
  static void resize(GLFWwindow * window, int framebufferWidth, int framebufferHeight);

  /// Key callback
  static void keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods);

  void nextStage();

private:
  std::unique_ptr<GameStage> m_stage; ///< menu
};

#endif // !defined(__RUBIK_APPLICATION_H__)
