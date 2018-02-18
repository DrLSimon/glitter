#version 410

struct Geometry {
  vec4 position;  ///< homogeneous position in world space
  vec3 normal;    ///< normal in world space
  vec3 tangent;   ///< tangent in world space
  vec3 bitangent; ///< bitangent (normal cross tangent)
};

// Fragment attributes
in Geometry geomInWorld; ///< All geometric attributes (in world space).
in vec2 uv;              ///< uv coordinates

// Directional light struct
struct DirLight {
  vec3 direction;
  vec3 intensity;
};

uniform DirLight lightsInWorld[3];  ///< lights in world space
uniform vec3 positionCameraInWorld; ///< camera center in worldSpace

// Material properties uniforms
struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;

  // Diffuse and normal maps
  sampler2D colormap;
  sampler2D normalmap;
  sampler2D specularmap;
};

uniform Material material;
uniform bool displayNormals;

// output color
out vec4 fragColor;

/**
 * @brief computes the diffuse contribution of a light source
 * @param light a directional light source
 * @param normal the object normal in the same coordinates as the light
 * @param diffuse the diffuse albedo of the material
 * @return the Lambertian contribution
 *
 * @note PA5 (part 2)
 */
vec3 computeLightLambert(const in DirLight light, const in vec3 normal, const in vec3 diffuse)
{
  return vec3(0);
}

/**
 * @brief computes the specular (Phong or Blinn-Phong) contribution of a light source
 * @param light a directional light source
 * @param normal the object normal in the same coordinates as the light
 * @param directionToCamera the direction from the fragment to the camera center
 * @param specular the specular albedo of the material
 * @param shininess the shininess of the material
 * @return the specular contribution
 *
 * @note PA5 (part 2): Here you should implement one specular illumination model (you may choose freely between Phong and Blinn-Phong model).
 */
vec3 computeLightSpecular(const in DirLight light, const in vec3 normal, const in vec3 directionToCamera, const in vec3 specular, const in float shininess)
{
  return vec3(0);
}

/**
 * @brief computes the diffuse contribution of a light source
 * @param macroNormal the macroscopic object normal
 * @param macroTangent the macroscopic object tangent
 * @param macroBitangent the macroscopic object bitangent
 *
 * @return the "microscopic" object normal
 *
 * @note PA5 (part 3): you must use the normal map to disturb the input
 * macroscopic normal and get the microscopic one.
 */
vec3 computeMicroNormal(const in vec3 macroNormal, const in vec3 macroTangent, const in vec3 macroBitangent)
{
  vec3 microNormal = macroNormal;
  return microNormal;
}

vec4 normal2Color(vec3 n)
{
  return vec4(0.5 * (n + 1), 1);
}

void main()
{
  vec3 microNormal = computeMicroNormal(geomInWorld.normal, geomInWorld.tangent, geomInWorld.bitangent);
  if (displayNormals) {
    fragColor = normal2Color(microNormal);
    return;
  }

  vec3 diffuse = material.diffuse * texture(material.colormap, uv).rgb;
  vec3 specular = material.specular * texture(material.specularmap, uv).rgb;
  vec3 lambert = vec3(0);
  vec3 phong = vec3(0);
  vec3 directionToCamera = normalize(positionCameraInWorld - geomInWorld.position.xyz / geomInWorld.position.w);
  for (int k = 0; k < 3; k++) {
    lambert += computeLightLambert(lightsInWorld[k], microNormal, diffuse);
    phong += computeLightSpecular(lightsInWorld[k], microNormal, directionToCamera, specular, material.shininess);
  }
  fragColor = vec4(material.ambient + lambert + phong, 1);
}
