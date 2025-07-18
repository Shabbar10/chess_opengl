// clang-format off
#include "Board.h"
#include "glm/gtc/matrix_transform.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include "SpriteSheet.h"
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include <Shader.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_resize2.h>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const float SCR_WIDTH = 800;
const float SCR_HEIGHT = 800;

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  // glfw window creation
  // --------------------
  GLFWwindow *window =
      glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Chess", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifdef _WIN32
  Shader ourShader("src\\vShader.vert", "src\\fShader.frag");
#else
  Shader boardShader("src/vShader.vert", "src/fShader.frag");
  Shader pieceShader("src/piece.vert", "src/piece.frag");
#endif

  Board board;
  glfwSetWindowUserPointer(window, &board);

  // Mouse button callback
  glfwSetMouseButtonCallback(
      window, [](GLFWwindow *win, int button, int action, int) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
          double x, y;
          glfwGetCursorPos(win, &x, &y);

          Board *board = static_cast<Board *>(glfwGetWindowUserPointer(win));
          if (board) {
            if (board->getGameState() == GameState::Playing)
              board->handleClick(static_cast<float>(x), static_cast<float>(y));
            else if (board->getGameState() == GameState::PromotionPending)
              board->handlePromotionClick(static_cast<float>(x),
                                          static_cast<float>(y));
          }
        }
      });

  SpriteSheet blackSheet("chess_sprites/16x16_pieces/BlackPieces.png");
  SpriteSheet whiteSheet("chess_sprites/16x16_pieces/WhitePieces_Wood.png");

  board.initializeBoard(blackSheet, whiteSheet);
  blackSheet.initQuad();
  whiteSheet.initQuad();

  boardShader.use();
  glm::mat4 projection = glm::ortho(0.f, SCR_WIDTH, 0.f, SCR_HEIGHT);
  boardShader.setMat4("uProjection", projection);

  pieceShader.use();
  pieceShader.setInt("uTexture", 0);
  pieceShader.setMat4("uProjection", projection);

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    boardShader.use();
    board.render(blackSheet, whiteSheet, pieceShader, projection);

    if (board.checkIfWon()) {
      std::cout << "GAME OVER!\n\n";
      break;
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow *, int width, int height) {
  // make sure the viewport matches the new window dimensions; note that width
  // and height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}
