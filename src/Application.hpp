#ifndef __APPLICATION_H__
#define __APPLICATION_H__
#include <memory>
#include <string>
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

  /**
   * @brief Clean up the state and quit
   * @param return_code
   */
  void shutDown(int return_code);
};

#endif // !defined(__APPLICATION_H__)
