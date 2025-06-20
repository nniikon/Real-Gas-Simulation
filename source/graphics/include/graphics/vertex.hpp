#ifndef VERTEX_HPP_
#define VERTEX_HPP_

#include <cassert>

#include <glad/gl.h>

#include "graphics/shaders.hpp"
#include "helpers/helpers.hpp"

#include "graphics/gl_log.hpp"

namespace gas {
namespace grx {

// vbo = memory buffer for vertexes
class VertexBuffer {
  public:
    enum class Type : GLenum {
        kArrayBuffer = GL_ARRAY_BUFFER,
        kInvalid = GL_INVALID_ENUM,
    };

    static GLenum BufferType(Type buffer_type) {
        return hlp::FromEnum(buffer_type);
    }
  private:
    Type buffer_type_;
    GLuint vertex_buffer_obj_;
    bool is_binded_;
  public:
    explicit VertexBuffer(Type buffer_type) 
        : buffer_type_{buffer_type}, vertex_buffer_obj_{0}, is_binded_{false}
    {
        // generates name for vertex buffer objects
        glGenBuffers(1, &vertex_buffer_obj_); GlDbg();
    }

    ~VertexBuffer() {
        // zero value is safe
        glDeleteBuffers(1, &vertex_buffer_obj_); GlDbg();
    }

    VertexBuffer(const VertexBuffer& other) = delete;
    VertexBuffer& operator=(const VertexBuffer& other) = delete;

    VertexBuffer(VertexBuffer&& other) noexcept
        : buffer_type_{other.buffer_type_}, 
        vertex_buffer_obj_{other.vertex_buffer_obj_},
        is_binded_{other.is_binded_}
    {
        other.buffer_type_ = Type::kInvalid;
        other.vertex_buffer_obj_ = 0;
        other.is_binded_ = false;
    }
    VertexBuffer& operator=(VertexBuffer&& other) noexcept {
        if (this == &other) { return *this; }

        glDeleteBuffers(1, &vertex_buffer_obj_); GlDbg();

        buffer_type_ = other.buffer_type_;
        vertex_buffer_obj_ = other.vertex_buffer_obj_;
        is_binded_ = other.is_binded_;
        other.buffer_type_ = Type::kInvalid;
        other.vertex_buffer_obj_ = 0;
        other.is_binded_ = false;

        return *this;
    }

    GLuint operator()() { return vertex_buffer_obj_; }
    Type Type() { return buffer_type_; }

    // bind means associate vbo with attribute type before any operation
    void Bind() {
        glBindBuffer(BufferType(buffer_type_), vertex_buffer_obj_); GlDbg();
        is_binded_ = true;
    }
    void Unbind() {
        glBindBuffer(BufferType(buffer_type_), 0); GlDbg();
        is_binded_ = false;
    }
    bool IsBinded() { return is_binded_; }

    void AllocateAndCopy(GLsizeiptr size, const void* data, GLenum usage_type) {
        assert(IsBinded());

        glBufferData(
            BufferType(buffer_type_), 
            size, 
            data, 
            usage_type
        );
    }
   
    void Allocate(GLsizeiptr size, GLenum usage_type) {
        assert(IsBinded());

        // nullptr = dont copy
        AllocateAndCopy(size, nullptr, usage_type);
    }

    void Copy(GLsizeiptr size, const void* data) {
        assert(IsBinded());

        glBufferSubData(
            BufferType(buffer_type_),
            0, // offset
            size, 
            data
        );
    }
};

// vao stores information for vbo's
class VertexArray {
  private:
    GLuint vertex_array_obj_;
    bool is_binded_;
  public:
    VertexArray() 
        : vertex_array_obj_{0},
        is_binded_{false}
    {
        // generates name for vertex array objects
        glGenVertexArrays(1, &vertex_array_obj_); GlDbg();
    }

    VertexArray(const VertexArray& other) = delete;
    VertexArray& operator=(const VertexArray& other) = delete;

    VertexArray(VertexArray&& other) noexcept
        : vertex_array_obj_{other.vertex_array_obj_},
        is_binded_{other.is_binded_}
    {
        other.vertex_array_obj_ = 0;
        other.is_binded_ = false;
    }
    VertexArray& operator=(VertexArray&& other) noexcept {
        if (this == &other) { return *this; }

        glDeleteVertexArrays(1, &vertex_array_obj_); GlDbg();

        vertex_array_obj_ = other.vertex_array_obj_;
        is_binded_ = other.is_binded_;
        other.vertex_array_obj_ = 0;
        other.is_binded_ = false;

        return *this;
    }

    ~VertexArray() {
        // zero value is safe 
        glDeleteVertexArrays(1, &vertex_array_obj_); GlDbg();
    }

    GLuint operator()() const { return vertex_array_obj_; }

    // always bind vao before vbo updates
    void Bind() {
        glBindVertexArray(vertex_array_obj_);
        is_binded_ = true;
    }
    void Unbind() {
        glBindVertexArray(0);
        is_binded_ = false;
    }
    bool IsBinded() { return is_binded_; }

    // defines how data in location is stored
    void AddAttribute(
        GLuint index, GLint size, GLenum type, GLboolean is_norm, GLsizei stride, const void* data
    ) {
        glVertexAttribPointer(index, size, type, is_norm, stride, data); GlDbg();
    }

    // enables attribute and stores it in vao
    void EnableAttribute(GLuint index) {
        glEnableVertexAttribArray(index); GlDbg();
    }

    void AddAndEnableAttribute(
        GLuint index, GLint size, GLenum type, GLboolean is_norm, GLsizei stride, const void* data
    ) {
        AddAttribute(index, size, type, is_norm, stride, data);
        EnableAttribute(index);
    }

    void AddAndEnableAttribute(
        const ShaderProgram& program, 
        const std::string& name, 
        GLint size, 
        GLenum type, 
        GLboolean is_norm, 
        GLsizei stride, 
        const void* data
    ) {
        auto&& index = glGetAttribLocation(program(), name.c_str()); GlDbg();
        AddAndEnableAttribute(
            static_cast<GLuint>(index), size, type, is_norm, stride, data
        );
    }
};

} // namespace grx
} // namespace gas

#endif // VERTEX_HPP_
