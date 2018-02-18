#ifndef __PA5_APPLICATION_H__
#define __PA5_APPLICATION_H__
#include <memory>
struct GLFWwindow;
#include "Application.hpp"
#include "glApi.hpp"

// forward declarations
struct SimpleMaterial;

class PA5Application : public Application {
public:
  PA5Application(int windowWidth, int windowHeight);
  void setCallbacks() override;
  static void usage(std::string & shortDescritpion, std::string & synopsis, std::string & description);

public:
  static bool displayNormals; ///< Toggles normal display

private:
  void renderFrame() override;
  void update() override;
  static void resize(GLFWwindow * window, int framebufferWidth, int framebufferHeight);
  static void keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods);
  void continuousKey();
  void computeView(bool reset = false);

private:
  class RenderObject {
  public:
    RenderObject() = delete;
    RenderObject(const RenderObject &) = delete;

    static std::unique_ptr<RenderObject> createCheckerBoardPlaneInstance(const glm::mat4 & modelWorld);
    /**
     * @brief creates an instance from a wavefront file and modelWorld matrix
     * @param objname the filename of the wavefront file
     * @param modelWorld the matrix transform between the object (a.k.a model) space and the world space
     * @return the created RenderObject as a smart pointer
     */
    static std::unique_ptr<RenderObject> createWavefrontInstance(const std::string & objname, const glm::mat4 & modelWorld);

    /**
     * @brief Sets all uniform variables related to material and lighting
     * @param program
     * @param material
     *
     * @note Besides the material, three directional lights (defined in world space) are passed to the GLSL program.
     */
    void setProgramMaterial(std::shared_ptr<Program> & program, const SimpleMaterial & material) const;

    /**
     * @brief Draw this VAO
     */
    void draw(GLenum mode = GL_TRIANGLES);

    /**
     * @brief update the program MVP uniform variable
     * @param proj the projection matrix
     * @param view the worldView matrix
     */
    void update(const glm::mat4 & proj, const glm::mat4 & view);

  private:
    RenderObject(const glm::mat4 & modelWorld);
    RenderObject(const std::string & objname, const glm::mat4 & modelWorld);
    void loadWavefront(const std::string & objname);

  private:
    std::vector<std::shared_ptr<VAO>> m_vaos;
    glm::mat4 m_mw; ///< modelWorld matrix
    std::vector<std::shared_ptr<Program>> m_programs;
    std::vector<std::shared_ptr<Texture>> m_diffuseTextures;
    std::vector<std::shared_ptr<Texture>> m_normalTextures;
    std::vector<std::shared_ptr<Texture>> m_specularTextures;
    std::unique_ptr<Sampler> m_colormap;
    std::unique_ptr<Sampler> m_specularmap;
    std::unique_ptr<Sampler> m_normalmap;
  };

private:
  std::vector<std::unique_ptr<RenderObject>> m_objects; ///< render objects
  glm::mat4 m_proj;                                     ///< Projection matrix
  glm::mat4 m_view;                                     ///< worldView matrix
  float m_eyePhi;                                       ///< Camera position longitude angle
  float m_eyeTheta;                                     ///< Camera position latitude angle
  float m_currentTime;                                  ///< elapsed time since first frame
  float m_deltaTime;                                    ///< elapsed time since last frame
};

#endif // !defined(__PA5_APPLICATION_H__)
