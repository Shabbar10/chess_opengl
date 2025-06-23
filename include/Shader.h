#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>
#include <string>

class Shader {
public:
  unsigned int ID; // program ID

  // constructor reads and builds the shader
  Shader(const char *vertexPath, const char *fragmentPath);

  // use/activate the shader
  void use();

  // utility uniform functions
  void setBool(const std::string &name, bool value) const;
  void setInt(const std::string &name, bool value) const;
  void setFloat(const std::string &name, bool value) const;
  void setMat4(const std::string &name, glm::mat4 matrix) const;
  void setVec2(const std::string &name, float x, float y) const;
};

#endif
