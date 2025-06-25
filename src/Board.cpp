#include "Board.h"
#include "Piece.h"
#include "Shader.h"
#include "SpriteSheet.h"
#include <algorithm>
#include <glad/glad.h>
#include <memory>

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

void Board::handleClick(float x, float y) {
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
        whiteTurn = !whiteTurn;
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
  movingPiece->setBoardPos(to);
}

bool Board::checkIfWon() { return hasWon; }
