#version 410
// Fragment attributes
in vec4 position;
in vec3 normal;
in vec3 tangent;
in vec3 bitangent;
in vec2 uv ;

// Directional light uniforms
struct DirLight {
  vec3 lightdirn;
  vec3 lightcolor;
};

uniform DirLight light0;
uniform DirLight light1;
uniform DirLight light2;

// Material properties uniforms
struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;

  // Diffuse and normal maps
  sampler2D colormap;
  sampler2D normalmap;
};

uniform Material material;

// output color
out vec4 fragColor ; 

/**
* @brief computes the diffuse contribution of a ligth source
* @param light a directional light source
* @param normal the object normal in eye coordinates
* @param mydiffuse the diffuse albedo of the material
* @return the Lambertian contribution
*
* @note PA5
*/
vec3 ComputeLightLambert(const in DirLight light, const in vec3 normal, const in vec3 mydiffuse)
{
    vec3 lambert = vec3(1.f);
    return lambert;
}

/**
* @brief computes the specular (Phong) contribution of a ligth source
* @param light a directional light source
* @param normal the object normal in eye coordinates
* @param eyedirn the direction from the fragment to the camera center
* @param myspecular the specular albedo of the material
* @param myshininess the shininess of the material
* @return the Phong contribution
*
* @note PA5
*/
vec3 ComputeLightSpecular (const in DirLight light, const in vec3 normal, const in vec3 eyedirn, const in vec3 myspecular, const in float myshininess)
{
    vec3 phong = vec3(0.f);
    return phong;
}

/**
* @brief computes the diffuse contribution of a ligth source
* @param macroNormal the macroscopic object normal
* @return the "microscopic" object normal
*
* @note PA5 : you must use the normal map to disturb the input
* macroscopic normal and get the microscopic one.
*/
vec3 ComputeNormal(const in vec3 macroNormal)
{
    vec3 microNormal = macroNormal;
    return microNormal;
}

void main()
{
  vec3 microNormal = ComputeNormal(normal);

  vec3 lambert = ComputeLightLambert(light0, microNormal, material.diffuse) +
                  ComputeLightLambert(light1, microNormal, material.diffuse) +
                  ComputeLightLambert(light2, microNormal, material.diffuse);

  const vec3 eyepos = vec3(0,0,0);
  vec3 eyedirn = normalize(eyepos - position.xyz/position.w);// don't forget to dehomogenize the vertex
  vec3 phong= ComputeLightSpecular(light0, microNormal, eyedirn, material.specular, material.shininess) +
                  ComputeLightSpecular(light1, microNormal, eyedirn, material.specular, material.shininess) +
                  ComputeLightSpecular(light2, microNormal, eyedirn, material.specular, material.shininess);

  fragColor= vec4(material.ambient*texture(material.colormap, uv).rgb + lambert*texture(material.colormap, uv).rgb + phong, 1);
}
