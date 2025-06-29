// clang-format off
#include "Shader.h"
#include "GLFW/glfw3.h"
// clang-format on

#include "Piece.h"
#include "Board.h"
#include "SpriteSheet.h"
#include <algorithm>
#include <cstdint>
#include <glm/gtc/matrix_transform.hpp>
#include <tuple>

#define SQUARE_SIZE 100

Piece::Piece(glm::ivec2 pos, bool white, PieceType type, SpriteSheet &sheet)
    : boardPos(pos), isWhite(white), type(type) {
  std::tie(u0, v0, u1, v1) = sheet.getUV(static_cast<int>(type));
}

bool Piece::checkifWhite() { return isWhite; }

glm::ivec2 Piece::getBoardPos() { return boardPos; }

void Piece::setBoardPos(const glm::ivec2 to) { boardPos = to; }

PieceType Piece::getType() { return type; }

std::vector<glm::ivec2> Pawn::getValidMoves(Board &board) {
  int8_t moveDirection = isWhite ? -1 : 1;

  std::vector<glm::ivec2> potentialMoves;

  glm::ivec2 forward = boardPos + glm::ivec2{0, moveDirection};
  if (!board.isOutOfBounds(forward) &&
      board.getPieceAt(forward.x, forward.y) == nullptr)
    potentialMoves.push_back(forward);

  if (firstMove) {
    potentialMoves.push_back(boardPos + glm::ivec2{0, 2 * moveDirection});
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

  return potentialMoves;
}

void Pawn::render(SpriteSheet &sheet, Shader &shader) {
  // Calculate screen space coordinates
  // float screenX = boardPos.x * SQUARE_SIZE;
  // float screenY = (7 - boardPos.y) * SQUARE_SIZE;
  float screenX, screenY;

  if (animation.isAnimating) {
    float t = glm::clamp((float)(glfwGetTime() - animation.startTime) / 0.3f,
                         0.0f, 1.0f);
    glm::vec2 interpolated =
        glm::mix(animation.startPos, animation.targetPos, t);
    screenX = interpolated.x;
    screenY = interpolated.y;

    if (t >= 1.0)
      animation.isAnimating = false;
  } else {
    screenX = boardPos.x * SQUARE_SIZE;
    screenY = (7 - boardPos.y) * SQUARE_SIZE;
  }

  // Now to place the pieces in the correct world coordinates, we need a model
  // matrix for translation and scaling (because pixels)
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

void Pawn::firstMoveFalse() { firstMove = false; }

std::vector<glm::ivec2> Rook::getValidMoves(Board &board) {
  std::vector<glm::ivec2> potentialMoves;

  int8_t row = boardPos.y, col = boardPos.x;

  // Check up
  for (int i = row + 1; i < 8; i++) {
    glm::ivec2 move{col, i};

    Piece *piece = board.getPieceAt(move.x, move.y);

    if (!piece) {
      potentialMoves.push_back(move);
    } else {
      bool targetIsWhite = board.getPieceAt(move.x, move.y)->checkifWhite();

      if (isWhite && targetIsWhite)
        break;
      else if (isWhite && !targetIsWhite) {
        potentialMoves.push_back(move);
        break;
      } else if (!isWhite && !targetIsWhite)
        break;
      else if (!isWhite && targetIsWhite) {
        potentialMoves.push_back(move);
        break;
      }
    }
  }

  // Check down
  for (int i = row - 1; i >= 0; i--) {
    glm::ivec2 move{col, i};

    Piece *piece = board.getPieceAt(move.x, move.y);

    if (!piece) {
      potentialMoves.push_back(move);
    } else {
      bool targetIsWhite = board.getPieceAt(move.x, move.y)->checkifWhite();

      if (isWhite && targetIsWhite)
        break;
      else if (isWhite && !targetIsWhite) {
        potentialMoves.push_back(move);
        break;
      } else if (!isWhite && !targetIsWhite)
        break;
      else if (!isWhite && targetIsWhite) {
        potentialMoves.push_back(move);
        break;
      }
    }
  }

  // Check left
  for (int i = col - 1; i >= 0; i--) {
    glm::ivec2 move{i, row};

    Piece *piece = board.getPieceAt(move.x, move.y);

    if (!piece) {
      potentialMoves.push_back(move);
    } else {
      // If white passing over white
      if (isWhite && board.getPieceAt(move.x, move.y)->checkifWhite())
        break;
      else if (isWhite && !board.getPieceAt(move.x, move.y)->checkifWhite()) {
        potentialMoves.push_back(move);
        break;
      }
    }
  }

  // Check right
  for (int i = col + 1; i < 8; i++) {
    glm::ivec2 move{i, row};

    Piece *piece = board.getPieceAt(move.x, move.y);

    if (!piece) {
      potentialMoves.push_back(move);
    } else {
      // If white passing over white
      if (isWhite && board.getPieceAt(move.x, move.y)->checkifWhite())
        break;
      else if (isWhite && !board.getPieceAt(move.x, move.y)->checkifWhite()) {
        potentialMoves.push_back(move);
        break;
      }
    }
  }

  return potentialMoves;
}

void Rook::render(SpriteSheet &sheet, Shader &shader) {
  // Calculate screen space coordinates
  float screenX = boardPos.x * SQUARE_SIZE;
  float screenY = (7 - boardPos.y) * SQUARE_SIZE;

  if (animation.isAnimating) {
    float t = glm::clamp((float)(glfwGetTime() - animation.startTime) / 0.3f,
                         0.0f, 1.0f);
    glm::vec2 interpolated =
        glm::mix(animation.startPos, animation.targetPos, t);
    screenX = interpolated.x;
    screenY = interpolated.y;

    if (t >= 1.0)
      animation.isAnimating = false;
  } else {
    screenX = boardPos.x * SQUARE_SIZE;
    screenY = (7 - boardPos.y) * SQUARE_SIZE;
  }

  // Now to place the pieces in the correct world coordinates, we need a model
  // matrix for translation and scaling (because pixels)
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
  std::vector<glm::ivec2> potentialMoves;

  int8_t row = boardPos.y, col = boardPos.x;

  // Check up-right
  for (int i = row - 1, j = col + 1; i >= 0 && j < 8; i--, j++) {
    glm::ivec2 move{j, i};

    Piece *piece = board.getPieceAt(move.x, move.y);

    if (!piece) {
      potentialMoves.push_back(move);
    } else {
      bool targetIsWhite = board.getPieceAt(move.x, move.y)->checkifWhite();

      if (isWhite && targetIsWhite)
        break;
      else if (isWhite && !targetIsWhite) {
        potentialMoves.push_back(move);
        break;
      } else if (!isWhite && !targetIsWhite)
        break;
      else if (!isWhite && targetIsWhite) {
        potentialMoves.push_back(move);
        break;
      }
    }
  }

  // Check up-left
  for (int i = row - 1, j = col - 1; i >= 0 && j >= 0; i--, j--) {
    glm::ivec2 move{j, i};

    Piece *piece = board.getPieceAt(move.x, move.y);

    if (!piece) {
      potentialMoves.push_back(move);
    } else {
      bool targetIsWhite = board.getPieceAt(move.x, move.y)->checkifWhite();

      if (isWhite && targetIsWhite)
        break;
      else if (isWhite && !targetIsWhite) {
        potentialMoves.push_back(move);
        break;
      } else if (!isWhite && !targetIsWhite)
        break;
      else if (!isWhite && targetIsWhite) {
        potentialMoves.push_back(move);
        break;
      }
    }
  }

  // Check down-right
  for (int i = row + 1, j = col + 1; i < 8 && j < 8; i++, j++) {
    glm::ivec2 move{j, i};

    Piece *piece = board.getPieceAt(move.x, move.y);

    if (!piece) {
      potentialMoves.push_back(move);
    } else {
      bool targetIsWhite = board.getPieceAt(move.x, move.y)->checkifWhite();

      if (isWhite && targetIsWhite)
        break;
      else if (isWhite && !targetIsWhite) {
        potentialMoves.push_back(move);
        break;
      } else if (!isWhite && !targetIsWhite)
        break;
      else if (!isWhite && targetIsWhite) {
        potentialMoves.push_back(move);
        break;
      }
    }
  }

  // Check down-left
  for (int i = row + 1, j = col - 1; i < 8 && j >= 0; i++, j--) {
    glm::ivec2 move{j, i};

    Piece *piece = board.getPieceAt(move.x, move.y);

    if (!piece) {
      potentialMoves.push_back(move);
    } else {
      bool targetIsWhite = board.getPieceAt(move.x, move.y)->checkifWhite();

      if (isWhite && targetIsWhite)
        break;
      else if (isWhite && !targetIsWhite) {
        potentialMoves.push_back(move);
        break;
      } else if (!isWhite && !targetIsWhite)
        break;
      else if (!isWhite && targetIsWhite) {
        potentialMoves.push_back(move);
        break;
      }
    }
  }

  return potentialMoves;
}

void Bishop::render(SpriteSheet &sheet, Shader &shader) {
  // Calculate screen space coordinates
  float screenX = boardPos.x * SQUARE_SIZE;
  float screenY = (7 - boardPos.y) * SQUARE_SIZE;

  if (animation.isAnimating) {
    float t = glm::clamp((float)(glfwGetTime() - animation.startTime) / 0.3f,
                         0.0f, 1.0f);
    glm::vec2 interpolated =
        glm::mix(animation.startPos, animation.targetPos, t);
    screenX = interpolated.x;
    screenY = interpolated.y;

    if (t >= 1.0)
      animation.isAnimating = false;
  } else {
    screenX = boardPos.x * SQUARE_SIZE;
    screenY = (7 - boardPos.y) * SQUARE_SIZE;
  }

  // Now to place the pieces in the correct world coordinates, we need a model
  // matrix for translation and scaling (because pixels)
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

  // Idea is to remove whichever of steps the pieces moves first, then just use
  // oneTwo[0] for the next
  // If initial direction is up, next can be left or right
  glm::ivec2 move1 = boardPos + glm::ivec2{0, -1};
  potentialMoves.push_back(move1 + glm::ivec2{2, 0});  // Right
  potentialMoves.push_back(move1 + glm::ivec2{-2, 0}); // Left

  glm::ivec2 move2 = boardPos + glm::ivec2{0, -2};
  potentialMoves.push_back(move2 + glm::ivec2{1, 0});  // Right
  potentialMoves.push_back(move2 + glm::ivec2{-1, 0}); // Left

  // If initial direction is down, next can be left or right
  move1 = boardPos + glm::ivec2{0, 1};
  potentialMoves.push_back(move1 + glm::ivec2{2, 0});  // Right
  potentialMoves.push_back(move1 + glm::ivec2{-2, 0}); // Left

  move2 = boardPos + glm::ivec2{0, 2};
  potentialMoves.push_back(move2 + glm::ivec2{1, 0});  // Right
  potentialMoves.push_back(move2 + glm::ivec2{-1, 0}); // Left

  // If initial direction is left, next can be up or down
  move1 = boardPos + glm::ivec2{-1, 0};
  potentialMoves.push_back(move1 + glm::ivec2{0, -2}); // Up
  potentialMoves.push_back(move1 + glm::ivec2{0, 2});  // Down

  move2 = boardPos + glm::ivec2{-2, 0};
  potentialMoves.push_back(move2 + glm::ivec2{0, -1}); // Up
  potentialMoves.push_back(move2 + glm::ivec2{0, 1});  // Down

  // If initial direction is right, next can be up or down
  move1 = boardPos + glm::ivec2{1, 0};
  potentialMoves.push_back(move1 + glm::ivec2{0, -2}); // Up
  potentialMoves.push_back(move1 + glm::ivec2{0, 2});  // Down

  move2 = boardPos + glm::ivec2{2, 0};
  potentialMoves.push_back(move2 + glm::ivec2{0, -1}); // Up
  potentialMoves.push_back(move2 + glm::ivec2{0, 1});  // Down

  // Remove moves that land on a piece of the same colour or isOutOfBounds
  potentialMoves.erase(
      std::remove_if(potentialMoves.begin(), potentialMoves.end(),
                     [&](const glm::ivec2 &move) {
                       // If the move is already out of bounds then discard it
                       if (board.isOutOfBounds(move))
                         return true;

                       // Else check colour match
                       bool sameColour = false;
                       Piece *target = board.getPieceAt(move.x, move.y);

                       // Make sure target is a piece and not an empty tile
                       // (nullptr)
                       if (target) {
                         if ((isWhite && target->checkifWhite()) ||
                             (!isWhite && !target->checkifWhite()))
                           sameColour = true;
                         return sameColour;
                       }
                       return false;
                     }),
      potentialMoves.end());

  return potentialMoves;
}

void Knight::render(SpriteSheet &sheet, Shader &shader) {
  // Calculate screen space coordinates
  float screenX = boardPos.x * SQUARE_SIZE;
  float screenY = (7 - boardPos.y) * SQUARE_SIZE;

  if (animation.isAnimating) {
    float t = glm::clamp((float)(glfwGetTime() - animation.startTime) / 0.3f,
                         0.0f, 1.0f);
    glm::vec2 interpolated =
        glm::mix(animation.startPos, animation.targetPos, t);
    screenX = interpolated.x;
    screenY = interpolated.y;

    if (t >= 1.0)
      animation.isAnimating = false;
  } else {
    screenX = boardPos.x * SQUARE_SIZE;
    screenY = (7 - boardPos.y) * SQUARE_SIZE;
  }

  // Now to place the pieces in the correct world coordinates, we need a model
  // matrix for translation and scaling (because pixels)
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
  std::vector<glm::ivec2> potentialMoves;

  int8_t row = boardPos.y, col = boardPos.x;

  // Check up
  for (int i = row + 1; i < 8; i++) {
    glm::ivec2 move{col, i};

    Piece *piece = board.getPieceAt(move.x, move.y);

    if (!piece) {
      potentialMoves.push_back(move);
    } else {
      // If white passing over white
      if (isWhite && board.getPieceAt(move.x, move.y)->checkifWhite())
        break;
      else if (isWhite && !board.getPieceAt(move.x, move.y)->checkifWhite()) {
        potentialMoves.push_back(move);
        break;
      }
    }
  }

  // Check down
  for (int i = row - 1; i >= 0; i--) {
    glm::ivec2 move{col, i};

    Piece *piece = board.getPieceAt(move.x, move.y);

    if (!piece) {
      potentialMoves.push_back(move);
    } else {
      bool targetIsWhite = board.getPieceAt(move.x, move.y)->checkifWhite();

      if (isWhite && targetIsWhite)
        break;
      else if (isWhite && !targetIsWhite) {
        potentialMoves.push_back(move);
        break;
      } else if (!isWhite && !targetIsWhite)
        break;
      else if (!isWhite && targetIsWhite) {
        potentialMoves.push_back(move);
        break;
      }
    }
  }

  // Check left
  for (int i = col - 1; i >= 0; i--) {
    glm::ivec2 move{i, row};

    Piece *piece = board.getPieceAt(move.x, move.y);

    if (!piece) {
      potentialMoves.push_back(move);
    } else {
      // If white passing over white
      if (isWhite && board.getPieceAt(move.x, move.y)->checkifWhite())
        break;
      else if (isWhite && !board.getPieceAt(move.x, move.y)->checkifWhite()) {
        potentialMoves.push_back(move);
        break;
      }
    }
  }

  // Check right
  for (int i = col + 1; i < 8; i++) {
    glm::ivec2 move{i, row};

    Piece *piece = board.getPieceAt(move.x, move.y);

    if (!piece) {
      potentialMoves.push_back(move);
    } else {
      // If white passing over white
      if (isWhite && board.getPieceAt(move.x, move.y)->checkifWhite())
        break;
      else if (isWhite && !board.getPieceAt(move.x, move.y)->checkifWhite()) {
        potentialMoves.push_back(move);
        break;
      }
    }
  }

  // Check up-right
  for (int i = row - 1, j = col + 1; i >= 0 && j < 8; i--, j++) {
    glm::ivec2 move{j, i};

    Piece *piece = board.getPieceAt(move.x, move.y);

    if (!piece) {
      potentialMoves.push_back(move);
    } else {
      bool targetIsWhite = board.getPieceAt(move.x, move.y)->checkifWhite();

      if (isWhite && targetIsWhite)
        break;
      else if (isWhite && !targetIsWhite) {
        potentialMoves.push_back(move);
        break;
      } else if (!isWhite && !targetIsWhite)
        break;
      else if (!isWhite && targetIsWhite) {
        potentialMoves.push_back(move);
        break;
      }
    }
  }

  // Check up-left
  for (int i = row - 1, j = col - 1; i >= 0 && j >= 0; i--, j--) {
    glm::ivec2 move{j, i};

    Piece *piece = board.getPieceAt(move.x, move.y);

    if (!piece) {
      potentialMoves.push_back(move);
    } else {
      bool targetIsWhite = board.getPieceAt(move.x, move.y)->checkifWhite();

      if (isWhite && targetIsWhite)
        break;
      else if (isWhite && !targetIsWhite) {
        potentialMoves.push_back(move);
        break;
      } else if (!isWhite && !targetIsWhite)
        break;
      else if (!isWhite && targetIsWhite) {
        potentialMoves.push_back(move);
        break;
      }
    }
  }

  // Check down-right
  for (int i = row + 1, j = col + 1; i < 8 && j < 8; i++, j++) {
    glm::ivec2 move{j, i};

    Piece *piece = board.getPieceAt(move.x, move.y);

    if (!piece) {
      potentialMoves.push_back(move);
    } else {
      bool targetIsWhite = board.getPieceAt(move.x, move.y)->checkifWhite();

      if (isWhite && targetIsWhite)
        break;
      else if (isWhite && !targetIsWhite) {
        potentialMoves.push_back(move);
        break;
      } else if (!isWhite && !targetIsWhite)
        break;
      else if (!isWhite && targetIsWhite) {
        potentialMoves.push_back(move);
        break;
      }
    }
  }

  // Check down-left
  for (int i = row + 1, j = col - 1; i < 8 && j >= 0; i++, j--) {
    glm::ivec2 move{j, i};

    Piece *piece = board.getPieceAt(move.x, move.y);

    if (!piece) {
      potentialMoves.push_back(move);
    } else {
      bool targetIsWhite = board.getPieceAt(move.x, move.y)->checkifWhite();

      if (isWhite && targetIsWhite)
        break;
      else if (isWhite && !targetIsWhite) {
        potentialMoves.push_back(move);
        break;
      } else if (!isWhite && !targetIsWhite)
        break;
      else if (!isWhite && targetIsWhite) {
        potentialMoves.push_back(move);
        break;
      }
    }
  }

  return potentialMoves;
}

void Queen::render(SpriteSheet &sheet, Shader &shader) {
  // Calculate screen space coordinates
  float screenX = boardPos.x * SQUARE_SIZE;
  float screenY = (7 - boardPos.y) * SQUARE_SIZE;

  if (animation.isAnimating) {
    float t = glm::clamp((float)(glfwGetTime() - animation.startTime) / 0.3f,
                         0.0f, 1.0f);
    glm::vec2 interpolated =
        glm::mix(animation.startPos, animation.targetPos, t);
    screenX = interpolated.x;
    screenY = interpolated.y;

    if (t >= 1.0)
      animation.isAnimating = false;
  } else {
    screenX = boardPos.x * SQUARE_SIZE;
    screenY = (7 - boardPos.y) * SQUARE_SIZE;
  }

  // Now to place the pieces in the correct world coordinates, we need a model
  // matrix for translation and scaling (because pixels)
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
      boardPos + glm::ivec2{0, -1},  // 1 forward
      boardPos + glm::ivec2{0, 1},   // 1 backward
      boardPos + glm::ivec2{-1, 0},  // 1 left
      boardPos + glm::ivec2{1, 0},   // 1 right
      boardPos + glm::ivec2{-1, -1}, // up left
      boardPos + glm::ivec2{-1, 1},  // up right
      boardPos + glm::ivec2{1, -1},  // down left
      boardPos + glm::ivec2{1, 1}    // down right
  };

  // Remove moves that land on a piece of the same colour or isOutOfBounds
  potentialMoves.erase(
      std::remove_if(potentialMoves.begin(), potentialMoves.end(),
                     [&](const glm::ivec2 &move) {
                       // If the move is already out of bounds then discard it
                       if (board.isOutOfBounds(move))
                         return true;

                       // Else check colour match
                       bool sameColour = false;
                       Piece *target = board.getPieceAt(move.x, move.y);

                       // Make sure target is a piece and not an empty tile
                       // (nullptr)
                       if (target) {
                         if ((isWhite && target->checkifWhite()) ||
                             (!isWhite && !target->checkifWhite()))
                           sameColour = true;
                         return sameColour;
                       }
                       return false;
                     }),
      potentialMoves.end());

  return potentialMoves;
}

void King::render(SpriteSheet &sheet, Shader &shader) {
  // Calculate screen space coordinates
  float screenX = boardPos.x * SQUARE_SIZE;
  float screenY = (7 - boardPos.y) * SQUARE_SIZE;

  if (animation.isAnimating) {
    float t = glm::clamp((float)(glfwGetTime() - animation.startTime) / 0.3f,
                         0.0f, 1.0f);
    glm::vec2 interpolated =
        glm::mix(animation.startPos, animation.targetPos, t);
    screenX = interpolated.x;
    screenY = interpolated.y;

    if (t >= 1.0)
      animation.isAnimating = false;
  } else {
    screenX = boardPos.x * SQUARE_SIZE;
    screenY = (7 - boardPos.y) * SQUARE_SIZE;
  }

  // Now to place the pieces in the correct world coordinates, we need a model
  // matrix for translation and scaling (because pixels)
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
