#include <Shader.h>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const char *vertexPath, const char *fragmentPath) {
  // 1. Retrieve the vertex and fragment source code
  std::string vertexCode;
  std::string fragmentCode;
  std::ifstream vShaderFile;
  std::ifstream fShaderFile;

  // vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  // fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try {
    // Open files
    vShaderFile.open(vertexPath);
    if (!vShaderFile.is_open()) {
      std::cerr << "ERR::SHADER::VERTEX_FILE_NOT_FOUND: " << vertexPath
                << std::endl;
      throw std::runtime_error("Vertex shader file not found\n");
    }

    fShaderFile.open(fragmentPath);
    if (!fShaderFile.is_open()) {
      std::cerr << "ERR::SHADER::FRAGMENT_FILE_NOT_FOUND: " << fragmentPath
                << std::endl;
      throw std::runtime_error("Fragment shader file not found\n");
    }

    std::stringstream vShaderStream, fShaderStream;
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();

    vShaderFile.close();
    fShaderFile.close();

    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();
  } catch (std::ifstream::failure &e) {
    std::cerr << "ERR::SHADER::FILE_NOT_READ: " << e.what() << std::endl;
  }

  const char *vShaderCode = vertexCode.c_str();
  const char *fShaderCode = fragmentCode.c_str();

  // 2. Compile shaders
  unsigned int vertex, fragment;
  int success;
  char infoLog[512];

  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vShaderCode, NULL);
  glCompileShader(vertex);

  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex, 512, NULL, infoLog);
    std::cout << "ERR::SHADER::VERTEX::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }

  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fShaderCode, NULL);
  glCompileShader(fragment);

  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment, 512, NULL, infoLog);
    std::cout << "ERR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }

  // Shader Program
  ID = glCreateProgram();
  glAttachShader(ID, vertex);
  glAttachShader(ID, fragment);
  glLinkProgram(ID);

  glGetProgramiv(ID, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(ID, 512, NULL, infoLog);
    std::cout << "ERR::SHADER::PROGRAM::LINKING_FAILED\n"
              << infoLog << std::endl;
  }

  // Delete shaders since they're unnecessary after linking is done
  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

void Shader::use() { glUseProgram(ID); }

void Shader::setBool(const std::string &name, bool value) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, bool value) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setFloat(const std::string &name, bool value) const {
  glUniform1f(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setMat4(const std::string &name, glm::mat4 matrix) const {
  glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                     glm::value_ptr(matrix));
}

void Shader::setVec2(const std::string &name, float x, float y) const {
  glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}
