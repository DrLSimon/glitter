#include "utils.hpp"
#include <cstring>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <fstream>
#include <iostream>

/*
 * OpenGL Error checking
 */
void checkGLerror()
{
  const auto gluErrorString = [](GLenum errorCode) -> const char * {
    switch (errorCode) {
    default:
      return "unknown error code";
    case GL_NO_ERROR:
      return "no error";
    case GL_INVALID_ENUM:
      return "invalid enumerant";
    case GL_INVALID_VALUE:
      return "invalid value";
    case GL_INVALID_OPERATION:
      return "invalid operation";
#ifndef GL_VERSION_3_0
    case GL_STACK_OVERFLOW:
      return "stack overflow";
    case GL_STACK_UNDERFLOW:
      return "stack underflow";
    case GL_TABLE_TOO_LARGE:
      return "table too large";
#endif
    case GL_OUT_OF_MEMORY:
      return "out of memory";
#ifdef GL_EXT_framebuffer_object
    case GL_INVALID_FRAMEBUFFER_OPERATION_EXT:
      return "invalid framebuffer operation";
#endif
    }
  };

  GLenum errCode;
  if ((errCode = glGetError()) != GL_NO_ERROR)
    std::cerr << "OpenGL Error: " << gluErrorString(errCode) << std::endl;
}

bool fileExists(const std::string & name)
{
  std::ifstream f(name.c_str());
  return f.good();
}

std::string fileContent(const std::string & filename)
{
  std::ifstream fs(std::string(RESOURCE_DIR) + "/" + filename);
  std::string source(std::istreambuf_iterator<char>(fs), (std::istreambuf_iterator<char>()));
  return source;
}

std::string absolutename(const std::string & filename)
{
  if (filename[0] == '/') {
    return filename;
  }
  return std::string(RESOURCE_DIR) + "/" + filename;
}

std::string basename(const std::string & filepath)
{
  std::string basedir;
  if (filepath.find_last_of("/\\") != std::string::npos) {
    basedir = filepath.substr(0, filepath.find_last_of("/\\"));
  } else {
    basedir = ".";
  }
#ifdef _WIN32
  basedir += "\\";
#else
  basedir += "/";
#endif
  return basedir;
}


bool endsWith(const std::string & str, const std::string & suffix)
{
  return (suffix.size() <= str.size() && !strcmp(str.c_str() + str.size() - suffix.size(), suffix.c_str()));
}
