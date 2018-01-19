#ifndef __PA2_APPLICATION_H__
#define __PA2_APPLICATION_H__
#include <memory>
#include "Application.hpp"
#include "glApi.hpp"

// forward declarations
struct GLFWwindow;

class PA2Application : public Application {
public:
  PA2Application(int windowWidth, int windowHeight);
  void setCallbacks() override;
  static void usage(std::string & shortDescritpion, std::string & synopsis, std::string & description);

private:
  /**
   * @brief creates a VAO corresponding to a 3d cube.
   *
   * The VAO is composed of a single VBO corresponding the the
   * geometric 3d position of the vertices.
   *
   * @note PA2 this function currently implements a 2d square VAO. You must
   * change it to create a 3d cube.
   */
  void makeA3dCube();

  /**
   * @brief renders a frame
   *
   * @note PA2 this function is currently incomplete. A local variable called @p mvp
   * is created but the code to send the corresponding values to the GLSL program is
   * missing.
   */
  void renderFrame() override;

  void update() override;

  static void resize(GLFWwindow * window, int framebufferWidth, int framebufferHeight);

private:
  VAO m_vao;
  Program m_program;
};

#endif // !defined(__PA2_APPLICATION_H__)
