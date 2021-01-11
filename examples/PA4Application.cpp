#include "PA4Application.hpp"
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "ObjLoader.hpp"
#include "utils.hpp"

PA4Application::RenderObject::RenderObject(const std::shared_ptr<Program> & program, const glm::mat4 & modelWorld) : m_program(program), m_mw(modelWorld)
{
  if (part >= 3) {
    m_colormap = std::unique_ptr<Sampler>(new Sampler(0));
  }
}

std::unique_ptr<PA4Application::RenderObject> PA4Application::RenderObject::createCheckerBoardCubeInstance(const std::shared_ptr<Program> & program, const glm::mat4 & modelWorld)
{
  std::unique_ptr<RenderObject> object(new RenderObject(program, modelWorld));
  std::shared_ptr<Texture> texture(new Texture(GL_TEXTURE_2D));
  std::vector<GLubyte> checkerboard = makeCheckerBoard();

  texture->setData(Image<>(checkerboard.data(), 20, 20, 4), true);
  if (part == 1) {
    texture->bind();
    // remove the need for mipmaps
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    texture->unbind();
  }

  if (part >= 3) {
    object->m_colormap->enableAnisotropicFiltering();
  }

  program->bind();
  program->setUniform("diffuseColor", glm::vec3(1));
  program->unbind();

  std::shared_ptr<VAO> vao(new VAO(2));
  std::vector<glm::vec3> vextexPositions = {
      {-0.5, -0.5, -0.5}, {0.5, -0.5, -0.5}, {0.5, 0.5, -0.5}, {-0.5, 0.5, -0.5}, // top
      {-0.5, -0.5, 0.5},  {0.5, -0.5, 0.5},  {0.5, 0.5, 0.5},  {-0.5, 0.5, 0.5},  // bottom
      {-0.5, -0.5, -0.5}, {-0.5, 0.5, -0.5}, {-0.5, 0.5, 0.5}, {-0.5, -0.5, 0.5}, // left
      {0.5, -0.5, -0.5},  {0.5, 0.5, -0.5},  {0.5, 0.5, 0.5},  {0.5, -0.5, 0.5},  // right
      {-0.5, -0.5, -0.5}, {-0.5, -0.5, 0.5}, {0.5, -0.5, 0.5}, {0.5, -0.5, -0.5}, // front
      {-0.5, 0.5, -0.5},  {-0.5, 0.5, 0.5},  {0.5, 0.5, 0.5},  {0.5, 0.5, -0.5}   // back
  };
  std::vector<glm::vec2> vertexUVs = {
      {0, 0},   {0, 100}, {100, 100}, {100, 0}, // top
      {0, 0},   {0, 0},   {0, 0},     {0, 0},   // bottom
      {0, 0.5}, {0, 0.5}, {0, 0.5},   {0, 0.5}, // left
      {0, 0.5}, {0, 0.5}, {0, 0.5},   {0, 0.5}, // right
      {0, 1},   {0, 1},   {0, 1},     {0, 1},   // front
      {0, 1},   {0, 1},   {0, 1},     {0, 1},   // back
  };
  std::vector<uint> ibo = {
      0,  1,  2,  0,  2,  3,  // top
      4,  5,  6,  4,  6,  7,  // bottom
      8,  9,  10, 8,  10, 11, // left
      12, 13, 14, 12, 14, 15, // right
      16, 17, 18, 16, 18, 19, // front
      20, 21, 22, 20, 22, 23, // back
  };
  vao->setVBO(0, vextexPositions);
  vao->setVBO(1, vertexUVs);
  vao->setIBO(ibo);

  glm::vec3 diffuse(1);
  object->m_parts.emplace_back(vao, program, diffuse, texture);

  return object;
}

void PA4Application::RenderObject::draw()
{
  update();
  if (m_colormap) {
    m_colormap->bind();
  }
  for (auto & part : m_parts) {
    part.draw(m_colormap.get());
  }
  if (m_colormap) {
    m_colormap->unbind();
  }
}

