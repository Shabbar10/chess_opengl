#include "Board.h"
#include "Piece.h"
#include "Shader.h"
#include "SpriteSheet.h"
#include <glad/glad.h>

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

void Board::initializeBoard(SpriteSheet &blackSheet, SpriteSheet &whiteSheet) {
  // Clear board
  for (std::array<Piece *, 8> &row : grid)
    row.fill(nullptr);

  // Add Pawns to the 2nd row from top and bottom
  for (int col = 0; col < 8; col++) {
    grid[1][col] =
        new Pawn({col, 1}, true, PieceType::Pawn, whiteSheet); // White Pawns
    grid[6][col] =
        new Pawn({col, 6}, false, PieceType::Pawn, blackSheet); // Black Pawns
  }

  // TODO add lambda function to add other Pieces
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

  placePieces(0, true, blackSheet);
  placePieces(7, false, blackSheet);
}

void Board::render(SpriteSheet &blackSheet, SpriteSheet &whiteSheet,
                   Shader &shader) {
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

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

bool Board::isValidMove() { return true; }

bool Board::isOutOfBounds() { return false; }

unsigned int Board::getVAO() { return VAO; }
