#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define TINYOBJLOADER_IMPLEMENTATION

#include "ObjLoader.hpp"
#include "Serialize.hpp"
#include "utils.hpp"

static glm::vec3 calcNormal(const glm::vec3 & v0, const glm::vec3 & v1, const glm::vec3 & v2)
{
  glm::vec3 v10 = v1 - v0;
  glm::vec3 v20 = v2 - v0;

  glm::vec3 n = glm::cross(v10, v20);
  return glm::normalize(n);
}

std::string ObjLoader::defaultDiffuseName = "OBL:default_diffuse";
std::string ObjLoader::defaultNormalName = "OBL:default_normal";
unsigned char ObjLoader::bluish[4] = {128, 128, 255, 255};
unsigned char ObjLoader::white[4] = {255, 255, 255, 255};

ObjLoader::ObjLoader(const std::string & filename)
{
  std::string absolutepath = absolutename(filename);
  m_rootDir = basename(absolutepath);
  m_images.add(defaultDiffuseName, Image<>(white, 1, 1, 4));
  m_images.add(defaultNormalName, Image<>(bluish, 1, 1, 4));
  if (endsWith(absolutepath, ".glitter")) {
    loadBinaryFile(absolutepath);
  } else {
    parseFile(absolutepath);
  }
}

const std::vector<SimpleMaterial> & ObjLoader::materials() const
{
  return m_materials;
}

Image<> ObjLoader::image(const std::string & name) const
{
  return m_images[name];
}

size_t ObjLoader::nbIBOs() const
{
  return m_ibos.size();
}

const std::vector<glm::vec3> & ObjLoader::vertexPositions() const
{
  return m_vertexPositions;
}

const std::vector<glm::vec4> & ObjLoader::vertexColors() const
{
  return m_vertexColors;
}

const std::vector<glm::vec2> & ObjLoader::vertexUVs() const
{
  return m_vertexUVs;
}

const std::vector<glm::vec3> & ObjLoader::vertexNormals() const
{
  return m_vertexNormals;
}

const std::vector<glm::vec3> & ObjLoader::vertexTangents() const
{
  return m_vertexTangents;
}

const std::vector<unsigned int> & ObjLoader::ibo(unsigned int materialIndex) const
{
  return m_ibos[materialIndex];
}

void ObjLoader::loadImage(std::string texture_filename)
{
  std::string key = texture_filename;
  if (texture_filename.length() > 0) {
    // Only load the texture if it is not already loaded
    if (not m_images.find(key)) {
      texture_filename = m_rootDir + texture_filename;
      if (!fileExists(texture_filename)) {
        std::cerr << "Unable to find file: " << texture_filename << std::endl;
        exit(1);
      }

      Image<> image;
      image.depth = 1;
      image.data = stbi_load(texture_filename.c_str(), &image.width, &image.height, &image.channels, STBI_default);
      if (!image.data) {
        std::cerr << "Unable to load texture: " << texture_filename << std::endl;
        exit(1);
      }
      m_images.add(key, image);
    }
  }
}

