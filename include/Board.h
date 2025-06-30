#pragma once

#include <array>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

enum class GameState { Playing, PromotionPending, OwariDa };

class Piece;
class Pawn;
class SpriteSheet;
class Shader;

enum class PieceType;

struct PromotionQuad {
  glm::vec2 min;
  glm::vec2 max;
  PieceType type;
};

class Board {
private:
  std::array<std::array<Piece *, 8>, 8> grid;
  std::vector<float> vertices;
  std::vector<unsigned int> indices;
  unsigned int squareSize = 100;

  unsigned int VAO, VBO, EBO;
  std::vector<glm::ivec2> highlightedSquares;
  std::unique_ptr<Shader> highlightShader;
  unsigned int highlightVAO;
  unsigned int highlightVBO;
  unsigned int highlightEBO;
  bool highlighted = false;

  Piece *clickedPiece;
  bool hasWon = false;
  bool whiteTurn = true;

  GameState gameState = GameState::Playing;
  std::unique_ptr<Shader> dimShader;
  unsigned int dimVBO, dimEBO, dimVAO;
  std::unique_ptr<Shader> promotionShader;
  unsigned int promoteVBO, promoteEBO, promoteVAO;
  std::unique_ptr<Shader> promotionPiecesShader;
  unsigned int pieceVBO, pieceEBO, pieceVAO;
  std::vector<PromotionQuad> pQuads;
  SpriteSheet *promotedPawnSheet;
  PieceType promoteTo;
  glm::vec2 movingTo;
  Piece *promotedPawn;

  void generateVertices();
  void renderHighlightedSquares(glm::mat4 projection);
  void renderDimWindow();
  void renderPromotionOverlay();
  void renderPromotionPieces();
  void initializeHighlightBuffers();
  void initializeDimBuffers();
  void initializePromotionBuffers();
  void initializePromotionPiecesBuffers();
  void changePiece();

public:
  Board();
  ~Board();
  void render(SpriteSheet &blackSheet, SpriteSheet &whiteSheet, Shader &shader,
              glm::mat4 projection);
  void initializeBoard(SpriteSheet &blackSheet,
                       SpriteSheet &whiteSheet); // place pieces initially
  Piece *getPieceAt(int x, int y) const;
  void handleClick(float x, float y);
  void handlePromotionClick(float x, float y);
  void movePiece(glm::ivec2 from, glm::ivec2 to);
  bool isOutOfBounds(const glm::ivec2 &move);
  bool checkIfWon();
  GameState getGameState();
};