std::unique_ptr<PA4Application::RenderObject> PA4Application::RenderObject::createWavefrontInstance(const std::shared_ptr<Program> & program, const std::string & objname, const glm::mat4 & modelWorld)
{
  std::unique_ptr<RenderObject> object(new RenderObject(program, modelWorld));
  object->loadWavefront(objname);
  return object;
}

std::vector<GLubyte> PA4Application::RenderObject::makeCheckerBoard()
{
  std::vector<GLubyte> checkerboard;
  for (uint k = 0; k < 10; ++k) {
    std::vector<GLubyte> row;
    for (uint j = 0; j < 10; ++j) {
      row.insert(row.begin(), {255, 0, 0, 255});
      row.insert(row.end(), {0, 255, 0, 255});
    }
    checkerboard.insert(checkerboard.end(), row.begin(), row.end());
  }
  for (uint k = 0; k < 10; ++k) {
    std::vector<GLubyte> row;
    for (uint j = 0; j < 10; ++j) {
      row.insert(row.begin(), {0, 255, 0, 255});
      row.insert(row.end(), {0, 0, 255, 255});
    }
    checkerboard.insert(checkerboard.end(), row.begin(), row.end());
  }
  return checkerboard;
}

void PA4Application::RenderObject::loadWavefront(const std::string & objname)
{
  ObjLoader objLoader(objname);
  const std::vector<SimpleMaterial> & materials = objLoader.materials();
  std::vector<glm::vec3> vextexPositions = objLoader.vertexPositions();
  const std::vector<glm::vec2> & vertexUVs = objLoader.vertexUVs();
  // set up the VBOs of the master VAO
  std::shared_ptr<VAO> vao(new VAO(2));
  vao->setVBO(0, vextexPositions);
  vao->setVBO(1, vertexUVs);
  size_t nbParts = objLoader.nbIBOs();
  for (size_t k = 0; k < nbParts; k++) {
    const std::vector<uint> & ibo = objLoader.ibo(k);
    if (ibo.size() == 0) {
      continue;
    }
    std::shared_ptr<VAO> vaoSlave;
    vaoSlave = vao->makeSlaveVAO();
    vaoSlave->setIBO(ibo);
    const SimpleMaterial & material = materials[k];
    Image<> colorMap = objLoader.image(material.diffuseTexName);
    std::shared_ptr<Texture> texture(new Texture(GL_TEXTURE_2D));
    texture->setData(colorMap);
    m_parts.push_back(RenderObjectPart(vaoSlave, m_program, material.diffuse, texture));
  }
  m_colormap->setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  m_colormap->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  m_colormap->setParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
  m_colormap->setParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
}

unsigned int PA4Application::part;

PA4Application::PA4Application(int windowWidth, int windowHeight)
    : Application(windowWidth, windowHeight), m_program(new Program("shaders/texture.v.glsl", "shaders/texture.f.glsl")), m_currentTime(0), m_deltaTime(0)
{
  GLFWwindow * window = glfwGetCurrentContext();
  glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
  resize(window, windowWidth, windowHeight);
  computeView(true);
  glEnable(GL_DEPTH_TEST);
  glm::mat4 mw(1);
  mw = glm::translate(mw, {0, 1.1, 0});
  mw = glm::scale(mw, glm::vec3(50, 50, 0.1));
  m_objects.push_back(RenderObject::createCheckerBoardCubeInstance(m_program, mw));
  if (part >= 3) {
    const float pi = glm::pi<float>();
    mw = glm::mat4(1);
    mw = glm::rotate(mw, -pi / 2, {1, 0, 0});
    mw = glm::scale(mw, glm::vec3(0.25));
    m_objects.push_back(RenderObject::createWavefrontInstance(m_program, "meshes/Tron/TronLightCycle.obj", mw));
    mw = glm::mat4(1);
    mw = glm::translate(mw, {0, -3, 0});
    m_objects.push_back(RenderObject::createWavefrontInstance(m_program, "meshes/capsule.obj", mw));
  }
}

