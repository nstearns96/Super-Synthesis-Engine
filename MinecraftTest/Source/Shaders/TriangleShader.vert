#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform ViewBufferObject 
{
    mat4 view;
    mat4 proj;
} viewObject;

layout(set = 2, binding = 0) uniform ModelBufferObject {mat4 model;} modelBufferObject;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoords;
layout(location = 2) in float inBlockLight;

layout(location = 0) out vec2 texCoords;
layout(location = 1) out float blockLight;

void main() 
{
    gl_Position = viewObject.proj * viewObject.view * modelBufferObject.model * vec4(inPosition, 1.0);
	texCoords = inTexCoords;
	blockLight = inBlockLight;
}