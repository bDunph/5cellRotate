#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <assert.h>
#include <math.h>
#include <iostream>

#include "log.h"
#include "shader_manager.h"
#include "utils.h"

#define GOLDRATIO 1.6180339887

#ifndef _countof
#define _countof(x) (sizeof(x)/sizeof((x)[0]))
#endif

//keep track of window size for things like the viewport and the mouse cursor
int g_gl_width;
int g_gl_height;

//some global variables for Porcess input
glm::vec3 cameraPos;
glm::vec3 cameraFront;
glm::vec3 cameraUp;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//a call-back function for re-sizing the window
void glfw_window_size_callback(GLFWwindow* window, int width, int height){
	g_gl_width = width;
	g_gl_height = height;

	/* update any perspective matrices here */
}

float lastX, lastY;
float pitch = 0.0f, yaw = 0.0f;
bool firstMouse = true;

//to keep track of mouse movement
void mouse_callback(GLFWwindow* window, double xpos, double ypos){

	if(firstMouse) // this bool variable is initially set to true
	{
    		lastX = xpos;
    		lastY = ypos;
    		firstMouse = false;
	}
	
	float xOffset = xpos - lastX;
	float yOffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;
	
	float sensitivity = 0.05f;
	xOffset *= sensitivity;
	yOffset *= sensitivity;	
	
	yaw += xOffset;
	pitch += yOffset;
	
	if(pitch > 89.0f)
  		pitch =  89.0f;
	if(pitch < -89.0f)
  		pitch = -89.0f; 

	glm::vec3 front;
    	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    	front.y = sin(glm::radians(pitch));
    	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    	cameraFront = glm::normalize(front);	
}

void ProcessInput(GLFWwindow *window){
	
	float cameraSpeed = 2.5f * deltaTime; // adjust accordingly
    	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        	cameraPos += cameraSpeed * cameraFront;
    	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        	cameraPos -= cameraSpeed * cameraFront;
    	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        	cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        	cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;	

	if(GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)){
		glfwSetWindowShouldClose(window, 1);
	}
}

