#include "MinimalApplication.hpp"
#include <GLFW/glfw3.h>
#include <iostream>
#include "utils.hpp"

MinimalApplication::MinimalApplication(int windowWidth, int windowHeight, unsigned int exercise) : Application(windowWidth, windowHeight), m_exercise(exercise)
{
  makeACube();
  makeATexturedCube();
  glEnable(GL_DEPTH_TEST);
}

void MinimalApplication::setCallbacks()
{
  GLFWwindow * window = glfwGetCurrentContext();
  glfwSetFramebufferSizeCallback(window, MinimalApplication::resize);
  glfwSetKeyCallback(window, MinimalApplication::keyCallback);
  // glfwSetMouseButtonCallback(window, MinimalApplication::mouseButtonCallback);
  glfwSetWindowUserPointer(window, this);
}

void MinimalApplication::usage(std::string & shortDescritpion, std::string & synopsis, std::string & description)
{
  shortDescritpion = "a minimal application displaying a cube";
  synopsis = "minimal [<exercise>]";
  description = "  A minimal application displaying a cube. Depending on the value of\n"
                "  <exercise> the cube is either colored (<exercise>=0) or textured.\n"
                "  The value of <exercise> can also be updated at run time with the\n"
                "  following key bindings:\n"
                "     e/E increment/decrement (modular version)";
}

void MinimalApplication::makeACube()
{
  m_vaoCube = std::unique_ptr<VAO>(new VAO(2));
  m_programCube = std::unique_ptr<Program>(new Program("shaders/simple.v.glsl", "shaders/simple.f.glsl"));
  std::vector<glm::vec3> positions = {
      {0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}, // top
      {0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1}, // bottom
      {0, 0, 0}, {0, 1, 0}, {0, 1, 1}, {0, 0, 1}, // left
      {1, 0, 0}, {1, 1, 0}, {1, 1, 1}, {1, 0, 1}, // right
      {0, 0, 0}, {0, 0, 1}, {1, 0, 1}, {1, 0, 0}, // front
      {0, 1, 0}, {0, 1, 1}, {1, 1, 1}, {1, 1, 0}  // back
  };
  // center the cube
  for (auto & p : positions) {
    p -= glm::vec3(0.5, 0.5, 0.5);
  }
  std::vector<glm::vec4> colors = {
      {1, 0, 0, 1}, {1, 0, 0, 1}, {1, 0, 0, 1}, {1, 0, 0, 1}, // top
      {1, 0, 0, 1}, {1, 0, 0, 1}, {1, 0, 0, 1}, {1, 0, 0, 1}, // bottom
      {0, 1, 0, 1}, {0, 1, 0, 1}, {0, 1, 0, 1}, {0, 1, 0, 1}, // left
      {0, 1, 0, 1}, {0, 1, 0, 1}, {0, 1, 0, 1}, {0, 1, 0, 1}, // right
      {1, 1, 0, 1}, {1, 1, 0, 1}, {1, 1, 0, 1}, {1, 1, 0, 1}, // front
      {1, 1, 0, 1}, {1, 1, 0, 1}, {1, 1, 0, 1}, {1, 1, 0, 1}  // back
  };
  std::vector<uint> ibo = {
      0,  1,  2,  0,  2,  3,  // top
      4,  5,  6,  4,  6,  7,  // bottom
      8,  9,  10, 8,  10, 11, // left
      12, 13, 14, 12, 14, 15, // right
      16, 17, 18, 16, 18, 19, // front
      20, 21, 22, 20, 22, 23, // back
  };
  m_vaoCube->setVBO(0, positions);
  m_vaoCube->setVBO(1, colors);
  m_vaoCube->setIBO(ibo);
}

