#ifndef __RUBIK_RENDERER_H__
#define __RUBIK_RENDERER_H__

#include "glApi.hpp"

// forward declarations
struct GLFWwindow;
struct RubikFace;

/// A class to handle the rendering of the Rubik's cube
class RubikRenderer {
public:
  /// Default constructor
  RubikRenderer();

  /// OpenGL state initialization
  void initGLState() const;

  /// Creates a unique vao for all the pieces and instanciates them
  void createTheVAO();

  /// Handles window resizing
  void resize(GLFWwindow * window, int framebufferWidth, int framebufferHeight);

  /// Starts the rotation animation of a face
  void launchViewRotation(const glm::vec3 & axis, float angleFactor);

  /// Resets the view to its default configuration
  void resetView();

  /// Starts the rotation animation of a face
  void launchFaceRotation(const RubikFace & face, const std::array<uint, 9> & pieces);

  /// Updates all time dependent members
  void update();

  /// Draws the cube in its current configuration
  void renderFrame();

  /// Denotes if some pieces are still moving
  bool isLocked() const;

  /// Activates / deactivates the deformation of the cube
  void deform(bool activate);

  /// Gets the RubikFace according to the current view
  RubikFace getFace(unsigned int face) const;

private:
  /// A class for handling animated rotations
  class RotateAnimation {
  public:
    /// Constructor
    RotateAnimation();

    /// Starts the animation
    void startAnimation(glm::mat4 & target, const glm::vec3 & axis, float angle);

    /// Updates the rotation based on elapsed time
    void update(float deltaTime);

    /// Denotes if the animation is still going on
    bool isLocked() const;

  private:
    glm::vec3 m_rotAxis;     ///< rotation axis for animation
    float m_rotAngle;        ///< rotation angle for animation
    glm::mat4 * m_target;    ///< the animated matrix
    glm::mat4 m_finalTarget; ///< next key frame for the animated matrix
    bool m_locked;           ///< toggle view rotation
  };

  /// A vao associated with a modelWorld matrix
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

    /// Launches a rotation animation.
    void launchRotation(const glm::vec3 & axis, float angle);

    /// Updates the rotation based on elapsed time
    void update(float deltaTime);

    /// Denotes if the vao is still being rotated
    bool isLocked() const;

    static std::shared_ptr<VAO> makeARoundedCube(unsigned int nbPhi, unsigned int nbTheta);

  private:
    InstancedVAO(const std::shared_ptr<VAO> & vao, const glm::mat4 & modelView);

  private:
    std::shared_ptr<VAO> m_vao; ///< VAO
    glm::mat4 m_mw;             ///< modelWorld matrix
    RotateAnimation m_anim;     ///< the  rotation animation
  };

private:
  std::shared_ptr<InstancedVAO> m_vaos[27]; ///< List of instanced VAOs (VAO + modelView matrix)
  std::shared_ptr<VAO> m_vao;               ///< a unique VAO (shared by all instanced one)
  Program m_program;                        ///< A GLSL progam
  glm::mat4 m_proj;                         ///< Projection matrix
  glm::mat4 m_view;                         ///< worldView matrix
  float m_currentTime;                      ///< elapsed time since first frame
  float m_deltaTime;                        ///< elapsed
  RotateAnimation m_viewAnim;               ///< the view rotation animation
};
#endif // !defined(__RUBIK_RENDERER_H__)
