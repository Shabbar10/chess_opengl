cmake_minimum_required(VERSION 3.10)
project(OpenGLProject)

add_compile_options(-Wall -Wextra)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(
  ${PROJECT_SOURCE_DIR}/include
  /usr/include/Tracy
)

add_executable(${PROJECT_NAME}
  src/glad.c
  src/main.cpp
  src/Shader.cpp
  src/Board.cpp
  src/SpriteSheet.cpp
  src/Piece.cpp
)

target_link_libraries(${PROJECT_NAME}
  GL
  ${PROJECT_SOURCE_DIR}/lib/libglfw3.a
  /usr/lib/libTracyClient.a
)

target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:DEBUG>:TRACY_ENABLE>)