void ObjLoader::parseFile(const std::string & filename)
{
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string err;
  bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str(), m_rootDir.c_str());
  if (!err.empty()) {
    std::cerr << err << std::endl;
  }
  if (!ret) {
    exit(1);
  }

  // Loop over materials
  tinyobj::material_t defaultMaterial;
  defaultMaterial.ambient[0] = defaultMaterial.ambient[1] = defaultMaterial.ambient[2] = 0.1;
  defaultMaterial.diffuse[0] = defaultMaterial.diffuse[1] = defaultMaterial.diffuse[2] = 0.3;
  defaultMaterial.specular[0] = defaultMaterial.specular[1] = defaultMaterial.specular[2] = 0.2;
  defaultMaterial.shininess = 1;
  defaultMaterial.name = "default_material";
  materials.push_back(defaultMaterial);
  for (size_t m = 0; m < materials.size(); m++) {
    tinyobj::material_t * mp = &materials[m];
    SimpleMaterial material;
    material.name = mp->name;
    material.ambient = *reinterpret_cast<glm::vec3 *>(mp->ambient);
    material.diffuse = *reinterpret_cast<glm::vec3 *>(mp->diffuse);
    material.specular = *reinterpret_cast<glm::vec3 *>(mp->specular);
    material.shininess = mp->shininess;
    material.diffuseTexName = (mp->diffuse_texname != "") ? mp->diffuse_texname : defaultDiffuseName;
    material.normalTexName = (mp->normal_texname != "") ? mp->normal_texname : defaultNormalName;
    material.specularTexName = (mp->normal_texname != "") ? mp->specular_texname : defaultDiffuseName;
    loadImage(mp->diffuse_texname);
    loadImage(mp->normal_texname);
    loadImage(mp->specular_texname);
    m_materials.push_back(material);
  }

  m_ibos.resize(m_materials.size());
  // Loop over shapes
  for (size_t s = 0; s < shapes.size(); s++) {
    // Loop over faces(polygon)
    for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
      int current_material_id = shapes[s].mesh.material_ids[f];
      if ((current_material_id < 0) || (current_material_id >= static_cast<int>(materials.size()))) {
        // Invalid material ID. Use default material.
        current_material_id = materials.size() - 1; // Default material is added to the last item in `materials`.
      }

      // Loop over vertices in the face.
      for (size_t v = 0; v < 3; v++) {
        // access to vertex
        tinyobj::index_t idx = shapes[s].mesh.indices[3 * f + v];
        tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
        tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
        tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
        m_vertexPositions.push_back(glm::vec3(vx, vy, vz));

        if (attrib.normals.size() > 0) {
          tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
          tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
          tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];
          m_vertexNormals.push_back(-glm::vec3(nx, ny, nz));
        }
        if (attrib.texcoords.size() > 0) {
          tinyobj::real_t tx = attrib.texcoords[2 * idx.texcoord_index + 0];
          tinyobj::real_t ty = attrib.texcoords[2 * idx.texcoord_index + 1];
          m_vertexUVs.push_back(glm::vec2(tx, ty));
        } else {
          m_vertexUVs.push_back({0, 0});
        }
        //! note: beware attrib.colors is not empty even if no colors were specified (it is filled with black colors)
        tinyobj::real_t cR = attrib.colors[4 * idx.vertex_index + 0];
        tinyobj::real_t cG = attrib.colors[4 * idx.vertex_index + 1];
        tinyobj::real_t cB = attrib.colors[4 * idx.vertex_index + 2];
        tinyobj::real_t cA = attrib.colors[4 * idx.vertex_index + 3];
        m_vertexColors.push_back(glm::vec4(cR, cG, cB, cA));
        m_ibos[current_material_id].push_back(m_vertexPositions.size() - 1);
      }

      // Compute the geometric normal if not specified.
      if (attrib.normals.size() == 0) {
        size_t nbVertices = m_vertexPositions.size();
        const glm::vec3 & v0 = m_vertexPositions[nbVertices - 3];
        const glm::vec3 & v1 = m_vertexPositions[nbVertices - 2];
        const glm::vec3 & v2 = m_vertexPositions[nbVertices - 1];
        glm::vec3 normal = calcNormal(v0, v1, v2);
        m_vertexNormals.push_back(normal);
        m_vertexNormals.push_back(normal);
        m_vertexNormals.push_back(normal);
      }
    }
  }
  computeTangents();
  cleanUpDuplicates();
}

