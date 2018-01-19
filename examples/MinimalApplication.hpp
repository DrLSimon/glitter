#ifndef __MINIMAL_APPLICATION_H__
#define __MINIMAL_APPLICATION_H__
#include <memory>
struct GLFWwindow;
#include "Application.hpp"
#include "glApi.hpp"

class MinimalApplication : public Application {
public:
  MinimalApplication(int windowWidth, int windowHeight, unsigned int exercise);
  void setCallbacks() override;
  static void usage(std::string & shortDescritpion, std::string & synopsis, std::string & description);

private:
  void renderFrame() override;
  void update() override;
  void makeACube();
  void makeATexturedCube();
  void renderTexturedCube() const;
  void renderCube() const;
  static void resize(GLFWwindow * window, int framebufferWidth, int framebufferHeight);
  static void keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods);

private:
  std::unique_ptr<VAO> m_vaoCube;
  std::unique_ptr<VAO> m_vaoTexturedCube;
  std::unique_ptr<Program> m_programCube;
  std::unique_ptr<Program> m_programTexturedCube;
  std::unique_ptr<Texture> m_texture;
  std::unique_ptr<Sampler> m_sampler;
  unsigned int m_exercise;
  static const unsigned int nbExercises = 2;
};

#endif // !defined(__MINIMAL_APPLICATION_H__)
