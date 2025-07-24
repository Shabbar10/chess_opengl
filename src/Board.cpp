// clang-format off
#include <glad/glad.h>
#include "GLFW/glfw3.h"
// clang-format on

#include "Board.h"
#include "Piece.h"
#include "Shader.h"
#include "SpriteSheet.h"
#include <algorithm>
#include <memory>
#include <print>

Board::Board() {
  generateVertices();

  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  glGenVertexArrays(1, &VAO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
               vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  initializeHighlightBuffers();
  initializeDimBuffers();
  initializePromotionBuffers();
  initializePromotionPiecesBuffers();
}

void Board::generateVertices() {
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      float x = 0 + col * squareSize;
      float y = 0 + row * squareSize;

      // Black square or white
      bool isWhite = (row + col) % 2 == 0;
      float r = isWhite ? 1.f : 0.f;
      float g = isWhite ? 1.f : 0.f;
      float b = isWhite ? 1.f : 0.f;

      unsigned int baseIndex = vertices.size() / 5;

      vertices.insert(vertices.end(), {x,
                                       y,
                                       r,
                                       g,
                                       b,
                                       x + squareSize,
                                       y,
                                       r,
                                       g,
                                       b,
                                       x + squareSize,
                                       y + squareSize,
                                       r,
                                       g,
                                       b,
                                       x,
                                       y + squareSize,
                                       r,
                                       g,
                                       b});

      indices.insert(indices.end(), {baseIndex, baseIndex + 1, baseIndex + 2,
                                     baseIndex + 2, baseIndex + 3, baseIndex});
    }
  }
}

void Board::initializeHighlightBuffers() {
  glGenBuffers(1, &highlightVBO);
  glGenBuffers(1, &highlightEBO);
  glGenVertexArrays(1, &highlightVAO);
  glBindVertexArray(highlightVAO);

  glBindBuffer(GL_ARRAY_BUFFER, highlightVBO);
  glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

  std::vector<unsigned int> indices = {0, 1, 2, 2, 3, 0};

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, highlightEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(),
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);

  highlightShader =
      std::make_unique<Shader>("src/highlight.vert", "src/highlight.frag");
}

void Board::initializeDimBuffers() {
  float dimVertices[] = {
      -1.0f, 1.0f,  // top left
      -1.0f, -1.0f, // bottom left
      1.0f,  -1.0f, // bottom right
      1.0f,  1.0f   // top right
  };

  unsigned int indices[] = {0, 3, 2, 0, 1, 2};

  glGenBuffers(1, &dimVBO);
  glGenBuffers(1, &dimEBO);
  glGenVertexArrays(1, &dimVAO);
  glBindVertexArray(dimVAO);

  glBindBuffer(GL_ARRAY_BUFFER, dimVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(dimVertices), dimVertices,
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dimEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);

  dimShader =
      std::make_unique<Shader>("src/dimWindow.vert", "src/dimWindow.frag");
}

void Board::initializePromotionBuffers() {
  float vertices[] = {
      -1.0f, 0.25f,  // top left
      -1.0f, -0.25f, // bottom left
      1.0f,  -0.25f, // bottom right
      1.0f,  0.25f   // top right
  };

  unsigned int indices[] = {0, 3, 2, 0, 1, 2};

  glGenVertexArrays(1, &promoteVAO);
  glGenBuffers(1, &promoteVBO);
  glGenBuffers(1, &promoteEBO);
  glBindVertexArray(promoteVAO);

  glBindBuffer(GL_ARRAY_BUFFER, promoteVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, promoteEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);

  promotionShader =
      std::make_unique<Shader>("src/promotion.vert", "src/promotion.frag");
}

void Board::initializePromotionPiecesBuffers() {
  struct Vertex {
    float x, y;
    float u, v;
  };

  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<PieceType> pieces{PieceType::Bishop, PieceType::Knight,
                                PieceType::Queen, PieceType::Rook};

  float startX = -0.5f;
  float quadWidth = 0.25;
  float quadHeight = 0.25f;

  for (int i = 0; i < pieces.size(); i++) {
    float x0 = startX + i * quadWidth;
    float x1 = x0 + quadWidth;
    float y0 = -(quadHeight / 2);
    float y1 = quadHeight / 2;

    pQuads.push_back({.min = {x0, y0}, .max = {x1, y1}, .type = pieces[i]});

    auto [u0, v0, u1, v1] =
        promotedPawnSheet->getUV(static_cast<unsigned int>(pieces[i]));

    unsigned int baseIndex = vertices.size();

    vertices.push_back({x0, y1, u0, v1});
    vertices.push_back({x0, y0, u0, v0});
    vertices.push_back({x1, y0, u1, v0});
    vertices.push_back({x1, y1, u1, v1});

    indices.insert(indices.end(), {baseIndex, baseIndex + 1, baseIndex + 2,
                                   baseIndex, baseIndex + 2, baseIndex + 3});
  }

  glGenVertexArrays(1, &pieceVAO);
  glGenBuffers(1, &pieceVBO);
  glGenBuffers(1, &pieceEBO);
  glBindVertexArray(pieceVAO);

  glBindBuffer(GL_ARRAY_BUFFER, pieceVBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
               vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pieceEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);

  promotionPiecesShader = std::make_unique<Shader>("src/promotionPiece.vert",
                                                   "src/promotionPiece.frag");
}

