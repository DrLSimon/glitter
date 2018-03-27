#define GLM_ENABLE_EXPERIMENTAL
#include "glm/ext.hpp"

#include "Image.hpp"
#include "TextPrinter.hpp"
#include "stb_image.h"
#include "utils.hpp"

TextPrinter::TextPrinter(uint width, uint height)
    : m_width(width), m_height(height), m_nbChar(16), m_wOverH(width / float(height)), m_program("rubik/font.v.glsl", "rubik/font.f.glsl"), m_fontTexture(GL_TEXTURE_2D), m_sampler(0)
{
  //
  Image<GLubyte> fontImage;
  std::string rgbFilename = absolutename("rubik/font.png");
  fontImage.data = stbi_load(rgbFilename.c_str(), &fontImage.width, &fontImage.height, &fontImage.channels, STBI_default);
  m_fontTexture.setData(fontImage, true);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void TextPrinter::printText(const std::string & text, uint x, uint y, uint fontsize, const glm::vec3 & fontColor, const glm::vec4 & fillColor, uint padding)
{
  m_colors.push_back(fontColor);
  m_fillColors.push_back(fillColor);
  m_vaos.push_back(std::unique_ptr<VAO>(new VAO(2)));
  std::vector<glm::vec2> positions;
  std::vector<glm::vec2> uvs;
  std::vector<uint> ibo;
  uint & width = m_width;
  uint & height = m_height;
  uint & nbChar = m_nbChar;
  fontsize = fontsize / 100.f * m_width;
  auto toClip = [width, height, fontsize](uint x, uint y) { return glm::vec2((x * fontsize - width / 2.) * 2. / width, -(y * fontsize - height / 2.) * 2.f / height); };
  auto toUV = [nbChar](unsigned char c, bool xoff, bool yoff) { return glm::vec2((c % nbChar + xoff) / float(nbChar), (c / nbChar + yoff) / float(nbChar)); };
  std::string message = text;
  if (padding > text.size()) {
    std::string paddingText(padding - text.size(), ' ');
    printText(paddingText, x, y, fontsize, fontColor, fillColor);
    message += paddingText;
  }
  for (char c : message) {
    glm::vec2 pos[4] = {toClip(x, y), toClip(x + 1, y), toClip(x + 1, y + 1), toClip(x, y + 1)};
    glm::vec2 uv[4] = {toUV(c, 0, 0), toUV(c, 1, 0), toUV(c, 1, 1), toUV(c, 0, 1)};
    uint k = positions.size();
    uint square[6] = {k, k + 2, k + 1, k, k + 3, k + 2};
    ibo.insert(ibo.end(), square, square + 6);
    positions.insert(positions.end(), pos, pos + 4);
    uvs.insert(uvs.end(), uv, uv + 4);
    x += 1;
  }
  m_vaos.back()->setIBO(ibo);
  m_vaos.back()->setVBO(0, positions);
  m_vaos.back()->setVBO(1, uvs);
}

void TextPrinter::draw()
{
  m_program.bind();
  m_sampler.attachTexture(m_fontTexture);
  m_sampler.attachToProgram(m_program, "fontSampler", Sampler::DoNotBind);
  glEnable(GL_BLEND);
  m_program.setUniform("wOverH", m_wOverH);
  for (uint k = 0; k < m_vaos.size(); ++k) {
    auto & vao = m_vaos[k];
    glm::vec3 fontColor = m_colors[k];
    glm::vec4 fillColor = m_fillColors[k];
    m_program.setUniform("fontColor", fontColor);
    m_program.setUniform("fillColor", fillColor);
    vao->draw();
  }
  glDisable(GL_BLEND);
  m_program.unbind();
}

void TextPrinter::setWOverH(float wOverH)
{
  m_wOverH = wOverH;
}
