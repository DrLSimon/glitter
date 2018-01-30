#ifndef __PA3_APPLICATION_H__
#define __PA3_APPLICATION_H__
#include <memory>
#include "Application.hpp"
#include "glApi.hpp"

// forward declarations
struct GLFWwindow;

/// A concrete implementation of Application for PA1
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
     * @param modelWorld the matrix transform between the object (a.k.a model) space and the camera (a.k.a view) space
     * @return the created InstancedVAO as a smart pointer
     */
    static std::shared_ptr<InstancedVAO> createInstance(const std::shared_ptr<VAO> & vao, const glm::mat4 & modelWorld);

    /**
     * @brief Draw this VAO
     */
    void draw(GLenum mode = GL_TRIANGLES) const;

    /**
     * @brief update the program MVP uniform variable
     * @param prog the target program
     * @param proj the projection matrix
     * @param view the worldView matrix
     */
    void updateProgram(Program & prog, const glm::mat4 & proj, const glm::mat4 & view = glm::mat4(1)) const;

  private:
    InstancedVAO(const std::shared_ptr<VAO> & vao, const glm::mat4 & modelView);

  private:
    std::shared_ptr<VAO> m_vao; ///< VAO
    glm::mat4 m_mw;             ///< modelWorld matrix
  };

private:
  /// a simple struct for discrete linear range of the form {minVal, minVal+delta,...,minVal+length}
  struct DiscreteLinRange {
    uint nbVals;  ///< the number of values in the range
    float minVal; ///< the minimum value of the range
    float length; ///< the lengh (max -min) of the range

    /// Constructor
    DiscreteLinRange(uint nbVals, float minVal, float length) : nbVals(nbVals), minVal(minVal), length(length) {}

    /// Returns the k^{th} value of the range
    float value(uint k) { return minVal + k * length / (nbVals - 1); }
  };

  /**
   * @brief Creates a VAO
   * @param rgPhi the discrete range of values for phi
   * @param rgTheta the discrete range of values for theta
   * @param posFunc the parametric function for the 3d position
   * @param isCyclicInPhi cyclic behavior in phi
   * @param isCyclicInTheta cyclic behavior in theta
   * @return a smart pointer to the created VAO
   *
   * The VAO is composed of two VBOs corresponding to the
   * geometric 3d position and the color of the vertices.
   *
   * @note PA3 this function is currently incomplete. You need to
   * complete the creation of the VAO (VBOs and IBO).
   */
  static std::shared_ptr<VAO> makeParamSurf(DiscreteLinRange rgPhi, DiscreteLinRange rgTheta, const std::function<glm::vec3(float, float)> & posFunc, bool isCyclicInPhi, bool isCyclicInTheta);

  /**
   * @brief creates a VAO corresponding to a 3d sphere
   * @param nbPhi number of discrete values for the longitude angle
   * @param nbTheta number of discrete values for the latitude angle
   *
   * The VAO is composed of two VBOs corresponding to the
   * geometric 3d position and the color of the vertices. The sphere is
   * centered and has a unit radius.
   */
  void makeASphere(unsigned int nbPhi, unsigned int nbTheta);

  /**
   * @brief creates a VAO corresponding to a 3d torus
   * @param nbPhi number of discrete values for the big circle angle
   * @param nbTheta number of discrete values for the small circle angle
   * @param smallRadius the radius of the small circle
   *
   * The VAO is composed of two VBOs corresponding to the
   * geometric 3d position and the color of the vertices. The torus is
   * centered and has a big circle radius of one unit.
   *
   * @note PA3 this function is currently incomplete. You need to
   * complete the creation of the VAO (VBO and IBO). To do so, you can
   * adapt the definition of makeASphere(). You mainly need to define
   * correctly the 3d position function and the cyclic behaviour.
   *
   * @see makeASphere()
   */
  void makeATorus(unsigned int nbPhi, unsigned int nbTheta, float smallRadius);

  /**
   * @brief creates a VAO corresponding to a 3d shell (snail)
   * @param nbPhi number of discrete values for the first parameter
   * @param nbTheta number of discrete values for the second parameter
   *
   * The VAO is composed of a two VBOs corresponding the the
   * geometric 3d position and the color of the vertices.
   */
  void makeAShell(unsigned int nbPhi, unsigned int nbTheta);

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
  static void keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods);
  void continuousKey();

  /**
   * @brief rotates the view around an axis
   * @param axis the rotation axis expressed in the view coordinates
   * @param angleFactor sign and sensitivity of the rotation
   */
  void rotateView(const glm::vec3 & axis, float angleFactor);

private:
  std::vector<std::shared_ptr<InstancedVAO>> m_vaos; ///< List of instanced VAOs (VAO + modelView matrix)
  Program m_program;                                 ///< A GLSL progam
  glm::mat4 m_proj;                                  ///< Projection matrix
  glm::mat4 m_view;                                  ///< worldView matrix
  float m_currentTime;                               ///< elapsed time since first frame
  float m_deltaTime;                                 ///< elapsed time since last frame
  GLenum m_renderMode;                               ///< GL_LINES or GL_TRIANGLES
};

#endif // !defined(__PA3_APPLICATION_H__)
