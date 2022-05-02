#ifndef __GLITTER_IMAGE_H__
#define __GLITTER_IMAGE_H__

/**
 * @brief The Image struct
 *
 * A simple template struct for holding an image and its format description.
 * Allocation and deallocation should be taken care of elsewhere.
 */
template <typename T = unsigned char> struct Image {
public:
  /**
   * @brief Default constructor (empty image).
   */
  Image() : width(0), height(0), depth(0), channels(0), data(nullptr) {}

  /**
   * @brief Constructor from buffer and 3 dimensions (RGB 2d image)
   * @param data pointer to the data
   * @param w width
   * @param h height
   * @param c channels
   */
  Image(T * data, int w, int h, int c) : width(w), height(h), depth(1), channels(c), data(data) {}

  /**
   * @brief Constructor from buffer and 4 dimensions (RGB 3d image)
   * @param data pointer to the data
   * @param w width
   * @param h height
   * @param d depth
   * @param c channels
   */
  Image(T * data, int w, int h, int d, int c) : width(w), height(h), depth(d), channels(c), data(data) {}
  int width;
  int height;
  int depth;
  int channels;
  T * data;
  typedef T value_type;
};

#endif // !defined(__GLITTER_IMAGE_H__)
