#ifndef __Project_APPLICATION_H__
#define __Project_APPLICATION_H__
#include <memory>
struct GLFWwindow;
#include "Application.hpp"
#include "glApi.hpp"

class ProjectApplication : public Application {
public:
  ProjectApplication(int windowWidth, int windowHeight);
  void setCallbacks() override;
  static void usage(std::string & shortDescritpion, std::string & synopsis, std::string & description);

private:
  void renderFrame() override;
  void update() override;
  static void resize(GLFWwindow * window, int framebufferWidth, int framebufferHeight);
  static void keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods);
  void continuousKey();
  void computeView(bool reset = false);

private:
  void loadWavefront(const std::string & objname);

private:
  std::shared_ptr<Program> m_program;                   ///< GLSL program
  std::shared_ptr<Sampler> m_colormap;                  ///< Sampler
  std::vector<std::shared_ptr<VAO>> m_vaos;             ///< list of VAOs
  std::vector<std::shared_ptr<Texture>> m_textures;     ///< list of Textures
  glm::mat4 m_proj;                                     ///< Projection matrix
  glm::mat4 m_view;                                     ///< worldView matrix
  float m_eyePhi;                                       ///< Camera position longitude angle
  float m_eyeTheta;                                     ///< Camera position latitude angle
  float m_currentTime;                                  ///< elapsed time since first frame
  float m_deltaTime;                                    ///< elapsed time since last frame
};

#endif // !defined(__Project_APPLICATION_H__)
