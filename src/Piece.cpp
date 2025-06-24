#include "Piece.h"
#include "Board.h"
#include "Shader.h"
#include "SpriteSheet.h"
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <tuple>

#define SQUARE_SIZE 100

Piece::Piece(glm::ivec2 pos, bool white, PieceType type, SpriteSheet &sheet)
    : boardPos(pos), isWhite(white), type(type) {
  std::tie(u0, v0, u1, v1) = sheet.getUV(static_cast<int>(type));
}

bool Piece::checkifWhite() { return isWhite; }

glm::ivec2 Piece::getBoardPos() { return boardPos; }

void Piece::setBoardPos(const glm::ivec2 to) { boardPos = to; }

std::vector<glm::ivec2> Pawn::getValidMoves(Board &board) {
  int moveDirection = isWhite ? -1 : 1;

  std::vector<glm::ivec2> potentialMoves{
      boardPos + glm::ivec2{0, moveDirection}, // 1 forward
  };

  if (firstMove) {
    potentialMoves.push_back(boardPos + glm::ivec2{0, 2 * moveDirection});
    firstMove = false;
  }

  // x is column, y is row
  if (isWhite) {
    glm::ivec2 left = boardPos + glm::ivec2{-1, moveDirection};
    if (!board.isOutOfBounds(left)) {
      Piece *leftDiagonal = board.getPieceAt(left.x, left.y);

      // Check if the leftDiagonal is black (to attack)
      if (leftDiagonal != nullptr && !leftDiagonal->checkifWhite())
        potentialMoves.push_back(boardPos + glm::ivec2{-1, moveDirection});
    }

    glm::ivec2 right = boardPos + glm::ivec2{1, moveDirection};
    if (!board.isOutOfBounds(right)) {
      Piece *rightDiagonal = board.getPieceAt(right.x, right.y);
      if (rightDiagonal != nullptr && !rightDiagonal->checkifWhite())
        potentialMoves.push_back(boardPos + glm::ivec2{1, moveDirection});
    }
  } else {
    glm::ivec2 left = boardPos + glm::ivec2{-1, moveDirection};
    if (!board.isOutOfBounds(left)) {
      Piece *leftDiagonal = board.getPieceAt(left.x, left.y);
      if (leftDiagonal != nullptr && leftDiagonal->checkifWhite())
        potentialMoves.push_back(boardPos + glm::ivec2{-1, moveDirection});
    }

    glm::ivec2 right = boardPos + glm::ivec2{1, moveDirection};
    if (!board.isOutOfBounds(right)) {
      Piece *rightDiagonal = board.getPieceAt(right.x, right.y);
      if (rightDiagonal != nullptr && rightDiagonal->checkifWhite())
        potentialMoves.push_back(boardPos + glm::ivec2{1, moveDirection});
    }
  }

  potentialMoves.erase(std::remove_if(potentialMoves.begin(),
                                      potentialMoves.end(),
                                      [&](const glm::ivec2 &move) {
                                        return board.isOutOfBounds(move);
                                      }),
                       potentialMoves.end());

  return potentialMoves;
}

