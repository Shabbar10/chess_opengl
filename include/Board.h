#pragma once

#include <array>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

class Piece;
class SpriteSheet;
class Shader;

class Board {
private:
  std::array<std::array<Piece *, 8>, 8> grid;
  std::vector<float> vertices;
  std::vector<unsigned int> indices;
  unsigned int squareSize = 100;
  unsigned int VAO, VBO, EBO;
  bool whiteMove;
  std::vector<glm::ivec2> highlightedSquares;
  std::unique_ptr<Shader> highlightShader;
  unsigned int highlightVAO;
  unsigned int highlightVBO;
  unsigned int highlightEBO;

  void generateVertices();
  void renderHighlightedSquares(glm::mat4 projection);
  void initializeHighlightBuffers();

public:
  Board();
  void render(SpriteSheet &blackSheet, SpriteSheet &whiteSheet, Shader &shader,
              glm::mat4 projection);
  void initializeBoard(SpriteSheet &blackSheet,
                       SpriteSheet &whiteSheet); // place pieces initially
  Piece *getPieceAt(int x, int y) const;
  void handleClick(float x, float y);
  void movePiece(glm::ivec2 from, glm::ivec2 to);
  bool isOutOfBounds(const glm::ivec2 &move);
};