void PA4Application::setCallbacks()
{
  GLFWwindow * window = glfwGetCurrentContext();
  glfwSetFramebufferSizeCallback(window, PA4Application::resize);
  glfwSetKeyCallback(window, PA4Application::keyCallback);
}

void PA4Application::usage(std::string & shortDescription, std::string & synopsis, std::string & description)
{
  shortDescription = "Application for programming assignment 4";
  synopsis = "pa4";
  description = "  An application for texture mapping.\n"
                "  The following key bindings are available to interact with thi application:\n"
                "     <up> / <down>    increase / decrease latitude angle of the camera position\n"
                "     <left> / <right> increase / decrease longitude angle of the camera position\n"
                "     R                reset the view\n";
}

void PA4Application::renderFrame()
{
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  glClear(GL_DEPTH_BUFFER_BIT);
  for (auto & object : m_objects) {
    object->draw();
  }
}

void PA4Application::update()
{
  float prevTime = m_currentTime;
  m_currentTime = glfwGetTime();
  m_deltaTime = m_currentTime - prevTime;

  m_program->bind();
  m_program->setUniform("V", m_view);
  m_program->setUniform("P", m_proj);
  m_program->unbind();

  continuousKey();
}

void PA4Application::RenderObject::update()
{
  for (auto & part : m_parts) {
    part.update(m_mw);
  }
}

void PA4Application::computeView(bool reset)
{
  if (reset) {
    const float pi = glm::pi<float>();
    m_eyePhi = pi / 5;
    m_eyeTheta = pi / 2 + pi / 10;
  }
  glm::vec3 center(0, 0, 0);
  glm::vec3 up(0, 0, -1);
  glm::vec3 eyePos = 5.f * glm::vec3(cos(m_eyePhi) * sin(m_eyeTheta), sin(m_eyePhi) * sin(m_eyeTheta), cos(m_eyeTheta));
  m_view = glm::lookAt(eyePos, center, up);
}

void PA4Application::continuousKey()
{
  GLFWwindow * window = glfwGetCurrentContext();
  const float pi = glm::pi<float>();
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    m_eyeTheta += m_deltaTime * pi;
  } else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    m_eyeTheta -= m_deltaTime * pi;
  }
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
    m_eyePhi += m_deltaTime * pi;
  } else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
    m_eyePhi -= m_deltaTime * pi;
  }
  computeView();
}

void PA4Application::resize(GLFWwindow * window, int framebufferWidth, int framebufferHeight)
{
  PA4Application & app = *static_cast<PA4Application *>(glfwGetWindowUserPointer(window));
  float aspect = framebufferWidth / float(framebufferHeight);
  app.m_proj = glm::perspective(120.f, aspect, 0.1f, 100.f);
  glViewport(0, 0, framebufferWidth, framebufferHeight);
}

void PA4Application::keyCallback(GLFWwindow * window, int key, int /*scancode*/, int /*action*/, int /*mods*/)
{
  PA4Application & app = *static_cast<PA4Application *>(glfwGetWindowUserPointer(window));
  switch (key) {
  case 'R':
    app.computeView(true);
    break;
  }
}

PA4Application::RenderObjectPart::RenderObjectPart(std::shared_ptr<VAO> vao, std::shared_ptr<Program> program, const glm::vec3 & diffuse, std::shared_ptr<Texture> texture)
    : m_vao(vao), m_program(program), m_diffuse(diffuse), m_texture(texture)
{
}

void PA4Application::RenderObjectPart::draw(Sampler * colormap)
{
  m_program->bind();
  m_program->setUniform("diffuseColor", m_diffuse);
  if (colormap) {
    colormap->attachTexture(*m_texture);
    colormap->attachToProgram(*m_program, "colorSampler", Sampler::DoNotBind);
  } else {
    const int unit = 0;
    glActiveTexture(GL_TEXTURE0 + unit);
    m_texture->bind();
    m_program->setUniform("colorSampler", unit);
  }
  m_vao->draw();
  m_program->unbind();
}

void PA4Application::RenderObjectPart::update(const glm::mat4 & mw)
{
  m_program->bind();
  m_program->setUniform("M", mw);
  m_program->unbind();
}
