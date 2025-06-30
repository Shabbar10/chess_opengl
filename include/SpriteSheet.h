#include <string>
#include <tuple>
#ifndef DEBUG

#endif // !DEBUG
class SpriteSheet {
private:
  unsigned int textureID;
  int width, height, numChannels;
  unsigned int quadVBO, quadVAO, quadEBO;

public:
  SpriteSheet(const std::string &path);
  ~SpriteSheet();
  std::tuple<float, float, float, float> getUV(unsigned int position);
  void initQuad();
  void bind();
  void bindQuadVAO() const;
};
