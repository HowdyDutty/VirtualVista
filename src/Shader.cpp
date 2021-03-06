
#include <iostream>
#include <fstream>
#include <sstream>

#include "vv/Shader.h"

namespace vv
{
  /////////////////////////////////////////////////////////////////////// public
  Shader::Shader(std::string path, std::string name) :
    Resource(path, name)
  {
  }


  Shader::~Shader()
  {
    glDeleteProgram(program_id_);
  }


  bool Shader::init()
  {
    program_id_ = glCreateProgram();

    std::string vert_source = loadShaderFromFile(file_path_ + file_name_ + ".vert");
    std::string frag_source = loadShaderFromFile(file_path_ + file_name_ + ".frag");

    return createProgram(vert_source, frag_source);
  }


  GLuint Shader::getProgramId() const
  {
    return program_id_;
  }


  GLint Shader::getUniformLocation(std::string name) const
  {
    GLint location = glGetUniformLocation(program_id_, name.c_str());
    if (location == -1)
      std::cerr << "WARNING: attempted access of non-existant uniform location: "
      << name << " within shader: " << program_id_ << "\n" <<
      "filename: " << file_path_ + file_name_ << "\n";

    return location;
  }


  void Shader::useProgram()
  {
    glUseProgram(program_id_);
  }


  ////////////////////////////////////////////////////////////////////// private
  std::string Shader::loadShaderFromFile(const std::string filename)
  {
    std::ifstream file(filename);
    std::string shader_source;

    try
    {
      std::stringstream shader_stream;
      shader_stream << file.rdbuf();
      file.close();

      shader_source = shader_stream.str();
    } catch (std::exception e)
    {
      std::cerr << "ERROR: file: " << filename << " not successfully read:\n";
      exit(EXIT_FAILURE); // todo: maybe don't exit
    }

    return shader_source;
  }


  bool Shader::createProgram(std::string vert_source, std::string frag_source)
  {
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    const GLchar *v_source = vert_source.c_str();
    glShaderSource(vertex_shader, 1, &v_source, NULL);
    glCompileShader(vertex_shader);

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    const GLchar *f_source = frag_source.c_str();
    glShaderSource(fragment_shader, 1, &f_source, NULL);
    glCompileShader(fragment_shader);

    auto compilationSuccess = [](GLuint shader, std::string type) -> bool
    {
      int info_log_length = 0;
      GLint compile_success = GL_FALSE;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);
      glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_success);

      if (!compile_success)
      {
        char *buffer = new char[info_log_length + 1];
        glGetShaderInfoLog(shader, info_log_length, NULL, buffer);
        std::cerr << "ERROR: failed to compile " << type << " shader:\n" << buffer << "\n";
        glDeleteShader(shader);
        delete buffer;
        return false;
      }
      return true;
    };

    bool vert_success = compilationSuccess(vertex_shader, "vertex");
    bool frag_success = compilationSuccess(fragment_shader, "fragment");

    if (!vert_success || !frag_success)
    {
      glDeleteShader(vertex_shader);
      glDeleteShader(fragment_shader);
      return false;
    }

    glAttachShader(program_id_, vertex_shader);
    glAttachShader(program_id_, fragment_shader);
    glLinkProgram(program_id_);

    GLint link_success = GL_FALSE;
    int program_info_log_length = 0;

    glGetProgramiv(program_id_, GL_INFO_LOG_LENGTH, &program_info_log_length);
    glGetProgramiv(program_id_, GL_LINK_STATUS, &link_success);

    if (!link_success)
    {
      char *buffer = new char[program_info_log_length + 1];
      glGetProgramInfoLog(program_id_, program_info_log_length, NULL, buffer);
      std::cerr << "Error: program failed to link correctly.\n\n" << buffer << "\n";
      glDeleteProgram(program_id_);
      delete buffer;
      return false;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return true;
  }
} // namespace vv