void Board::initializeBoard(SpriteSheet &blackSheet, SpriteSheet &whiteSheet) {
  // Clear board
  for (std::array<Piece *, 8> &row : grid)
    row.fill(nullptr);

  // Add Pawns to the 2nd row from top and bottom
  for (int col = 0; col < 8; col++) {
    grid[1][col] =
        new Pawn({col, 1}, false, PieceType::Pawn, blackSheet); // Black Pawns
    grid[6][col] =
        new Pawn({col, 6}, true, PieceType::Pawn, whiteSheet); // White Pawns
  }

  auto placePieces = [&](int row, bool isWhite, SpriteSheet &sheet) {
    grid[row][0] = new Rook({0, row}, isWhite, PieceType::Rook, sheet);
    grid[row][7] = new Rook({7, row}, isWhite, PieceType::Rook, sheet);

    grid[row][1] = new Knight({1, row}, isWhite, PieceType::Knight, sheet);
    grid[row][6] = new Knight({6, row}, isWhite, PieceType::Knight, sheet);

    grid[row][2] = new Bishop({2, row}, isWhite, PieceType::Bishop, sheet);
    grid[row][5] = new Bishop({5, row}, isWhite, PieceType::Bishop, sheet);

    grid[row][3] = new Queen({3, row}, isWhite, PieceType::Queen, sheet);
    grid[row][4] = new King({4, row}, isWhite, PieceType::King, sheet);
  };

  placePieces(0, false, blackSheet);
  placePieces(7, true, whiteSheet);
}

void Board::render(SpriteSheet &blackSheet, SpriteSheet &whiteSheet,
                   Shader &shader, glm::mat4 projection) {
  // Render black and white squares
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

  renderHighlightedSquares(projection);

  // Render pieces
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      if (grid[row][col] != nullptr) {
        if (grid[row][col]->checkifWhite()) {
          grid[row][col]->render(whiteSheet, shader);
        } else
          grid[row][col]->render(blackSheet, shader);
      }
    }
  }

  if (gameState == GameState::PromotionPending) {
    renderDimWindow();
    renderPromotionOverlay();
    renderPromotionPieces();
  }
}

void Board::renderHighlightedSquares(glm::mat4 projection) {
  highlightShader->use();
  glBindVertexArray(highlightVAO);
  glBindBuffer(GL_ARRAY_BUFFER, highlightVBO);

  // For each square, get base screen coords
  for (const glm::ivec2 &move : highlightedSquares) {
    float screenX = move.x * squareSize;
    float screenY = (7 - move.y) * squareSize;

    // Fill the rest of the vertices of the quad
    std::vector<float> vertices = {
        // Bottom left
        screenX + 5, screenY + 5, 1.0f, 1.0f, 0.0f,

        // Bottom right
        screenX + squareSize - 5, screenY + 5, 1.0f, 1.0f, 0.0f,

        // Top right
        screenX + squareSize - 5, screenY + squareSize - 5, 1.0f, 1.0f, 0.0f,

        // Top left
        screenX + 5, screenY + squareSize - 5, 1.0f, 1.0f, 0.0f};

    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float),
                    vertices.data());

    highlightShader->setMat4("uProjection", projection);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  }
  glBindVertexArray(0);
}

