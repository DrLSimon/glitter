#ifndef __GLAPI__HPP
#define __GLAPI__HPP
#include <GL/glew.h>
#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
typedef GLuint uint;

#include "AttributeProperties.hpp"
#include "Image.hpp"

/**
 * @brief Tiny abstraction for OpenGL objects that can be bound to
 * the current openGL state (like VBOs, VAOs, Programs, Textures, ...)
 *
 * This interface exposes bind / unbind mechanisms.
 */
class OGLStateObject {
public:
  /**
   * @brief binds this instance GPU location to the current state
   */
  virtual void bind() const = 0;

  /**
   * @brief unbinds this instance GPU location to the current state
   */
  virtual void unbind() const = 0;
};

/**
 * @brief Buffer Object class (VBO / IBO) and formatting (nb / type / size of attibutes)
 *
 * Copy constructor and assignment operator are disabled.
 */
class Buffer : public OGLStateObject {
public:
  /**
   * @brief constructs a buffer of a given type
   * @param target the desired type (VBO or IBO)
   *
   * @note PA1: This method  allocates GPU memory for the buffer
   * and sets up the type of Buffer as a VBO or an IBO.
   */
  Buffer(GLenum target = GL_ARRAY_BUFFER);

  Buffer(const Buffer &) = delete;
  Buffer & operator=(const Buffer &) = delete;

  /**
   * @brief destructor
   *
   * @note PA1: At destruction, the GPU memory should be released
   */
  ~Buffer();

  /**
   * @brief binds this Buffer to the current state
   *
   * @note PA1
   */
  void bind() const override;

  /**
   * @brief unbinds this Buffer from the current state
   *
   * @note PA1
   */
  void unbind() const override;

  /**
   * @brief Sends data to the GPU location attached to this instance.
   * @param values the data to be sent
   *
   * @note PA1: This function send the @p values to the GPU location
   * attched to this instance. To do so, you must:
   *   - bind this Buffer
   *   - call ::glBufferData with the correct parameters
   *   - unbind this instance
   * As a side effect this method must store the formatting
   * of the buffer:
   *	- total number of attributes
   *	- number of component per attribute
   *	- type of the attributes
   * The first one can be inferred from @p values and the last two from the template type @a T (see AttributeProperies.hpp)
   */
  template <typename T> void setData(const std::vector<T> & values);

  /**
   * @brief attributeCount
   * @return the number of attributes
   */
  uint attributeCount() const;

  /**
   * @brief attributeType
   * @return return the type of attributes
   */
  GLenum attributeType() const;

  /**
   * @brief attributeSize
   * @return the size (nb of components) of each attribute
   */
  GLenum attributeSize() const;

private:
  uint m_location;        ///< GPU location of the buffer
  GLenum m_target;        ///< Type of buffer (VBO or IBO)
  uint m_attributeCount;  ///< Buffer formatting : number of attributes
  GLenum m_attributeType; ///< Buffer formatting : type of attributes
  uint m_attributeSize;   ///< Buffer formatting : components per attribute
};

/**
 * @brief The VAO class.
 *
 * Encapsulates several VBOs and one IBO.
 * VBOs can be share between instances.
 * Copy constructor and assignment operator are disabled.
 */
class VAO : public OGLStateObject {
public:
  /**
   * @brief Constructor.
   * @param nbVBO number of VBOs.
   *
   * @note PA1: in this method you must allocate GPU location for the VAO and
   * check if the required number of VBOs (@p nbVBO) does not exceed
   * the maximum allowed by the harware (you may use ::glGetIntegerv and
   * GL_MAX_VERTEX_ATTRIBS, or use a default value of 16 to make things
   * simpler).
   */
  VAO(uint nbVBO);

  VAO(const VAO &) = delete;
  VAO & operator=(const VAO &) = delete;

  /**
   * @brief Destructor
   *
   * @note PA1: at destruction all allocated GPU memory must be released
   */
  ~VAO();

  /**
   * @brief binds this VAO to the current state
   *
   * @note PA1
   */
  void bind() const override;

  /**
   * @brief unbinds this VAO from the current state
   *
   * @note PA1
   */
  void unbind() const override;

  /**
   * @brief sets up a given VBO.
   * @param attributeIndex the anchor point of the VBO to set-up
   * @param values the values to be sent to the VBO location.
   *
   * @note PA1 (part 2): this method must do the following operations:
   * 	- check that @p attributeIndex is not out of bounds
   * 	- bind this VAO
   * 	- instantiate a new VBO for this index if not already done
   * 	- set up this VBO with the @p values
   * 	- encapsulate the VBO GPU location in this VAO GPU location using VAO::encapsulateVBO
   * 	- reset the openGL state so that no VAO is left bound
   *
   * @see encapsulateVBO
   */
  template <typename T> void setVBO(uint attributeIndex, const std::vector<T> & values);

