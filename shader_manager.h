#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

bool load_shader(const char* filename, char* &string);
bool shader_compile_check(GLuint shader);
bool shader_link_check(GLuint program);
bool is_valid(GLuint program);

#endif
