#version 410

layout(location = 0) in vec4 position4D;
layout(location = 1) in vec4 normal4D;

uniform mat4 projMat;
uniform mat4 viewMat;
uniform mat4 modelMat;
uniform mat4 rotZW;
uniform mat4 rotXW;
uniform mat4 rotYW;
uniform mat4 scaleMat;
 
out vec3 vertNormal_worldSpace;
out vec3 fragPos_worldSpace;

void main() {

	
	//vec4 scaledPos = scaleMat * position4D;
	//vec4 rotatedPos = rotZW * scaledPos;
	vec4 rotatedPos = rotYW * rotXW * rotZW * position4D;

	float dist = 2.0;
	//float xPos3D = position4D.x / (dist - position4D.w);
	//float yPos3D = position4D.y / (dist - position4D.w);
	//float zPos3D = position4D.z / (dist - position4D.w);

	float xPos3D = rotatedPos.x / (dist - rotatedPos.w);
	float yPos3D = rotatedPos.y / (dist - rotatedPos.w);
	float zPos3D = rotatedPos.z / (dist - rotatedPos.w);

	//vec4 position3D = scaleMat * vec4(xPos3D, yPos3D, zPos3D, 1.0);
	vec4 position3D = vec4(xPos3D, yPos3D, zPos3D, 1.0);

	fragPos_worldSpace = vec3(modelMat * position3D).xyz;

	//project normal to 3D
	float xNorm3D = normal4D.x / (dist - normal4D.w);
	float yNorm3D = normal4D.y / (dist - normal4D.w);
	float zNorm3D = normal4D.z / (dist - normal4D.w);
	
	vec3 normal3D = vec3(xNorm3D, yNorm3D, zNorm3D);
	vertNormal_worldSpace = mat3(transpose(inverse(modelMat))) * normal3D;

	gl_Position = projMat * viewMat * modelMat *  position3D;	
}
