#version 330 core

layout (location = 0) in vec3 inVertex;
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in vec3 inNormal;

layout (std140) uniform MeshUBO_Data {
	mat4 pMatrix;
	mat4 vMatrix;
	mat4 mMatrix;

	mat4 mvMatrix;
	mat4 mvpMatrix;

	mat4 mNormalMatrix;
	mat4 mvNormalMatrix;
	mat4 mvpNormalMatrix;
};

out vec2 texCoord;

void main() {
	texCoord = vec2(inTexCoord.x, 1.0 - inTexCoord.y);
	gl_Position = mvpMatrix * vec4(inVertex, 1.f);
}