void ObjLoader::saveBinaryFile(const std::string & filename) const
{
#define GLITTER_BINFILE_MAGIC "GLITTER_BIN_OBJ\n"
  std::ofstream file(filename.c_str());
  file.write(GLITTER_BINFILE_MAGIC, strlen(GLITTER_BINFILE_MAGIC));

  write(std::string("[VertexPositions]"), file);
  write(m_vertexPositions, file);
  write(std::string("[VertexColors]"), file);
  write(m_vertexColors, file);
  write(std::string("[VertexUVs]"), file);
  write(m_vertexUVs, file);
  write(std::string("[VertexNormals]"), file);
  write(m_vertexNormals, file);
  write(std::string("[VertexTangents]"), file);
  write(m_vertexTangents, file);

  write(std::string("[IBOS]"), file);
  // IBOs
  std::uint64_t count = m_ibos.size();
  write(count, file);
  for (const IBO & ibo : m_ibos) {
    write(ibo, file);
  }

  write(std::string("[NamedTextureImages]"), file);
  // NamedTextureImages m_images;
  std::vector<std::string> textureImageNames = m_images.names();
  count = textureImageNames.size();
  write(count, file);
  for (const std::string & name : textureImageNames) {
    write(std::string("_Image_"), file);
    write(name, file);
    Image<> image = m_images[name];
    glm::int32 w = image.width;
    glm::int32 h = image.height;
    glm::int32 d = image.depth;
    glm::int32 c = image.channels;
    write(w, file);
    write(h, file);
    write(d, file);
    write(c, file);
    file.write(reinterpret_cast<const char *>(image.data), w * h * d * c);
  }

  write(std::string("[SimpleMaterials]"), file);
  // std::vector<SimpleMaterial> m_materials;
  count = m_materials.size();
  write(count, file);
  for (const SimpleMaterial & material : m_materials) {
    write(std::string("_Material_"), file);
    write(material.name, file);
    write(material.ambient, file);
    write(material.diffuse, file);
    write(material.specular, file);
    write(material.shininess, file);
    write(material.diffuseTexName, file);
    write(material.normalTexName, file);
    write(material.specularTexName, file);
  }
}

void ObjLoader::loadBinaryFile(const std::string & filename)
{
  std::ifstream file(filename.c_str());
  char magicBuffer[255];
  memset(magicBuffer, 0, 255);
  file.read(magicBuffer, strlen(GLITTER_BINFILE_MAGIC));
  assert(!strcmp(magicBuffer, GLITTER_BINFILE_MAGIC) && "ObjLoader::loadBinaryFile(): Wrong file magic number");

  std::string magic;

  read(magic, file);
  assert((magic == "[VertexPositions]") && "ObjLoader::loadBinaryFile(): Tag not found");
  read(m_vertexPositions, file);

  read(magic, file);
  assert((magic == "[VertexColors]") && "ObjLoader::loadBinaryFile(): Tag not found");
  read(m_vertexColors, file);

  read(magic, file);
  assert((magic == "[VertexUVs]") && "ObjLoader::loadBinaryFile(): Tag not found");
  read(m_vertexUVs, file);

  read(magic, file);
  assert((magic == "[VertexNormals]") && "ObjLoader::loadBinaryFile(): Tag not found");
  read(m_vertexNormals, file);

  read(magic, file);
  assert((magic == "[VertexTangents]") && "ObjLoader::loadBinaryFile(): Tag not found");
  read(m_vertexTangents, file);

  read(magic, file);
  assert((magic == "[IBOS]") && "ObjLoader::loadBinaryFile(): Tag not found");
  // IBOs
  std::uint64_t count;
  read(count, file);
  m_ibos.resize(count);
  for (IBO & ibo : m_ibos) {
    read(ibo, file);
  }

  read(magic, file);
  assert((magic == "[NamedTextureImages]") && "ObjLoader::loadBinaryFile(): Tag not found");
  // NamedTextureImages m_images;
  read(count, file);
  while (count--) {
    read(magic, file);
    assert((magic == "_Image_") && "ObjLoader::loadBinaryFile(): Tag not found");
    std::string name;
    read(name, file);
    Image<> image;
    glm::int32 value;
    read(value, file);
    image.width = value;
    read(value, file);
    image.height = value;
    read(value, file);
    image.depth = value;
    read(value, file);
    image.channels = value;
    size_t dataSize = image.width * image.height * image.depth * image.channels;
    image.data = new Image<>::value_type[dataSize];
    file.read(reinterpret_cast<char *>(image.data), dataSize * sizeof(Image<>::value_type));
    m_images.add(name, image);
  }

  read(magic, file);
  assert((magic == "[SimpleMaterials]") && "ObjLoader::loadBinaryFile(): Tag not found");
  // std::vector<SimpleMaterial> m_materials;
  read(count, file);
  m_materials.resize(count);
  for (SimpleMaterial & material : m_materials) {
    read(magic, file);
    assert((magic == "_Material_") && "ObjLoader::loadBinaryFile(): Tag not found");
    read(material.name, file);
    read(material.ambient, file);
    read(material.diffuse, file);
    read(material.specular, file);
    read(material.shininess, file);
    read(material.diffuseTexName, file);
    read(material.normalTexName, file);
    read(material.specularTexName, file);
  }
}

