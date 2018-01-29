#ifndef __PA3_APPLICATION_H__
#define __PA3_APPLICATION_H__
#include <memory>
#include "Application.hpp"
#include "glApi.hpp"

// forward declarations
struct GLFWwindow;

class PA3Application : public Application {
public:
  PA3Application(int windowWidth, int windowHeight);
  void setCallbacks() override;
  static void usage(std::string & shortDescritpion, std::string & synopsis, std::string & description);

private:
  class InstancedVAO {
  public:
    InstancedVAO() = delete;
    InstancedVAO(const InstancedVAO &) = delete;

    /**
     * @brief creates an instance from a vao and modelView matrix
     * @param vao  the VAO to be instanciated
     * @param modelView the matrix transform between the object (a.k.a model) space and the camera (a.k.a view) space
     * @param proj the projection matrix
     * @return the created InstancedVAO as a smart pointer
     */
    static std::shared_ptr<InstancedVAO> createInstance(const std::shared_ptr<VAO> & vao, const glm::mat4 & modelView, const glm::mat4 & proj);

    /**
     * @brief Draw this VAO
     */
    void draw() const;

    /**
     * @brief update the program MVP uniform variable
     * @param prog the target program
     */
    void updateProgram(Program & prog) const;

    void rotate(float angle, const glm::vec3 & axis);

  private:
    InstancedVAO(const std::shared_ptr<VAO> & vao, const glm::mat4 & modelView, const glm::mat4 & proj);

  private:
    std::shared_ptr<VAO> m_vao;
    glm::mat4 m_mvp;
  };

private:
  /**
   * @brief creates a VAO corresponding to a 3d sphere
   * @param nbPhi number of discrete values for the longitude angle
   * @param nbTheta number of discrete values for the latitude angle
   *
   * The VAO is composed of a single VBO corresponding the the
   * geometric 3d position of the vertices. The sphere is
   * centered and has a unit radius.
   *
   * @note PA3 this function is currently incomplete. You need to
   * complete the creation of the VAO (VBO and IBO).
   */
  void makeASphere(unsigned int nbPhi, unsigned int nbTheta);

  /**
   * @brief creates a VAO corresponding to a 3d torus
   * @param nbPhi number of discrete values for the big circle angle
   * @param nbTheta number of discrete values for the small circle angle
   * @param smallRadius the radius of the small circle
   *
   * The VAO is composed of a single VBO corresponding the the
   * geometric 3d position of the vertices. The torus is
   * centered and has a big circle radius of one unit.
   *
   * @note PA3 this function is currently incomplete. You need to
   * complete the creation of the VAO (VBO and IBO).
   */
  void makeATorus(unsigned int nbPhi, unsigned int nbTheta, float smallRadius);

  /**
   * @brief initialize OpenGL state variables
   *
   * @note PA3 this function is currently incomplete. You need to
   * activate the back face culling and the depth test.
   */
  void initGLState() const;

  /**
   * @brief renders a frame
   *
   * @note PA3 this function is currently incomplete. You need to
   * clear the depth buffer at the beginning of the frame rendering.
   */
  void renderFrame() override;
  void update() override;
  static void resize(GLFWwindow * window, int framebufferWidth, int framebufferHeight);

private:
  std::vector<std::shared_ptr<InstancedVAO>> m_vaos;
  Program m_program;
  glm::mat4 m_proj;
  float m_currentTime;
  float m_deltaTime;
};

#endif // !defined(__PA3_APPLICATION_H__)