int main(){

	bool logStarted = restart_gl_log();
	assert(logStarted);

	//start gl context and O/S window using the glfw helper library
	if(!glfwInit()){
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}

	//uncomment these lines if on osx
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//anti-aliasing
	glfwWindowHint(GLFW_SAMPLES, 4);

	// GLFWmonitor* mon = glfwGetPrimaryMonitor();
	// const GLFWvidmode* vmode = glfwGetVideoMode(mon);

	g_gl_width = 640;//vmode->width;
	g_gl_height = 480;//vmode->height;

	lastX = 320.0f;
	lastY = 240.0f;

	GLFWwindow* window = glfwCreateWindow(g_gl_width, g_gl_height, "Extended GL Init", NULL, NULL);
	if(!window){
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);

	//uncomment if you want to print the OpenGL parameters to the log file
	// log_gl_params();

	//setup for mouse camera control by disabling the cursor while the program is running 
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();


	//get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); //get renderer string
	const GLubyte* version = glGetString(GL_VERSION); //version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//tell GL only to draw onto a pixel if a shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); //enable depth testing
	glDepthFunc(GL_LESS); //depth testing interprets a smaller value as "closer"
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glDisable(GL_CULL_FACE);

	/* specify 4D coordinates of 5-cell from https://en.wikipedia.org/wiki/5-cell */
	/*float vertices [20] = {
		2.0, 0.0, 0.0, 0.0,
		0.0, 2.0, 0.0, 0.0,
		0.0, 0.0, 2.0, 0.0,
		0.0, 0.0, 0.0, 2.0,
		GOLDRATIO, GOLDRATIO, GOLDRATIO, GOLDRATIO
	};*/
	/*float vertices [20] = {
		1.0f/sqrt(10.0f), 1.0f/sqrt(6.0f), 1.0f/sqrt(3.0f), 1.0f,
		1.0f/sqrt(10.0f), 1.0f/sqrt(6.0f), 1.0f/sqrt(3.0f), -1.0f,
		1.0f/sqrt(10.0f), 1.0f/sqrt(6.0f), -2.0f/sqrt(3.0f), 0.0f,
		1.0f/sqrt(10.0f), -sqrt(3.0f/2.0f), 0.0f, 0.0f,
		-2.0f * sqrt(2.0f/5.0f), 0.0f, 0.0f, 0.0f
	};*/
	float vertices [20] = {
		0.3162f, 0.4082f, 0.5774f, 1.0f,
		0.3162f, 0.4082f, 0.5774f, -1.0f,
		0.3162f, 0.4082f, -1.1547, 0.0f,
		0.3162f, -1.2247f, 0.0f, 0.0f,
		-1.2649f, 0.0f, 0.0f, 0.0f
	};

	/* indices specifying 10 faces */
	unsigned int indices [30] = {
		4, 2, 3,
		3, 0, 2,
		2, 0, 4,
		4, 0, 3,
		3, 1, 0,
		1, 4, 0,
		0, 1, 2,
		2, 3, 1,
		1, 3, 4,
		4, 2, 1	
	};

	unsigned int lineIndices [20] = {
		4, 2,
		2, 3,
		3, 4,
		4, 0,
		0, 2, 
		0, 3,
		3, 1,
		1, 2,
		4, 1,
		1, 0
	};

	//array of verts
	glm::vec4 vertArray [5] = {
		glm::vec4(vertices[0], vertices[1], vertices[2], vertices[3]),
		glm::vec4(vertices[4], vertices[5], vertices[6], vertices[7]),
		glm::vec4(vertices[8], vertices[9], vertices[10], vertices[11]),
		glm::vec4(vertices[12], vertices[13], vertices[14], vertices[15]),
		glm::vec4(vertices[16], vertices[17], vertices[18], vertices[19])
	};

	//array of faces
	glm::vec3 faceArray [10] = {
		glm::vec3(indices[0], indices[1], indices[2]),
		glm::vec3(indices[3], indices[4], indices[5]),
		glm::vec3(indices[6], indices[7], indices[8]),
		glm::vec3(indices[9], indices[10], indices[11]),
		glm::vec3(indices[12], indices[13], indices[14]),
		glm::vec3(indices[15], indices[16], indices[17]),
		glm::vec3(indices[18], indices[19], indices[20]),
		glm::vec3(indices[21], indices[22], indices[23]),
		glm::vec3(indices[24], indices[25], indices[26]),
		glm::vec3(indices[27], indices[28], indices[29]),
	};

	glm::vec4 faceNormalArray [10];
	
	//calculate vertex normals in 4D to send to shaders for lighting
	for(int i = 0; i < _countof(faceArray); i++){
		//calculate three linearly independent vectors for each face
		unsigned int indexA = faceArray[i].x;
		unsigned int indexB = faceArray[i].y;
		unsigned int indexC = faceArray[i].z;

		glm::vec4 vertA = vertArray[indexA];
		glm::vec4 vertB = vertArray[indexB];
		glm::vec4 vertC = vertArray[indexC];

		glm::vec4 vectorA = glm::vec4(vertB.x - vertA.x, vertB.y - vertA.y, vertB.z - vertA.z, vertB.w - vertA.w);
		glm::vec4 vectorB = glm::vec4(vertC.x - vertB.x, vertC.y - vertB.y, vertC.z - vertB.z, vertC.w - vertB.w);
		glm::vec4 vectorC = glm::vec4(vertA.x - vertC.x, vertA.y - vertC.y, vertA.z - vertC.z, vertA.w - vertC.w);

		//calculate orthonormal basis for vectorA, B and C using Gram-Schmidt. We can then calculte
		//the 4D normal
		glm::vec4 u1 = glm::normalize(vectorA);
		
		glm::vec4 y2 = vectorB - ((glm::dot(vectorB, u1)) * u1);
		glm::vec4 u2 = glm::normalize(y2);

		glm::vec4 y3 = vectorC - ((glm::dot(vectorC, u2)) * u2);
		glm::vec4 u3 = glm::normalize(y3);
		
		//calculate the  normal for each face
	 	//using matrices and  Laplace expansion we can find the normal 
		//vector in 4D given three input vectors	
		//this procedure is following the article at https://ef.gy/linear-algebra:normal-vectors-in-higher-dimensional-spaces 
		/* a x b x c = 	| a0 b0 c0 right|
				| a1 b1 c1 up	|
				| a2 b2 c2 back	|	
				| a3 b3 c3 charm|*/
		glm::vec4 right = glm::vec4(1.0, 0.0, 0.0, 0.0);	
		glm::vec4 up = glm::vec4(0.0, 1.0, 0.0, 0.0);	
		glm::vec4 back = glm::vec4(0.0, 0.0, 1.0, 0.0);	
		glm::vec4 charm = glm::vec4(0.0, 0.0, 0.0, 1.0);	

		/*glm::mat3 matA = glm::mat3(	vectorA.y, vectorB.y, vectorC.y,
						vectorA.z, vectorB.z, vectorC.z,
						vectorA.w, vectorB.w, vectorC.w);

		glm::mat3 matB = glm::mat3(	vectorA.x, vectorB.x, vectorC.x,
						vectorA.z, vectorB.z, vectorC.z,
						vectorA.w, vectorB.w, vectorC.w);

		glm::mat3 matC = glm::mat3(	vectorA.x, vectorB.x, vectorC.x,
						vectorA.y, vectorB.y, vectorC.y,
						vectorA.w, vectorB.w, vectorC.w);
	
		glm::mat3 matD = glm::mat3(	vectorA.x, vectorB.x, vectorC.x,
						vectorA.y, vectorB.y, vectorC.y,
						vectorA.z, vectorB.z, vectorC.z);*/

		glm::mat3 matA = glm::mat3(	u1.y, u2.y, u3.y,
						u1.z, u2.z, u3.z,
						u1.w, u2.w, u3.w);

		glm::mat3 matB = glm::mat3(	u1.x, u2.x, u3.x,
						u1.z, u2.z, u3.z,
						u1.w, u2.w, u3.w);

		glm::mat3 matC = glm::mat3(	u1.x, u2.x, u3.x,
						u1.y, u2.y, u3.y,
						u1.w, u2.w, u3.w);
	
		glm::mat3 matD = glm::mat3(	u1.x, u2.x, u3.x,
						u1.y, u2.y, u3.y,
						u1.z, u2.z, u3.z);	

		float determinantA = glm::determinant(matA);	
		float determinantB = glm::determinant(matB);	
		float determinantC = glm::determinant(matC);	
		float determinantD = glm::determinant(matD);	

		glm::vec4 termA = (determinantA * right) * -1.0f;
		glm::vec4 termB = determinantB * up;
		glm::vec4 termC = (determinantC * back) * -1.0f;
		glm::vec4 termD = determinantD * charm;

		glm::vec4 faceNormal = termA + termB + termC + termD;
		faceNormalArray[i] += faceNormal;
	}
	
	float vertexNormalArray [20];

	//calculate the normal for each vertex by taking the average of the normals of each adjacent face
	for(int i = 0; i < _countof(vertArray); i++){
		glm::vec4 cumulativeNormals = glm::vec4(0.0);
		for(int j = 0; j < _countof(faceArray); j++){
				
			//does this face [j] contain vert [i]?
			unsigned int vertA = faceArray[j].x;				 
			unsigned int vertB = faceArray[j].y;
			unsigned int vertC = faceArray[j].z;
			if(vertA == i || vertB == i || vertC == i){
				cumulativeNormals += faceNormalArray[j];
			}	
		}
		glm::vec4 vertexNormal = glm::normalize(cumulativeNormals);
		vertexNormalArray[i*4] += vertexNormal.x;
		vertexNormalArray[i*4+1] += vertexNormal.y;
		vertexNormalArray[i*4+2] += vertexNormal.z;
		vertexNormalArray[i*4+3] += vertexNormal.w;
	}
		
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(float), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	GLuint vertNormals;
	glGenBuffers(1, &vertNormals);
	glBindBuffer(GL_ARRAY_BUFFER, vertNormals);
	glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(float), vertexNormalArray, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	GLuint index;
	glGenBuffers(1, &index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 30 * sizeof(unsigned int), indices, GL_STATIC_DRAW);
	
	//GLuint lineIndex;
	//glGenBuffers(1, &lineIndex);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineIndex);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, 20 * sizeof(unsigned int), lineIndices, GL_STATIC_DRAW);

	//load shaders
	char* vertex_shader;
	bool isVertLoaded = load_shader("rasterPolychoron.vert", vertex_shader);
	if(!isVertLoaded) return 1;

	char* fragment_shader;
	bool isFragLoaded = load_shader("rasterPolychoron.frag", fragment_shader);
	if(!isFragLoaded) return 1;

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, NULL);
	glCompileShader(vs);
	delete[] vertex_shader;
	//check for compile errors
	bool isVertCompiled = shader_compile_check(vs);
	if(!isVertCompiled) return 1;

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, NULL);
	glCompileShader(fs);
	delete[] fragment_shader;
	//check for compile errors
	bool isFragCompiled = shader_compile_check(fs);
	if(!isFragCompiled) return 1;
	
	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, fs);
	glAttachShader(shader_program, vs);
	glLinkProgram(shader_program);
	bool didShadersLink = shader_link_check(shader_program);
	if(!didShadersLink) return 1;

	//matrices
	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;
	glm::mat4 modelMatrix;

	// projection matrix setup	
	projectionMatrix = glm::perspective(45.0f, (float)g_gl_width / (float)g_gl_height, 0.1f, 1000.0f);

	GLint projMatLoc = glGetUniformLocation(shader_program, "projMat");
	GLint viewMatLoc = glGetUniformLocation(shader_program, "viewMat");
	GLint modelMatLoc = glGetUniformLocation(shader_program, "modelMat");
	GLint rotationZWLoc = glGetUniformLocation(shader_program, "rotZW");
	GLint rotationXWLoc = glGetUniformLocation(shader_program, "rotXW");
	GLint rotationYWLoc = glGetUniformLocation(shader_program, "rotYW");
	GLint scaleMatLoc = glGetUniformLocation(shader_program, "scaleMat");	
	//view matrix setup
	//glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.5f);
	//glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	//glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	//glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);

	//glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	//glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	
	//glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

	cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
	cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	//model matrix
	modelMatrix = glm::mat4(1.0f);

	//scale matrix
	glm::mat4 scaleMatrix = glm::mat4(
		50.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 50.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 50.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	//uniforms
	GLint lightPosLoc = glGetUniformLocation(shader_program, "lightPos");
	glm::vec3 lightPos = glm::vec3(0.0f, 1.0f, -0.2f); 

	GLint light2PosLoc = glGetUniformLocation(shader_program, "light2Pos");
	glm::vec3 lightPos2 = glm::vec3(0.0f, 1.0f, 0.5f);

	GLint cameraPosLoc = glGetUniformLocation(shader_program, "camPos");
	GLint alphaLoc = glGetUniformLocation(shader_program, "alpha");

	print_all(shader_program);

	//only use during development as computationally expensive
	bool validProgram = is_valid(shader_program);
	if(!validProgram){
		fprintf(stderr, "ERROR: shader program not valid\n");
		return 1;
	}

	//workaround for macOS Mojave bug
	bool needDraw = true;

	float radius = 0.75f;

	//glPointSize(4.0f);

	while(!glfwWindowShouldClose(window)){

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		
		_update_fps_counter(window);
		
		ProcessInput(window);

		glClearColor(0.87, 0.85, 0.75, 0.95);
		//wipe the drawing surface clear
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//resize openGL elements
		glViewport(0, 0, g_gl_width, g_gl_height);

		/* draw stuff here */
		
		//float camX = sin(glfwGetTime()) * radius;
		//float camZ = cos(glfwGetTime()) * radius;
		//glm::vec3 camPos = glm::vec3(camX, 0.0f, camZ);
		//viewMatrix = glm::lookAt(camPos, cameraTarget, cameraUp);

		viewMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);	

		//rotation around W axis
		glm::mat4 rotationZW = glm::mat4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, cos(glfwGetTime() * 0.2f), -sin(glfwGetTime() * 0.2f),
			0.0f, 0.0f, sin(glfwGetTime() * 0.2f), cos(glfwGetTime() * 0.2f)
		);

		glm::mat4 rotationXW = glm::mat4(	
			cos(glfwGetTime() * 0.2f), 0.0f, 0.0f, sin(glfwGetTime() * 0.2f),
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f, 
			-sin(glfwGetTime() * 0.2f), 0.0f, 0.0f, cos(glfwGetTime() * 0.2f) 
		);
	
		glm::mat4 rotationYW = glm::mat4(	
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, cos(glfwGetTime() * 0.2f), 0.0f, -sin(glfwGetTime() * 0.2f),
			0.0f, 0.0f, 1.0f, 0.0f, 
			0.0f, sin(glfwGetTime() * 0.2f), 0.0f, cos(glfwGetTime() * 0.2f)
		);

		float a = 0.0f;

		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index);
		glUseProgram(shader_program);
		// glUniform1f(glGetUniformLocation(shader_program, "iGlobalTime"), global_time);

		glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, &projectionMatrix[0][0]);
		glUniformMatrix4fv(viewMatLoc, 1, GL_FALSE, &viewMatrix[0][0]);
		glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, &modelMatrix[0][0]);
		glUniformMatrix4fv(rotationZWLoc, 1, GL_FALSE, &rotationZW[0][0]);
		glUniformMatrix4fv(rotationXWLoc, 1, GL_FALSE, &rotationXW[0][0]);
		glUniformMatrix4fv(rotationYWLoc, 1, GL_FALSE, &rotationYW[0][0]);
		glUniformMatrix4fv(scaleMatLoc, 1, GL_FALSE, &scaleMatrix[0][0]);
		glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(light2PosLoc, lightPos2.x, lightPos2.y, lightPos2.z);
		glUniform3f(cameraPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);
		glUniform1f(alphaLoc, a);


      		//draw 5-cell using index buffer and 5 pass transparency technique from http://www.alecjacobson.com/weblog/?p=2750
		//1st pass
		glDisable(GL_CULL_FACE);
		glDepthFunc(GL_LESS);
		float f = 0.75f;
		float origAlpha = 0.4f;	
		a = 0.0f;
		glUniform1f(alphaLoc, a);
		glDrawElements(GL_TRIANGLES, 30 * sizeof(unsigned int), GL_UNSIGNED_INT, (void*)0);

		//2nd pass
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glDepthFunc(GL_ALWAYS);
		a = origAlpha * f;
		glUniform1f(alphaLoc, a);
		glDrawElements(GL_TRIANGLES, 30 * sizeof(unsigned int), GL_UNSIGNED_INT, (void*)0);
		
		//3rd pass
		glDepthFunc(GL_LEQUAL);
		a = (origAlpha - (origAlpha * f)) / (1.0f - (origAlpha * f));
		glUniform1f(alphaLoc, a);
		glDrawElements(GL_TRIANGLES, 30 * sizeof(unsigned int), GL_UNSIGNED_INT, (void*)0);
		
		//4th pass
		glCullFace(GL_BACK);
		glDepthFunc(GL_ALWAYS);
		a = origAlpha * f;
		glUniform1f(alphaLoc, a);
		glDrawElements(GL_TRIANGLES, 30 * sizeof(unsigned int), GL_UNSIGNED_INT, (void*)0);

		//5th pass
		glDisable(GL_CULL_FACE);
		glDepthFunc(GL_LEQUAL);
		a = (origAlpha - (origAlpha * f)) / (1.0f - (origAlpha * f));
		glUniform1f(alphaLoc, a);
		glDrawElements(GL_TRIANGLES, 30 * sizeof(unsigned int), GL_UNSIGNED_INT, (void*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// draw 5-cell using index buffer
		//glDrawElements(GL_TRIANGLES, 30 * sizeof(unsigned int), GL_UNSIGNED_INT, (void*)0);
		//glDrawElements(GL_LINES, 20 * sizeof(unsigned int), GL_UNSIGNED_INT, (void*)0);
		//glBindVertexArray(0);

		//update other events like input handling
		glfwPollEvents();

		// workaround for macOS Mojave bug
		if(needDraw){
			glfwShowWindow(window);
			glfwHideWindow(window);
			glfwShowWindow(window);
			needDraw = false;
		}

		//put the stuff we've been drawing onto the display
		glfwSwapBuffers(window);

		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetWindowSizeCallback(window, glfw_window_size_callback);
		glfwSetErrorCallback(glfw_error_callback);
		
		lastFrame = currentFrame;
	}

	//close GL context and any other GL resources
	glfwTerminate();
	return 0;
}