void ObjLoader::computeTangents()
{
  //! note: in barycentric form the tangent is parameterized as:
  //! t = x0 + t1(x1-x0) + t2(x2-x0)
  //! then to get t1 and t2, t is characterized by:
  //! u(x0+t) = u(x_0)+1 and bilinear interp says that u(x0+t) = u(x_0) + t1\delta u1 + t2\delta u2
  //! v(x0+t) = v(x_0) and bilinear interp says that v(x0+t) = v(x_0) + t1\delta v1 + t2\delta v2
  //! Solving this 2x2 linear system with Cramer's rule gives:
  //! t1 = det([1, delta u2; 0, delta v2]) /  det([delta u1, delta u2; delta v1, delta v2])
  //! t2 = det([delta u1, 1; delta v1, 0]) /  det([delta u1, delta u2; delta v1, delta v2])
  glm::vec3 tangent;
  for (unsigned int i = 0; i < m_vertexPositions.size(); i += 3) {
    glm::vec2 & uv0 = m_vertexUVs[i + 0];
    glm::vec2 & uv1 = m_vertexUVs[i + 1];
    glm::vec2 & uv2 = m_vertexUVs[i + 2];
    // UV delta
    glm::vec2 deltaUV1 = uv1 - uv0;
    glm::vec2 deltaUV2 = uv2 - uv0;
    float detDenom = deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x;
    if (detDenom != 0) {
      // Shortcuts for m_vertexPositions
      glm::vec3 & x0 = m_vertexPositions[i + 0];
      glm::vec3 & x1 = m_vertexPositions[i + 1];
      glm::vec3 & x2 = m_vertexPositions[i + 2];
      // Edges of the triangle : postion delta
      glm::vec3 deltaPos1 = x1 - x0;
      glm::vec3 deltaPos2 = x2 - x0;
      float r = 1.0f / detDenom;
      tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
    }
    // Set the same tangent for all three vertices of the triangle.
    m_vertexTangents.push_back(tangent);
    m_vertexTangents.push_back(tangent);
    m_vertexTangents.push_back(tangent);
  }

  for (unsigned int i = 0; i < m_vertexPositions.size(); i += 1) {
    glm::vec3 & n = m_vertexNormals[i];
    glm::vec3 & t = m_vertexTangents[i];
    // Gram-Schmidt orthogonalize
    t = glm::normalize(t - n * glm::dot(n, t));
  }
}

struct PackedVertexPNTCUV {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec3 tangent;
  glm::vec4 color;
  glm::vec2 uv;

  PackedVertexPNTCUV(const glm::vec3 & position = glm::vec3(), const glm::vec3 & normal = glm::vec3(), const glm::vec3 & tangent = glm::vec3(), const glm::vec4 & color = glm::vec4(1),
                     const glm::vec2 & uv = glm::vec2())
      : position(position), normal(normal), tangent(tangent), color(color), uv(uv)
  {
  }
  bool operator==(const PackedVertexPNTCUV & other) const
  {
    return is_near(position, other.position) and is_near(normal, other.normal) and is_near(tangent, other.tangent) and is_near(uv, other.uv) and is_near(color, other.color);
  }

  static bool is_near(float v1, float v2, float epsilon = 1e-2) { return fabs(v1 - v2) < epsilon; }

  static bool is_near(const glm::vec2 & vecA, const glm::vec2 & vecB) { return is_near(vecA[0], vecB[0]) and is_near(vecA[1], vecB[1]); }

  static bool is_near(const glm::vec3 & vecA, const glm::vec3 & vecB) { return is_near(vecA[0], vecB[0]) and is_near(vecA[1], vecB[1]) and is_near(vecA[2], vecB[2]); }

  static bool is_near(const glm::vec4 & vecA, const glm::vec4 & vecB)
  {
    const float epsilon = 1 / 256.f;
    return is_near(vecA[0], vecB[0], epsilon) and is_near(vecA[1], vecB[1], epsilon) and is_near(vecA[2], vecB[2], epsilon) and is_near(vecA[3], vecB[3], epsilon);
  }
};

