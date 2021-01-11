#include "Serialize.hpp"
#include <cstring>

#ifdef IS_LITTLE_ENDIAN
void swapEndianness(glm::int16 &) {}
void swapEndianness(glm::int32 &) {}
void swapEndianness(glm::uint32 &) {}
void swapEndianness(glm::float32 &) {}
void swapEndianness(glm::vec2 &) {}
void swapEndianness(glm::vec3 &) {}
void swapEndianness(glm::vec4 &) {}
void swapEndianness(uint64_t &) {}
#else
void swapEndianness(glm::int16 & value)
{
  glm::int16 v = value;
  value = 0;
  value |= (v & 0xFF00) >> 8;
  value |= (v & 0xFF) << 8;
}

void swapEndianness(glm::int32 & value)
{
  glm::int32 v = value;
  value = 0;
  value |= (v & 0xFF000000) >> 24;
  value |= (v & 0x00FF0000) >> 8;
  value |= (v & 0x0000FF00) << 8;
  value |= (v & 0x000000FF) << 24;
}

void swapEndianness(glm::uint32 &)
{
  glm::uint32 v = value;
  value = 0;
  value |= (v & 0xFF000000) >> 24;
  value |= (v & 0x00FF0000) >> 8;
  value |= (v & 0x0000FF00) << 8;
  value |= (v & 0x000000FF) << 24;
}

void swapEndianness(glm::float32 & value)
{
  glm::int32 & intValue = reinterpret_cast<glm::int32 &>(value);
  glm::int32 v = intValue;
  intValue = 0;
  intValue |= (v & 0xFF000000) >> 24;
  intValue |= (v & 0x00FF0000) >> 8;
  intValue |= (v & 0x0000FF00) << 8;
  intValue |= (v & 0x000000FF) << 24;
}

void swapEndianness(glm::vec2 & v)
{
  swapEndianness(v.x);
  swapEndianness(v.y);
}

void swapEndianness(glm::vec3 & v)
{
  swapEndianness(v.x);
  swapEndianness(v.y);
  swapEndianness(v.z);
}

void swapEndianness(glm::vec4 & v)
{
  swapEndianness(v.x);
  swapEndianness(v.y);
  swapEndianness(v.z);
  swapEndianness(v.w);
}

void swapEndianness(uint64_t & value)
{
  std::uint64_t v = value;
  value = 0;
  value |= (v & 0xFF00000000000000) >> 48;
  value |= (v & 0x00FF000000000000) >> 32;
  value |= (v & 0x0000FF0000000000) >> 16;
  value |= (v & 0x000000FF00000000) >> 8;
  value |= (v & 0x00000000FF000000) << 8;
  value |= (v & 0x0000000000FF0000) << 16;
  value |= (v & 0x000000000000FF00) << 32;
  value |= (v & 0x00000000000000FF) << 48;
}
#endif

void writeStringLength(uint64_t value, std::ostream & stream)
{
  /*
   *  Length bits are saved in an array of unsigned char
   *  shaped like this: 1xxxxxxx 1xxxxxxx 1xxxxxxx 0xxxxxxx
   *  Most significant bit means:
   *    - 1 for more bytes to come
   *    - 0 for the last byte
   *  (Strings with length <= 127 only have a 1 byte prefix)
   */
  unsigned char c;
  while (value > 127) {
    c = (value & 0x7F) | 128;
    stream.write(reinterpret_cast<char *>(&c), 1);
    value >>= 7;
  }
  c = value;
  stream.write(reinterpret_cast<char *>(&c), 1);
}

std::uint64_t readStringLength(std::istream & stream)
{
  std::uint64_t value = 0;
  unsigned char c;
  do {
    stream.read(reinterpret_cast<char *>(&c), 1);
    value <<= 7;
    value |= (c & 0x7F);
  } while (c & 128);
  return value;
}

void write(const std::vector<std::string> & v, std::ostream & stream)
{
  // Write TAG(4B), SIZE(8B), VALUES...
  stream.write(SerializationTraits<std::string>::VectorTag(), 4);
  std::uint64_t size = v.size();
  swapEndianness(size);
  stream.write(reinterpret_cast<char *>(&size), sizeof(size));
  for (const std::string & str : v) {
    write(str, stream);
  }
}

void read(std::vector<std::string> & v, std::istream & stream)
{
  // Check TAG
  char tag[5] = {0, 0, 0, 0, 0};
  assert(stream.read(tag, 4) && "Cannot read input file");
  assert(!strcmp(tag, SerializationTraits<std::string>::VectorTag()) && "read(): Missing vector tag");
  std::uint64_t size;
  stream.read(reinterpret_cast<char *>(&size), sizeof(size));
  swapEndianness(size);
  v.clear();
  v.resize(size);
  std::vector<std::string>::iterator it = v.begin();
  while (it != v.end()) {
    read(*it, stream);
  }
}
