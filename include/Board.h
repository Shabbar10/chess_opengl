#pragma once

#include <array>
#include <glm/glm.hpp>
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

  void generateVertices();
  Piece *getPieceAt(int x, int y) const;

public:
  Board();
  void render(SpriteSheet &blackSheet, SpriteSheet &whiteSheet, Shader &shader);
  void initializeBoard(SpriteSheet &blackSheet,
                       SpriteSheet &whiteSheet); // place pieces initially
  void movePiece(glm::ivec2 from, glm::ivec2 to);
  bool isOutOfBounds();
  bool isValidMove(); // Check if attacking same-coloured piece
  unsigned int getVAO();
};
