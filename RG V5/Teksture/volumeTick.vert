#version 330 core

layout(location = 0) in vec2 inPos;
out vec4 vertexColor;

uniform vec2 uPos;
uniform vec4 uColor;

void main()
{
	gl_Position = vec4(inPos + uPos, 0.0, 1.0);
	vertexColor = uColor;
}