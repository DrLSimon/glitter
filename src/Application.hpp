#ifndef __APPLICATION_H__
#define __APPLICATION_H__
#include <memory>
#include <string>
#include <glApi.hpp>

struct GLFWwindow;

/**
 * @brief An abstract class for the main application (Based on GLFW)
 */
class Application {
public:
  /**
   * @brief Constructs a window of given geometry and initializes the GL context
   * @param windowWidth horizontal size
   * @param windowHeight vertical size
   */
  Application(int windowWidth = 800, int windowHeight = 600, const char * title = "An application");

  virtual ~Application();

  /**
   * @brief sets the callbacks (key, mouse, resize)
   */
  virtual void setCallbacks() = 0;

  /**
   * @brief Main application loop
   *
   * Continues until 'Q' or 'Esc' are pressed.
   */
  void mainLoop();

private:
  /**
   * @brief updates the state of the application based on events
   */
  virtual void update() = 0;

  /**
   * @brief renders a frame
   */
  virtual void renderFrame() = 0;

  /**
   * @brief creates a window, and init the OpenGL context
   * @param windowWidth desired width for the window
   * @param windowHeight desired height for the window
   */
  void initOGLContext(int windowWidth, int windowHeight, const char * title);

  /// @brief handles glew initialisation errors
  static bool checkGLEWInitResult(GLenum glewResult){
      if (glewResult == GLEW_OK) {
          return true;
      }

      const char* sessionType = std::getenv("XDG_SESSION_TYPE");
      bool isWayland = (sessionType && std::string(sessionType) == "wayland");

      if (glewResult == GLEW_ERROR_NO_GLX_DISPLAY && isWayland) {
          std::cerr << "NO_GLX_DISPLAY ignored safely because we are on Wayland." << std::endl;
          return true;
      }

      std::cerr << "ERROR: " << glewGetErrorString(glewResult) << std::endl;
      return false;
  }

  /**
   * @brief Clean up the state and quit
   * @param return_code
   */
  void shutDown(int return_code);
};



#endif // !defined(__APPLICATION_H__)
