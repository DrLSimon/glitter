#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include "PA5Application.hpp"
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "ObjLoader.hpp"
#include "stb_image.h"
#include "utils.hpp"

PA5Application::RenderObject::RenderObject(const glm::mat4 & modelWorld) : m_mw(modelWorld)
{
  m_colormap = std::unique_ptr<Sampler>(new Sampler(0));
  m_specularmap = std::unique_ptr<Sampler>(new Sampler(2));
  m_normalmap = std::unique_ptr<Sampler>(new Sampler(1));
}

std::unique_ptr<PA5Application::RenderObject> PA5Application::RenderObject::createCheckerBoardPlaneInstance(const glm::mat4 & modelWorld)
{
  std::unique_ptr<RenderObject> object(new RenderObject(modelWorld));
  std::shared_ptr<Texture> texture(new Texture(GL_TEXTURE_2D));
  object->m_diffuseTextures.push_back(texture);
  Image<> rgbMapImage;
  std::string rgbFilename = absolutename("meshes/checkerboardRGB.png");
  rgbMapImage.data = stbi_load(rgbFilename.c_str(), &rgbMapImage.width, &rgbMapImage.height, &rgbMapImage.channels, STBI_default);
  texture->setData(rgbMapImage, true);

  std::shared_ptr<Texture> stexture(new Texture(GL_TEXTURE_2D));
  object->m_specularTextures.push_back(stexture);
  stexture->setData(rgbMapImage, true);

  std::shared_ptr<Texture> ntexture(new Texture(GL_TEXTURE_2D));
  object->m_normalTextures.push_back(ntexture);
  Image<> normalMapImage;
  std::string nmFilename = absolutename("meshes/checkerboardNM.png");
  normalMapImage.data = stbi_load(nmFilename.c_str(), &normalMapImage.width, &normalMapImage.height, &normalMapImage.channels, STBI_default);
  ntexture->setData(normalMapImage, true);

  object->m_colormap->enableAnisotropicFiltering();

  std::shared_ptr<Program> program(new Program("shaders/simplemat.v.glsl", "shaders/simplemat.f.glsl"));
  object->m_programs.push_back(program);
  SimpleMaterial material;
  material.name = "checkerboard";
  material.ambient = {0.1, 0.1, 0.1};
  material.diffuse = {0.5, 0.5, 0.5};
  material.specular = {1, 1, 1};
  material.shininess = 90;
  object->setProgramMaterial(program, material);
  std::shared_ptr<VAO> vao(new VAO(4));
  object->m_vaos.push_back(vao);
  std::vector<glm::vec3> vertexPositions = {
      {-0.5, -0.5, -0.5}, {0.5, -0.5, -0.5}, {0.5, 0.5, -0.5}, {-0.5, 0.5, -0.5}, // front
  };
  std::vector<glm::vec2> vertexUVs = {
      {0, 0}, {0, 40}, {40, 40}, {40, 0}, // front
  };
  std::vector<glm::vec3> vertexNormals = {
      {0, 0, -1}, {0, 0, -1}, {0, 0, -1}, {0, 0, -1}, // front
  };
  std::vector<glm::vec3> vertexTangents = {
      {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, // front
  };
  std::vector<uint> ibo = {
      0, 1, 2, 0, 2, 3, // front
  };
  vao->setVBO(0, vertexPositions);
  vao->setVBO(1, vertexUVs);
  vao->setVBO(2, vertexNormals);
  vao->setVBO(3, vertexTangents);
  vao->setIBO(ibo);

  return object;
}

void PA5Application::RenderObject::draw(GLenum mode)
{
  size_t nbObjects = m_vaos.size();
  m_colormap->bind();
  m_normalmap->bind();
  m_specularmap->bind();
  for (size_t k = 0; k < nbObjects; k++) {
    m_programs[k]->bind();
    Texture & texture = *m_diffuseTextures[k];
    m_colormap->attachTexture(texture);
    Texture & ntexture = *m_normalTextures[k];
    m_normalmap->attachTexture(ntexture);
    Texture & stexture = *m_specularTextures[k];
    m_specularmap->attachTexture(stexture);
    m_vaos[k]->draw();
    m_programs[k]->unbind();
  }
  m_colormap->unbind();
  m_normalmap->unbind();
  m_specularmap->unbind();
}

void PA5Application::RenderObject::update(const glm::mat4 & proj, const glm::mat4 & view)
{
  for (auto & prog : m_programs) {
    prog->bind();
    prog->setUniform("M", m_mw);
    prog->setUniform("V", view);
    prog->setUniform("P", proj);
    prog->setUniform("positionCameraInWorld", glm::vec3(glm::inverse(view) * glm::vec4(0, 0, 0, 1)));
    if (displayNormals) {
      prog->setUniform("displayNormals", 1);
    } else {
      prog->setUniform("displayNormals", 0);
    }
    prog->unbind();
  }
}

PA5Application::RenderObject::RenderObject(const std::string & objname, const glm::mat4 & modelWorld)
    : m_mw(modelWorld), m_colormap(std::unique_ptr<Sampler>(new Sampler(0))), m_specularmap(std::unique_ptr<Sampler>(new Sampler(2))), m_normalmap(std::unique_ptr<Sampler>(new Sampler(1)))
{
  loadWavefront(objname);
}

std::unique_ptr<PA5Application::RenderObject> PA5Application::RenderObject::createWavefrontInstance(const std::string & objname, const glm::mat4 & modelWorld)
{
  return std::unique_ptr<RenderObject>(new RenderObject(objname, modelWorld));
}

void PA5Application::RenderObject::setProgramMaterial(std::shared_ptr<Program> & program, const SimpleMaterial & material) const
{
  program->bind();
  program->setUniform("lightsInWorld[0].direction", glm::normalize(glm::vec3(0, -1, -1)));
  program->setUniform("lightsInWorld[0].intensity", glm::vec3(0.7, 0.7, 0.7));
  program->setUniform("lightsInWorld[1].direction", glm::normalize(glm::vec3(0, 1, -0.5)));
  program->setUniform("lightsInWorld[1].intensity", glm::vec3(0.5, 0.5, 0.5));
  program->setUniform("lightsInWorld[2].direction", glm::normalize(glm::vec3(-1, 0, -1)));
  program->setUniform("lightsInWorld[2].intensity", glm::vec3(0.6, 0.6, 0.6));
  program->setUniform("material.ambient", material.ambient);
  program->setUniform("material.diffuse", material.diffuse);
  program->setUniform("material.specular", material.specular);
  program->setUniform("material.shininess", material.shininess);
  m_colormap->attachToProgram(*program, "material.colormap", Sampler::DoNotBind);
  m_normalmap->attachToProgram(*program, "material.normalmap", Sampler::DoNotBind);
  program->unbind();
}

void PA5Application::RenderObject::loadWavefront(const std::string & objname)
{
  ObjLoader objLoader(objname);
  const std::vector<SimpleMaterial> & materials = objLoader.materials();
  std::vector<glm::vec3> vertexPositions = objLoader.vertexPositions();
  const std::vector<glm::vec2> & vertexUVs = objLoader.vertexUVs();
  std::vector<glm::vec3> vertexNormals = objLoader.vertexNormals();
  std::vector<glm::vec3> vertexTangents = objLoader.vertexTangents();
  // set up the VBOs of the master VAO
  m_vaos.emplace_back(new VAO(4));
  auto & vao = m_vaos.back();
  vao->setVBO(0, vertexPositions);
  vao->setVBO(1, vertexUVs);
  vao->setVBO(2, vertexNormals);
  vao->setVBO(3, vertexTangents);
  size_t nbObjects = objLoader.nbIBOs();
  for (size_t k = 0; k < nbObjects; k++) {
    const std::vector<uint> & ibo = objLoader.ibo(k);
    if (ibo.size() == 0) {
      continue;
    }
    if (k != 0) {
      // push a slave VAO (sharing the VBOs with the master)
      m_vaos.push_back(m_vaos.front()->makeSlaveVAO());
    }
    m_vaos.back()->setIBO(ibo);
    m_programs.emplace_back(new Program("shaders/simplemat.v.glsl", "shaders/simplemat.f.glsl"));
    auto & program = m_programs.back();
    const SimpleMaterial & material = materials[k];
    setProgramMaterial(program, material);
    Image<> colorMap = objLoader.image(material.diffuseTexName);
    m_diffuseTextures.emplace_back(new Texture(GL_TEXTURE_2D));
    Texture & texture = *m_diffuseTextures.back();
    texture.setData(colorMap);
    Image<> normalMap = objLoader.image(material.normalTexName);
    m_normalTextures.emplace_back(new Texture(GL_TEXTURE_2D));
    Texture & ntexture = *m_normalTextures.back();
    ntexture.setData(normalMap);
    Image<> specularMap = objLoader.image(material.specularTexName);
    m_specularTextures.emplace_back(new Texture(GL_TEXTURE_2D));
    Texture & stexture = *m_specularTextures.back();
    stexture.setData(specularMap);
  }
  m_colormap->setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  m_colormap->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  m_colormap->setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  m_colormap->setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  m_normalmap->setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  m_normalmap->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  m_normalmap->setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  m_normalmap->setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  m_specularmap->setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  m_specularmap->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  m_specularmap->setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  m_specularmap->setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

bool PA5Application::displayNormals;

PA5Application::PA5Application(int windowWidth, int windowHeight) : Application(windowWidth, windowHeight), m_currentTime(0), m_deltaTime(0)
{
  GLFWwindow * window = glfwGetCurrentContext();
  glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
  resize(window, windowWidth, windowHeight);
  computeView(true);
  glEnable(GL_DEPTH_TEST);
  glm::mat4 mw(1);
  mw = glm::translate(mw, {0, 1.1, 0});
  mw = glm::scale(mw, glm::vec3(50, 50, 0.1));
  m_objects.push_back(RenderObject::createCheckerBoardPlaneInstance(mw));
  const float pi = glm::pi<float>();
  mw = glm::mat4(1);
  mw = glm::translate(mw, {0.75, 0, 0});
  mw = glm::rotate(mw, -pi / 2, {1, 0, 0});
  mw = glm::scale(mw, glm::vec3(0.25));
  m_objects.push_back(RenderObject::createWavefrontInstance("meshes/Tron/TronLightCycle.obj", mw));
  mw = glm::mat4(1);
  mw = glm::translate(mw, {2, 1, -0.1});
  mw = glm::rotate(mw, pi, {1, 0, 0});
  m_objects.push_back(RenderObject::createWavefrontInstance("meshes/Pallet/Bswap_HPBake_Planks.obj", mw));
}

void PA5Application::setCallbacks()
{
  GLFWwindow * window = glfwGetCurrentContext();
  glfwSetFramebufferSizeCallback(window, PA5Application::resize);
  glfwSetKeyCallback(window, PA5Application::keyCallback);
}

void PA5Application::usage(std::string & shortDescription, std::string & synopsis, std::string & description)
{
  shortDescription = "Application for programming assignment 4";
  synopsis = "pa4";
  description = "  An application for texture mapping.\n"
                "  The following key bindings are available to interact with thi application:\n"
                "     <up> / <down>    increase / decrease latitude angle of the camera position\n"
                "     <left> / <right> increase / decrease longitude angle of the camera position\n"
                "     R                reset the view\n";
}

void PA5Application::renderFrame()
{
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  glClear(GL_DEPTH_BUFFER_BIT);
  for (auto & object : m_objects) {
    object->draw();
  }
}

void PA5Application::update()
{
  float prevTime = m_currentTime;
  m_currentTime = glfwGetTime();
  m_deltaTime = m_currentTime - prevTime;
  continuousKey();
  for (auto & object : m_objects) {
    object->update(m_proj, m_view);
  }
}

void PA5Application::computeView(bool reset)
{
  if (reset) {
    const float pi = glm::pi<float>();
    m_eyePhi = pi / 8;
    m_eyeTheta = pi / 2 + pi / 10;
  }
  glm::vec3 center(0, 0, 0);
  glm::vec3 up(0, 0, -1);
  glm::vec3 eyePos = 3.f * glm::vec3(cos(m_eyePhi) * sin(m_eyeTheta), sin(m_eyePhi) * sin(m_eyeTheta), cos(m_eyeTheta));
  m_view = glm::lookAt(eyePos, center, up);
}

void PA5Application::continuousKey()
{
  GLFWwindow * window = glfwGetCurrentContext();
  const float pi = glm::pi<float>();
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    m_eyeTheta += m_deltaTime * pi;
    if (m_eyeTheta > pi - pi / 10) {
      m_eyeTheta = pi - pi / 10.;
    }
  } else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    m_eyeTheta -= m_deltaTime * pi;
    if (m_eyeTheta < pi / 10) {
      m_eyeTheta = pi / 10.;
    }
  }
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
    m_eyePhi += m_deltaTime * pi;
  } else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
    m_eyePhi -= m_deltaTime * pi;
  }
  computeView();
}

void PA5Application::resize(GLFWwindow * window, int framebufferWidth, int framebufferHeight)
{
  PA5Application & app = *static_cast<PA5Application *>(glfwGetWindowUserPointer(window));
  float aspect = framebufferWidth / float(framebufferHeight);
  app.m_proj = glm::perspective(90.f, aspect, 0.1f, 100.f);
  glViewport(0, 0, framebufferWidth, framebufferHeight);
}

void PA5Application::keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
  PA5Application & app = *static_cast<PA5Application *>(glfwGetWindowUserPointer(window));
  switch (key) {
  case 'R':
    app.computeView(true);
    break;
  case 'N':
    if (action == GLFW_PRESS or action == GLFW_RELEASE) {
      displayNormals = not displayNormals;
    }
    break;
  }
}
