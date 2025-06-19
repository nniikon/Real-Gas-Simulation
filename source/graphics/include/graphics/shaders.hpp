#ifndef GRAPHICS_SHADERS_HPP_
#define GRAPHICS_SHADERS_HPP_

#include <array>
#include <filesystem>
#include <stdexcept>
#include <vector>
#include <string>
#include <cassert>
#include <utility>

#include <glad/gl.h>

#include "graphics/gl_log.hpp"

namespace gas {
namespace grx {

class Shader {
  public:
    enum class Type {
        kFragment,
        kVertex,
        kInvalid,
    };
     
    static GLenum ShaderType(Type shader_type) {
        switch (shader_type) {
            case Type::kFragment: return GL_FRAGMENT_SHADER;
            case Type::kVertex: return GL_VERTEX_SHADER;
            case Type::kInvalid: 
            default: 
                throw std::runtime_error{"invalid shader type conversion"};
        }
    }
  private:
    Type shader_type_;
    GLuint shader_obj_;
    bool is_compiled_;

    bool NoCompileErrors() {
        GLint status = 0;
        // getter for shader object parameter
        // GL_TRUE == succesfull 
        glGetShaderiv(shader_obj_, GL_COMPILE_STATUS, &status); GlDbg();
        return status == GL_TRUE;
    }

    std::string Error() {
        constexpr size_t kMaxErrorLen = 256;
        static std::array<GLchar, kMaxErrorLen> error_array{}; 

        glGetShaderInfoLog(shader_obj_, kMaxErrorLen, nullptr, error_array.data()); GlDbg();

        return error_array.data();
    }
  public:
    explicit Shader(Type shader_type) 
        : shader_type_{shader_type}, shader_obj_{0}, is_compiled_{false} 
    {
        // creates shader object
        shader_obj_ = glCreateShader(ShaderType(shader_type)); GlDbg(); 
    }

    Shader(const Shader& other) = delete;
    Shader& operator=(const Shader& other) = delete;

    Shader(Shader&& other) noexcept
        : shader_type_{other.shader_type_}, 
        shader_obj_{other.shader_obj_}, 
        is_compiled_{other.is_compiled_} 
    {
        other.shader_type_ = Type::kInvalid;
        other.shader_obj_ = 0;
        other.is_compiled_ = false;
    }
    Shader& operator=(Shader&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        glDeleteShader(shader_obj_); GlDbg();
        shader_type_ = other.shader_type_;
        shader_obj_ = other.shader_obj_;
        is_compiled_ = other.is_compiled_;

        other.shader_type_ = Type::kInvalid;
        other.shader_obj_ = 0;
        other.is_compiled_ = false;

        return *this;
    }

    ~Shader() {
        // frees memory and invalidates shader_obj_
        glDeleteShader(shader_obj_); GlDbg();
    }

    void Compile(const std::string& shader_source) {
        auto&& shader_cstr = shader_source.data(); 
        // replaces shader sources for shader object
        // string is copied
        // not compiles, just copies string
        glShaderSource(
            shader_obj_, 
            1 /* number of sources */, 
            &shader_cstr, 
            nullptr /* means strings are null terminated */
        ); GlDbg(); 

        glCompileShader(shader_obj_); GlDbg();
        if (!NoCompileErrors()) {
            throw std::runtime_error{"Unable to compile shader: " + Error()};
        }

        is_compiled_ = true;
    }

    GLuint operator()() const { return shader_obj_; }
    Type Type() const { return shader_type_; }
    bool IsVertex() const { return Type() == Type::kVertex; }
    bool IsFragment() const { return Type() == Type::kFragment; }
    bool IsCompiled() const { return is_compiled_; }
};

// Program object provide mechanism to link(connect) shaders throught attach and link calls
class ShaderProgram {
  private:
    GLuint program_obj_;
    bool is_linked_;

    bool NoLinkErrors() {
        GLint status = 0;
        // getter for program object parameter
        // GL_TRUE == succesfull 
        glGetProgramiv(program_obj_, GL_LINK_STATUS, &status); GlDbg();
        return status == GL_TRUE;
    }

    std::string Error() {
        constexpr size_t kMaxErrorLen = 256;
        static std::array<GLchar, kMaxErrorLen> error_array{}; 

        glGetProgramInfoLog(program_obj_, kMaxErrorLen, nullptr, error_array.data()); GlDbg();

        return error_array.data();
    }
  public:
    ShaderProgram()
        : program_obj_{0}, is_linked_{false}
    {
        // creates program object
        program_obj_ = glCreateProgram(); GlDbg();
    }

    ShaderProgram(const ShaderProgram& other) = delete;
    ShaderProgram& operator=(const ShaderProgram& other) = delete;

    ShaderProgram(ShaderProgram&& other) noexcept
        : program_obj_{other.program_obj_}, is_linked_{other.is_linked_}
    {
        other.program_obj_ = 0;
        other.is_linked_ = false;
    }

    ShaderProgram& operator=(ShaderProgram&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        glDeleteProgram(program_obj_); GlDbg();

        program_obj_ = other.program_obj_;
        is_linked_ = other.is_linked_;

        other.program_obj_ = 0;
        other.is_linked_ = false;

        return *this;
    }

    ~ShaderProgram() {
        // frees memory and invalidates program_obj_
        glDeleteProgram(program_obj_); GlDbg();
    }

    void AttachAndLink(const Shader& vertex, const Shader& fragment) {
        assert(vertex.IsVertex() && vertex.IsCompiled());
        assert(fragment.IsFragment() && fragment.IsCompiled());
        
        // before attachment shader object should be compiled
        glAttachShader(program_obj_, vertex()); GlDbg();
        glAttachShader(program_obj_, fragment()); GlDbg();

        // links shaders 
        glLinkProgram(program_obj_); GlDbg();

        // detaches shaders after linking
        glDetachShader(program_obj_, vertex()); GlDbg();
        glDetachShader(program_obj_, fragment()); GlDbg();

        if (!NoLinkErrors()) {
            throw std::runtime_error{"Unable to link program: " + Error()};
        }

        is_linked_ = true;
    }

    // NOTE should it be const?
    void Use() const {
        glUseProgram(program_obj_); GlDbg();
    }

    GLuint operator()() const { return program_obj_; }
    bool IsLinked() const { return is_linked_; }
};

std::string LoadShader(const std::filesystem::path& shader_path);

} // namespace grx
} // namespace gas

#endif // GRAPHICS_SHADERS_HPP_
