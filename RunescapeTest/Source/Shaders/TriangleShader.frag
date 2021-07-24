#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 1, binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 texCoords;
layout(location = 1) in float fragHeight;

layout(location = 0) out vec4 outColor;

void main() 
{	
	vec4 texColor = texture(texSampler, texCoords);
	if(fragHeight < 0.45f)
	{	
	    outColor = vec4(0.1f, 0.3f, 0.7f, 1.0f);
	}
	else if(fragHeight < 0.5f)
	{	
	    outColor = vec4(0.7f, 0.7f, 0.2f, 1.0f);
	}
	else if(fragHeight < 0.6f)
	{	
	    outColor = vec4(0.1f, 0.7f, 0.1f, 1.0f);
	}
	else if(fragHeight < 0.7f)
	{	
	    outColor = vec4(0.2f, 0.1f, 0.0f, 1.0f);
	}
	else 
	{	
	    outColor = vec4(1.0f);
	}
}