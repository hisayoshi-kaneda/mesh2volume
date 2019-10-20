#pragma once

#ifndef SHADER_H
#define SHADER_H

#include "Texture2D.h"
#include "Texture2DArray.h"
#include "Texture3D.h"
#include "common.h"

class Shader {
public:
    GLuint program_id = 0;

    void create(string &vert_file, string &frag_file) {
        program_id = build_shader_program(vert_file, frag_file);
    }

    GLuint compile_shader(const string &filename, GLuint type) {
        GLuint shaderId = glCreateShader(type);

        ifstream reader;
        size_t code_size;
        string code;
        reader.open(filename.c_str(), ios::in);
        if (!reader.is_open()) {
            fprintf(stderr, "Failed to load a shader: %s\n", filename.c_str());
            abort();
        }

        reader.seekg(0, ios::end);
        code_size = reader.tellg();
        code.resize(code_size);
        reader.seekg(0);
        reader.read(&code[0], code_size);

        reader.close();

        const char *code_chars = code.c_str();
        glShaderSource(shaderId, 1, &code_chars, NULL);
        glCompileShader(shaderId);

        GLint compile_status;
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compile_status);
        if (compile_status == GL_FALSE) {
            fprintf(stderr, "Failed to compile a shader!\n");
            GLint log_length;
            glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &log_length);
            if (log_length > 0) {
                GLsizei length;
                string err_msg;
                err_msg.resize(log_length);
                glGetShaderInfoLog(shaderId, log_length, &length, &err_msg[0]);

                fprintf(stderr, "[ ERROR ] %s\n", err_msg.c_str());
                fprintf(stderr, "%s\n", code.c_str());
            }
            exit(1);
        }
        return shaderId;
    }

    GLuint build_shader_program(const string &vert_shader_file, const string frag_shader_file) {
        // �V�F�[�_�̍쐬
        GLuint vertShader_id = compile_shader(vert_shader_file, GL_VERTEX_SHADER);
        GLuint fragShader_id = compile_shader(frag_shader_file, GL_FRAGMENT_SHADER);

        program_id = glCreateProgram();
        glAttachShader(program_id, vertShader_id);
        glAttachShader(program_id, fragShader_id);
        glLinkProgram(program_id);

        GLint link_state;
        glGetProgramiv(program_id, GL_LINK_STATUS, &link_state);
        if (link_state == GL_FALSE) {
            fprintf(stderr, "Failed to link shaders!\n");

            GLint log_length;
            glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);
            if (log_length > 0) {
                GLsizei length;
                std::string errMsg;
                errMsg.resize(log_length);
                glGetProgramInfoLog(program_id, log_length, &length, &errMsg[0]);

                fprintf(stderr, "[ ERROR ] %s\n", errMsg.c_str());
            }
            exit(1);
        }
        glUseProgram(0);
        return program_id;
    }

    void set_uniform_value(glm::mat4 val, const char *val_name) {
        GLuint loc_id = glGetUniformLocation(program_id, val_name);
        glUniformMatrix4fv(loc_id, 1, GL_FALSE, glm::value_ptr(val));
    }

    void set_uniform_value(glm::vec3 val, const char *val_name) {
        GLuint loc_id = glGetUniformLocation(program_id, val_name);
        glUniform3fv(loc_id, 1, glm::value_ptr(val));
    }

    void set_uniform_value(glm::vec2 val, const char *val_name) {
        GLuint loc_id = glGetUniformLocation(program_id, val_name);
        glUniform2fv(loc_id, 1, glm::value_ptr(val));
    }

    void set_uniform_value(float val, const char *val_name) {
        GLuint loc_id = glGetUniformLocation(program_id, val_name);
        glUniform1f(loc_id, val);
    }

    void set_uniform_texture(Texture2D &texture, const char *val_name) {
        texture.bind();
        glUniform1i(glGetUniformLocation(program_id, val_name), texture.textureUnit - GL_TEXTURE0);
    }
    void set_uniform_texture(Texture2DArray &textureArray, const char *val_name) {
        textureArray.bind();
        glUniform1i(glGetUniformLocation(program_id, val_name), textureArray.textureArrayUnit - GL_TEXTURE0);
    }

    void set_uniform_texture(Texture3D &texture, const char *val_name) {
        texture.bind();
        glUniform1i(glGetUniformLocation(program_id, val_name), texture.textureUnit - GL_TEXTURE0);
    }

    void bind() {
        glUseProgram(program_id);
    }

    void release() {
        glUseProgram(0);
    }
};

#endif //SHADER_H