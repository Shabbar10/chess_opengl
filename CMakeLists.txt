cmake_minimum_required(VERSION 3.10)
project(OpenGLProject)

add_compile_options(-Wall -Wextra)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(
  ${PROJECT_SOURCE_DIR}/include
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
  "-framework OpenGL"
  "-framework Cocoa"
  "-framework IOKit"
  "-framework CoreFoundation"
  ${PROJECT_SOURCE_DIR}/lib/libglfw3.a
)

target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:DEBUG>:TRACY_ENABLE>)
