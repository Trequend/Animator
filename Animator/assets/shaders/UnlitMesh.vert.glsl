#version 430

layout (location=0) in vec3 a_Position;
layout (location=1) in vec3 a_Normal;
layout (location=2) in vec2 a_TexCoords;

uniform mat4 u_ViewProjectionMatrix;
uniform mat4 u_ModelMatrix;

out vec2 v_TexCoords;

void main()
{
	v_TexCoords = a_TexCoords;
	gl_Position = u_ViewProjectionMatrix * u_ModelMatrix * vec4(a_Position, 1.0);
}