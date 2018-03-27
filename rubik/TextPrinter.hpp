#ifndef __TEXT_PRINTER_H__
#define __TEXT_PRINTER_H__
#include <memory>
#include <vector>
#include "glApi.hpp"

/// A class for print text overlays
class TextPrinter {
public:
  /**
   * @brief Constructor
   * @param width
   * @param height
   */
  TextPrinter(uint width, uint height);
  /**
   * @brief creates a VAO for displaying some text overlay
   * @param text
   * @param x
   * @param y
   * @param fontsize percentage of width covered by one character
   */
  void printText(const std::string & text, uint x, uint y, uint fontsize, const glm::vec3 & fontColor = glm::vec3(1, 1, 1), const glm::vec4 & fillColor = glm::vec4(1, 1, 1, 0), uint padding = 0);

  /// Draws all the vaos created with printText
  void draw();

  /// sets the aspect ratio
  void setWOverH(float wOverH);

private:
  uint m_width;   ///< width of the viewport
  uint m_height;  ///< height of the viewport
  uint m_nbChar;  ///< number of characters per row (in the font texture)
  float m_wOverH; ///< aspect ratio

  Program m_program;                        ///< GLSL program for displaying text
  Texture m_fontTexture;                    ///< texture containing all the font characters
  Sampler m_sampler;                        ///< Texture sampler
  std::vector<std::unique_ptr<VAO>> m_vaos; ///< VAOs created by calling printText
  std::vector<glm::vec3> m_colors;          ///< Font colors
  std::vector<glm::vec4> m_fillColors;      ///< Fill colors
};

#endif // !defined(__TEXT_PRINTER_H__)
