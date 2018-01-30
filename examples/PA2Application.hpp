#ifndef __PA2_APPLICATION_H__
#define __PA2_APPLICATION_H__
#include <memory>
#include "Application.hpp"
#include "glApi.hpp"

// forward declarations
struct GLFWwindow;

/// A concrete implementation of Application for PA1
class PA2Application : public Application {
public:
  PA2Application(int windowWidth, int windowHeight);
  void setCallbacks() override;
  static void usage(std::string & shortDescritpion, std::string & synopsis, std::string & description);

private:
  /**
   * @brief creates a VAO corresponding to a 3d cube.
   *
   * The VAO is composed of a two VBOs corresponding the
   * geometric 3d position and the color of the vertices.
   *
   * @note PA2 (part 2) this function currently implements a 2d square VAO. You must
   * change it to create a 3d cube. Besides you will strive to assign
   * a given color per face (i.e. the six following colors: red, green, blue, yellow, magenta, and cyan).
   * To do so you will need to modify the two VBOs and the IBO.
   */
  void makeA3dCube();

  /**
   * @brief renders a frame
   *
   * @note PA2 (part 3) this function is currently incomplete. Currently the VAO is rendered twice
   * but nothing changes between the two draw calls. In order to instanciate the VAO in two
   * different location, you should
   * 	- create a MVP matrix
   * 	- set it in a first configuration
   * 	- send its values to the corresponding uniform of the GLSL program (@p m_program)
   * 	- do the first draw call
   * 	- set the MVP matrix in a second configuration
   * 	- send its values to the corresponding uniform
   * 	- do the second draw call
   */
  void renderFrame() override;

  void update() override;

  static void resize(GLFWwindow * window, int framebufferWidth, int framebufferHeight);

private:
  VAO m_vao;
  Program m_program;
  float m_currentTime;
};

#endif // !defined(__PA2_APPLICATION_H__)
