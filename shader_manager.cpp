#include <cstdio>
#include <GL/glew.h>
#include <cstdlib>

#include "shader_manager.h"
#include "log.h"

bool load_shader(const char* filename, char* &string){
	//read shaders from file
	FILE* f = fopen(filename, "r");

	if(!f){
		fprintf(stderr, "ERROR: %s not opened", filename);
		return false;
	}

	//Determine file size
	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);

	//calloc() initialises memory to zero
	string = (char*)calloc(sizeof(char), size + 1);

	rewind(f);
	fread((void*)string, sizeof(char), size, f);
	*(string + size + 1) = '\0';
	printf("%s\n", string);
	return true;
}

bool shader_compile_check(GLuint shader){
	int params = -1;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &params);
	if(GL_TRUE != params){
		fprintf(stderr, "ERROR: GL shader %i did not compile\n", shader);
		print_shader_info_log(shader);
		return false;
	}
	return true;
}

bool shader_link_check(GLuint program){
	int params = -1;
	glGetProgramiv(program, GL_LINK_STATUS, &params);
	if(GL_TRUE != params){
		fprintf(stderr, "ERROR: could not link shader program GL index %u\n", program);
		return false;
	}
	return true;
}

bool is_valid(GLuint program){
	glValidateProgram(program);
	int params = -1; 
	glGetProgramiv(program, GL_VALIDATE_STATUS, &params);
	printf("program %i GL_VALIDATE_STATUS = %i\n", program, params);
	if(GL_TRUE != params){
		print_program_info_log(program);
		return false;
	}
	return true;
}
