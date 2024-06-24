#version 330 core

in vec4 vertexColor;
out vec4 outCol;

//uniform float uA;

void main()
{
	outCol = vertexColor;
}