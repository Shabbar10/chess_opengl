#include "SpriteSheet.h"
#include "stb_image.h"
#include <glad/glad.h>
#include <iostream>

SpriteSheet::SpriteSheet(const std::string &path) {
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);

  stbi_set_flip_vertically_on_load(true);
  unsigned char *data =
      stbi_load(path.c_str(), &width, &height, &numChannels, STBI_rgb_alpha);

  if (!data) {
    std::cout << "Sprite Sheet not found brother\n";
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, data);

  glGenerateMipmap(GL_TEXTURE_2D);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_image_free(data);
}

std::tuple<float, float, float, float>
SpriteSheet::getUV(unsigned int position) {
  float pieceWidth = 1.f / 6.f;

  float u0 = position * pieceWidth;
  float u1 = (position + 1) * pieceWidth;
  float v0 = 0.f;
  float v1 = 1.f;

  return {u0, v0, u1, v1};
}

void SpriteSheet::bind() {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textureID);
}

void SpriteSheet::bindQuadVAO() const { glBindVertexArray(quadVAO); }

void SpriteSheet::initQuad() {
  // clang-format off
  float quadVertices[] = {
        // pos      // tex
        0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
    };
  
  unsigned int indices[] = {
    0, 1, 2,
    2, 3, 0
  };
  // clang-format on

  glGenVertexArrays(1, &quadVAO);
  glGenBuffers(1, &quadVBO);
  glGenBuffers(1, &quadEBO);

  glBindVertexArray(quadVAO);

  glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices,
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *)(2 * sizeof(float)));
  glEnableVertexAttribArray(2);

  glBindVertexArray(0);
}

SpriteSheet::~SpriteSheet() {
  glDeleteVertexArrays(1, &quadVAO);
  glDeleteBuffers(1, &quadVBO);
  glDeleteBuffers(1, &quadEBO);
  glDeleteTextures(1, &textureID);
}
