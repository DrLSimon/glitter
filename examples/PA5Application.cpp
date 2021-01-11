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
  m_diffusemap = std::unique_ptr<Sampler>(new Sampler(0));
  m_normalmap = std::unique_ptr<Sampler>(new Sampler(1));
  m_specularmap = std::unique_ptr<Sampler>(new Sampler(2));
}

std::unique_ptr<PA5Application::RenderObject> PA5Application::RenderObject::createCheckerBoardPlaneInstance(const glm::mat4 & modelWorld)
{
  std::unique_ptr<RenderObject> object(new RenderObject(modelWorld));
  std::shared_ptr<Texture> texture(new Texture(GL_TEXTURE_2D));
  Image<> rgbMapImage;
  std::string rgbFilename = absolutename("meshes/checkerboardRGB.png");
  rgbMapImage.data = stbi_load(rgbFilename.c_str(), &rgbMapImage.width, &rgbMapImage.height, &rgbMapImage.channels, STBI_default);
  texture->setData(rgbMapImage, true);

  std::shared_ptr<Texture> stexture(new Texture(GL_TEXTURE_2D));
  stexture->setData(rgbMapImage, true);

  std::shared_ptr<Texture> ntexture(new Texture(GL_TEXTURE_2D));
  Image<> normalMapImage;
  std::string nmFilename = absolutename("meshes/checkerboardNM.png");
  normalMapImage.data = stbi_load(nmFilename.c_str(), &normalMapImage.width, &normalMapImage.height, &normalMapImage.channels, STBI_default);
  ntexture->setData(normalMapImage, true);

  object->m_diffusemap->enableAnisotropicFiltering();

  std::shared_ptr<Program> program(new Program("shaders/simplemat.v.glsl", "shaders/simplemat.f.glsl"));
  SimpleMaterial material;
  material.name = "checkerboard";
  material.ambient = {0.1, 0.1, 0.1};
  material.diffuse = {0.5, 0.5, 0.5};
  material.specular = {1, 1, 1};
  material.shininess = 90;
  object->setProgramMaterial(program, material);
  std::shared_ptr<VAO> vao(new VAO(4));
  std::vector<glm::vec3> vertexPositions = {{-0.5, -0.5, 0}, {0.5, -0.5, 0}, {0.5, 0.5, 0}, {-0.5, 0.5, 0}};
  std::vector<glm::vec2> vertexUVs = {{0, 0}, {0, 40}, {40, 40}, {40, 0}};
  std::vector<glm::vec3> vertexNormals = {{0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}};
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

  object->m_parts.emplace_back(vao, program, texture, ntexture, stexture);
  return object;
}

void PA5Application::RenderObject::draw()
{
  m_diffusemap->bind();
  m_normalmap->bind();
  m_specularmap->bind();
  for (auto & part : m_parts) {
    part.draw(m_diffusemap.get(), m_normalmap.get(), m_specularmap.get());
  }
  m_diffusemap->unbind();
  m_normalmap->unbind();
  m_specularmap->unbind();
}

void PA5Application::RenderObject::update(const glm::mat4 & proj, const glm::mat4 & view)
{
  for (auto & part : m_parts) {
    part.update(proj, view, m_mw, displayNormals);
  }
}

std::unique_ptr<PA5Application::RenderObject> PA5Application::RenderObject::createWavefrontInstance(const std::string & objname, const glm::mat4 & modelWorld)
{
  std::unique_ptr<RenderObject> object(new RenderObject(modelWorld));
  object->loadWavefront(objname);
  return object;
}

