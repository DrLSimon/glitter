#ifndef __PA1_APPLICATION_H__
#define __PA1_APPLICATION_H__
#include <memory>
#include "Application.hpp"
#include "glApi.hpp"

// forward declarations
struct GLFWwindow;

/// A concrete implementation of Application for PA1
class PA1Application : public Application {
public:
  /**
   * @brief Constructor
   * @param windowWidth initial width of the application
   * @param windowHeight initial height of the application
   * @param part should be 1 or 2 to select the corresponding tutorial part
   */
  PA1Application(int windowWidth, int windowHeight, unsigned int part);
  void setCallbacks() override;
  static void usage(std::string & shortDescritpion, std::string & synopsis, std::string & description);

private:
  void makeA2DSquareWithRealVAO();
  void makeA2DSquareWithFakeVAO();
  void renderFrame() override;
  void update() override;
  static void resize(GLFWwindow * window, int framebufferWidth, int framebufferHeight);

private:
  unsigned int m_part; ///< Part 1 or 2 of the tutorial
  Buffer m_vbo;        ///< A VBO used in part 1
  VAO m_fakevao;       ///< An incomplete VAO used in part 1
  VAO m_vao;           ///<  A complete VAO used in part 2
  Program m_program;   ///< A program used in both parts.
};

#endif // !defined(__PA1_APPLICATION_H__)
