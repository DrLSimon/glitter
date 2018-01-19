#ifndef __IMAGE_H__
#define __IMAGE_H__

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
  Image() : width(0), height(0), channels(0), data(nullptr) {}

  /**
   * @brief Constructor from buffer and dimensions
   * @param d pointer to the data
   * @param w width
   * @param h height
   * @param c channels
   */
  Image(T * d, int w, int h, int c) : width(w), height(h), channels(c), data(d) {}
  int width;
  int height;
  int channels;
  T * data;
};

#endif // !defined(__IMAGE_H__)