void PA5Application::RenderObject::setProgramMaterial(std::shared_ptr<Program> & program, const SimpleMaterial & material) const
{
  program->bind();
  program->setUniform("lightsInWorld[0].direction", glm::normalize(glm::vec3(0, -1, 1)));
  program->setUniform("lightsInWorld[0].intensity", glm::vec3(0.7, 0.7, 0.7));
  program->setUniform("lightsInWorld[1].direction", glm::normalize(glm::vec3(0, 1, 0.5)));
  program->setUniform("lightsInWorld[1].intensity", glm::vec3(0.5, 0.5, 0.5));
  program->setUniform("lightsInWorld[2].direction", glm::normalize(glm::vec3(-1, 0, 1)));
  program->setUniform("lightsInWorld[2].intensity", glm::vec3(0.6, 0.6, 0.6));
  program->setUniform("material.ambient", material.ambient);
  program->setUniform("material.diffuse", material.diffuse);
  program->setUniform("material.specular", material.specular);
  program->setUniform("material.shininess", material.shininess);
  m_diffusemap->attachToProgram(*program, "material.colormap", Sampler::DoNotBind);
  m_normalmap->attachToProgram(*program, "material.normalmap", Sampler::DoNotBind);
  m_specularmap->attachToProgram(*program, "material.specularmap", Sampler::DoNotBind);
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
  std::shared_ptr<VAO> vao(new VAO(4));
  vao->setVBO(0, vertexPositions);
  vao->setVBO(1, vertexUVs);
  vao->setVBO(2, vertexNormals);
  vao->setVBO(3, vertexTangents);
  size_t nbParts = objLoader.nbIBOs();
  for (size_t k = 0; k < nbParts; k++) {
    const std::vector<uint> & ibo = objLoader.ibo(k);
    if (ibo.size() == 0) {
      continue;
    }
    std::shared_ptr<VAO> vaoSlave;
    vaoSlave = vao->makeSlaveVAO();
    vaoSlave->setIBO(ibo);

    std::shared_ptr<Program> program(new Program("shaders/simplemat.v.glsl", "shaders/simplemat.f.glsl"));
    const SimpleMaterial & material = materials[k];
    setProgramMaterial(program, material);
    Image<> colorMap = objLoader.image(material.diffuseTexName);
    std::shared_ptr<Texture> texture(new Texture(GL_TEXTURE_2D));
    texture->setData(colorMap);
    Image<> normalMap = objLoader.image(material.normalTexName);
    std::shared_ptr<Texture> ntexture(new Texture(GL_TEXTURE_2D));
    ntexture->setData(normalMap);
    Image<> specularMap = objLoader.image(material.specularTexName);
    std::shared_ptr<Texture> stexture(new Texture(GL_TEXTURE_2D));
    stexture->setData(specularMap);
    m_parts.emplace_back(vaoSlave, program, texture, ntexture, stexture);
  }
  m_diffusemap->setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  m_diffusemap->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  m_diffusemap->setParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
  m_diffusemap->setParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
  m_normalmap->setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  m_normalmap->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  m_normalmap->setParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
  m_normalmap->setParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
  m_specularmap->setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  m_specularmap->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  m_specularmap->setParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
  m_specularmap->setParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
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
  mw = glm::translate(mw, {1., 0, 0});
  mw = glm::rotate(mw, -pi / 2, {1, 0, 0});
  mw = glm::rotate(mw, -5 * pi / 6, {0, 1, 0});
  mw = glm::scale(mw, glm::vec3(0.25));
  m_objects.push_back(RenderObject::createWavefrontInstance("meshes/Tron/TronLightCycle.obj", mw));
  // m_objects.push_back(RenderObject::createWavefrontInstance("tmp/tron.glitter", mw)); // TODO : Check this
  mw = glm::mat4(1);
  mw = glm::translate(mw, {2, 1, -0.1});
  mw = glm::rotate(mw, pi, {1, 0, 0});
  m_objects.push_back(RenderObject::createWavefrontInstance("meshes/Pallet/Bswap_HPBake_Planks.obj", mw));
  // m_objects.push_back(RenderObject::createWavefrontInstance("tmp/pallet.glitter", mw)); // TODO : Check this
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
  glm::vec3 eyePos = 5.f * glm::vec3(cos(m_eyePhi) * sin(m_eyeTheta), sin(m_eyePhi) * sin(m_eyeTheta), cos(m_eyeTheta));
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
  app.m_proj = glm::perspective(120.f, aspect, 0.1f, 100.f);
  glViewport(0, 0, framebufferWidth, framebufferHeight);
}

void PA5Application::keyCallback(GLFWwindow * window, int key, int /*scancode*/, int action, int /*mods*/)
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

PA5Application::RenderObjectPart::RenderObjectPart(std::shared_ptr<VAO> vao, std::shared_ptr<Program> program, std::shared_ptr<Texture> texture, std::shared_ptr<Texture> ntexture,
                                                   std::shared_ptr<Texture> stexture)
    : m_vao(vao), m_program(program), m_diffuseTexture(texture), m_normalTexture(ntexture), m_specularTexture(stexture)
{
}

void PA5Application::RenderObjectPart::draw(Sampler * colormap, Sampler * normalmap, Sampler * specularmap)
{
  m_program->bind();
  colormap->attachTexture(*m_diffuseTexture);
  normalmap->attachTexture(*m_normalTexture);
  specularmap->attachTexture(*m_specularTexture);
  m_vao->draw();
  m_program->unbind();
}

void PA5Application::RenderObjectPart::update(const glm::mat4 & proj, const glm::mat4 & view, const glm::mat4 & mw, bool displayNormals)
{
  m_program->bind();
  m_program->setUniform("M", mw);
  m_program->setUniform("V", view);
  m_program->setUniform("P", proj);
  m_program->setUniform("positionCameraInWorld", glm::vec3(glm::inverse(view) * glm::vec4(0, 0, 0, 1)));
  if (displayNormals) {
    m_program->setUniform("displayNormals", 1);
  } else {
    m_program->setUniform("displayNormals", 0);
  }
  m_program->unbind();
}
