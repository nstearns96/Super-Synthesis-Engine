#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 1, binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 texCoords;
layout(location = 1) in float blockLight;

layout(location = 0) out vec4 outColor;

void main() 
{	
	vec4 texColor = texture(texSampler, texCoords);
    outColor = vec4(blockLight * texColor.x, blockLight * texColor.y, blockLight * texColor.z, texColor.w);
}