// Template specialization of std::hash
namespace std
{
template <> struct hash<PackedVertexPNTCUV> {
public:
  std::size_t operator()(const PackedVertexPNTCUV & v) const
  {
    std::size_t hashes[] = {std::hash<float>()(v.position.x), std::hash<float>()(v.position.y), std::hash<float>()(v.position.z), std::hash<float>()(v.normal.x),  std::hash<float>()(v.normal.y),
                            std::hash<float>()(v.normal.z),   std::hash<float>()(v.tangent.x),  std::hash<float>()(v.tangent.y),  std::hash<float>()(v.tangent.z), std::hash<float>()(v.color.x),
                            std::hash<float>()(v.color.y),    std::hash<float>()(v.color.z),    std::hash<float>()(v.color.w),    std::hash<float>()(v.uv.x),      std::hash<float>()(v.uv.y)};
    std::size_t seed = 0;
    for (std::size_t h : hashes) {
      // from boost::hash_combine
      seed ^= h + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
  }
};
} // namespace std

void ObjLoader::cleanUpDuplicates()
{
  std::vector<unsigned int> cleanIBO;
  std::vector<glm::vec3> cleanPositions;
  std::vector<glm::vec3> cleanNormals;
  std::vector<glm::vec3> cleanTangents;
  std::vector<glm::vec4> cleanColors;
  std::vector<glm::vec2> cleanUVs;
  std::unordered_map<PackedVertexPNTCUV, size_t> uniqueVertexIndices;
  std::vector<size_t> vertexNewIndices;
  for (size_t k = 0; k < m_vertexPositions.size(); k++) {
    const glm::vec3 & position = m_vertexPositions[k];
    const glm::vec3 & normal = m_vertexNormals[k];
    const glm::vec3 & tangent = m_vertexTangents[k];
    const glm::vec4 & color = m_vertexColors[k];
    const glm::vec2 & uv = m_vertexUVs[k];
    PackedVertexPNTCUV vertex(position, normal, tangent, color, uv);
    auto uniqueIndexIterator = uniqueVertexIndices.find(vertex);
    bool found = (uniqueIndexIterator != uniqueVertexIndices.end());
    if (found) {
      size_t index = uniqueIndexIterator->second;
      vertexNewIndices.push_back(index);
    } else {
      cleanPositions.push_back(position);
      cleanNormals.push_back(normal);
      cleanTangents.push_back(tangent);
      cleanColors.push_back(color);
      cleanUVs.push_back(uv);
      size_t index = cleanPositions.size() - 1;
      uniqueVertexIndices[vertex] = index;
      vertexNewIndices.push_back(index);
    }
  }
  for (auto & ibo : m_ibos) {
    std::vector<unsigned int> cleanIBO;
    for (unsigned int oldIndex : ibo) {
      cleanIBO.push_back(vertexNewIndices[oldIndex]);
    }
    ibo = cleanIBO;
  }
  m_vertexPositions = cleanPositions;
  m_vertexNormals = cleanNormals;
  m_vertexTangents = cleanTangents;
  m_vertexColors = cleanColors;
  m_vertexUVs = cleanUVs;
}

bool ObjLoader::NamedTextureImages::find(const std::string & name) const
{
  return m_images.find(name) != m_images.end();
}

void ObjLoader::NamedTextureImages::add(const std::string & name, const Image<> & image)
{
  m_images[name] = image;
}

const Image<> & ObjLoader::NamedTextureImages::operator[](const std::string & name) const
{
  static Image<> defaultImage(white, 1, 1, 4);
  auto searchRes = m_images.find(name);
  if (searchRes != m_images.end()) {
    return searchRes->second;
  }
  return defaultImage;
}

ObjLoader::NamedTextureImages::~NamedTextureImages()
{
  for (auto namedImage : m_images) {
    if (namedImage.first == defaultDiffuseName or namedImage.first == defaultNormalName) {
      continue;
    }
    unsigned char * imgData = namedImage.second.data;
    stbi_image_free(imgData);
  }
}

std::vector<std::string> ObjLoader::NamedTextureImages::names() const
{
  std::vector<std::string> names;
  std::unordered_map<std::string, Image<>>::const_iterator it = m_images.cbegin();
  while (it != m_images.end()) {
    names.push_back(it->first);
    ++it;
  }
  return names;
}