  /**
   * @brief sets up the IBO
   * @param values the values to be sent to the IBO location.
   *
   * @note PA1 (part 2): this method must :
   * 	- bind this VAO instance
   * 	- set up the IBO with the @p values
   * 	- update the element buffer binding of this VAO GPU location (to do so you just need to bind the IBO)
   * 	- reset the openGL state so that no VAO / Buffer is left bound
   */
  template <typename T> void setIBO(const std::vector<T> & values);

  /**
   * @brief makes a VAO sharing the same VBOs and with an empty IBO
   * @return the slave VAO
   *
   * @note the implementation of this method is already complete
   */
  std::shared_ptr<VAO> makeSlaveVAO() const;

  /**
   * @brief Make the draw call to render the VAO
   * @param mode primitive type
   *
   * @note PA1 (part 2): in addition to the draw call, make sure to bind and unbind the VAO correctly.
   */
  void draw(GLenum mode = GL_TRIANGLES) const;

private:
  /**
   * @brief encapsulates the VBO in this VAO
   * @param attributeIndex
   * @param vbo
   *
   * @note PA1 (part 2): the VBO is encapsultated at the @p attributeIndex anchor point of this VAO.
   * To do so, the method must:
   * 		- enable the @p attributeIndex anchor point
   * 		- bind the buffer to the OpenGL state
   * 		- store the formatting information for the @p attributeIndex anchor point
   * 		- reset the OpenGL state so that no VBO is left bound
   */
  void encapsulateVBO(unsigned int attributeIndex) const;

private:
  uint m_location;                             ///< GPU location of the VAO
  std::vector<std::shared_ptr<Buffer>> m_vbos; ///< List of the VBOs
  Buffer m_ibo;                                ///< IBO
};

/**
 * @brief Shader class (Vertex / fragment)
 *
 * Shaders are always compiled at construction.
 * Copy constructor and assignment operator are disabled.
 */
class Shader {
public:
  /**
   * @brief Constructor from a filename
   * @param type Vertex or Fragment shader
   * @param filename the name of the source file
   *
   * @note PA1: At construction, the following actions must take place:
   * 	- GPU memory allocation
   * 	- reading the content of the file named @p filename (see utils.hpp ::fileContent)
   * 	- setting the source code of the shader
   *  - compiling the shader
   */
  Shader(GLenum type, const std::string & filename);
  Shader(const Shader &) = delete;
  Shader & operator=(const Shader &) = delete;

  /**
   * @brief Destructor
   *
   * @note PA1: At destruction, GPU memory must be released.
   */
  ~Shader();

  /**
   * @brief location
   * @return the GPU location of this instance
   */
  uint location() const;

private:
  uint m_location; ///< GPU location of the shader
};

/**
 * @brief The Program class.
 *
 * Encapsulates a vertex and a fragment shader.
 * Copy constructor and assignment operator are disabled.
 */
class Program : public OGLStateObject {
public:
  /**
   * @brief Constructs a program from two filenames (vertex & fragment shaders)
   *
   * @param vname filename of the vertex shader
   * @param fname filename of the fragment shader
   *
   * @note PA1: this function must
   * 	- allocate the GPU memory for the program
   * 	- attach the fragment and vertex shaders
   * 	- link the program
   * 	- detach the fragment and vertex shaders (so they can be deleted)
   */
  Program(const std::string & vname, const std::string & fname);

  Program(const Program &) = delete;
  Program & operator=(const Program &) = delete;

  /**
   * @brief Destructor
   *
   * @note PA1: At destruction, GPU memory must be released. Do not
   * forget to dettach the shaders before releasing the program memory.
   */
  ~Program();

  /**
   * @brief binds this Program to the current state
   *
   * @note PA1
   */
  void bind() const override;

  /**
   * @brief unbinds this Program from the current state
   *
   * @note PA1
   */
  void unbind() const override;

  /**
   * @brief assigns the value of a uniform variable of this program
   * @param name the uniform variable name
   * @param val the value to be assign
   *
   * @note The implementation of this method is already complete.
   */
  template <typename T> void setUniform(const std::string & name, const T & val) const;

private:
  /**
   * @brief a template wrapper for glUniform functions
   * @param location the GPU location of the uniform
   * @param value the value to be assigned
   *
   * @note PA2 Specialize this function for all the necessary types (@a T)
   */
  template <typename T> static void uniformDispatcher(int location, const T & value);

  /**
   * @brief Retrieves the location of a uniform variable defined in this program
   * @param name the name of the uniform variable
   * @param location the retrived location
   * @return true if the location is valid and false otherwise
   *
   * @note PA2
   */
  bool getUniformLocation(const std::string & name, int & location) const;

  /**
   * @brief bound
   * @return true if this Program is already bound to the current openGL state
   */
  bool bound() const;

private:
  uint m_location;  ///< GPU location of the program
  Shader m_vshader; ///< Vertex shader
  Shader m_fshader; ///< Fragment shader
};

/**
 * @brief The Texture class.
 *
 * Copy constructor and assignment operator are disabled.
 */