void Board::renderDimWindow() {
  dimShader->use();
  glBindVertexArray(dimVAO);

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Board::renderPromotionOverlay() {
  promotionShader->use();
  glBindVertexArray(promoteVAO);

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Board::renderPromotionPieces() {
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  promotionPiecesShader->use();
  promotionPiecesShader->setInt("uTexture", 0);
  glActiveTexture(GL_TEXTURE0);
  promotedPawnSheet->bind();

  glBindVertexArray(pieceVAO);
  glDrawElements(GL_TRIANGLES, 6 * 4, GL_UNSIGNED_INT, 0);
}

void Board::handleClick(float x, float y) {
  if (gameState == GameState::PromotionPending)
    return;

  int gridCol = x / squareSize, gridRow = y / squareSize;

  if (highlighted) {
    const glm::ivec2 move{gridCol, gridRow};

    auto it =
        std::find(highlightedSquares.begin(), highlightedSquares.end(), move);

    if (it != highlightedSquares.end())
      movePiece(clickedPiece->getBoardPos(), move);

    highlighted = false;
    highlightedSquares.clear();
  } else {
    clickedPiece = getPieceAt(gridCol, gridRow);
    if (clickedPiece) {
      if ((whiteTurn && clickedPiece->checkifWhite()) ||
          (!whiteTurn && !clickedPiece->checkifWhite())) {
        std::cout << *clickedPiece << std::endl;
        highlightedSquares = clickedPiece->getValidMoves(*this);
        highlighted = true;
      } else {
        std::string turn = whiteTurn ? "White's" : "Black's";
        std::cout << "It is " << turn << " turn\n";
      }
    } else {
      std::cout << "No piece clicked\n";
      highlightedSquares.clear();
      highlighted = false;
    }
  }
}

void Board::handlePromotionClick(float x, float y) {
  std::println("({}, {})", x, y);
  float NDCx = x / 800.f * 2.f - 1.f;
  float NDCy = y / 800.f * 2.f - 1.f;

  for (const PromotionQuad &piece : pQuads) {
    if ((NDCx >= piece.min.x && NDCy >= piece.min.y) &&
        (NDCx <= piece.max.x && NDCy <= piece.max.y)) {
      promoteTo = piece.type;
    }
  }

  changePiece();
}

bool Board::isOutOfBounds(const glm::ivec2 &move) {
  if (move.x < 0 || move.x > 7)
    return true;

  if (move.y < 0 || move.y > 7)
    return true;

  return false;
}

Piece *Board::getPieceAt(int x, int y) const { return grid[y][x]; }

void Board::movePiece(glm::ivec2 from, glm::ivec2 to) {
  Piece *movingPiece = grid[from.y][from.x];
  Piece *targetPiece = grid[to.y][to.x];

  if (targetPiece) {
    if (targetPiece->getType() == PieceType::King)
      hasWon = true;

    delete targetPiece;
  }

  grid[to.y][to.x] = grid[from.y][from.x];
  grid[from.y][from.x] = nullptr;

  movingPiece->animation.startPos =
      glm::vec2(from.x, 7 - from.y) * (float)squareSize;
  movingPiece->animation.targetPos =
      glm::vec2(to.x, 7 - to.y) * (float)squareSize;
  movingPiece->animation.startTime = glfwGetTime();
  movingPiece->animation.isAnimating = true;

  movingPiece->setBoardPos(to);

  if (movingPiece->getType() == PieceType::Pawn) {
    Pawn *p = dynamic_cast<Pawn *>(movingPiece);
    p->firstMoveFalse();

    bool isWhite = p->checkifWhite();
    promotedPawnSheet = p->getSheet();

    if ((isWhite && (to.y == 0)) || (!isWhite && (to.y == 7))) {
      gameState = GameState::PromotionPending;
      movingTo = to;
      promotedPawn = movingPiece;
    }
  }
  whiteTurn = !whiteTurn;
}

bool Board::checkIfWon() { return hasWon; }

GameState Board::getGameState() { return gameState; }

void Board::changePiece() {
  Pawn *p = dynamic_cast<Pawn *>(promotedPawn);

  delete grid[movingTo.y][movingTo.x];

  switch (promoteTo) {
  case PieceType::Bishop: {
    grid[movingTo.y][movingTo.x] =
        new Bishop({movingTo.x, movingTo.y}, p->checkifWhite(),
                   PieceType::Bishop, (*p->getSheet()));
    gameState = GameState::Playing;
    break;
  }
  case PieceType::Rook: {
    grid[movingTo.y][movingTo.x] =
        new Rook({movingTo.x, movingTo.y}, p->checkifWhite(), PieceType::Rook,
                 (*p->getSheet()));
    gameState = GameState::Playing;
    break;
  }
  case PieceType::Knight: {
    grid[movingTo.y][movingTo.x] =
        new Knight({movingTo.x, movingTo.y}, p->checkifWhite(),
                   PieceType::Knight, (*p->getSheet()));
    gameState = GameState::Playing;
    break;
  }
  case PieceType::Queen: {
    grid[movingTo.y][movingTo.x] =
        new Queen({movingTo.x, movingTo.y}, p->checkifWhite(), PieceType::Queen,
                  (*p->getSheet()));
    gameState = GameState::Playing;
    break;
  }
  }

  promotedPawn = nullptr;
}

Board::~Board() {
  for (auto &row : grid)
    for (auto &piece : row)
      if (piece)
        delete piece;

  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteVertexArrays(1, &VAO);

  glDeleteBuffers(1, &highlightVBO);
  glDeleteBuffers(1, &highlightEBO);
  glDeleteVertexArrays(1, &highlightVAO);

  glDeleteBuffers(1, &dimVBO);
  glDeleteBuffers(1, &dimEBO);
  glDeleteVertexArrays(1, &dimVAO);

  glDeleteBuffers(1, &promoteVBO);
  glDeleteBuffers(1, &promoteEBO);
  glDeleteVertexArrays(1, &promoteVAO);

  glDeleteBuffers(1, &pieceVBO);
  glDeleteBuffers(1, &pieceEBO);
  glDeleteVertexArrays(1, &pieceVAO);
}
