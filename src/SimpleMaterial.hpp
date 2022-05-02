#ifndef __GLITTER_SIMPLE_MATERIAL_H__
#define __GLITTER_SIMPLE_MATERIAL_H__

#include <glm/glm.hpp>
#include <string>

struct SimpleMaterial {
  std::string name;
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
  float shininess;

  std::string diffuseTexName;
  std::string normalTexName;
  std::string specularTexName;
};
#endif // !defined(__GLITTER_SIMPLE_MATERIAL_H__)
