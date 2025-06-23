#pragma once

class Board;
class SpriteSheet;
class Shader;

#include <glm/glm.hpp>
#include <vector>

enum class PieceType { Pawn, Knight, Rook, Bishop, Queen, King };

class Piece {
protected:
  glm::ivec2 boardPos;
  bool isWhite;
  PieceType type;
  float u0, v0, u1, v1;

public:
  Piece(glm::ivec2 pos, bool white, PieceType type, SpriteSheet &sheet);
  virtual std::vector<glm::ivec2> getValidMoves(Board &board) = 0;
  virtual void render(SpriteSheet &sheet, Shader &shader) = 0;
  bool checkifWhite();
};

class Pawn : public Piece {
public:
  using Piece::Piece; // Use Piece's constructor

  std::vector<glm::ivec2> getValidMoves(Board &board) override;
  void render(SpriteSheet &sheet, Shader &shader) override;
};

class Rook : public Piece {
public:
  using Piece::Piece; // Use Piece's constructor

  std::vector<glm::ivec2> getValidMoves(Board &board) override;
  void render(SpriteSheet &sheet, Shader &shader) override;
};

class Bishop : public Piece {
public:
  using Piece::Piece; // Use Piece's constructor

  std::vector<glm::ivec2> getValidMoves(Board &board) override;
  void render(SpriteSheet &sheet, Shader &shader) override;
};

class Knight : public Piece {
public:
  using Piece::Piece; // Use Piece's constructor

  std::vector<glm::ivec2> getValidMoves(Board &board) override;
  void render(SpriteSheet &sheet, Shader &shader) override;
};

class Queen : public Piece {
public:
  using Piece::Piece; // Use Piece's constructor

  std::vector<glm::ivec2> getValidMoves(Board &board) override;
  void render(SpriteSheet &sheet, Shader &shader) override;
};

class King : public Piece {
public:
  using Piece::Piece; // Use Piece's constructor

  std::vector<glm::ivec2> getValidMoves(Board &board) override;
  void render(SpriteSheet &sheet, Shader &shader) override;
};