class Texture : public OGLStateObject {
public:
  /**
   * @brief Constructs a Texture of a given type (2D, 3D, ...)
   * @param target the binding target type of the texture
   *
   * @note PA4 (part 1): At construction the GPU memory must be allocated, and the
   * target must be recorded.
   */
  Texture(GLenum target);
  Texture(const Texture &) = delete;
  Texture & operator=(const Texture &) = delete;

  /**
   * @brief Destructor
   *
   * @note PA4 (part 1): At destruction, GPU memory must be released.
   */
  ~Texture();

  /**
   * @brief binds this Texture to the current state
   *
   * @note PA4 (part1)
   */
  void bind() const override;

  /**
   * @brief unbinds this Texture from the current state
   *
   * @note PA4 (part 1)
   */
  void unbind() const override;

  /**
   * @brief Sends data to the GPU location attached to this instance.
   * @param image the data to be sent
   * @param mipmaps toggles mipmap generation
   *
   * @note PA4 (part 1): This method shall be specialized for @a T = @a GLubyte only. It should bind this texture, and send the data to it and then unbind the texture.
   * You should take care of calling the correct ::glTexImage function depending on
   * the texture target. You should at least handle GL_TEXTURE_2D and GL_TEXTURE_3D.
   *
   *
   * @note PA4 (part 2): You should generate mipmaps if they are toggled by the @p mipmaps argument
   */
  template <typename T> void setData(const Image<T> & image, bool mipmaps = false) const;

private:
  uint m_location; ///< GPU location of the texture
  GLenum m_target; ///< Texture target type (e.g. GL_TEXTURE_2D)
};

/**
 * @brief The Sampler class
 *
 * A sampler is associated with a texture unit once for all.
 * It describes the texture sampling parameters (filters, warping, ...).
 * It can be attached to a program, and a texture can be attached to it.
 */
class Sampler : public OGLStateObject {
public:
  enum BindOption
  {
    DoNotBind,
    BindUnbind
  };

  /**
   * @brief Sampler constructor
   * @param texUnit Texture unit number (0..15)
   *
   * @note PA4 (part 3): At construction GPU memory shall be allocated and
   * the texture unit shall be recorded.
   */
  Sampler(int texUnit);
  Sampler(const Sampler &) = delete;
  Sampler & operator=(const Sampler &) = delete;

  /**
   * @brief Destructor
   *
   * @note PA4 (part 3): At destruction, GPU memory must be released.
   */
  ~Sampler();

  /**
   * @brief binds this Sampler to the current state
   *
   * @note PA4 (part 3): bind this sampler onto its texture unit
   */
  void bind() const override;

  /**
   * @brief unbinds this Sampler from its texture unit
   *
   * @note PA4 (part 3)
   */
  void unbind() const override;

  /**
   * @brief attaches this sampler to a program
   * @param prog the target program
   * @param samplerName the name of the sampler variable
   * @param bindOption specifies if the program needs to be bound and unbound.
   *
   * @note PA4 (part 3): this method makes the link between this Sampler and a uniform variable
   * of the @p program. Beware that the uniform variable, should be assigned the value
   * of the texture unit, not the sampler location.
   */
  void attachToProgram(const Program & prog, const std::string & samplerName, BindOption bindOption) const;

  /**
   * @brief attaches a texture to this sampler (more precisely to its texture unit)
   * @param texture the target texture
   *
   * @note PA4 (part 3): this method must activate this Sampler texture unit, and bind the Texture given
   * in parameter
   */
  void attachTexture(const Texture & texture) const;

  /**
   * @brief sets a Sampler parameter
   * @param paramName the target parameter
   * @param value the desired affectation
   *
   * @note PA4 (part 3): this method must be specialized for @a float and @a int types
   */
  template <typename T> void setParameter(GLenum paramName, const T & value) const;

  /**
   * @brief sets up this Sampler unit to perform anistropic filtering
   *
   * @note PA4 (part 3)
   */
  void enableAnisotropicFiltering() const;

private:
  uint m_location; ///< GPU location of the sampler
  int m_texUnit;   ///< texture unit
};

/*
 * Definition of method templates
 */
template <typename T> void Buffer::setData(const std::vector<T> & values)
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must complete the implementation (look at the documentation in the header)" << std::endl;
  assert(false);
}

template <typename T> void VAO::setVBO(uint attributeIndex, const std::vector<T> & values)
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must complete the implementation (look at the documentation in the header)" << std::endl;
  assert(false);
}

template <typename T> void VAO::setIBO(const std::vector<T> & values)
{
  std::cerr << __PRETTY_FUNCTION__ << ": You must complete the implementation (look at the documentation in the header)" << std::endl;
  assert(false);
}

template <typename T> void Program::setUniform(const std::string & name, const T & val) const
{
  int location;
  if (getUniformLocation(name, location) and bound()) {
    uniformDispatcher<T>(location, val);
  } else if (not bound()) {
    std::cerr << "===== Program is not attached (for uniform '" << name << "')\n";
  } else {
    std::cerr << "=====" << name << " uniform was queried but does not exist\n";
  }
}

#endif /* end of include guard: __GLAPI__HPP */
