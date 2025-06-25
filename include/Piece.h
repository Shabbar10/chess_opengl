#pragma once

class Board;
class SpriteSheet;
class Shader;

#include <glm/glm.hpp>
#include <iostream>
#include <vector>

enum class PieceType { Pawn, Knight, Rook, Bishop, Queen, King };

struct PieceAnimation {
  glm::vec2 startPos;
  glm::vec2 targetPos;
  float startTime;
  bool isAnimating = false;
};

class Piece {
protected:
  glm::ivec2 boardPos;
  glm::vec2 currentPos; // In floats for animation
  bool isWhite;
  PieceType type;
  float u0, v0, u1, v1;

public:
  PieceAnimation animation;

  Piece(glm::ivec2 pos, bool white, PieceType type, SpriteSheet &sheet);
  virtual ~Piece() = default;
  virtual std::vector<glm::ivec2> getValidMoves(Board &board) = 0;
  virtual void render(SpriteSheet &sheet, Shader &shader) = 0;
  bool checkifWhite();
  glm::ivec2 getBoardPos();
  void setBoardPos(const glm::ivec2 to);
  PieceType getType();

  friend std::ostream &operator<<(std::ostream &os, const Piece &piece) {
    os << "Piece Type: " << static_cast<int>(piece.type) << ", Position: ("
       << piece.boardPos.x << ", " << piece.boardPos.y << ")"
       << ", Color: " << (piece.isWhite ? "White" : "Black") << ", UV0: ("
       << piece.u0 << ", " << piece.v0 << ")"
       << ", UV1: (" << piece.u1 << ", " << piece.v1 << ")";
    return os;
  }
};

class Pawn : public Piece {
private:
  bool firstMove = true;

public:
  using Piece::Piece; // Use Piece's constructor

  std::vector<glm::ivec2> getValidMoves(Board &board) override;
  void render(SpriteSheet &sheet, Shader &shader) override;
  void firstMoveFalse();
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