void Pawn::render(SpriteSheet &sheet, Shader &shader) {
  // Calculate screen space coordinates
  float screenX = boardPos.x * SQUARE_SIZE;
  float screenY = (7 - boardPos.y) * SQUARE_SIZE;

  // Now to place the pieces in the correct world coordinates, we need a model
  // matrix for translation
  glm::mat4 model = glm::translate(
      glm::mat4(1.0f), glm::vec3(screenX + (SQUARE_SIZE / 4.0f),
                                 screenY + (SQUARE_SIZE / 8.0f), 0.1f));
  model = glm::scale(model, glm::vec3(SQUARE_SIZE / 2, SQUARE_SIZE / 2, 1.0f));

  shader.use();
  shader.setMat4("uModel", model);
  shader.setVec2("uUV0", u0, v0);
  shader.setVec2("uUV1", u1, v1);

  sheet.bind();
  sheet.bindQuadVAO();
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

std::vector<glm::ivec2> Rook::getValidMoves(Board &board) {
  std::vector<glm::ivec2> potentialMoves{
      boardPos + glm::ivec2{-1, 0},  // 1 forward
      boardPos + glm::ivec2{-1, -1}, // diagonal left
      boardPos + glm::ivec2{-1, 1}   // diagonal right
  };

  potentialMoves.erase(std::remove_if(potentialMoves.begin(),
                                      potentialMoves.end(),
                                      [&](const glm::ivec2 &move) {
                                        return board.isOutOfBounds(move);
                                      }),
                       potentialMoves.end());

  return potentialMoves;
}

void Rook::render(SpriteSheet &sheet, Shader &shader) {
  // Calculate screen space coordinates
  float screenX = boardPos.x * SQUARE_SIZE;
  float screenY = (7 - boardPos.y) * SQUARE_SIZE;

  // Now to place the pieces in the correct world coordinates, we need a model
  // matrix for translation
  glm::mat4 model = glm::translate(
      glm::mat4(1.0f), glm::vec3(screenX + (SQUARE_SIZE / 4.0f),
                                 screenY + (SQUARE_SIZE / 8.0f), 0.1f));
  model = glm::scale(model, glm::vec3(SQUARE_SIZE / 2, SQUARE_SIZE / 2, 1.0f));

  shader.use();
  shader.setMat4("uModel", model);
  shader.setVec2("uUV0", u0, v0);
  shader.setVec2("uUV1", u1, v1);

  sheet.bind();
  sheet.bindQuadVAO();
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

std::vector<glm::ivec2> Bishop::getValidMoves(Board &board) {
  std::vector<glm::ivec2> potentialMoves{
      boardPos + glm::ivec2{-1, 0},  // 1 forward
      boardPos + glm::ivec2{-1, -1}, // diagonal left
      boardPos + glm::ivec2{-1, 1}   // diagonal right
  };

  potentialMoves.erase(std::remove_if(potentialMoves.begin(),
                                      potentialMoves.end(),
                                      [&](const glm::ivec2 &move) {
                                        return board.isOutOfBounds(move);
                                      }),
                       potentialMoves.end());

  return potentialMoves;
}

void Bishop::render(SpriteSheet &sheet, Shader &shader) {
  // Calculate screen space coordinates
  float screenX = boardPos.x * SQUARE_SIZE;
  float screenY = (7 - boardPos.y) * SQUARE_SIZE;

  // Now to place the pieces in the correct world coordinates, we need a model
  // matrix for translation
  glm::mat4 model = glm::translate(
      glm::mat4(1.0f), glm::vec3(screenX + (SQUARE_SIZE / 4.0f),
                                 screenY + (SQUARE_SIZE / 8.0f), 0.1f));
  model = glm::scale(model, glm::vec3(SQUARE_SIZE / 2, SQUARE_SIZE / 2, 1.0f));

  shader.use();
  shader.setMat4("uModel", model);
  shader.setVec2("uUV0", u0, v0);
  shader.setVec2("uUV1", u1, v1);

  sheet.bind();
  sheet.bindQuadVAO();
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

std::vector<glm::ivec2> Knight::getValidMoves(Board &board) {
  std::vector<glm::ivec2> potentialMoves;

  potentialMoves.erase(std::remove_if(potentialMoves.begin(),
                                      potentialMoves.end(),
                                      [&](const glm::ivec2 &move) {
                                        return board.isOutOfBounds(move);
                                      }),
                       potentialMoves.end());

  return potentialMoves;
}

void Knight::render(SpriteSheet &sheet, Shader &shader) {
  // Calculate screen space coordinates
  float screenX = boardPos.x * SQUARE_SIZE;
  float screenY = (7 - boardPos.y) * SQUARE_SIZE;

  // Now to place the pieces in the correct world coordinates, we need a model
  // matrix for translation
  glm::mat4 model = glm::translate(
      glm::mat4(1.0f), glm::vec3(screenX + (SQUARE_SIZE / 4.0f),
                                 screenY + (SQUARE_SIZE / 8.0f), 0.1f));
  model = glm::scale(model, glm::vec3(SQUARE_SIZE / 2, SQUARE_SIZE / 2, 1.0f));

  shader.use();
  shader.setMat4("uModel", model);
  shader.setVec2("uUV0", u0, v0);
  shader.setVec2("uUV1", u1, v1);

  sheet.bind();
  sheet.bindQuadVAO();
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

std::vector<glm::ivec2> Queen::getValidMoves(Board &board) {
  std::vector<glm::ivec2> potentialMoves{
      boardPos + glm::ivec2{-1, 0},  // 1 forward
      boardPos + glm::ivec2{-1, -1}, // diagonal left
      boardPos + glm::ivec2{-1, 1}   // diagonal right
  };

  potentialMoves.erase(std::remove_if(potentialMoves.begin(),
                                      potentialMoves.end(),
                                      [&](const glm::ivec2 &move) {
                                        return board.isOutOfBounds(move);
                                      }),
                       potentialMoves.end());

  return potentialMoves;
}

void Queen::render(SpriteSheet &sheet, Shader &shader) {
  // Calculate screen space coordinates
  float screenX = boardPos.x * SQUARE_SIZE;
  float screenY = (7 - boardPos.y) * SQUARE_SIZE;

  // Now to place the pieces in the correct world coordinates, we need a model
  // matrix for translation
  glm::mat4 model = glm::translate(
      glm::mat4(1.0f), glm::vec3(screenX + (SQUARE_SIZE / 4.0f),
                                 screenY + (SQUARE_SIZE / 8.0f), 0.1f));
  model = glm::scale(model, glm::vec3(SQUARE_SIZE / 2, SQUARE_SIZE / 2, 1.0f));

  shader.use();
  shader.setMat4("uModel", model);
  shader.setVec2("uUV0", u0, v0);
  shader.setVec2("uUV1", u1, v1);

  sheet.bind();
  sheet.bindQuadVAO();
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

std::vector<glm::ivec2> King::getValidMoves(Board &board) {
  std::vector<glm::ivec2> potentialMoves{
      boardPos + glm::ivec2{-1, 0},  // 1 forward
      boardPos + glm::ivec2{-1, -1}, // diagonal left
      boardPos + glm::ivec2{-1, 1}   // diagonal right
  };

  potentialMoves.erase(std::remove_if(potentialMoves.begin(),
                                      potentialMoves.end(),
                                      [&](const glm::ivec2 &move) {
                                        return board.isOutOfBounds(move);
                                      }),
                       potentialMoves.end());

  return potentialMoves;
}

void King::render(SpriteSheet &sheet, Shader &shader) {
  // Calculate screen space coordinates
  float screenX = boardPos.x * SQUARE_SIZE;
  float screenY = (7 - boardPos.y) * SQUARE_SIZE;

  // Now to place the pieces in the correct world coordinates, we need a model
  // matrix for translation
  glm::mat4 model = glm::translate(
      glm::mat4(1.0f), glm::vec3(screenX + (SQUARE_SIZE / 4.0f),
                                 screenY + (SQUARE_SIZE / 8.0f), 0.1f));
  model = glm::scale(model, glm::vec3(SQUARE_SIZE / 2, SQUARE_SIZE / 2, 1.0f));

  shader.use();
  shader.setMat4("uModel", model);
  shader.setVec2("uUV0", u0, v0);
  shader.setVec2("uUV1", u1, v1);

  sheet.bind();
  sheet.bindQuadVAO();
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
