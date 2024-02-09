#include "ProjectApplication.hpp"
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "ObjLoader.hpp"
#include "utils.hpp"

ProjectApplication::ProjectApplication(int windowWidth, int windowHeight)
    : Application(windowWidth, windowHeight), m_program(new Program("shaders/texture.v.glsl", "shaders/texture.f.glsl")), m_currentTime(0), m_deltaTime(0)
{
    GLFWwindow * window = glfwGetCurrentContext();
    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
    resize(window, windowWidth, windowHeight);
    computeView(true);
    glEnable(GL_DEPTH_TEST);
    m_colormap = std::unique_ptr<Sampler>(new Sampler(0));
    m_colormap->setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    m_colormap->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    m_colormap->setParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
    m_colormap->setParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
    loadWavefront("meshes/capsule.obj");
}


void ProjectApplication::loadWavefront(const std::string & objname)
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
    m_vaos.push_back(vaoSlave);
    const SimpleMaterial & material = materials[k];
    Image<> colorMap = objLoader.image(material.diffuseTexName);
    std::shared_ptr<Texture> texture(new Texture(GL_TEXTURE_2D));
    texture->setData(colorMap);
    m_textures.push_back(texture);
  }
}

void ProjectApplication::renderFrame()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
    m_colormap->bind();
    m_program->bind();
    m_program->setUniform("diffuseColor", glm::vec3(1,1,1));
    m_colormap->attachToProgram(*m_program, "colorSampler", Sampler::DoNotBind);
    auto mw = glm::mat4(1);
    m_program->setUniform("M",mw);
    for(uint k=0; k<m_vaos.size(); ++k){ 
         auto vao = m_vaos[k];
         auto texture = m_textures[k];
        m_colormap->attachTexture(*texture);
        vao->draw();
    }
    m_program->unbind(); 
    m_colormap->unbind();
}


void ProjectApplication::setCallbacks()
{
  GLFWwindow * window = glfwGetCurrentContext();
  glfwSetFramebufferSizeCallback(window, ProjectApplication::resize);
  glfwSetKeyCallback(window, ProjectApplication::keyCallback);
}

void ProjectApplication::usage(std::string & shortDescription, std::string & synopsis, std::string & description)
{
  shortDescription = "Application for programming assignment 4";
  synopsis = "pa6";
  description = "  An application for texture mapping.\n"
                "  The following key bindings are available to interact with thi application:\n"
                "     <up> / <down>    increase / decrease latitude angle of the camera position\n"
                "     <left> / <right> increase / decrease longitude angle of the camera position\n"
                "     R                reset the view\n";
}


void ProjectApplication::update()
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


void ProjectApplication::computeView(bool reset)
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

void ProjectApplication::continuousKey()
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

void ProjectApplication::resize(GLFWwindow * window, int framebufferWidth, int framebufferHeight)
{
  ProjectApplication & app = *static_cast<ProjectApplication *>(glfwGetWindowUserPointer(window));
  float aspect = framebufferWidth / float(framebufferHeight);
  app.m_proj = glm::perspective(120.f, aspect, 0.1f, 100.f);
  glViewport(0, 0, framebufferWidth, framebufferHeight);
}

void ProjectApplication::keyCallback(GLFWwindow * window, int key, int /*scancode*/, int /*action*/, int /*mods*/)
{
  ProjectApplication & app = *static_cast<ProjectApplication *>(glfwGetWindowUserPointer(window));
  switch (key) {
  case 'R':
    app.computeView(true);
    break;
  }
}

