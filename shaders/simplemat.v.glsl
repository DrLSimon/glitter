#version 410

// ins (vertex input attributes)
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec3 vertexTangent;

// uniforms
uniform mat4 M; ///< model world matrix
uniform mat4 V; ///< world view matrix
uniform mat4 P; ///< projection matrix

struct Geometry {
  vec4 position;  ///< homogeneous position in world space
  vec3 normal;    ///< normal in world space
  vec3 tangent;   ///< tangent in world space
  vec3 bitangent; ///< bitangent (normal cross tangent)
};

// out (vertex output attributes)
out Geometry geomInWorld; ///< All geometric attributes (in world space).
out vec2 uv;              ///< uv coordinates

/**
 * @brief computes the normal in world space
 * @param modelWorld the transform between the object and the world
 * @param normalInObject the normal in object space
 * @return the normal in world coordinates
 *
 * @note PA5 (part 1): Here, you should compute the so-called normal matrix, and apply it to the input normal. Do not forget to normalize the resulting normal (since scale can be changed).
 */
vec3 transformNormal(const in mat4 modelWorld, const in vec3 normalInObject)
{
  return normalInObject;
}

void main()
{
  geomInWorld.position = M * vec4(vertexPosition, 1);
  gl_Position = P * V * geomInWorld.position;
  geomInWorld.normal = transformNormal(M, vertexNormal);
  geomInWorld.tangent = normalize(mat3(M) * vertexTangent);
  geomInWorld.bitangent = cross(geomInWorld.normal, geomInWorld.tangent);
  uv = vertexUV;
}