void MinimalApplication::makeATexturedCube()
{
  m_vaoTexturedCube = std::unique_ptr<VAO>(new VAO(2));
  m_programTexturedCube = std::unique_ptr<Program>(new Program("shaders/texture.v.glsl", "shaders/texture.f.glsl"));
  m_texture = std::unique_ptr<Texture>(new Texture(GL_TEXTURE_2D));
  GLubyte texels[12] = {255, 0,   0,   255,  // red
                        0,   255, 0,   255,  // green
                        0,   0,   255, 255}; // blue
  m_texture->setData(Image<>(texels, 1, 3, 4));

  std::vector<glm::vec3> vextexPositions = {
      {0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}, // top
      {0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1}, // bottom
      {0, 0, 0}, {0, 1, 0}, {0, 1, 1}, {0, 0, 1}, // left
      {1, 0, 0}, {1, 1, 0}, {1, 1, 1}, {1, 0, 1}, // right
      {0, 0, 0}, {0, 0, 1}, {1, 0, 1}, {1, 0, 0}, // front
      {0, 1, 0}, {0, 1, 1}, {1, 1, 1}, {1, 1, 0}  // back
  };
  // Center the cube on (0,0,0)
  for (auto & p : vextexPositions) {
    p -= glm::vec3(0.5, 0.5, 0.5);
  }
  std::vector<glm::vec2> vertexUVs = {
      {0, 0},   {0, 0.5}, {0, 0},   {0, 0.5}, // top
      {0, 0},   {0, 0},   {0, 0},   {0, 0},   // bottom
      {0, 0.5}, {0, 0.5}, {0, 0.5}, {0, 0.5}, // left
      {0, 0.5}, {0, 0.5}, {0, 0.5}, {0, 0.5}, // right
      {0, 1},   {0, 1},   {0, 1},   {0, 1},   // front
      {0, 1},   {0, 1},   {0, 1},   {0, 1},   // back
  };
  std::vector<uint> ibo = {
      0,  1,  2,  0,  2,  3,  // top
      4,  5,  6,  4,  6,  7,  // bottom
      8,  9,  10, 8,  10, 11, // left
      12, 13, 14, 12, 14, 15, // right
      16, 17, 18, 16, 18, 19, // front
      20, 21, 22, 20, 22, 23, // back
  };
  m_vaoTexturedCube->setVBO(0, vextexPositions);
  m_vaoTexturedCube->setVBO(1, vertexUVs);
  m_vaoTexturedCube->setIBO(ibo);

  m_sampler = std::unique_ptr<Sampler>(new Sampler(0));
  m_sampler->setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  m_sampler->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  m_sampler->setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  m_sampler->setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  m_sampler->attachTexture(*m_texture);
  m_sampler->attachToProgram(*m_programTexturedCube, "colorSampler", Sampler::BindUnbind);
}

void MinimalApplication::renderTexturedCube() const
{
  m_programTexturedCube->bind();
  m_sampler->bind();
  m_vaoTexturedCube->draw();
  m_programTexturedCube->unbind();
  m_sampler->unbind();
}

void MinimalApplication::renderCube() const
{
  m_programCube->bind();
  m_vaoCube->draw();
  m_programCube->unbind();
}

void MinimalApplication::renderFrame()
{
  glClearColor(1, 1, 1, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  glClear(GL_DEPTH_BUFFER_BIT);
  if (m_exercise == 0) {
    renderCube();
  } else {
    renderTexturedCube();
  }
}

void MinimalApplication::update()
{
  float current_time = glfwGetTime();
  if (m_exercise == 0) {
    m_programCube->bind();
    m_programCube->setUniform("time", current_time);
    m_programCube->unbind();
  } else {
    m_programTexturedCube->bind();
    m_programTexturedCube->setUniform("time", current_time);
    m_programTexturedCube->unbind();
  }
}

void MinimalApplication::resize(GLFWwindow *, int framebufferWidth, int framebufferHeight)
{
  glViewport(0, 0, framebufferWidth, framebufferHeight);
}

void MinimalApplication::keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
  MinimalApplication & app = *static_cast<MinimalApplication *>(glfwGetWindowUserPointer(window));
  if (action == GLFW_PRESS) {
    switch (key) {
    case 'E':
      if (mods & GLFW_MOD_SHIFT) {
        app.m_exercise -= 1;
      } else {
        app.m_exercise += 1;
      }
      app.m_exercise = (app.m_exercise + MinimalApplication::nbExercises) % MinimalApplication::nbExercises;
    }
  }
}
