/** @file */
#ifndef __GLITTER_SERIALIZE_H__
#define __GLITTER_SERIALIZE_H__

#ifndef RESOURCE_DIR
#define RESOURCE_DIR "."
#endif

#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include "glm/glm.hpp"

/// @brief swap bytes of a 2-bytes integer
void swapEndianness(glm::int16 &);

/// @brief swap bytes of a 4-bytes integer
void swapEndianness(glm::int32 &);

/// @brief swap bytes of a 4-bytes integer
void swapEndianness(glm::uint32 &);

/// @brief swap bytes of an 8-bytes unsigned integer
void swapEndianness(std::uint64_t &);

/// @brief swap bytes of a 4-bytes float
void swapEndianness(glm::float32 &);

/// @brief swap bytes of a vector of floats
void swapEndianness(glm::vec2 &);

/// @brief swap bytes of a vector of floats
void swapEndianness(glm::vec3 &);

/// @brief swap bytes of a vector of floats
void swapEndianness(glm::vec4 &);

template <typename T> struct SerializationTraits {
  static const bool IsSerializable = false;
  static const bool IsEndiannessDependent = true;
  static const char * VectorTag() { return "VOID"; }
};

template <> struct SerializationTraits<glm::int16> {
  static const bool IsSerializable = true;
  static const bool IsEndiannessDependent = true;
  static const char * VectorTag() { return "VI16"; }
};

template <> struct SerializationTraits<glm::int32> {
  static const bool IsSerializable = true;
  static const bool IsEndiannessDependent = true;
  static const char * VectorTag() { return "VI32"; }
};

template <> struct SerializationTraits<glm::uint32> {
  static const bool IsSerializable = true;
  static const bool IsEndiannessDependent = true;
  static const char * VectorTag() { return "VU32"; }
};

template <> struct SerializationTraits<glm::float32> {
  static const bool IsSerializable = true;
  static const bool IsEndiannessDependent = true;
  static const char * VectorTag() { return "VF32"; }
};

template <> struct SerializationTraits<glm::vec2> {
  static const bool IsSerializable = true;
  static const bool IsEndiannessDependent = true;
  static const char * VectorTag() { return "VV2F"; }
};

template <> struct SerializationTraits<glm::vec3> {
  static const bool IsSerializable = true;
  static const bool IsEndiannessDependent = true;
  static const char * VectorTag() { return "VV3F"; }
};

template <> struct SerializationTraits<glm::vec4> {
  static const bool IsSerializable = true;
  static const bool IsEndiannessDependent = true;
  static const char * VectorTag() { return "VV4F"; }
};

template <> struct SerializationTraits<std::string> {
  static const bool IsSerializable = true;
  static const bool IsEndiannessDependent = false;
  static const char * VectorTag() { return "VSTR"; }
};

template <> struct SerializationTraits<std::uint64_t> {
  static const bool IsSerializable = true;
  static const bool IsEndiannessDependent = true;
  static const char * VectorTag() { return "VU64"; }
};

template <typename T> void write(const std::vector<T> & v, std::ostream & stream)
{
  static_assert(SerializationTraits<T>::IsSerializable, "write(): Vector element type is not serializable");

  // Write TAG(4B), SIZE(8B), VALUES...
  stream.write(SerializationTraits<T>::VectorTag(), 4);
  std::uint64_t size = v.size();
  swapEndianness(size);
  stream.write(reinterpret_cast<char *>(&size), sizeof(size));

  if (SerializationTraits<T>::IsEndiannessDependent) {
#ifdef IS_BIG_ENDIAN
    std::vector<T> duplicate = v;
    for (T & value : duplicate) {
      swapEndianness(value);
    }
    stream.write(duplicate.data(), duplicate.size() * sizeof(T));
#else
    stream.write(reinterpret_cast<const char *>(v.data()), v.size() * sizeof(T));
#endif
  } else {
    stream.write(reinterpret_cast<const char *>(v.data()), v.size() * sizeof(T));
  }
}

void write(const std::vector<std::string> & v, std::ostream & stream);

template <typename T> void read(std::vector<T> & v, std::istream & stream)
{
  static_assert(SerializationTraits<T>::IsSerializable, "read(): Vector element type is not serializable");

  // Check TAG
  char tag[5] = {0, 0, 0, 0, 0};
  assert(stream.read(tag, 4) && "Cannot read input file");
  assert(!strcmp(tag, SerializationTraits<T>::VectorTag()) && "read(): Missing vector tag");

  std::uint64_t size;
  stream.read(reinterpret_cast<char *>(&size), sizeof(size));
  swapEndianness(size);

  v.clear();
  v.resize(size);
  assert(stream.read(reinterpret_cast<char *>(v.data()), v.size() * sizeof(T)) && "Cannot read vector data from file");

  if (SerializationTraits<T>::IsEndiannessDependent) {
#ifdef IS_BIG_ENDIAN
    for (T & value : v) {
      swapEndianness(value);
    }
#endif
  }
}

void read(std::vector<std::string> & v, std::istream & stream);

void writeStringLength(uint64_t value, std::ostream & stream);

std::uint64_t readStringLength(std::istream & stream);

template <typename T> inline void write(const T & v, std::ostream & stream)
{
  static_assert(SerializationTraits<T>::IsSerializable, "write(): Type is not serializable");
  T value = v;
  if (SerializationTraits<T>::IsEndiannessDependent) {
    swapEndianness(value);
  }
  stream.write(reinterpret_cast<const char *>(&v), sizeof(v));
}

inline void write(const std::string & str, std::ostream & stream)
{
  writeStringLength(str.length(), stream);
  stream.write(str.c_str(), str.length());
}

template <typename T> inline void read(T & v, std::istream & stream)
{
  static_assert(SerializationTraits<T>::IsSerializable, "read(): Type is not serializable");
  stream.read(reinterpret_cast<char *>(&v), sizeof(v));
  if (SerializationTraits<T>::IsEndiannessDependent) {
    swapEndianness(v);
  }
}

inline void read(std::string & str, std::istream & stream)
{
  std::uint64_t size = readStringLength(stream);
  std::vector<char> v(size);
  stream.read(v.data(), size);
  str.assign(v.data(), size);
}

#endif // __GLITTER_SERIALIZE_H__
