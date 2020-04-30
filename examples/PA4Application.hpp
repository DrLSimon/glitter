#ifndef __PA4_APPLICATION_H__
#define __PA4_APPLICATION_H__
#include <memory>
struct GLFWwindow;
#include "Application.hpp"
#include "glApi.hpp"

class PA4Application : public Application {
public:
  PA4Application(int windowWidth, int windowHeight);
  void setCallbacks() override;
  static void usage(std::string & shortDescritpion, std::string & synopsis, std::string & description);

public:
  static unsigned int part; ///< Part 1 or 2 of the tutorial

private:
  void renderFrame() override;
  void update() override;
  static void resize(GLFWwindow * window, int framebufferWidth, int framebufferHeight);
  static void keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods);
  void continuousKey();
  void computeView(bool reset = false);

private:
  class RenderObjectPart {
  public:
    RenderObjectPart() = delete;
    RenderObjectPart(const RenderObjectPart &) = delete;
    RenderObjectPart(RenderObjectPart &&) = default;

    RenderObjectPart(std::shared_ptr<VAO> vao, std::shared_ptr<Program> program, const glm::vec3 & diffuse, std::shared_ptr<Texture> texture);
    void draw(Sampler * colormap);
    void update(const glm::mat4 & mw);

  private:
    std::shared_ptr<VAO> m_vao;
    std::shared_ptr<Program> m_program;
    glm::vec3 m_diffuse;
    std::shared_ptr<Texture> m_texture;
  };

  /**
   * @brief The RenderObject class
   *
   * A RenderObject is split into parts, sharing the same geometry (VBOs), but referencing different primitive subsets (IBO) and materials (textures, ...).
   */
  class RenderObject {
  public:
    RenderObject() = delete;
    RenderObject(const RenderObject &) = delete;

    static std::unique_ptr<RenderObject> createCheckerBoardCubeInstance(const std::shared_ptr<Program> & program, const glm::mat4 & modelWorld);
    /**
     * @brief creates an instance from a wavefront file and modelWorld matrix
     * @param program the GLSL program
     * @param objname the filename of the wavefront file
     * @param modelWorld the matrix transform between the object (a.k.a model) space and the world space
     * @return the created RenderObject as a smart pointer
     */
    static std::unique_ptr<RenderObject> createWavefrontInstance(const std::shared_ptr<Program> & program, const std::string & objname, const glm::mat4 & modelWorld);

    /**
     * @brief Draw this RenderObject
     */
    void draw();

    /**
     * @brief update the program M uniform variable
     */
    void update();

  private:
    RenderObject(const std::shared_ptr<Program> & program, const glm::mat4 & modelWorld);
    void loadWavefront(const std::string & objname);
    static std::vector<GLubyte> makeCheckerBoard();

  private:
    std::shared_ptr<Program> m_program;
    glm::mat4 m_mw; ///< modelWorld matrix
    std::vector<RenderObjectPart> m_parts;
    std::unique_ptr<Sampler> m_colormap;
  };

private:
  std::shared_ptr<Program> m_program;                   ///< GLSL program
  std::vector<std::unique_ptr<RenderObject>> m_objects; ///< render objects
  glm::mat4 m_proj;                                     ///< Projection matrix
  glm::mat4 m_view;                                     ///< worldView matrix
  float m_eyePhi;                                       ///< Camera position longitude angle
  float m_eyeTheta;                                     ///< Camera position latitude angle
  float m_currentTime;                                  ///< elapsed time since first frame
  float m_deltaTime;                                    ///< elapsed time since last frame
};

#endif // !defined(__PA4_APPLICATION_H__)
