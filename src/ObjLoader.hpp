#ifndef __GLITTER_OBJLOADER_H__
#define __GLITTER_OBJLOADER_H__
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "Image.hpp"
#include "SimpleMaterial.hpp"
#include "tiny_obj_loader.h"
typedef unsigned int uint;

/**
 * @brief A facade class for loading wavefront files (.obj)
 *
 * The following features are currently supported:
 *	+ vertex explicit attributes (positions, normals, colors, uvs)
 *	+ vertex implicit attributes (normals if not specified, uv-compatible tangents)
 *	+ faces
 *	+ materials (partial support)
 *		+ ambient (Ka), diffuse (Kd), specular (Ks), shininess (Ns)
 *		+ diffuse texture map (map_Ka)
 *		+ normal texture map (norm)
 *	+ per face material affectations
 *
 * @note the class exposes vertex attributes as vectors of glm::vec, and faces as
 * IBOs (vectors of indices). One IBO is created per material, so that all faces
 * associated to a given material are represented in the corresponding IBO.
 * Besides, the vertex attributes and the IBOs are optimized in order to avoid
 * identical vertex repetitions.
 */
class ObjLoader {
public:
  /**
   * @brief Constructor from a wavefront filename
   * @param filename the file to be parsed.
   *
   * The parsing is performed at construction time.
   * Then all the exposed attributes are accessible through getters.
   */
  ObjLoader(const std::string & filename);

  /**
   * @brief Serialize the object in a .glitter file.
   * @param filename
   */
  void saveBinaryFile(const std::string & filename) const;

  /**
   * @brief getter for vertex positions
   * @return the list of vertex position attributes.
   */
  const std::vector<glm::vec3> & vertexPositions() const;

  /**
   * @brief getter for vertex colors
   * @return the list of vertex color attributes.
   */
  const std::vector<glm::vec4> & vertexColors() const;

  /**
   * @brief getter for vertex uVs
   * @return the list of vertex uv attributes.
   */
  const std::vector<glm::vec2> & vertexUVs() const;

  /**
   * @brief getter for vertex normals
   * @return the list of vertex normal attributes.
   */
  const std::vector<glm::vec3> & vertexNormals() const;

  /**
   * @brief getter for vertex tangents
   * @return the list of vertex tangent attributes.
   */
  const std::vector<glm::vec3> & vertexTangents() const;

  /**
   * @brief getter for a given IBO
   * @param materialIndex index of the material associated with the desired IBO.
   * @return
   */
  const std::vector<unsigned int> & ibo(unsigned int materialIndex = 0) const;

  /**
   * @brief getter for the materials
   * @return the list of materials.
   */
  const std::vector<SimpleMaterial> & materials() const;

  /**
   * @brief getter for a image referenced in the materials
   * @param name an alias for the image
   * @return the image designed by name.
   */
  Image<> image(const std::string & name) const;

  /**
   * @brief provides the number of IBOs available after parsing
   * @return the number of IBOS.
   */
  size_t nbIBOs() const;

private:
  class NamedTextureImages {
  public:
    NamedTextureImages() {}
    NamedTextureImages(const NamedTextureImages &) = delete;
    NamedTextureImages & operator=(const NamedTextureImages &) = delete;
    bool find(const std::string & name) const;
    void add(const std::string & name, const Image<> & image);
    const Image<> & operator[](const std::string & name) const;
    ~NamedTextureImages();
    std::vector<std::string> names() const;

  private:
    std::unordered_map<std::string, Image<>> m_images;
  };

private:
  void parseFile(const std::string & filename);
  void loadBinaryFile(const std::string & filename);
  void cleanUpDuplicates();
  void computeTangents();

private:
  std::string m_rootDir;
  std::vector<glm::vec3> m_vertexPositions;
  std::vector<glm::vec4> m_vertexColors;
  std::vector<glm::vec2> m_vertexUVs;
  std::vector<glm::vec3> m_vertexNormals;
  std::vector<glm::vec3> m_vertexTangents;
  typedef std::vector<unsigned int> IBO;
  std::vector<IBO> m_ibos;
  NamedTextureImages m_images;
  std::vector<SimpleMaterial> m_materials;
  void loadImage(std::string texture_filename);
  static unsigned char white[4];
  static unsigned char bluish[4];
  static std::string defaultDiffuseName;
  static std::string defaultNormalName;
};

#endif // !defined(__GLITTER_OBJLOADER_H__)
